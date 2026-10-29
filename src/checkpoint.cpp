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

static const int CKPT_VERSION = 3;   // 2: + deferred seeds; 3: + zone vertices


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
	o.i64(deferred_seeds.size());
	for (size_t i=0; i<deferred_seeds.size(); i++) { o.i64(deferred_seeds[i].size()); for (size_t j=0; j<deferred_seeds[i].size(); j++) o.pod(deferred_seeds[i][j]); }
	o.podvec(zone_pts);

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
static std::vector<Point3> g_merge_zone;   // vertices near the cut sides of all merged slabs
static std::vector< std::vector<Point3> > g_merge_deferred;   // seeds deferred by the slab runs
static std::vector<int> g_merge_conn_state;   // for each boundary connector: index of the state file it came from
static int g_merge_state = 0;

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
		if (merge) for (long long i=0; i<n; i++) g_merge_conn_state.push_back(g_merge_state);
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
	{
		long long nd = in.i64();
		for (long long i=0; i<nd; i++) {
			long long m = in.i64(); std::vector<Point3> l(m);
			for (long long j=0; j<m; j++) in.pod(l[j]);
			if (merge) g_merge_deferred.push_back(l); else deferred_seeds.push_back(l);
		}
	}
	{ std::vector<Point3> z; in.podvec(z); if (merge) g_merge_zone.insert(g_merge_zone.end(), z.begin(), z.end()); else zone_pts = z; }
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
		if (!one.empty()) { LoadCheckpointImpl(one.c_str(), true); g_merge_state++; }
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
	if (!getenv("AFRONT_NO_STITCH")) StitchConnectors(g_merge_conn_state);
	{   // seeds that the slab runs skipped because they lay in a cut strip are checked at the end of the run (AuditDeferredSeeds):
		// a seed whose surface point has no vertex nearby once everything is meshed belonged to a component nobody covered
		merge_deferred = g_merge_deferred;
		merge_audit = true;
		for (size_t i=0; i<g_merge_elems.size(); i++) audit_pts.push_back(g_merge_elems[i]->position);
		audit_pts.insert(audit_pts.end(), g_merge_zone.begin(), g_merge_zone.end());
		track_vertices = true;
		fprintf(stderr, "\n[STITCH] %zu deferred seeds from the slab runs (checked at the end of the run)\n", merge_deferred.size());
	}

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


// ---------------------------------------------------------------------------------------------------------------------
// Parallel slabs, phase 2: a boundary curve on the volume's outer faces that crosses the cut between two slabs ends in a
// connector (pseudo-edge c, positioned at the end of the curve's piece) in one slab, and starts at the same point in the other
// slab (q0 = the element after that slab's connector d). In a sequential run the second piece would be joined to the first
// one's connector by InsertBoundary; here we splice the two fronts at that point:
//      before:  F_a: ... u c | a ...          F_b: ... d | q0 q1 ...        (| = chord edges c->a and d->q0)
//      after :  ... u c q1 ... d a ...        (c becomes a real growing edge, d keeps its chord, now to a)
// If both belong to the same front, the splice cuts it in two: [a .. d] closed by d's chord, and [q1 .. u c].
// The vertex at the junction exists twice (c's and q0's); q0 is removed from the front and the pair is written to the
// -weld_file so that the driver can merge the two vertices in the final mesh.
extern char *weld_file;

// The junction point of a join exists as two vertices (c's from one slab, q0's from the other; the driver welds them in the
// output). Make the algorithm see them as one: the kept vertex inherits the neighbors of the dropped one, and the neighbors that
// are still on a front are told that their neighbor "drop_id" is now "keep_id". Without this EdgeExistsAlready cannot see the
// edges of the other slab's fan around the junction, and an ear cut can put a second triangle on an existing edge.
static std::unordered_map<int,int> g_id2slot;   // vertex id -> vertinfo slot, for the vertices on fronts

void Triangulator::UnifyVertexRings(int keep_slot, int keep_id, int drop_slot, int drop_id) {
	static const bool off = getenv("AFRONT_NO_UNIFY") != NULL;
	if (off) return;
	std::vector<int> dr = vertinfo[drop_slot].ring;
	std::vector<int> &kr = vertinfo[keep_slot].ring;
	for (size_t i=0; i<dr.size(); i++) {
		int n = dr[i];
		if (n == keep_id || n == drop_id) continue;
		if (std::find(kr.begin(), kr.end(), n) == kr.end()) kr.push_back(n);
		std::unordered_map<int,int>::iterator it = g_id2slot.find(n);
		if (it != g_id2slot.end()) {
			std::vector<int> &nr = vertinfo[it->second].ring;
			for (size_t j=0; j<nr.size(); j++) if (nr[j] == drop_id) nr[j] = keep_id;
		}
	}
	// the dropped vertex may also appear in rings it does not list itself
	g_id2slot.erase(drop_id);
}

