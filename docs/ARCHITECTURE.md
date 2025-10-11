# Afront Software Architecture

This document describes the internal architecture of the Afront advancing front triangulation implementation, based on John Schreiner's PhD thesis (University of Utah, 2009).

## Overview

Afront is written in C++ and uses two key language features for flexibility:

- **Templates**: Allow calculations in single or double precision (controlled by `-DREAL_IS_FLOAT`)
- **Abstract Classes**: Hide surface representation details from the core triangulation algorithm

The implementation uses abstract base classes to separate concerns and allow different surface types (meshes, volumes, point clouds, CSG) to share the same core triangulation code.

## Architecture Diagram

```
Input
├── Surface (triangulate_*.cpp)
└── Features (main.cpp:1408)
    ↓
Seed Identification
├── Boundaries (triangulate_iso.cpp:3006)
└── Connected Components (triangulate_iso.cpp:4058)
    ↓
Guidance Field Construction
├── Generate Surface Samples (triangulate_iso.cpp:2183)
├── Generate Curve Samples (triangulate_iso.cpp:4009)
├── Trim Samples (guidance.h:79)
└── Guidance Field (guidance.cpp:62)
    ↓
Initialization
└── Initial Fronts (guidance.cpp:125)
    ↓
Triangulation
├── Tentative Triangle (triangulator.cpp:163)
├── Project Point (triangulator.cpp:205)
├── Interference Detection (triangulator.cpp:847)
├── Grow Triangle (triangulator.cpp:1436)
├── Connect Triangle (triangulator.cpp:1187)
└── Advance Fronts (triangulator.cpp:1265)
    ↓
Output Stream
├── Edge Flipper (edgeflipper.cpp)
└── Disk (output_controller_*.cpp)
```

## Core Components

### 1. Guidance Field

**File:** [guidance.cpp](../src/guidance.cpp), [guidance.h](../src/guidance.h)

**Purpose:** Spatially-varying ideal edge length field based on surface curvature

**Abstract Base Class:**
```cpp
class GuidanceField {
    virtual void* OrderedPointTraverseStart(const Point &p) = 0;
    virtual bool OrderedPointTraverseNext(void* ctx,
                                         real &dist,
                                         Point &point,
                                         real &ideal) = 0;
    virtual void OrderedPointTraverseEnd(void* ctx) = 0;

    real MaxStepLength(const Point &p);
};
```

**Design Pattern:** Iterator pattern with traversal context
- `OrderedPointTraverseStart` returns context for parallel evaluations
- `OrderedPointTraverseNext` iterates samples ordered by distance from query point
- `MaxStepLength` evaluates guidance field using the iterator

**Implementations:**
- **Mesh surfaces**: Use mesh vertices directly as samples, build KD-tree over them
- **CSG operations**: Contains two GuidanceField instances, traverses both in parallel
- **Volumetric data**: Out-of-core design with per-block guidance fields swapped in/out

**Key Feature:** Abstract interface allows different ideal edge size functions (curvature-based, local feature size, etc.)

---

### 2. Surface Definitions

**Files:** [triangulate_mesh.cpp](../src/triangulate_mesh.cpp), [triangulate_iso.cpp](../src/triangulate_iso.cpp), [triangulate_mls.cpp](../src/triangulate_mls.cpp), [triangulate_csg.cpp](../src/triangulate_csg.cpp), [triangulate_tet.cpp](../src/triangulate_tet.cpp)

**Purpose:** Encapsulate surface-specific operations (projection, boundary detection)

**Abstract Base Class:**
```cpp
class Surface {
    virtual int ProjectPoint(const FrontElement &base1,
                           const FrontElement &base2,
                           const Point &from_point,
                           Point &to_point,
                           Vector &to_normal) const = 0;

    virtual bool GetNextBlock(vector<vector<Point>> &points,
                            vector<vector<Vector>> &normals,
                            vector<bool> &loops) const = 0;
};
```

**Responsibilities:**
1. **Point Projection**: Project tentative vertices onto the surface
   - Uses `FrontElement` bases to match guidance field edge lengths
   - Returns consistently oriented normals (even for implicit surfaces)
   - Used by all surface types except point set surfaces

2. **Initial Front Generation**: Find boundaries and seed edges
   - Returns ordered point/normal sequences
   - `loops` flag indicates closed loops vs. open segments
   - Supports out-of-core meshing with incremental boundaries

