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

INCLUDE_DIRECTORIES(. ./src ./lib)

# Add subdirectories with explicit binary directories
ADD_SUBDIRECTORY(gtb ${CMAKE_CURRENT_BINARY_DIR}/gtb)
ADD_SUBDIRECTORY(lib/viewer ${CMAKE_CURRENT_BINARY_DIR}/viewer)
ADD_SUBDIRECTORY(lib/mlslib ${CMAKE_CURRENT_BINARY_DIR}/mlslib)
ADD_SUBDIRECTORY(lib/rmlslib ${CMAKE_CURRENT_BINARY_DIR}/rmlslib)
ADD_SUBDIRECTORY(lib/ThreadLib ${CMAKE_CURRENT_BINARY_DIR}/ThreadLib)

LINK_DIRECTORIES(gtb lib/mlslib lib/rmlslib lib/viewer lib/ThreadLib)

ADD_EXECUTABLE(afront
	src/crease.cpp      src/main.cpp                   src/output_controller_reeb.cpp  src/triangulate_mesh.cpp
	src/front.cpp       src/MarchingCubes.cpp          src/rg.cpp
	src/generaldef.cpp  src/output_controller_gui.cpp  src/triangulate_csg.cpp         src/triangulate_mls.cpp
	src/guidance.cpp    src/output_controller_hhm.cpp  src/triangulate_tet.cpp
	src/lsqr.cpp        src/output_controller_obj.cpp  src/triangulate_iso.cpp         src/triangulator.cpp
	src/edgeflipper.cpp src/FLF_io.cpp                 src/PC_io.cpp)

# Find GLUT and OpenGL
find_package(GLUT)
find_package(OpenGL)

# Add GLUT include directories
if(GLUT_FOUND)
    include_directories(${GLUT_INCLUDE_DIR})
endif()
if(OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

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
ADD_EXECUTABLE(test_glut_simple src/test_glut_simple.cpp)

if(GLUT_FOUND AND OPENGL_FOUND)
    TARGET_LINK_LIBRARIES(test_glut_simple viewer ThreadLib gtb
                         ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES}
                         m pthread z)
else()
    TARGET_LINK_LIBRARIES(test_glut_simple viewer ThreadLib gtb
                         GL GLU glut m pthread z)
endif()
