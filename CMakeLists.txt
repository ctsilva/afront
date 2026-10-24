cmake_minimum_required(VERSION 3.10)
project(afront CXX C)

# afront, CVS head of 2009-03-24 (the out-of-core / interval-arithmetic version
# behind Schreiner's dissertation and the Vis 2009 submission vis-1255).
# real_type is double: the 2008 vcproj used REAL_IS_DOUBLE and the last
# multimaterial commit (triangulate_iso.cpp 1.41) only compiles that way.
# Build system written 2026-10 for modern macOS/clang; the original used
# autotools (see src/afront/Makefile.am) and per-directory CMake files.

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(
  -DUSENR -DREAL_IS_DOUBLE -D_REENTRANT
  -Wno-c++11-narrowing -Wno-deprecated-declarations -Wno-register
  -Wno-writable-strings -Wno-dangling-else -Wno-format
)

if(APPLE)
  add_definitions(-Dstricmp=strcasecmp -DGL_SILENCE_DEPRECATION)
  include_directories(/opt/X11/include /opt/homebrew/include)
  link_directories(/opt/X11/lib /opt/homebrew/lib)
endif()

set(SRC ${CMAKE_SOURCE_DIR})
include_directories(${SRC} ${SRC}/lib ${SRC}/src)

# ---- gtb -------------------------------------------------------------------
# source list from src/gtb/Makefile.am (the 2008 gtb/CMakeLists.txt is stale: still lists tiff)
set(GTB_SRC_LIST
  ui/commandline.cpp
  graphics/line2.cpp
  graphics/box2.cpp
  graphics/segment2.cpp
  graphics/ray2.cpp
  graphics/surfelset.cpp
  gtb.cpp
  debug/debug.cpp
  error/err.cpp
  error/error.cpp
  graphics/box3.cpp
  graphics/camera.cpp
  graphics/camera_path.cpp
  graphics/circle3.cpp
  graphics/color_rgb.cpp
  graphics/cone.cpp
  graphics/coordinate_system.cpp
  graphics/graphics.cpp
  graphics/icosahedron.cpp
  graphics/image.cpp
  graphics/imageio.cpp
  graphics/image_float.cpp
  graphics/image_tile.cpp
  graphics/image_tile_set.cpp
  graphics/image_viewer.cpp
  graphics/indexed_triangle.cpp
  graphics/indexed_triangle_set.cpp
  graphics/kdtree.cpp
  graphics/light.cpp
  graphics/line3.cpp
  graphics/mat3.cpp
  graphics/material.cpp
  graphics/model.cpp
  graphics/model_rtpi.cpp
  graphics/model_viewer.cpp
  graphics/octree.cpp
  graphics/octree_node.cpp
  graphics/octree_node_cache.cpp
  graphics/ogltools.cpp
  graphics/plane.cpp
  graphics/point2.cpp
  graphics/point3.cpp
  graphics/polygon3.cpp
  graphics/ray3.cpp
  graphics/rtpi.cpp
  graphics/rtpi_grid.cpp
  graphics/rtpi_strip.cpp
  graphics/segment3.cpp
  graphics/sphere.cpp
  graphics/surfelhierarchy.cpp
  graphics/surfelsetmultiview.cpp
  graphics/surfelsettools.cpp
  graphics/surfelsetview.cpp
  graphics/surfel_set_io.cpp
  graphics/text.cpp
  graphics/triangle3.cpp
  graphics/triangle_mesh.cpp
  graphics/tsetio.cpp
  graphics/vector2.cpp
  graphics/vector3.cpp
  graphics/view.cpp
  graphics/viewport.cpp
  io/io.cpp
  math/amat.cpp
  math/amatc.cpp
  math/math.cpp
  math/mathtools.cpp
  math/matrix4.cpp
  math/mynr.cpp
  math/nrold/nrutil.cpp
  memory/memory.cpp
  network/netutil.cpp
  network/network.cpp
  network/socket_client.cpp
  network/socket_server.cpp
  plp/extinction_coef.cpp
  plp/extinction_coef_set.cpp
  plp/front.cpp
  plp/plp.cpp
  real/real.cpp
  ui/keyboard.cpp
  ui/mouse.cpp
  ui/ui.cpp
  util/timer.cpp
  util/util.cpp
  math/ran1.c
)
list(TRANSFORM GTB_SRC_LIST PREPEND ${SRC}/gtb/)
add_library(gtb STATIC ${GTB_SRC_LIST})
target_include_directories(gtb PRIVATE ${SRC}/gtb)