**Surface Types:**
- **Mesh** (`triangulate_mesh.cpp`): Remeshing of triangle meshes
- **Regular Grid** (`triangulate_iso.cpp`): Isosurface extraction from volumes
- **Tetrahedral** (`triangulate_tet.cpp`): Isosurface from irregular grids
- **MLS** (`triangulate_mls.cpp`): Point cloud reconstruction
- **CSG** (`triangulate_csg.cpp`): Boolean operations on meshes

---

### 3. Output Streams

**Files:** [output_controller_gui.cpp](../src/output_controller_gui.cpp), [output_controller_obj.cpp](../src/output_controller_obj.cpp), [output_controller_hhm.cpp](../src/output_controller_hhm.cpp), [edgeflipper.cpp](../src/edgeflipper.cpp)

**Purpose:** Handle mesh output with streaming and post-processing

**Abstract Base Class:**
```cpp
class OutputController {
    virtual void AddVertex(int index, const Point &p, const Vector &n) = 0;
    virtual void AddTriangle(int v1, int v2, int v3) = 0;
    virtual void FinalizeVertex(int index) = 0;
};
```

**Design Pattern:** Chain of Responsibility - controllers can be chained

**Call Sequence:**
1. `AddVertex(index, p, n)` - Vertex generated and indexed
2. `AddTriangle(v1, v2, v3)` - Triangle created using vertex indices
3. `FinalizeVertex(index)` - Vertex no longer on any front (can be written)

**Implementations:**