void Triangulator::JoinConnectors(feli c, feli d) {
	feli a = Front::NextElement(c), q0 = Front::NextElement(d), q1 = Front::NextElement(q0);
	Front *fa = c->front, *fb = d->front;
	int c_vert = c->vertindex, q0_vert = q0->vertindex;
	controller.MarkVertexNoFlip(c_vert);    // before anything is finalized: no edge flips around the two coincident vertices
	controller.MarkVertexNoFlip(q0_vert);

	UnifyVertexRings(c->vertindex_l, c->vertindex, q0->vertindex_l, q0->vertindex);

	KDRemove(c);
	KDRemove(d);

	if (fa != fb) {
		std::vector<feli> seg;                      // q1 .. d in cycle order of F_b
		for (feli e = q1; ; e = Front::NextElement(e)) { seg.push_back(e); if (e == d) break; }
		for (size_t i=0; i<seg.size(); i++) Front::MoveBefore(a, seg[i]);
	} else {
		std::vector<feli> seg;                      // a .. d
		for (feli e = a; ; e = Front::NextElement(e)) { seg.push_back(e); if (e == d) break; }
		Front *nf = new Front();
		for (size_t i=0; i<seg.size(); i++) Front::MoveToEnd(nf, seg[i]);
		fronts.push_back(nf);
	}

	// q0 is now alone (different fronts) or sits between c and q1 (same front): drop it; its vertex stays in the mesh
	// (RemoveFrontElement finalizes it) and is welded to c's vertex by the driver
	RemoveFrontElement(q0);

	// c is a real growing edge from now on, like the connector that InsertBoundary converts
	c->flags = FRONT_FLAG_FEATURE | FRONT_FLAG_SEED | FRONT_FLAG_EDGEGROW;
	c->priority.first = PRIORITY_GROW_EDGE | controller.GetBlockForPoint(c->position);
	c->priority.second = 1e34;
	heap.update_position(c->heap_position);
	KDInsert(c);
	KDInsert(d);
	RequestProjection(c);
	PrioritizeEdgeGrow(c);
	AddNeighborsToRings(*c, *Front::NextElement(c));

	if (weld_file) {
		if (FILE *wf = fopen(weld_file, "a")) { fprintf(wf, "%d %d\n", c_vert, q0_vert); fclose(wf); }
	}
}

// A piece whose chord has zero length (its end point is its own start point, after joins across slabs) is a closed loop:
// drop the chord element c; the edge before it now ends at the start vertex a (c's vertex is a duplicate of a's: welded).
void Triangulator::CloseConnectorLoop(feli c) {
	feli a = Front::NextElement(c), u = Front::PrevElement(c);
	int c_vert = c->vertindex, a_vert = a->vertindex;
	controller.MarkVertexNoFlip(a_vert);
	controller.MarkVertexNoFlip(c_vert);
	UnifyVertexRings(a->vertindex_l, a->vertindex, c->vertindex_l, c->vertindex);
	KDRemove(u);
	RemoveFrontElement(c);      // also forgets c in boundary_connectors (no blocks left)
	KDInsert(u);
	RequestProjection(u);
	PrioritizeEdgeGrow(u);
	AddNeighborsToRings(*u, *a);
	extern char *weld_file;
	if (weld_file) {
		if (FILE *wf = fopen(weld_file, "a")) { fprintf(wf, "%d %d\n", a_vert, c_vert); fclose(wf); }
	}
}

