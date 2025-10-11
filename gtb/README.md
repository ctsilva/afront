# GTB - Graphics Toolbox

A comprehensive C++ library for graphics, geometry, and mathematical operations developed in the mid-2000s at the University of Utah.

## Overview

GTB (Graphics Toolbox) is a foundational library that provides essential data structures and algorithms for geometric computing, graphics applications, and mesh processing. It was developed to support research projects in visualization and geometric computing.

## Components

### Graphics
- **Point/Vector/Matrix operations** - Basic geometric primitives (Point2, Point3, Vector3, Matrix4, etc.)
- **Geometric primitives** - Circles, cones, cylinders, planes, spheres, boxes
- **Coordinate systems** - Transformations and reference frames
- **Color management** - RGB, HSV color spaces
- **OpenGL utilities** - Helper functions for OpenGL rendering

### Math
- **Linear algebra** - Vector and matrix operations
- **Numerical algorithms** - Includes Numerical Recipes routines (SVD, linear systems)
- **Interpolation** - Various interpolation schemes
- **Root finding** - Numerical root finders

### I/O
- **Mesh formats** - OFF, OBJ, M format readers/writers
- **Volume data** - Regular and irregular grid I/O
- **Image I/O** - TIFF support

### Data Structures
- **KD-trees** - Spatial indexing for nearest-neighbor queries
- **Triangulated surfaces** - Mesh data structures and operations
- **Indexed geometry** - Efficient indexed triangle meshes

### Utilities
- **Memory management** - Custom allocators
- **Error handling** - Assertion and error reporting
- **Command-line parsing** - Argument processing
- **Debug support** - Debugging utilities

## Dependencies

- **OpenGL/GLU** - For graphics operations
- **TIFF library** - For image I/O
- **Standard C++ library**

## Legacy Code Notice

⚠️ This is legacy code from the mid-2000s:
- Uses ancient OpenGL fixed-function pipeline
- Includes Numerical Recipes routines (some with numerical stability issues)
- Not thread-safe in many areas
- Patched to compile on modern systems but not modernized

## Building

GTB is built as part of the afront project. See the main repository README for build instructions.

## Historical Context

GTB was developed to support multiple research projects in mesh processing, visualization, and geometric computing at the University of Utah's Visualization and Geometric Computing group.
