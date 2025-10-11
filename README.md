# Afront - Advancing Front Triangulation

**High-quality adaptive surface meshing and remeshing**

## Project History

Afront was developed in the mid-2000s and published at Eurographics and TVCG (see details below for the full papers). At some point the code was released on SourceForge, but it disappeared. With the help of Claude, we have compiled it on macOS, and we plan to get it working on Linux and Windows. The code uses lots of legacy things, including ancient OpenGL, GLUT, etc.

## Build Status

✅ **Successfully compiled and running on macOS (Apple Silicon ARM64)**
- Platform: macOS Darwin 25.0.0
- Compiler: Apple Clang 17.0.0
- Architecture: ARM64
- GUI: Functional (with GLUT threading fixes)
- Tested: Basic mesh remeshing with GUI works

⚠️ **Important Caveats**
- This 20-year-old codebase has been patched to compile on current systems, but **not all functionality has been tested**
- Many features may not work due to legacy library incompatibilities (ancient OpenGL, Numerical Recipes routines, etc.)
- Batch mode (`-nogui`) currently crashes due to numerical instability in the SVD implementation
- Use with caution and expect issues with untested workflows

See [CLAUDE.md](CLAUDE.md) for detailed compilation fixes applied.

## About Afront

Afront is an advancing front triangulation algorithm that can be applied to many different meshing scenarios. It makes use of a guidance field to determine triangle sizing that is adaptive to the curvature of the input surface, but also maintains smooth gradation to prevent poor quality triangles from being created. The prototype implementation can be used to generate meshes from other meshes (remeshing), from volumes defined on regular and irregular grids (isosurface extraction), and from point sets. It can be used to mesh either the entire input surface or just a local region, and can preserve sharp features that are annotated ahead of time.

**Main applications:**
- **Remeshing:** Improve existing triangle meshes with curvature-adaptive sizing
- **Isosurface extraction:** Extract surfaces from volumetric data (regular/irregular grids, tetrahedral meshes)
- **Point cloud reconstruction:** Generate meshes from point sets using Moving Least Squares (MLS) surfaces
- **CSG operations:** Boolean operations (union, intersection, subtraction) on meshes
- **Local region meshing:** Generate meshes for specific surface regions
- **Sharp feature preservation:** Maintains annotated creases and corners

**Developed by:** Visualization and Geometric Computing (VGC) group, University of Utah

---

## Features

### Core Capabilities
- ✅ **Curvature-adaptive triangle sizing** - Automatically adjusts triangle size based on local surface curvature
- ✅ **Multiple input formats** - Triangle meshes (.m, .off, .obj), volumes (.vol, .nhdr), tetrahedral meshes (.offt), point clouds (.pc, .obj)
- ✅ **Sharp feature preservation** - Detects and maintains creases and corners in the output mesh
- ✅ **CSG boolean operations** - Union, intersection, and subtraction of meshes
- ✅ **Interactive GUI** - OpenGL-based visualization for debugging and inspection
- ✅ **Multi-threaded** - Parallel surface projection for performance

### Algorithm Highlights
- **Priority-based front propagation** - Uses a heap to process edges in optimal order
- **Fence-based intersection prevention** - Prevents fronts from crossing each other
- **KD-tree spatial indexing** - Efficient nearest-neighbor queries
- **Failsafe triangulation** - Closes remaining holes when normal operations stall
- **Smooth gradation** - Prevents sudden changes in triangle size

---

## Building from Source

### Prerequisites

**macOS:**
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install dependencies via Homebrew
brew install cmake
brew install --cask xquartz  # X11 for GUI support
brew install freeglut
brew install libtiff

# IMPORTANT: After installing XQuartz, logout and login (or restart)
```

**Linux:**
```bash
# Debian/Ubuntu
sudo apt-get install build-essential cmake
sudo apt-get install libx11-dev libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
sudo apt-get install libtiff-dev zlib1g-dev

