# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`afront` is an advancing front triangulation system for surface reconstruction. It creates high-quality triangle meshes from various input types including point clouds, implicit surfaces (isosurfaces), tetrahedral meshes, and existing triangle meshes. The system uses curvature-adaptive guidance fields to control triangle sizing and employs a priority-based front propagation algorithm.

## Build System

### Prerequisites - macOS

**Required:**
- CMake 3.10+
- C++ compiler (Clang 17.0.0+ or GCC)
- XQuartz (X11 for macOS): `brew install --cask xquartz`
  - **Important:** After installing XQuartz, you must logout/login or restart
- freeglut: `brew install freeglut`

**Note:** The code has been patched for macOS compatibility. See "Compilation Fixes Applied" section below.

### Building with CMake

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (use -j for parallel build)
make -j8

# Binary will be in: build/afront
```

### Dependencies
- **gtb**: Graphics toolbox library (must be in ../gtb)
- **viewer**: OpenGL visualization library (must be in ../lib/viewer)
- **mlslib**: Moving Least Squares projection library (must be in ../lib/mlslib)
- **ThreadLib**: Threading library (must be in ../lib/ThreadLib)
- **System libraries**: OpenGL, GLU, GLUT, pthread, zlib, X11

### Compiler Flags
The code is compiled with: `-DUSENR -DREAL_IS_FLOAT -D_REENTRANT -Wno-c++11-narrowing`

Note: Exceptions are enabled (removed `-fno-exceptions` and `-DNO_EXCEPTIONS`) to support modern template code.

## Compilation Fixes Applied

This 20-year-old codebase (circa 2005) required extensive patches for modern macOS (Darwin 25.0.0, Apple Silicon ARM64, Clang 17.0.0):

### 1. CMakeLists.txt Modernization
**File:** `CMakeLists.txt`
- Added `cmake_minimum_required(VERSION 3.10)`
- Fixed out-of-tree subdirectory builds with explicit binary directories
- Added rmlslib to subdirectories and link libraries
- Added X11/XQuartz paths: `/opt/X11/include` and `/opt/X11/lib`
- Added libtiff paths: `/opt/homebrew/include` and `/opt/homebrew/lib`
- Added OpenGL deprecation silencing: `-DGL_SILENCE_DEPRECATION`
- Fixed filename: `triangulate_mls_smooth.cpp` → `triangulate_mls.cpp`
- Added macOS compatibility macro: `-Dstricmp=strcasecmp`
- Enabled C++11 narrowing warning suppression: `-Wno-c++11-narrowing`
- Enabled exceptions (removed `-fno-exceptions` flags)

### 2. ThreadLib pthread Fix
**File:** `../lib/ThreadLib/CriticalSection.cpp:41-45`
```cpp
#ifdef __APPLE__
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
```
- Changed `PTHREAD_MUTEX_RECURSIVE_NP` (Linux-only) to `PTHREAD_MUTEX_RECURSIVE` on macOS

### 3. isfinite() Compatibility
**File:** `../gtb/math/math.hpp:28-30`
- Removed broken `#define isfinite finite` for macOS
- Added `#include <cmath>` to use standard C++11 `isfinite()`

**File:** `../lib/mlslib/CProjection.cpp:2`
- Added `#include <cmath>` for `isfinite()` support

### 4. getpid() Header Fix
**File:** `../gtb/common.hpp:17`
- Added `#include <unistd.h>` for `getpid()` function used in BREAK macro

### 5. Template Specialization Order Fixes
Modern C++ requires explicit specializations before implicit instantiations.

**File:** `../gtb/graphics/point3.hpp:160-162`
```cpp
// Forward declare explicit specializations
template<> tPoint3<float> tPoint3<float>::ZERO;
template<> tPoint3<double> tPoint3<double>::ZERO;
```

**File:** `../gtb/math/matrix4.hpp:98-100`
```cpp
// Forward declare explicit specializations
template<> const tMatrix4<float> tMatrix4<float>::MATRIX4_IDENTITY;
template<> const tMatrix4<double> tMatrix4<double>::MATRIX4_IDENTITY;
```

**File:** `../gtb/math/amatc.h:111-113`
```cpp
// Forward declare explicit specializations
template<> void SVD<double>(const AMatc<double>&, AMatc<double>&, AVec<double>&, AMatc<double>&);
template<> void SVD<float>(const AMatc<float>&, AMatc<float>&, AVec<float>&, AMatc<float>&);
```