void Triangulator::StitchConnectors(const std::vector<int> &state_of_in) {
	g_id2slot.clear();
	for (size_t fi=0; fi<fronts.size(); fi++) {
		if (fronts[fi]->empty()) continue;
		feli first = fronts[fi]->FirstElement(), e = first;
		do { g_id2slot[e->vertindex] = e->vertindex_l; e = Front::NextElement(e); } while (e != first);
	}
	std::vector<int> state_of = state_of_in;
	if (getenv("AFRONT_DEBUG_STITCH"))
		for (size_t i=0; i<boundary_connectors.size(); i++) {
			feli c = boundary_connectors[i]; feli q0 = Front::NextElement(c);
			fprintf(stderr, "[STITCH] connector %zu (state %d): end (%.6f %.6f %.6f) start (%.6f %.6f %.6f)\n", i, state_of[i],
					(double)c->position[0], (double)c->position[1], (double)c->position[2], (double)q0->position[0], (double)q0->position[1], (double)q0->position[2]);
		}
	int njoined = 0, nclosed = 0;
	bool again = true;
	while (again) {
		again = false;
		for (size_t i=0; i<boundary_connectors.size() && !again; i++) {
			feli c = boundary_connectors[i];
			{
				feli a0 = Front::NextElement(c);
				if (a0 != c && Front::PrevElement(c) != a0 && !(a0->flags & FRONT_FLAG_CONNECTOR) &&
					Point3::distance(c->position, a0->position) <= 1e-6 && c->normal.dot(a0->normal) > 0.5) {
					CloseConnectorLoop(c);      // removes c from boundary_connectors
					state_of.erase(state_of.begin() + i);
					nclosed++;
					again = true;
					break;
				}
			}
			for (size_t j=0; j<boundary_connectors.size(); j++) {
				if (i == j) continue;   // (pairs from the same state can be legitimate: after a join, the chord of a piece points at the start of another state's piece)
				feli d = boundary_connectors[j];
				feli q0 = Front::NextElement(d);
				if (q0->flags & FRONT_FLAG_CONNECTOR) continue;
				if (Point3::distance(c->position, q0->position) > 1e-6 || c->normal.dot(q0->normal) <= 0.5) continue;
				JoinConnectors(c, d);
				boundary_connectors.erase(boundary_connectors.begin() + i);
				state_of.erase(state_of.begin() + i);
				njoined++;
				again = true;
				break;
			}
		}
	}
	for (size_t i=0; i<boundary_connectors.size(); i++) {   // why are these unmatched? nearest start point of another state's piece
		feli c = boundary_connectors[i];
		real_type best = 1e30; real_type bdot = 0; int bj = -1;
		for (size_t j=0; j<boundary_connectors.size(); j++) {
			if (i == j) continue;
			feli q0 = Front::NextElement(boundary_connectors[j]);
			real_type dist = Point3::distance(c->position, q0->position);
			if (dist < best) { best = dist; bdot = c->normal.dot(q0->normal); bj = (int)j; }
		}
		fprintf(stderr, "[STITCH]   unmatched connector (state %d) at (%g %g %g): nearest other piece start %.3g away (normal dot %.2f, state %d)\n",
				state_of[i], (double)c->position[0], (double)c->position[1], (double)c->position[2], (double)best, (double)bdot, bj >= 0 ? state_of[bj] : -1);
	}
	fprintf(stderr, "\n[STITCH] joined %d connector pairs between the merged states, closed %d loops; %zu connectors left unmatched\n", njoined, nclosed, boundary_connectors.size());
}


// Parallel slabs, end of the merge run: start the components that nobody covered. A deferred seed (see TryAddSeed) is covered if a
// vertex of the merge run (or a front vertex it started from) lies within 1.5 local steps of the seed's point on the surface.
int Triangulator::RemoveCoveredSeeds(vector< vector<Point3> > &seeds) {
	if (seeds.empty()) return 0;
	extern real_type get_max_step();
	const real_type cell = 2 * get_max_step();
	auto key = [&](const Point3 &p) { return ((long long)floor(p[0]/cell) * 73856093LL) ^ ((long long)floor(p[1]/cell) * 19349663LL) ^ ((long long)floor(p[2]/cell) * 83492791LL); };
	std::unordered_map<long long, std::vector<int> > cells;
	for (size_t i=0; i<audit_pts.size(); i++) cells[key(audit_pts[i])].push_back((int)i);
	std::vector< std::vector<Point3> > keep; int removed = 0;
	for (size_t i=0; i<seeds.size(); i++) {
		bool covered = false;
		for (size_t j=0; j<seeds[i].size(); j++) {
			Point3 p1; Vector3 n1;
			if (controller.ProjectPoint(seeds[i][j], p1, n1) != PROJECT_SUCCESS) continue;
			real_type R = (real_type)1.5 * controller.MaxStepLength(p1);
			for (int dx=-1; dx<=1 && !covered; dx++) for (int dy=-1; dy<=1 && !covered; dy++) for (int dz=-1; dz<=1 && !covered; dz++) {
				Point3 q(p1[0]+dx*cell, p1[1]+dy*cell, p1[2]+dz*cell);
				std::unordered_map<long long, std::vector<int> >::iterator it = cells.find(key(q));
				if (it == cells.end()) continue;
				for (size_t m=0; m<it->second.size(); m++) if (Point3::distance(audit_pts[it->second[m]], p1) < R) { covered = true; break; }
			}
			break;   // the first projectable candidate decides
		}
		if (covered) removed++; else keep.push_back(seeds[i]);
	}
	seeds.swap(keep);
	return removed;
}

// Parallel slabs, end of the merge run: start the components that nobody covered. A deferred seed (see TryAddSeed) is covered if a
// vertex of the merge run, a front vertex it started from, or a vertex near the cut sides of any slab lies within 1.5 local steps
// of the seed's point on the surface.
bool Triangulator::AuditDeferredSeeds() {
	if (merge_deferred.empty()) return false;
	int ncovered = RemoveCoveredSeeds(merge_deferred);
	int nstarted = 0;
	std::vector< std::vector<Point3> > rest;
	for (size_t i=0; i<merge_deferred.size(); i++) {
		if (TryAddSeed(merge_deferred[i], NULL, 0)) nstarted++;
		else rest.push_back(merge_deferred[i]);
	}
	merge_deferred.clear();
	fprintf(stderr, "\n[STITCH] audit of deferred seeds: %d covered, %d started (%zu could not be started)\n", ncovered, nstarted, rest.size());
	return nstarted > 0;
}