# ---- support libraries ------------------------------------------------------
add_library(ThreadLib STATIC
  ${SRC}/lib/ThreadLib/CriticalSection.cpp
  ${SRC}/lib/ThreadLib/Semaphore.cpp
  ${SRC}/lib/ThreadLib/Thread.cpp)

add_library(viewer STATIC
  ${SRC}/lib/viewer/debug_draw.cpp
  ${SRC}/lib/viewer/trackball.cpp
  ${SRC}/lib/viewer/viewer.cpp)

add_library(mlslib STATIC
  ${SRC}/lib/mlslib/CProjection.cpp
  ${SRC}/lib/mlslib/knn.cpp
  ${SRC}/lib/mlslib/NR/nrutil.cpp
  ${SRC}/lib/mlslib/poly1.cpp
  ${SRC}/lib/mlslib/poly2.cpp)

# Isenburg's streaming-mesh library, source list from src/lib/sm/Makefile.am
file(GLOB SM_SRCS ${SRC}/lib/sm/*.cpp)
add_library(sm STATIC ${SM_SRCS})
target_include_directories(sm PRIVATE ${SRC}/lib/sm)

# ---- afront ---------------------------------------------------------------
set(AFRONT_SRCS
  crease.cpp main.cpp triangulate_mesh.cpp front.cpp MarchingCubes.cpp
  output_controller_gui.cpp triangulate_csg.cpp triangulate_mls.cpp
  guidance.cpp output_controller_sma.cpp output_controller_hhm.cpp
  triangulate_tet.cpp lsqr.cpp output_controller_obj.cpp triangulate_iso.cpp
  triangulator.cpp edgeflipper.cpp FLF_io.cpp PC_io.cpp)
list(TRANSFORM AFRONT_SRCS PREPEND ${SRC}/src/)
add_executable(afront ${AFRONT_SRCS})

find_package(OpenGL REQUIRED)
find_library(GLUT_LIB NAMES glut freeglut PATHS /opt/homebrew/lib)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

set(COMMON_LIBS viewer ThreadLib gtb mlslib sm
    ${GLUT_LIB} OpenGL::GL OpenGL::GLU ZLIB::ZLIB Threads::Threads m)
target_link_libraries(afront ${COMMON_LIBS})

add_executable(sm_viewer ${SRC}/sm_viewer/sm_viewer.cpp)
target_include_directories(sm_viewer PRIVATE ${SRC}/lib/sm)
target_link_libraries(sm_viewer ${COMMON_LIBS})

# ---- code kept from the VGC-internal 2007 tree (not in the SourceForge release) ----
# Robust MLS (rmlslib) and the Reeb-graph output controller (rg.cpp, generaldef.cpp,
# output_controller_reeb.cpp). rmlslib is built so it keeps compiling; the afront
# binary only uses it when NO_RMLS is undefined in src/triangulate_mls.cpp.
add_subdirectory(lib/rmlslib ${CMAKE_CURRENT_BINARY_DIR}/rmlslib)
add_library(reeb STATIC ${SRC}/src/rg.cpp ${SRC}/src/generaldef.cpp ${SRC}/src/output_controller_reeb.cpp)
target_link_libraries(afront rmlslib)
# src/test_glut*.cpp (2007 GLUT threading tests) no longer compile against the 2009 viewer
# (SimpleViewer gained pure virtuals); kept as reference only.
