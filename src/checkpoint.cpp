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
	fprintf(stderr, "\n[CHECKPOINT] switch %d: wrote %s (%zu front elements, %d faces, active block %d) in %.1f s\n",
			sw_nswitch, path, elems.size(), numFacesAdded, ab, get_time_seconds() - t0);
	ContinueProjections();
}

void Triangulator::LoadCheckpoint(const char *path) {
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
			unsigned int pf; real_type ps; in.pod(pf); in.pod(ps); e.priority = FrontElement::priority_type(pf, ps);
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
		heap.restore(level, h);
	}
	{
		long long n = in.i64();
		boundary_connectors.resize(n);
		for (long long i=0; i<n; i++) { int x; in.pod(x); boundary_connectors[i] = elems.at(x); }
	}
	{
		long long n = in.i64();
		vertinfo.resize(n);
		for (long long i=0; i<n; i++) { in.pod(vertinfo[i].front_refcount); in.pod(vertinfo[i].vertindex); in.podvec(vertinfo[i].ring); }
		in.podvec(available_vertinfo);
		in.pod(max_active_verts);
	}
	in.pod(numVertsAdded); in.pod(numFacesAdded); in.pod(flipOutput);
	in.pod(haveWorkingBlock); in.pod(workingBlock); in.pod(workingAreaRadius); in.pod(workingAreaCenter);
	in.pod(kd_max_step_seen); in.pod(kdg_edits); in.pod(kdg_size_at_rebuild); in.pod(sw_nswitch);
	int ab; in.pod(ab);
	controller.SetActiveBlockIndex(ab);
	controller.LoadOutputState(fp);
	in.tag("END!");
	fclose(fp);

	KDRebuildFromHeap();

	// projections that were unfinished are requested again (finished results were restored as they were)
	long long nreq = 0;
	for (size_t i=0; i<elems.size(); i++)
		if (!(elems[i]->flags & FRONT_FLAG_CONNECTOR) && elems[i]->proj_res.result == PROJECT_INCOMPLETE) { RequestProjection(elems[i]); nreq++; }

	if (const char *rs = getenv("AFRONT_CKPT_RESAVE")) SaveCheckpoint(rs);   // debugging: save/load round-trip test
	fprintf(stderr, "\n[CHECKPOINT] resumed from %s: switch %d, %zu front elements in %zu fronts, %d verts %d faces, active block %d, %lld projections re-requested\n",
			path, sw_nswitch, elems.size(), fronts.size(), numVertsAdded, numFacesAdded, ab, nreq);
}

// called at the end of UpdateWorkingBlock
void Triangulator::MaybeCheckpoint() {
	if (!checkpoint_dir || checkpoint_every <= 0 || sw_nswitch % checkpoint_every != 0)
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
