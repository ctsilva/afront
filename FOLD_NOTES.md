# Folding the 2009 (SourceForge) afront into this repository — trial, 2026-10-24

## Why it is a clean fold

The SourceForge CVS repository's revision 1.1 of every file is dated 2007-01-09 22:10,
the same day the VGC-internal CVS history of afront ends. John Schreiner imported the
VGC tree into SourceForge, stripped the Reeb-graph and robust-MLS research code, added
GPL headers, and did all further work (interval arithmetic, B-spline bounds, out-of-core
volumes, streaming output, multimaterial) there through 2009-03-24. So this repository
(github ctsilva/afront, imported from the VGC CVS) and the 2009 tree have a common
ancestor and no divergent edits to the same lines, apart from the 2025 macOS patches,
which the revived 2009 tree already carries.

Substantive 2007 -> 2009 changes outside src/: viewer.cpp (+~480 lines: unsharp
masking, key sending, screenshots), ThreadLib CriticalSection (trylock loop),
mlslib CProjection (Adamson normal fix, gradient-step test), gtb kdtree.h (compact
leaves), gtb stlext.h (compact()), gtb triangle_mesh (ReadSMA), commandline const-
correctness. Every other gtb/lib difference is the 26-line GPL header.

## What this commit does

- Replaces src/, gtb/, lib/{mlslib,ThreadLib,viewer,stat} with the revived 2009 tree
  (~/src/afront-2009-cvs/work, commit 1b3bb8d), adds lib/sm (Isenburg streaming
  meshes), sm_viewer/, scripts/ (blocksize_test.sh, volsize_test.sh, meminfo.py,
  gfdistrib.py, hafront.sh).
- Keeps the VGC-only code the SourceForge release dropped: lib/rmlslib, src/rg.*,
  src/generaldef.*, src/output_controller_reeb.*. rmlslib and a `reeb` static library
  still build; neither is linked into the behaviour of `afront` (NO_RMLS is defined in
  src/triangulate_mls.cpp, as it was in 2007; main.cpp no longer has the -reeb hooks).
- New top-level CMakeLists.txt (from the 2009 revival). real_type is now double
  (-DREAL_IS_DOUBLE); the 2009 multimaterial code does not compile as float
  (12 errors in triangulate_iso.cpp, all TrivariateSpline<double> call sites).
- Ports the one 2007-revival fix the 2009 tree lacked: the isolated-vertex check in
  gtb/graphics/triangle_mesh.cpp VertexVertexIteratorI.

## Regression results (Apple M3 Max, 8 threads)

| test | 2007 repo binary | folded binary |
|---|---|---|
| data/bunny.off `-rho 0.5 -tri` | 21.1 s wall, 17675 V / 35106 F | 2.5 s wall, 17726 V / 35187 F (identical to 2009 work binary) |
| data/bunny-points-with-normals.obj, MLS path (`-tri_smoothmls` / `-tri_mls`) | degenerate: 4 V / 4 F (the 2007 build fails on this input) | 6.8 s wall, 31261 V / 62518 F, closed genus 0 |
| torus64 in-core `-tri 0.5 bspline` | n/a | V-E+F = 0 |
| torus64 out-of-core, 2x2x2 blocks | n/a | V-E+F = 0 |

The 9x speedup on bunny comes from the 2009 ThreadLib/triangulator changes
(the 2007 run spends 42 s of system time in lock contention).

## Not done / decisions left

- GUI: the GLUT-on-main-thread port was redone on the 2009 viewer in the next commit
  (done 2026-10-24, verified on bunny.off).
- The [TIMING] logging was brought back in the same commit. The 2007 repo's
  svdcmp.inl tolerance/iteration patch was not (no SVD convergence failures with
  double precision on the test data).
- Command renames: tri_smoothmls -> tri_mls, volsmooth -> vol_smooth; save_vol,
  reeb, reeb_eps are gone; options load_field/save_field/proj_tol/saliency/
  grid_intersect_overestimate are gone. New: save_ooc, make_gf, multi_material,
  project_dense, segmentation, compute_pointset_normals, and the -prefetch_*,
  -reuse_*, -noedgeflip, -value_tol/-step_tol/-max_iter options.
- src/test_glut*.cpp no longer compile (SimpleViewer gained pure virtuals); left out
  of the build.
- README.md, CLAUDE.md, docs/ARCHITECTURE.md were updated for the 2009 code (same day).