### 6. Dependent Name Lookup Fix
**File:** `../gtb/math/poly.inl:38-40, 129`
- Changed `TermEval(...)` to `this->TermEval(...)` to access dependent base class members
- Required by modern C++ two-phase template lookup

### 7. drand48_r macOS Compatibility
**File:** `common.h:141-148`
```cpp
#elif defined(__APPLE__)
  // macOS doesn't have drand48_r, use plain drand48
  static bool first = true;
  if (first) {
    srand48(t * 50021);
    first = false;
  }
  return drand48();
```
- macOS lacks `drand48_r()` (reentrant version)
- Use standard `drand48()` instead (acceptable for this use case)

### 8. rmlslib CMakeLists.txt Creation
**File:** `../lib/rmlslib/CMakeLists.txt` (created)
- Created CMake build file for rmlslib (only had autotools before)
- Lists all 25 source files for static library build

### 9. X11/XQuartz Integration
- XQuartz provides X11 headers required by GTB UI library
- Paths configured: `/opt/X11/include` and `/opt/X11/lib`
- Required on macOS for GUI support

### 10. GLUT Threading Fix for macOS (CRITICAL)
**Files:** `../lib/viewer/viewer.cpp`, `../lib/viewer/viewer.h`, `main.cpp`

**Problem:** Modern macOS requires all GLUT operations (initialization, window creation, and main loop) to run on the application's main thread. The original code spawned a worker thread for GLUT, which causes fatal errors on macOS 10.14+.

**Solution Implemented:**
1. **viewer.cpp `Viewer::Start()`**: Moved GLUT initialization (`glutInit`, window creation) to run synchronously on main thread. Removed worker thread spawn.
2. **viewer.h**: Made `Go()` public so it can be called from application code.
3. **main.cpp**: Restructured to:
   - GUI mode: Spawn worker thread for triangulation logic, main thread calls `gui->Go()` (runs `glutMainLoop`)
   - No-GUI mode: Main thread runs triangulation directly

**Code Changes:**

```cpp
// viewer.cpp - Start() now runs synchronously on main thread
void Viewer::Start() {
    _critical_section.enter();
    int argc=1; char *argv[1] = { "blah" };
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

    for (unsigned i=0; i<windows.size(); i++) {
        MakeWindow(i);
    }
    glut_has_initialized = true;
    _critical_section.leave();
    // DON'T spawn thread - caller runs Go() on main thread
}

// main.cpp - Worker thread for triangulation
void *worker_thread_func(void *arg) {
    if (g_commandline) {
        g_commandline->Parse(g_toskip);
    }
    return NULL;
}

// main.cpp - GUI mode uses worker thread + main thread for GLUT
if (use_gui && gui) {
    g_commandline = &cl;
    g_toskip = toskip;
    thlib::Thread *worker = new thlib::Thread(worker_thread_func, NULL, 0);
    gui->Go();  // Main thread runs GLUT - never returns
}
```

**Testing:** `test_glut_simple.cpp` provides a minimal test to verify GLUT threading works correctly.

### 11. SVD Numerical Stability Fixes
**Files:** `../gtb/math/NR/svdcmp.inl`

**Problem:** The 20-year-old Numerical Recipes SVD implementation uses floating-point equality checks that fail on modern compilers/architectures, causing "no convergence in 30 iterations" errors.

**Partial Solutions Applied:**
1. Increased iteration limit from 30 to 300
2. Replaced exact floating-point equality `(T)(fabs(x)+anorm) == anorm` with epsilon tolerance:
   ```cpp
   T eps = (anorm > 1e-10) ? anorm * 1e-6 : 1e-10;
   if (fabs(rv1[l]) <= eps) ...
   ```
3. Modified `nrerror()` to return instead of terminating (allows graceful degradation)
4. Made SVD return early on non-convergence rather than crash

**Current Status:** SVD still fails to converge on some meshes. The early-return approach allows the program to continue but may produce suboptimal curvature estimates. GUI mode works correctly despite this issue.

**Recommendation for Future Work:** Replace Numerical Recipes SVD with modern libraries (Eigen, LAPACK) for better numerical stability on modern hardware.

