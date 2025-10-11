# Supporting Libraries

This directory contains specialized libraries used by the afront advancing front triangulation system.

## Libraries

### ThreadLib
**Purpose:** Cross-platform threading abstraction layer

Provides a portable threading interface built on top of pthreads (POSIX) and Windows threads. Includes:
- Thread creation and management
- Mutex and critical section support
- Condition variables
- Thread-local storage

### mlslib
**Purpose:** Moving Least Squares (MLS) surface reconstruction

Implements MLS surface approximation for point cloud processing:
- Local surface fitting from point samples
- Surface projection operators
- Normal estimation from point clouds
- Used for mesh generation from unorganized point sets

Key features:
- KD-tree spatial indexing for efficient neighbor queries
- Various MLS weight functions
- Numerical Recipes routines for least-squares solving

### rmlslib
**Purpose:** Robust Moving Least Squares

An enhanced version of mlslib with improved robustness:
- Outlier handling
- Better numerical stability
- Support for noisy point clouds
- Adaptive radius selection

Used for high-quality surface reconstruction from imperfect point data.

### viewer
**Purpose:** OpenGL/GLUT-based 3D visualization framework

A lightweight interactive 3D viewer for debugging and visualization:
- OpenGL rendering infrastructure
- Mouse-based camera controls (rotate, pan, zoom)
- Keyboard event handling
- Multiple window support
- Trackball-style rotation

**macOS Note:** The viewer has been patched to work on modern macOS by running GLUT on the main thread (required by Apple's frameworks).

## Dependencies

- **pthreads** - POSIX threads (for ThreadLib)
- **OpenGL/GLU** - Graphics rendering (for viewer)
- **GLUT** - Window management (for viewer)
- **X11** - Windowing system on macOS (via XQuartz)

## Legacy Code Notice

⚠️ These libraries were developed in the mid-2000s:
- Use ancient OpenGL fixed-function pipeline
- Threading model predates modern C++11 threads
- Not designed for modern GPU pipelines
- Patched to compile but not modernized

The code serves as a historical artifact supporting the published afront research papers.

## Building

These libraries are built as part of the afront project. See the main repository README for build instructions.