# Fedora/RHEL
sudo dnf install gcc-c++ cmake
sudo dnf install libX11-devel mesa-libGL-devel mesa-libGLU-devel freeglut-devel
sudo dnf install libtiff-devel zlib-devel
```

### Compilation

```bash
# Clone or download the source code
cd afront

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (use -j for parallel compilation)
make -j8

# Binary will be created at: build/afront
```

### Verification

```bash
# Check the binary
./afront --help  # Should display usage information
file ./afront    # Should show: Mach-O 64-bit executable (macOS) or ELF 64-bit (Linux)
```

---

## Usage

### Command Line Format

```bash
./afront <-nogui> <-noedgeflip> surface1 <mesh2> [-commands]
```

**Special options** (must be specified first):
- `-nogui` - Run afront in batch mode
- `-noedgeflip` - Bypass the streaming edge flipper (enabled by default for quality improvement)

**Note:** Commands are evaluated in the order they are specified, so set variables before invoking triangulation. The `-tri` command should usually be last.

### Quick Examples

**Remeshing:**
```bash
./afront bunny.off -rho 0.5 -tri
```

**Isosurface Extraction:**
```bash
./afront skull.nhdr -rho 0.8 -tri 150.5 bspline
```

### Input File Formats

| Format | Extension | Type | Description |
|--------|-----------|------|-------------|
| Triangle Mesh | `.m`, `.off` | Mesh | Input mesh for remeshing or CSG |
| Point Cloud | `.obj`, `.pc` | Points | Point set for MLS reconstruction |
| Volume | `.vol`, `.nhdr` | Regular grid | 3D scalar field for isosurface extraction |
| Tetrahedral Mesh | `.offt` | Irregular grid | Tetrahedral mesh (renamed from .off for distinction) |
| Feature Lines | `.flf` | Constraints | Sharp features for constrained triangulation |
| Second Mesh | mesh2 | Mesh | Optional second triangle mesh for CSG operations |

---

## Examples

### 1. Remesh an Existing Triangle Mesh

Improve mesh quality with curvature-adaptive sizing:

```bash
./afront input.m -tri_mesh -outname output.m
```

**With sharp feature detection (45° threshold):**
```bash
./afront input.m -sharp -0.707 -tri_mesh -outname output.m
```

**With Loop subdivision for better curvature estimation:**
```bash
./afront input.m -tri_mesh 2 -outname output.m  # 2 = subdivision level
```

### 2. Reconstruct Surface from Point Cloud

Using Moving Least Squares (MLS) surface:

```bash
./afront points.obj -tri_smoothmls -outname surface.m
```

**With feature line constraints:**
```bash
./afront points.obj features.flf -tri_smoothmls -outname surface.m
```

### 3. Extract Isosurface from Volume

Extract the zero-level isosurface:

```bash
./afront volume.vol -tri_vol 0.0 -outname surface.m
```

**With B-spline smoothing:**
```bash
./afront volume.vol -tri_vol 0.0 bspline -outname surface.m
```

### 4. Boolean Operations (CSG)

**Union:**
```bash
./afront mesh1.m mesh2.m -csg union -outname result.m
```

**Intersection:**
```bash
./afront mesh1.m mesh2.m -csg int -outname result.m
```

**Subtraction:**
```bash
./afront mesh1.m mesh2.m -csg sub -outname result.m
```

### 5. Run Without GUI

For batch processing or server environments:

```bash
./afront -nogui input.m -tri_mesh -outname output.m
```

---

## Parameters

### Triangle Sizing Control

| Parameter | Description | Default | Example |
|-----------|-------------|---------|---------|
| `rho <value>` | Angle subtended on osculating sphere (controls triangle size) | - | `rho 0.3` |
| `rho_N <N>` | Set rho based on N edges per circle | - | `rho_N 20` |
| `min_step <value>` | Minimum edge length | - | `min_step 0.001` |
| `max_step <value>` | Maximum edge length | - | `max_step 0.1` |
| `reduction <value>` | Size reduction per step (0-1) | 0.8 | `reduction 0.9` |

**How rho works:** Smaller values → smaller triangles (more detail)
**Typical values:** 0.1 (very fine) to 1.0 (coarse)

### Feature Detection

| Parameter | Description | Default | Example |
|-----------|-------------|---------|---------|
| `sharp <angle>` | Cosine of angle for sharp features | -2.0 (off) | `sharp -0.707` (45°) |

**Common angles:**
- `sharp -0.707` → 45 degrees
- `sharp -0.866` → 30 degrees
- `sharp 0.0` → 90 degrees

### MLS Surface Parameters

| Parameter | Description | Default | Example |
|-----------|-------------|---------|---------|
| `radius_factor <value>` | MLS radius scaling | 2.0 | `radius_factor 3.0` |
| `adamson <mode>` | MLS projection mode (0-3) | 0 | `adamson 1` |

### Output Control

| Parameter | Description | Default | Example |
|-----------|-------------|---------|---------|
| `outname <file>` | Output filename | `outmesh.m` | `outname result.m` |
| `failsafe <bool>` | Close holes that fail triangulation | true | `failsafe false` |

### Other Options

| Option | Description |
|--------|-------------|
| `-nogui` | Run without graphical interface |
| `gui` | Start GUI mode |
| `meshinfo` | Display mesh statistics |
| `flip` | Flip mesh orientation |
| `subdiv <N>` | Apply N Loop subdivisions |

---

## Complete Command-Line Options Reference

| Option | Parameters | Description |
|--------|------------|-------------|
| `-nogui` | - | Run afront in batch mode without GUI |
| `-noedgeflip` | - | Bypass streaming edge flipper (enabled by default) |
| `-tri` | - | Triangulate entire input mesh |
| `-tri_mesh` | `[subdiv]` | Triangulate mesh (optional Loop subdivision level) |
| `-tri_vol` | `<isovalue> [bspline]` | Extract isosurface from volume |
| `-tri_tet` | `<isovalue>` | Extract isosurface from tetrahedral mesh |
| `-tri_mls` | - | Triangulate MLS surface from points |
| `-tri_smoothmls` | - | Triangulate smooth MLS surface |
| `-tri_region` | `<point>` | Triangulate region around specified point |
| `-csg` | `union\|int\|sub [subdiv]` | Boolean operation on two meshes |
| `-sharp` | `<cos_angle>` | Sharp feature detection threshold |
| `-rho` | `<value>` | Triangle sizing angle (0.2-0.8 typical) |
| `-rho_N` | `<edges>` | Set rho based on N edges per circle |
| `-min_step` | `<length>` | Minimum edge length |
| `-max_step` | `<length>` | Maximum edge length |
| `-reduction` | `<factor>` | Size reduction rate (0.7-0.85 typical) |
| `-radius_factor` | `<value>` | MLS radius scaling factor |
| `-adamson` | `<0-3>` | MLS projection mode |
| `-outname` | `<filename>` | Output mesh filename |
| `-failsafe` | `<0\|1>` | Enable/disable failsafe hole closing |
| `-gui` | - | Enable GUI mode |
| `-meshinfo` | - | Display detailed mesh statistics |
| `-flip` | - | Flip mesh orientation |
| `-subdiv` | `<N>` | Apply N Loop subdivision iterations |
| `-load_field` | `<file>` | Load pre-computed guidance field |
| `-save_field` | `<file>` | Save guidance field to file |

---

## GUI Keyboard Controls

When running with the GUI enabled, the following keyboard shortcuts are available:

### Triangulation Control

| Key | Action |
|-----|--------|
| `p` / `P` | Toggle pause/unpause triangulation (WaitForUser mode) |
| `Spacebar` | Single-step triangulation when paused (triggers redraw) |

### Display Toggles

| Key | Action |
|-----|--------|
| `m` / `M` | Cycle mesh drawing mode (0→1→2) |
| `o` / `O` | Cycle output drawing mode (0→1→2) |
| `b` / `B` | Toggle boundaries display |
| `n` / `N` | Toggle normals display |
| `g` / `G` | Toggle guidance field visualization |
| `f` / `F` | Toggle front display |
| `e` / `E` | Toggle fence display |
| `+` | Increase point size |
| `-` | Decrease point size (minimum 1) |

### View Controls

| Key | Action |
|-----|--------|
| `a` / `A` | View all - reset camera to fit entire scene |
| `i` / `I` | Toggle immediate rendering mode |
| `2` | Toggle orthographic projection |

### Save/Load View

| Key | Action |
|-----|--------|
| `s` / `S` | Save current view to `view.txt` |
| `l` / `L` | Load view from `view.txt` |

### Debug

| Key | Action |
|-----|--------|
| `c` / `C` | Clear debug drawings |

### Help & Exit

| Key | Action |
|-----|--------|
| `?` / `h` / `H` | Show keyboard controls help in terminal |
| `q` / `Q` | Quit application |
| `ESC` | Quit application |

### Mouse Controls

| Action | Control |
|--------|---------|
| Rotate view | Left mouse button + drag |
| Zoom | Shift + Left mouse drag OR Ctrl + Left mouse drag |
| Pan view | Alt + Left mouse drag |

**Note:** Ctrl+Left mouse drag is especially useful on macOS with single-button trackpads.

### Mesh Editing Mode

When in mesh editing mode (activated via `UIEditMesh`):

| Key | Action |
|-----|--------|
| `b` / `B` | Exit mesh editing mode |
| `r` / `R` | Set to rotate mode |
| `t` / `T` | Set to translate mode |
| `s` / `S` | Set to scale mode |
| `x` / `X` | Transform along X-axis (lowercase=negative, uppercase=positive) |
| `y` / `Y` | Transform along Y-axis (lowercase=negative, uppercase=positive) |
| `z` / `Z` | Transform along Z-axis (lowercase=negative, uppercase=positive) |
| `+` / `=` / `F` | Increase edit factor (double it) |
| `-` / `_` / `f` | Decrease edit factor (halve it) |

---

## Complete Command Examples

### High-Quality Remeshing with Feature Preservation

```bash
./afront bunny.m \
  -rho 0.2 \
  -min_step 0.001 \
  -max_step 0.05 \
  -sharp -0.707 \
  -tri_mesh 1 \
  -outname bunny_remeshed.m