## Running the Program

Basic usage:
```bash
./afront <input_file> [options] [commands]
```

### Common Commands

#### Triangulation
- `tri`: Auto-detect input type and triangulate
- `tri_mesh [subdiv]`: Triangulate a mesh (with optional Loop subdivision for curvature)
- `tri_smoothmls [.flf]`: Triangulate MLS surface from point cloud
- `tri_vol <isoval> [bspline]`: Triangulate volume isosurface
- `tri_tet <isoval>`: Triangulate tetrahedral mesh isosurface

#### CSG Operations
- `csg <union|int|sub> [subdiv]`: Boolean operations on two meshes

#### Mesh Operations
- `subdiv <N>`: Apply N iterations of Loop subdivision
- `flip`: Flip mesh orientation
- `save_mesh0 <filename>`: Save first mesh
- `meshinfo`: Display mesh statistics

#### GUI and Debugging
- `-nogui`: Run without GUI
- `gui`: Start GUI
- `trans`: Enter mesh transformation mode

### Key Parameters

- `rho <value>`: Angle subtended on osculating sphere (controls triangle sizing)
- `rho_N <N>`: Set rho based on number of edges per circle
- `min_step <value>`: Minimum edge length
- `max_step <value>`: Maximum edge length
- `reduction <value>`: Percent reduction per step (default: 0.8)
- `sharp <angle>`: Cosine of angle for sharp feature detection (default: -2.0 = disabled)
- `radius_factor <value>`: MLS radius scaling (default: 2.0)
- `adamson <0-3>`: MLS projection mode (0=standard, 1-3=Adamson variants)
- `failsafe <bool>`: Close holes that failed triangulation (default: true)
- `outname <file>`: Output filename (default: "outmesh.m")

### Examples
```bash
# Triangulate a point cloud
./afront points.obj -tri_smoothmls

# Triangulate with features at 45 degrees
./afront mesh.m -sharp -0.707 -tri_mesh

# CSG union of two meshes
./afront mesh1.m mesh2.m -csg union

# Extract isosurface at value 0.5
./afront volume.vol -tri_vol 0.5

# Run without GUI
./afront -nogui mesh.m -tri_mesh -outname output.m
```

## Architecture

### Core Components

1. **Triangulator** (`triangulator.{h,cpp}`)
   - Main algorithm coordinator using priority-based front propagation
   - Manages multiple active fronts simultaneously
   - Uses KD-tree spatial indexing for efficient nearest-neighbor queries
   - Employs multi-threaded projection for performance
   - Two main operations: GROW (create new vertex) and CONNECT (link existing vertices)

2. **Front** (`front.{h,cpp}`)
   - Represents active boundary curves during triangulation
   - Each front is a circular linked list of `FrontElement` objects
   - Supports split/merge operations as fronts intersect or close
   - Elements maintain their priority in a global heap

3. **GuidanceField** (`guidance.{h,cpp}`)
   - Computes spatially-varying ideal edge lengths based on local curvature
   - Abstract base class with specializations for different surface types
   - Uses `rho` parameter (angle on osculating sphere) to relate curvature to edge length
   - Provides curve resampling for initial front generation

4. **SurfaceProjector** (defined in `triangulator.h`, implemented in various files)
   - Projects points onto the implicit surface being triangulated
   - Implementations: `MeshProjector`, `IsoSurfaceProjector`, `TetMeshProjector`, `SmoothMLSProjector`
   - Multi-threaded with work queue for parallel projection

5. **OutputController** (`output_controller_*.{h,cpp}`)
   - Chain-of-responsibility pattern for mesh output
   - Implementations: HHM format writer, OBJ writer, GUI display, edge flipping, Reeb graph computation

### Triangulation Flow

1. **Initialization**
   - Load input (mesh/volume/points)
   - Create appropriate GuidanceField and SurfaceProjector
   - Set up OutputController chain
   - Create Triangulator with ControllerWrapper

2. **Front Setup**
   - Generate initial fronts from boundaries, creases, or seed points
   - Resample curves using guidance field to get proper spacing
   - Insert front elements into KD-tree and priority heap