#### File Writers
- **SMA/SMB formats** ([Isenburg & Lindstrom](https://dl.acm.org/doi/10.1145/1057432.1057465)): Allow interleaved vertices/triangles
  - Avoids storing entire mesh in memory
  - Streaming mesh format

- **OBJ format**: Standard Wavefront OBJ output
- **HHM format**: Hierarchical mesh format

#### Edge Flipper
**File:** [edgeflipper.cpp](../src/edgeflipper.cpp)

Performs streaming edge flips to improve mesh quality:
- Caches vertices and triangles
- Performs edge flips in band around fronts
- Passes finalized triangles to next OutputController
- Described in thesis Section 4.5

#### GUI Visualizer
**File:** [output_controller_gui.cpp](../src/output_controller_gui.cpp)

Interactive visualization during generation:
- Stores entire mesh in memory
- Draws with OpenGL during construction
- Useful for debugging
- **Performance Impact**: Disabled in batch mode (`-nogui`)

---

### 4. Triangulator Module

**File:** [triangulator.cpp](../src/triangulator.cpp), [triangulator.h](../src/triangulator.h)

**Purpose:** Core advancing front algorithm

**Class Structure:**
```cpp
class Triangulator {
    Triangulator(GuidanceField &guidance,
                Surface &surface,
                OutputController &output);

    Heap<FrontElement*> heap_local;
    Heap<FrontElement*> heap_global;

    KDTree<FrontElement*> kdtree_local;
    KDTree<FrontElement*> kdtree_global;
};
```

**Data Structures:**

1. **Fronts**: Doubly-linked lists of `FrontElement` structures
   - Constant-time split/merge operations
   - Each element contains: position, normal, guidance field value

2. **Priority Heap**: Choose best edge for triangle creation
   - Prioritizes high-quality triangles
   - Edge priority based on ideal edge lengths

3. **KD-Tree**: Spatial indexing for interference detection
   - Checks tentative triangles against existing fronts
   - Rebuilt periodically to maintain balance

**Two-Level Structure (Local/Global):**

For out-of-core meshing efficiency:
- **Local**: Edges in active working area
  - Rebuilt when working area changes
  - Most queries use local structures

- **Global**: Edges outside active area
  - Rebuilt only when active block changes
  - Reduces KD-tree depth and heap size

**Algorithm Flow:**

1. Retrieve initial fronts from `Surface::GetNextBlock()`
2. Insert fronts into heaps/KD-trees with guidance field values
3. **Main Loop:**
   - Pop best edge from heap
   - Create tentative triangle vertex
   - Project with `Surface::ProjectPoint()`
   - Check interference with KD-tree
   - Either GROW (new vertex) or CONNECT (existing vertex)
   - Send vertex/triangle to `OutputController`
   - Update fronts (split/merge)
4. Continue until all fronts closed

**Block Management:**
- Active block triangulated completely
- Migrate local edges → global structures
- Load new block from `Surface::GetNextBlock()`
- Move relevant global edges → local structures

---

## Key Design Patterns

### Template Metaprogramming
- All numeric types templated (single vs double precision)
- Volume data can use any numeric type (`unsigned char`, `short`, `float`)
- Compile-time selection via `-DREAL_IS_FLOAT`

### Abstract Factory
- Different surface implementations behind common `Surface` interface
- Allows core algorithm to be surface-agnostic

### Iterator Pattern
- `GuidanceField` traversal hides implementation details
- Supports parallel queries with context objects

### Chain of Responsibility
- `OutputController` chain for processing pipeline
- Example: Mesh → EdgeFlipper → FileWriter

### Strategy Pattern
- Different projection strategies per surface type
- Different guidance field implementations

---

## File Organization

### Main Entry Point
- [main.cpp](../src/main.cpp) - Command-line parsing, setup, dispatch to surface-specific triangulation

### Core Algorithm
- [triangulator.cpp](../src/triangulator.cpp) - Main advancing front loop
- [guidance.cpp](../src/guidance.cpp) - Guidance field evaluation
- [front.cpp](../src/front.cpp) - Front data structures

### Surface Implementations
- [triangulate_mesh.cpp](../src/triangulate_mesh.cpp) - Mesh remeshing
- [triangulate_iso.cpp](../src/triangulate_iso.cpp) - Regular grid isosurfaces
- [triangulate_tet.cpp](../src/triangulate_tet.cpp) - Tetrahedral mesh isosurfaces
- [triangulate_mls.cpp](../src/triangulate_mls.cpp) - MLS point cloud surfaces
- [triangulate_csg.cpp](../src/triangulate_csg.cpp) - CSG boolean operations

### Output Handling
- [edgeflipper.cpp](../src/edgeflipper.cpp) - Streaming edge flip post-processing
- [output_controller_gui.cpp](../src/output_controller_gui.cpp) - OpenGL visualization
- [output_controller_obj.cpp](../src/output_controller_obj.cpp) - Wavefront OBJ output
- [output_controller_hhm.cpp](../src/output_controller_hhm.cpp) - Hierarchical mesh output

### Utilities
- [crease.cpp](../src/crease.cpp) - Sharp feature detection
- [generaldef.cpp](../src/generaldef.cpp) - General definitions and utilities
- [rg.cpp](../src/rg.cpp) - Range operations
- [lsqr.cpp](../src/lsqr.cpp) - Sparse linear solver (for CSG)

### Data Structures
- [uheap.h](../src/uheap.h) - Updatable heap
- [conekdtree.h](../src/conekdtree.h) - KD-tree for spatial queries
- [parallel.h](../src/parallel.h) - Threading support

---

## Performance Considerations

### Memory Efficiency
- Out-of-core guidance fields for large volumes
- Streaming output formats (SMA/SMB)
- Two-level heap/KD-tree structure
- Batch mode (`-nogui`) disables memory-intensive visualization

### Computational Efficiency
- KD-tree spatial indexing (O(log n) queries)
- Priority heap for best edge selection (O(log n) operations)
- Doubly-linked lists for fronts (O(1) split/merge)
- Multi-threaded surface projection (see [lib/ThreadLib](../lib/ThreadLib/))

### Numerical Stability
- Template precision control (single vs double)
- Guidance field gradation prevents extreme size changes
- Failsafe mode for difficult regions

---

## Extension Points

### Adding New Surface Types

1. Implement `Surface` abstract class:
   - `ProjectPoint()` - Surface projection logic
   - `GetNextBlock()` - Boundary/seed identification

2. Implement `GuidanceField` abstract class:
   - `OrderedPointTraverse*` - Sample iteration
   - Compute ideal edge lengths from surface properties

3. Add command-line handler in [main.cpp](../src/main.cpp)

### Adding New Output Formats

1. Implement `OutputController` abstract class:
   - `AddVertex()` - Handle new vertex
   - `AddTriangle()` - Handle new triangle
   - `FinalizeVertex()` - Vertex finalized (optional)

2. Chain with existing controllers if needed (e.g., after edge flipper)

### Adding New Ideal Edge Size Functions

Modify guidance field implementation to use different ideal edge length computation:
- Replace curvature-based sizing
- Use local feature size
- User-specified function

---

## References

- John M. Schreiner. *Surface Modeling Using Curvature-Adaptive Advancing Front Triangulation*. PhD Thesis, University of Utah, 2009. [PDF](https://www.sci.utah.edu/~csilva/papers/thesis/john-schreiner-thesis.pdf)

- Related source code in [src/](../src/) directory

---

**Last Updated:** October 2025