```

### Point Cloud to Mesh with Custom MLS Settings

```bash
./afront scan.pc \
  -radius_factor 2.5 \
  -adamson 1 \
  -rho_N 15 \
  -tri_smoothmls \
  -outname scan_surface.m
```

### Isosurface Extraction with Sizing Control

```bash
./afront ct_scan.vol \
  -tri_vol 128.0 \
  -rho 0.3 \
  -reduction 0.85 \
  -outname bone_surface.m
```

### Batch Processing (No GUI)

```bash
#!/bin/bash
for file in data/*.obj; do
  output="results/$(basename $file .obj).m"
  ./afront -nogui $file -tri_smoothmls -outname $output
done
```

---

## Output Formats

Afront can generate output in multiple formats through different output controllers:

- **`.m` format** - Standard mesh format (Meshview compatible)
- **`.off` format** - Object File Format
- **HHM format** - Custom hierarchical mesh format
- **OBJ format** - Wavefront OBJ (via output controller)

Output controllers can be chained to produce multiple outputs simultaneously.

---

## Algorithm Overview

### How It Works

1. **Initialization**
   - Load input surface (mesh/volume/points)
   - Create guidance field from curvature or point spacing
   - Initialize active fronts from boundaries or seed points

2. **Front Propagation Loop**
   - Pop highest priority edge from heap
   - **GROW operation:** Attempt to create new vertex
     - Project tentative point onto surface
     - Check for intersections with existing fronts (fence test)
     - If legal, create new triangle and update fronts
   - **CONNECT operation:** If growth fails, connect to existing vertex
     - Find nearby vertices
     - Check legality and quality
     - Create triangle and merge/split fronts

3. **Termination**
   - Continue until all fronts are closed
   - Failsafe mode closes remaining small holes

### Key Data Structures

- **Priority Heap:** Orders front edges by quality/size metrics
- **KD-Tree:** Spatial indexing for efficient neighbor queries
- **Active Fronts:** Circular linked lists of vertices being triangulated
- **Guidance Field:** Spatially-varying ideal edge length field

---

## Troubleshooting

### Build Issues

**Error: `X11/Xlib.h` not found**
```bash
# Install XQuartz on macOS
brew install --cask xquartz
# Logout and login after installation
```

**Error: `tiffio.h` not found**
```bash
brew install libtiff  # macOS
sudo apt-get install libtiff-dev  # Linux
```

**Error: OpenGL deprecation warnings (macOS)**
- These are harmless - OpenGL still works despite macOS deprecation
- Warnings are silenced with `-DGL_SILENCE_DEPRECATION` flag

### Runtime Issues

**Crash on startup with GUI**
- Ensure XQuartz is installed and you've logged out/in
- Try running with `-nogui` flag

**Poor triangle quality**
- Decrease `rho` value for finer triangles
- Adjust `min_step` and `max_step` bounds
- Try increasing `reduction` factor

**Missing features in output**
- Use `-sharp` parameter to detect sharp edges
- Provide feature lines via `.flf` file
- Increase subdivision level for better curvature estimation

---

## Advanced Usage

### Custom Guidance Fields

The guidance field controls how triangle size varies across the surface. For meshes, it's computed from curvature. For point clouds, it's based on point spacing.

**Modify sizing:**
- `rho` controls the base relationship between curvature and size
- `reduction` controls how quickly size decreases in detailed regions
- `min_step`/`max_step` provide hard bounds

### CSG with Custom Sizing

```bash
# CSG with subdivision for better curvature
./afront mesh1.m mesh2.m -csg union 2 -rho 0.25 -outname result.m
```

### Debugging

Enable GUI to visualize fronts during triangulation:
```bash
./afront input.m -gui -tri_mesh
# Use keyboard to step through algorithm
```

---

## Technical Details

### Dependencies

**Core libraries:**
- **gtb** - Graphics toolbox (point/vector/matrix operations, KD-trees)
- **mlslib** - Moving Least Squares surface projection
- **rmlslib** - Robust MLS implementation
- **ThreadLib** - Multi-threading support
- **viewer** - OpenGL visualization

**External dependencies:**
- OpenGL/GLU/GLUT - Graphics
- X11 - Windowing (via XQuartz on macOS)
- libtiff - TIFF image support
- zlib - Compression
- pthreads - Threading

### Compilation Flags

The code is compiled with:
- `-DUSENR` - Use Numerical Recipes algorithms
- `-DREAL_IS_FLOAT` - Use float precision (vs double)
- `-D_REENTRANT` - Thread-safe code
- `-Wno-c++11-narrowing` - Suppress modern C++ warnings

### Threading Model

- **Main thread:** Front propagation (sequential for correctness)
- **Worker threads:** Surface projection (parallel for performance)
- **GUI thread:** Visualization (separate with mutex protection)

Default: 8 threads on modern CPUs

---

## Publications

Please cite the relevant papers when using our algorithm or code in your research.

### Primary Papers

**High-Quality Extraction of Isosurfaces from Regular and Irregular Grids**
- J. Schreiner, C. Scheidegger, C. Silva
- IEEE Transactions on Visualization and Computer Graphics (Proceedings of IEEE Visualization), 2006

```bibtex
@article{schreiner2006isosurfaces,
  title={High-Quality Extraction of Isosurfaces from Regular and Irregular Grids},
  author={Schreiner, John and Scheidegger, Carlos and Silva, Claudio T.},
  journal={IEEE Transactions on Visualization and Computer Graphics},
  year={2006},
  publisher={IEEE}
}
```

**Direct (Re)Meshing for Efficient Surface Processing**
- J. Schreiner, C. Scheidegger, S. Fleishman, C. Silva
- Computer Graphics Forum (Proceedings of Eurographics), 2006

```bibtex
@article{schreiner2006remeshing,
  title={Direct (Re)Meshing for Efficient Surface Processing},
  author={Schreiner, John and Scheidegger, Carlos and Fleishman, Shachar and Silva, Claudio T.},
  journal={Computer Graphics Forum},
  year={2006},
  organization={Wiley Online Library}
}
```

**Triangulating Point-Set Surfaces With Bounded Error**
- C. Scheidegger, S. Fleishman, C. Silva
- Proceedings of the third Eurographics/ACM Symposium on Geometry Processing, 2005

```bibtex
@inproceedings{scheidegger2005triangulating,
  title={Triangulating Point-Set Surfaces With Bounded Error},
  author={Scheidegger, Carlos and Fleishman, Shachar and Silva, Claudio T.},
  booktitle={Proceedings of the third Eurographics/ACM Symposium on Geometry Processing},
  year={2005}
}
```

### PhD Thesis

**Surface Modeling Using Curvature-Adaptive Advancing Front Triangulation**
- John Schreiner
- University of Utah, 2007
- [Full thesis PDF](https://www.sci.utah.edu/~csilva/papers/thesis/john-schreiner-thesis.pdf)

This thesis provides comprehensive coverage of the advancing front triangulation algorithm, including detailed explanations of the guidance field, front propagation, and applications to various meshing scenarios.

---

## People

This software was produced by members of the Visualization and Geometric Computing (VGC) group at the University of Utah.

**Main contributors to the algorithm:**
- **John Schreiner** - Algorithm development and implementation
- **Carlos E. Scheidegger** - Core algorithm design and development
- **Shachar Fleishman** - Point-set surface methods
- **Claudio T. Silva** - Project supervision and direction

---

## Funding

This work was partially supported by:
- DOE under the VIEWS program and the MICS office
- National Science Foundation (grants CCF-0401498, CCF-0528201, EIA-0323604, IIS-0513692, and OISE-0405402)
- Sandia National Laboratories
- Lawrence Livermore National Laboratory
- IBM Faculty Award
- University of Utah Seed Grant

---

## License

This software is released under the **GNU General Public License (GPL)**.

See individual source files for specific license headers and copyright information.

---

## Project Links

- **Homepage:** https://afront.sourceforge.net/
- **SourceForge:** https://sourceforge.net/projects/afront/
- **Original Institution:** University of Utah, Visualization and Geometric Computing Group

---

## Changelog (2025 Update)

### Modernization for macOS Apple Silicon (October 2025)

The original codebase (circa 2005) has been successfully modernized to compile on:
- macOS Darwin 25.0.0 (Sequoia)
- Apple Silicon (ARM64) architecture
- Clang 17.0.0 compiler

**Fixes applied:**
- CMake modernization (3.10+ compatibility)
- macOS pthread compatibility (`PTHREAD_MUTEX_RECURSIVE`)
- XQuartz/X11 integration for GUI support
- Template specialization order corrections
- C++11 standard compliance fixes
- Dependent name lookup in templates
- Platform-specific RNG functions

The code maintains backwards compatibility with Linux while now supporting modern macOS.

---

## Contributing

This is a research prototype. For bug reports or questions, please use the SourceForge project page.

---

## Acknowledgments

Developed by the Visualization and Geometric Computing group at the University of Utah.

Special thanks to all contributors and funding organizations that made this research possible

---

**Last Updated:** October 2025
**Build Tested:** macOS 15.0 (ARM64), Xcode 17.0.0