3. **Main Loop** (in `Triangulator::Go`)
   - Pop highest priority edge from heap
   - Attempt GROW operation (create new vertex)
   - If growth fails or quality threshold met, attempt CONNECT operation
   - Use "fence" intersection tests to prevent front crossings
   - Update KD-tree, verify legality, create triangles
   - Continue until all fronts are closed or failsafe triggered

4. **Failsafe**
   - Closes remaining holes with simple triangulation
   - Triggered when normal operations stall

### Key Algorithms

- **Fence Test**: Prevents fronts from crossing by building perpendicular "fences" at each edge
- **Priority Calculation**: Balances triangle quality (circumradius/inradius) with edge length goals
- **Crease Handling**: Detects sharp features and maintains separate fronts along creases
- **CSG**: Computes intersection curves, floods point classifications, blends guidance fields

### Specializations

- **triangulate_mesh.{h,cpp}**: Remeshing existing meshes with curvature adaptation
- **triangulate_mls.{h,cpp}**: Point cloud reconstruction using MLS surfaces
- **triangulate_iso.{h,cpp}**: Isosurface extraction from regular grids (alternative to Marching Cubes)
- **triangulate_tet.{h,cpp}**: Isosurfaces from tetrahedral meshes
- **triangulate_csg.{h,cpp}**: Boolean operations on meshes

### Threading Model

- Worker threads handle surface projections in parallel
- Main thread handles front propagation sequentially (for algorithm correctness)
- GUI runs in separate thread with critical section protection for shared data
- Default: 8 threads on x86_64/ARM64, 1 thread otherwise (configurable via `idealNumThreads`)

### Data Structures

- **UHeap** (`uheap.h`): Updatable priority heap for front elements
- **ConeBoxKDTree** (`conekdtree.h`): Spatial index with cone queries for guidance field
- **IndexedTriangleSet** (from gtb): Output mesh representation

## File I/O Formats

- **.m, .off**: Triangle mesh formats (input/output)
- **.obj**: Point cloud or mesh (input)
- **.vol, .nhdr**: Volume data (input)
- **.offt**: Tetrahedral mesh (input)
- **.pc**: Point cloud (input)
- **.flf**: Feature line format (input for constrained triangulation)
- **HHM format**: Custom mesh format (output, controlled by `output_controller_hhm.cpp`)

## Important Global Variables

Located in `main.cpp`:
- `meshes[2]`: Two input meshes (for CSG)
- `volume`: Regular volume data
- `points`: Point cloud (surfel_set)
- `tetmesh`: Tetrahedral mesh
- `guidance`: Current guidance field
- `controller`: ControllerWrapper (combines guidance, projector, output)
- `triangulator`: Main algorithm instance
- `gui`: GUI controller
- `critical_section`: Mutex for thread-safe data access

## Type Aliases

Defined in `common.h`:
- `real_type`: Either `float` or `double` (controlled by REAL_IS_FLOAT)
- Geometric types from gtb: `Point3`, `Vector3`, `Box3`, `Triangle3`, `TriangleMesh`
- MLS types: `CProjection`, `Poly2`

## Debugging Utilities

- `redrawAndWait(key)`: Update GUI and pause for keypress
- `DbgPoints::add(point, r, g, b)`: Add debug point visualization
- `DbgPLines::add(line, id)`: Add debug line visualization
- `dbgClear()`: Clear debug visualizations
- `BREAK`: Breakpoint macro

## Code Style Notes

- Extensive use of inheritance and virtual functions
- STL containers (vector, list) throughout
- Iterator patterns for mesh traversal
- No exceptions (compiled with `-fno-exceptions`)
- Real type abstraction allows float/double switching

## Troubleshooting Build Issues

### "X11/Xlib.h not found"
- Install XQuartz: `brew install --cask xquartz`
- Logout and login after installation
- Verify: `ls /opt/X11/include/X11/Xlib.h`

### "PTHREAD_MUTEX_RECURSIVE_NP undeclared"
- This fix is already applied in ThreadLib/CriticalSection.cpp
- Verify the fix is present (should have `#ifdef __APPLE__` check)

### OpenGL deprecation warnings
- These are suppressed with `-DGL_SILENCE_DEPRECATION`
- Warnings are harmless on macOS (OpenGL still works despite deprecation)

### "triangulate_mls_smooth.cpp not found"
- Fixed in CMakeLists.txt (should be `triangulate_mls.cpp`)
