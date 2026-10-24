# Plan: integrate the 2009 afront into this repository

Written 2026-10-24. Branch `fold-2009`. Status is updated as steps finish.

## Background

This repository is the January 2007 VGC-internal afront (github ctsilva/afront),
patched in October 2025 to build on macOS. John Schreiner's later work, the
interval-arithmetic guidance field, out-of-core bricked volumes, streaming output
and multimaterial extraction behind his 2009 dissertation and the Vis 2009
submission "Accurate and Robust Out-of-Core Isosurface Extraction", only existed in
the SourceForge CVS repository, recovered on 2026-10-24 into
`~/src/afront-2009-cvs` and revived there (see its README.md).

The SourceForge repository's first revision of every file is dated 2007-01-09, the
day the VGC CVS history of afront ends, so the two trees share an ancestor and the
fold is a replacement of sources rather than a merge of divergent edits. Measured
on this machine the 2009 code is 9x faster on `data/bunny.off` (2.5 s vs 21 s) and
succeeds on the bunny point cloud where the 2007 build produced a degenerate
4-vertex mesh. Details: `FOLD_NOTES.md`.

## Steps

1. [done] Fold the revived 2009 tree into this layout (`src/`, `gtb/`, `lib/`), keep
   the VGC-only rmlslib and Reeb-graph code, new CMake build, real_type = double,
   port the 2007 isolated-vertex fix. Verified: bunny mesh, bunny point cloud,
   torus volume in-core and out-of-core. Commit "Fold in the 2009 SourceForge afront".
2. [done] Fetch the branch into this repository and rebuild from scratch.
3. [done] Re-port the macOS GUI fix (GLUT must run on the main thread) onto the 2009
   viewer: `lib/viewer/viewer.cpp` `Start()` initializes GLUT synchronously and
   `Go()` only runs the event loop; `src/main.cpp` runs the command parsing in a
   worker thread while the main thread runs `gui->Go()`. Also the Command-key
   dolly modifier. Verify the GUI opens and triangulates `data/bunny.off`.
4. [done] Bring back the `[TIMING]` log lines from the 2007 revival (curvature,
   trimming, triangulation) in `triangulate_mesh.cpp`, `guidance.cpp`,
   `triangulator.cpp`. The 2007 SVD tolerance patch is deliberately not re-applied:
   with double precision no convergence failures were seen; revisit if they appear.
5. [done] Update documentation: `README.md`, `CLAUDE.md`, `docs/ARCHITECTURE.md`
   describe the 2007 code. Record the new build flags, the 2009 fixes, the command
   renames (`tri_smoothmls` -> `tri_mls`, `volsmooth` -> `vol_smooth`; `reeb`,
   `save_vol` removed) and the out-of-core workflow (`-save_ooc`, `.nooc`,
   `-prefetch_*`, `-reuse_guidance`).
   Step 3 verified by running `./afront ../data/bunny.off -rho 0.5 -tri` for 20 s: the
   window opens, the bunny renders, the worker thread triangulates ("waiting for key"
   stepping mode), no main-thread GLUT abort.
6. [done] Merge `fold-2009` into `main` (local only, no push).

## Kept as-is, decided

- rmlslib and the Reeb-graph files stay in the tree and still compile, although
  nothing links them into afront's behaviour. They are the only surviving copy of
  that VGC research code.
- `src/test_glut*.cpp` stay as reference but are not built (the 2009 viewer added
  pure virtuals).

## Open after this plan

- Rerun the paper's experiments on real data (Richtmyer-Meshkov from LLNL, a 512^3
  CT scan) for a resubmission; see `~/src/afront-2009-cvs/README.md`.
- Every 512^3 synthetic run closes at least one hole with the failsafe; understand why
  before claiming robustness.
- `.ply` output is not supported; use `.m`, `.obj` or the streaming `.sma/.smb/.smc`.
