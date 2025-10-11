cmake_minimum_required(VERSION 3.10)

PROJECT(afront)

SET (CMAKE_CXX_FLAGS "-DUSENR -DREAL_IS_FLOAT -D_REENTRANT -Wno-c++11-narrowing")

# Add compatibility for macOS
if(APPLE)
    add_definitions(-Dstricmp=strcasecmp)
    add_definitions(-DGL_SILENCE_DEPRECATION)
    # Add X11 include path for XQuartz
    include_directories(/opt/X11/include)
    link_directories(/opt/X11/lib)
    # Add libtiff include path
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

INCLUDE_DIRECTORIES(. ./lib)

# Add subdirectories with explicit binary directories
ADD_SUBDIRECTORY(gtb ${CMAKE_CURRENT_BINARY_DIR}/gtb)
ADD_SUBDIRECTORY(lib/viewer ${CMAKE_CURRENT_BINARY_DIR}/viewer)
ADD_SUBDIRECTORY(lib/mlslib ${CMAKE_CURRENT_BINARY_DIR}/mlslib)
ADD_SUBDIRECTORY(lib/rmlslib ${CMAKE_CURRENT_BINARY_DIR}/rmlslib)
ADD_SUBDIRECTORY(lib/ThreadLib ${CMAKE_CURRENT_BINARY_DIR}/ThreadLib)

LINK_DIRECTORIES(gtb lib/mlslib lib/rmlslib lib/viewer lib/ThreadLib)

ADD_EXECUTABLE(afront
	crease.cpp      main.cpp                   output_controller_reeb.cpp  triangulate_mesh.cpp
	front.cpp       MarchingCubes.cpp          rg.cpp
	generaldef.cpp  output_controller_gui.cpp  triangulate_csg.cpp         triangulate_mls.cpp
	guidance.cpp    output_controller_hhm.cpp  triangulate_tet.cpp
	lsqr.cpp        output_controller_obj.cpp  triangulate_iso.cpp         triangulator.cpp
	edgeflipper.cpp FLF_io.cpp                 PC_io.cpp)

# Find GLUT and OpenGL
find_package(GLUT)
find_package(OpenGL)

# Link libraries
if(GLUT_FOUND AND OPENGL_FOUND)
    TARGET_LINK_LIBRARIES(afront viewer ThreadLib gtb mlslib rmlslib
                         ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES}
                         m pthread z)
else()
    # Fallback to old-style library names
    TARGET_LINK_LIBRARIES(afront viewer ThreadLib gtb mlslib rmlslib
                         GL GLU glut m pthread z)
endif()

# Ultra-simple GLUT test program - just tests window creation
ADD_EXECUTABLE(test_glut_simple test_glut_simple.cpp)

if(GLUT_FOUND AND OPENGL_FOUND)
    TARGET_LINK_LIBRARIES(test_glut_simple viewer ThreadLib gtb
                         ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES}
                         m pthread z)
else()
    TARGET_LINK_LIBRARIES(test_glut_simple viewer ThreadLib gtb
                         GL GLU glut m pthread z)
endif()
