// 2026: checkpoint / restart of the triangulation state (see PLAN.md in the paper repo, "checkpoint/restart design").
//
// A checkpoint is written at the end of Triangulator::UpdateWorkingBlock (a serial point: projections paused, no kd-tree
// traversal in progress) and holds everything the main loop needs to continue: fronts, heap, vertex bookkeeping, the
// working block/area, the output stages' buffered state and the output file size. The guidance fields are not part of
// it: they live in the oocgf files (run with -reuse_guidance t -reuse_seeds t). In-flight projections are not saved;
// elements whose projection was unfinished are re-requested after loading.
//
// Both kd-trees are rebuilt from the heap levels when a checkpoint is written *and* when it is loaded, so a resumed
// run takes exactly the same steps as the uninterrupted one (bit-identical output with one thread).

#include <unordered_map>
#include <string>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "ckpt_io.h"   // (this file is #included at the end of triangulator.cpp: it needs that file's heap glue)

extern char *checkpoint_dir, *resume_from;
extern int checkpoint_every, checkpoint_keep, checkpoint_keep_every;
extern int sw_nswitch;
extern bool ckpt_ready;
extern long long kdg_edits, kdg_size_at_rebuild;
extern real_type kd_max_step_seen;

static const int CKPT_VERSION = 1;


// make both kd-trees match the heap levels exactly (also used right after loading)
void Triangulator::KDRebuildFromHeap() {
	if (!heap.contents(0).empty()) kdtree_l.ReBuild(heap.contents(0), *this);
	if (!heap.contents(1).empty()) kdtree_g.ReBuild(heap.contents(1), *this);
	kdg_edits = 0;
	kdg_size_at_rebuild = (long long)heap.contents(1).size();
}

void Triangulator::SaveCheckpoint(const char *path) {
	PauseProjections();
	double t0 = get_time_seconds();

	KDRebuildFromHeap();   // so the saving process and a resumed one continue identically

	std::string tmp = std::string(path) + ".tmp";
	FILE *fp = fopen(tmp.c_str(), "wb");
	if (!fp) { fprintf(stderr, "checkpoint: cannot write %s\n", tmp.c_str()); ContinueProjections(); return; }
	CkptOut o(fp);

	o.tag("AFCK"); o.pod(CKPT_VERSION);

	// element ids: fronts in order, elements in list order
	std::unordered_map<const FrontElement*, int> id;
	std::vector<feli> elems;
	o.i64(fronts.size());
	for (size_t fi=0; fi<fronts.size(); fi++) {
		Front *f = fronts[fi];
		long long n = 0;
		if (!f->empty()) {
			feli first = f->FirstElement(), e = first;
			do { n++; e = Front::NextElement(e); } while (e != first);
		}
		o.i64(n);
		if (n) {
			feli first = f->FirstElement(), e = first;
			do {
				id[&*e] = (int)elems.size(); elems.push_back(e);
				o.pod(e->position); o.pod(e->normal); o.pod(e->vertindex); o.pod(e->vertindex_l);
				unsigned int pf = e->priority.first; real_type ps = e->priority.second; o.pod(pf); o.pod(ps);
				o.pod(e->max_step); o.pod(e->flags);
				o.pod(e->proj_res.position); o.pod(e->proj_res.normal); o.pod(e->proj_res.step);
				int res = e->proj_res.result; o.pod(res);
				e = Front::NextElement(e);
			} while (e != first);
		}
	}
	for (int level=0; level<2; level++) {
		const std::vector<feli> &h = heap.contents(level);
		o.i64(h.size());
		for (size_t i=0; i<h.size(); i++) { int x = id.at(&*h[i]); o.pod(x); }
	}
	o.i64(boundary_connectors.size());
	for (size_t i=0; i<boundary_connectors.size(); i++) { int x = id.at(&*boundary_connectors[i]); o.pod(x); }

	o.i64(vertinfo.size());
	for (size_t i=0; i<vertinfo.size(); i++) { o.pod(vertinfo[i].front_refcount); o.pod(vertinfo[i].vertindex); o.podvec(vertinfo[i].ring); }
	o.podvec(available_vertinfo);
	o.pod(max_active_verts);

	o.pod(numVertsAdded); o.pod(numFacesAdded); o.pod(flipOutput);
	o.pod(haveWorkingBlock); o.pod(workingBlock); o.pod(workingAreaRadius); o.pod(workingAreaCenter);
	o.pod(kd_max_step_seen); o.pod(kdg_edits); o.pod(kdg_size_at_rebuild); o.pod(sw_nswitch);
	int ab = controller.GetActiveBlockIndex(); o.pod(ab);

	controller.SaveOutputState(fp);
	o.tag("END!");
	fflush(fp); fsync(fileno(fp)); fclose(fp);
	rename(tmp.c_str(), path);
	if (FILE *tf = trace_file()) fprintf(tf, "C switch %d saved\n", sw_nswitch);
	if (getenv("AFRONT_TRACE")) fprintf(stderr, "[CKPTSTATE] r=%.17g c=(%.17g %.17g %.17g)\n", (double)workingAreaRadius, (double)workingAreaCenter[0], (double)workingAreaCenter[1], (double)workingAreaCenter[2]);
	fprintf(stderr, "\n[CHECKPOINT] switch %d: wrote %s (%zu front elements, %d faces, active block %d) in %.1f s\n",
			sw_nswitch, path, elems.size(), numFacesAdded, ab, get_time_seconds() - t0);
	ContinueProjections();
}

extern int vertex_offset, face_offset;
static std::vector<Triangulator::feli> g_merge_elems;      // parallel slabs: elements collected from all merged states
static std::vector<Triangulator::feli> g_merge_unprojected;

// merge=false: restore the state of a checkpoint into a fresh Triangulator.
// merge=true (parallel slabs, phase 2): append the end state of another slab run; the working-block gate is switched off
// (PRIORITY_OWB cleared) as when the blocks run out, the heap is filled by MergeCheckpoints.
void Triangulator::LoadCheckpointImpl(const char *path, bool merge) {
	FILE *fp = fopen(path, "rb");
	if (!fp) { fprintf(stderr, "checkpoint: cannot read %s\n", path); exit(2); }
	CkptIn in(fp);
	in.tag("AFCK"); int ver; in.pod(ver);
	if (ver != CKPT_VERSION) { fprintf(stderr, "checkpoint: version %d, expected %d\n", ver, CKPT_VERSION); exit(2); }

	std::vector<feli> elems;
	long long nf = in.i64();
	for (long long fi=0; fi<nf; fi++) {
		Front *f = new Front();
		fronts.push_back(f);
		long long n = in.i64();
		for (long long k=0; k<n; k++) {
			FrontElement e;
			in.pod(e.position); in.pod(e.normal); in.pod(e.vertindex); in.pod(e.vertindex_l);
			unsigned int pf; real_type ps; in.pod(pf); in.pod(ps);
			if (merge && !(pf & PRIORITY_FAILSAFE_ANY)) pf &= ~PRIORITY_OWB;
			e.priority = FrontElement::priority_type(pf, ps);
			in.pod(e.max_step); in.pod(e.flags);
			in.pod(e.proj_res.position); in.pod(e.proj_res.normal); in.pod(e.proj_res.step);
			int res; in.pod(res); e.proj_res.result = res; e.proj_res.pending = 0;
			e.heap_position = 0xFFFFFFFF;
			elems.push_back(f->AddElement(e));
		}
	}
	for (int level=0; level<2; level++) {
		long long n = in.i64();
		std::vector<feli> h(n);
		for (long long i=0; i<n; i++) { int x; in.pod(x); h[i] = elems.at(x); }
		if (!merge) heap.restore(level, h);
	}
	{
		long long n = in.i64();
		size_t base = boundary_connectors.size();
		if (!merge) boundary_connectors.clear();
		boundary_connectors.resize(merge ? base + n : n);
		for (long long i=0; i<n; i++) { int x; in.pod(x); boundary_connectors[(merge ? base : 0) + i] = elems.at(x); }
	}
	{
		long long n = in.i64();
		int vbase = merge ? (int)vertinfo.size() : 0;
		vertinfo.resize(vbase + n);
		for (long long i=0; i<n; i++) { in.pod(vertinfo[vbase+i].front_refcount); in.pod(vertinfo[vbase+i].vertindex); in.podvec(vertinfo[vbase+i].ring); }
		std::vector<int> av; in.podvec(av);
		if (!merge) available_vertinfo = av;
		else for (size_t i=0; i<av.size(); i++) available_vertinfo.push_back(av[i] + vbase);
		int mav; in.pod(mav);
		max_active_verts = merge ? std::max(max_active_verts, mav) : mav;
		if (merge) for (size_t i=0; i<elems.size(); i++) elems[i]->vertindex_l += vbase;
	}
	{
		int nv, nfc; bool fo, hwb; Box3 wb; real_type war; Point3 wac; real_type kms; long long ke, ks; int nsw;
		in.pod(nv); in.pod(nfc); in.pod(fo);
		in.pod(hwb); in.pod(wb); in.pod(war); in.pod(wac);
		in.pod(kms); in.pod(ke); in.pod(ks); in.pod(nsw);
		if (!merge) {
			numVertsAdded = nv; numFacesAdded = nfc; flipOutput = fo;
			haveWorkingBlock = hwb; workingBlock = wb; workingAreaRadius = war; workingAreaCenter = wac;
			kd_max_step_seen = kms; kdg_edits = ke; kdg_size_at_rebuild = ks; sw_nswitch = nsw;
		} else {
			flipOutput = fo;
			kd_max_step_seen = std::max(kd_max_step_seen, kms);
		}
	}
	int ab; in.pod(ab);
	if (!merge) { controller.SetActiveBlockIndex(ab); controller.LoadOutputState(fp); }
	else controller.MergeOutputState(fp);
	in.tag("END!");
	fclose(fp);

	if (merge) {
		for (size_t i=0; i<elems.size(); i++) {
			g_merge_elems.push_back(elems[i]);
			if (!(elems[i]->flags & FRONT_FLAG_CONNECTOR) && elems[i]->proj_res.result == PROJECT_INCOMPLETE) g_merge_unprojected.push_back(elems[i]);
		}
		fprintf(stderr, "\n[CHECKPOINT] merged %s: %zu front elements, %lld fronts\n", path, elems.size(), nf);
		return;
	}

	KDRebuildFromHeap();

	// projections that were unfinished are requested again (finished results were restored as they were)
	long long nreq = 0;
	for (size_t i=0; i<elems.size(); i++)
		if (!(elems[i]->flags & FRONT_FLAG_CONNECTOR) && elems[i]->proj_res.result == PROJECT_INCOMPLETE) { RequestProjection(elems[i]); nreq++; }

	if (const char *rs = getenv("AFRONT_CKPT_RESAVE")) SaveCheckpoint(rs);   // debugging: save/load round-trip test
	fprintf(stderr, "\n[CHECKPOINT] resumed from %s: switch %d, %zu front elements in %zu fronts, %d verts %d faces, active block %d, %lld projections re-requested\n",
			path, sw_nswitch, elems.size(), fronts.size(), numVertsAdded, numFacesAdded, ab, nreq);
}

void Triangulator::LoadCheckpoint(const char *path) { LoadCheckpointImpl(path, false); }

// Parallel slabs, phase 2: merge the end states of the slab runs and continue without the working-block gate.
void Triangulator::MergeCheckpoints(const char *list) {
	std::string all(list);
	size_t pos = 0;
	while (pos <= all.size()) {
		size_t c = all.find(',', pos);
		std::string one = all.substr(pos, c == std::string::npos ? std::string::npos : c - pos);
		if (!one.empty()) LoadCheckpointImpl(one.c_str(), true);
		if (c == std::string::npos) break;
		pos = c + 1;
	}
	numVertsAdded = vertex_offset; numFacesAdded = face_offset;
	haveWorkingBlock = false;
	for (size_t i=0; i<g_merge_elems.size(); i++) heap.push(g_merge_elems[i]);
	workingAreaRadius = INFINITY;
	workingAreaCenter = g_merge_elems.empty() ? Point3(0,0,0) : g_merge_elems[0]->position;
	KDRebuildFromHeap();
	for (size_t i=0; i<g_merge_unprojected.size(); i++) RequestProjection(g_merge_unprojected[i]);
	fprintf(stderr, "\n[CHECKPOINT] merged %zu front elements (%zu fronts); new vertices start at %d, faces at %d\n",
			g_merge_elems.size(), fronts.size(), vertex_offset, face_offset);
}

// called at the end of UpdateWorkingBlock
void Triangulator::MaybeCheckpoint() {
	if (!ckpt_ready || !checkpoint_dir || checkpoint_every <= 0 || sw_nswitch % checkpoint_every != 0)
		return;
	mkdir(checkpoint_dir, 0777);
	char path[2048];
	snprintf(path, sizeof path, "%s/ckpt_%06d.afck", checkpoint_dir, sw_nswitch);
	SaveCheckpoint(path);

	// retention: keep the newest checkpoint_keep files, plus every checkpoint_keep_every-th switch
	if (checkpoint_keep > 0) {
		std::vector<int> have;
		if (DIR *d = opendir(checkpoint_dir)) {
			while (struct dirent *de = readdir(d)) {
				int k; if (sscanf(de->d_name, "ckpt_%d.afck", &k) == 1 && std::string(de->d_name).size() == 16) have.push_back(k);
			}
			closedir(d);
		}
		std::sort(have.begin(), have.end());
		for (int i=0; i+checkpoint_keep < (int)have.size(); i++) {
			if (checkpoint_keep_every > 0 && have[i] % checkpoint_keep_every == 0) continue;
			snprintf(path, sizeof path, "%s/ckpt_%06d.afck", checkpoint_dir, have[i]);
			unlink(path);
		}
	}
}
