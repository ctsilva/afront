# Microsoft Developer Studio Project File - Name="gtb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gtb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gtb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gtb.mak" CFG="gtb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gtb - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gtb - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gtb - Win32 ProfRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gtb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /Ob2 /I ".." /I "..\.." /I "C:\matlabR12\simulink\include" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /FR /Yu"gtb/gtb.hpp" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"gtb.lib"

!ELSEIF  "$(CFG)" == "gtb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I ".." /I "..\.." /I "." /I "C:\matlabR12\simulink\include" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /Yu"gtb/gtb.hpp" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib\gtbd.lib"

!ELSEIF  "$(CFG)" == "gtb - Win32 ProfRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gtb___Win32_ProfRelease"
# PROP BASE Intermediate_Dir "gtb___Win32_ProfRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ProfRelease"
# PROP Intermediate_Dir "ProfRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /Zi /O2 /I ".." /I "..\.." /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "_NOTHREADS" /D "NOTHREADS" /FR /Yu"gtb/gtb.hpp" /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /O2 /I ".." /I "..\.." /I "C:\matlabR12\simulink\include" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "_NOTHREADS" /D "NOTHREADS" /FR /Yu"gtb/gtb.hpp" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "gtb - Win32 Release"
# Name "gtb - Win32 Debug"
# Name "gtb - Win32 ProfRelease"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "IO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=io\io.cpp
# End Source File
# Begin Source File

SOURCE=io\io.hpp
# End Source File
# Begin Source File

SOURCE=.\io\io.ipp
# End Source File
# End Group
# Begin Group "graphics"

# PROP Default_Filter ""
# Begin Group "Models"

# PROP Default_Filter ""
# Begin Source File

SOURCE=graphics\indexed_triangle.cpp
# End Source File
# Begin Source File

SOURCE=graphics\indexed_triangle.hpp
# End Source File
# Begin Source File

SOURCE=graphics\indexed_triangle.ipp
# End Source File
# Begin Source File

SOURCE=graphics\indexed_triangle_set.cpp
# End Source File
# Begin Source File

SOURCE=graphics\indexed_triangle_set.hpp
# End Source File
# Begin Source File

SOURCE=graphics\indexed_triangle_set.ipp
# End Source File
# Begin Source File

SOURCE=graphics\model.cpp
# End Source File
# Begin Source File

SOURCE=graphics\model.hpp
# End Source File
# Begin Source File

SOURCE=graphics\model.ipp
# End Source File
# Begin Source File

SOURCE=graphics\model_rtpi.cpp
# End Source File
# Begin Source File

SOURCE=graphics\model_rtpi.hpp
# End Source File
# Begin Source File

SOURCE=graphics\model_rtpi.ipp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi.cpp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi.hpp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi.ipp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi_grid.cpp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi_grid.hpp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi_grid.ipp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi_strip.cpp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi_strip.hpp
# End Source File
# Begin Source File

SOURCE=graphics\rtpi_strip.ipp
# End Source File
# Begin Source File

SOURCE=.\graphics\surfel_set_io.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\surfel_set_io.hpp
# End Source File
# Begin Source File

SOURCE=.\graphics\surfelhierarchy.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\surfelhierarchy.h
# End Source File
# Begin Source File

SOURCE=.\graphics\surfelhierarchy.inl
# End Source File
# Begin Source File

SOURCE=.\graphics\surfelset.cpp
# End Source File
# Begin Source File

SOURCE=graphics\SurfelSet.hpp
# End Source File
# Begin Source File

SOURCE=.\graphics\surfelset.inl
# End Source File
# Begin Source File

SOURCE=.\graphics\surfelsetview.hpp
# End Source File
# Begin Source File

SOURCE=.\graphics\surfelsetview.inl
# End Source File
# Begin Source File

SOURCE=.\graphics\tsetio.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\tsetio.hpp
# End Source File
# End Group
# Begin Group "Geometry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=graphics\box3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\box3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\box3.ipp
# End Source File
# Begin Source File

SOURCE=graphics\circle3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\circle3.ipp
# End Source File
# Begin Source File

SOURCE=graphics\cone.cpp
# End Source File
# Begin Source File

SOURCE=graphics\cone.hpp
# End Source File
# Begin Source File

SOURCE=graphics\cone.ipp
# End Source File
# Begin Source File

SOURCE=.\graphics\icosahedron.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\icosahedron.hpp
# End Source File
# Begin Source File

SOURCE=graphics\line3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\line3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\line3.ipp
# End Source File
# Begin Source File

SOURCE=.\graphics\mat3.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\mat3.hpp
# End Source File
# Begin Source File

SOURCE=.\graphics\mat3.inl
# End Source File
# Begin Source File

SOURCE=graphics\plane.cpp
# End Source File
# Begin Source File

SOURCE=graphics\plane.hpp
# End Source File
# Begin Source File

SOURCE=graphics\plane.ipp
# End Source File
# Begin Source File

SOURCE=graphics\point2.cpp
# End Source File
# Begin Source File

SOURCE=graphics\point2.hpp
# End Source File
# Begin Source File

SOURCE=graphics\point2.ipp
# End Source File
# Begin Source File

SOURCE=graphics\point3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\point3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\point3.ipp
# End Source File
# Begin Source File

SOURCE=graphics\polygon3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\polygon3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\polygon3.ipp
# End Source File
# Begin Source File

SOURCE=graphics\ray3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\ray3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\ray3.ipp
# End Source File
# Begin Source File

SOURCE=graphics\segment3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\segment3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\segment3.ipp
# End Source File
# Begin Source File

SOURCE=graphics\sphere.cpp
# End Source File
# Begin Source File

SOURCE=graphics\sphere.hpp
# End Source File
# Begin Source File

SOURCE=graphics\sphere.ipp
# End Source File
# Begin Source File

SOURCE=graphics\triangle3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\triangle3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\triangle3.ipp
# End Source File
# Begin Source File

SOURCE=graphics\vector2.cpp
# End Source File
# Begin Source File

SOURCE=graphics\vector2.hpp
# End Source File
# Begin Source File

SOURCE=graphics\vector2.ipp
# End Source File
# Begin Source File

SOURCE=graphics\vector3.cpp
# End Source File
# Begin Source File

SOURCE=graphics\vector3.hpp
# End Source File
# Begin Source File

SOURCE=graphics\vector3.ipp
# End Source File
# End Group
# Begin Group "SpacePartitioning"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\graphics\kdtree.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\kdtree.h
# End Source File
# Begin Source File

SOURCE=graphics\octree.cpp
# End Source File
# Begin Source File

SOURCE=graphics\octree.hpp
# End Source File
# Begin Source File

SOURCE=graphics\octree.ipp
# End Source File
# Begin Source File

SOURCE=graphics\octree_node.cpp
# End Source File
# Begin Source File

SOURCE=graphics\octree_node.hpp
# End Source File
# Begin Source File

SOURCE=graphics\octree_node.ipp
# End Source File
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Source File

SOURCE=graphics\image.cpp
# End Source File
# Begin Source File

SOURCE=graphics\image.hpp
# End Source File
# Begin Source File

SOURCE=graphics\image.ipp
# End Source File
# Begin Source File

SOURCE=.\graphics\image_float.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\image_float.hpp
# End Source File
# Begin Source File

SOURCE=.\graphics\image_float.inl
# End Source File
# Begin Source File

SOURCE=.\graphics\image_tile.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\image_tile.hpp
# End Source File
# Begin Source File

SOURCE=.\graphics\image_tile_set.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\image_tile_set.hpp
# End Source File
# Begin Source File

SOURCE=graphics\image_viewer.cpp
# End Source File
# Begin Source File

SOURCE=graphics\image_viewer.ipp
# End Source File
# Begin Source File

SOURCE=.\graphics\imageio.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\imageio.h
# End Source File
# Begin Source File

SOURCE=graphics\tiff_rgba_image.cpp
# End Source File
# Begin Source File

SOURCE=graphics\tiff_rgba_image.hpp
# End Source File
# Begin Source File

SOURCE=graphics\tiff_rgba_image.ipp
# End Source File
# End Group
# Begin Group "OGL_AND_UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=graphics\camera.cpp
# End Source File
# Begin Source File

SOURCE=graphics\camera.hpp
# End Source File
# Begin Source File

SOURCE=graphics\camera.ipp
# End Source File
# Begin Source File

SOURCE=graphics\camera_path.cpp
# End Source File
# Begin Source File

SOURCE=graphics\camera_path.hpp
# End Source File
# Begin Source File

SOURCE=graphics\color_rgb.cpp
# End Source File
# Begin Source File

SOURCE=graphics\color_rgb.hpp
# End Source File
# Begin Source File

SOURCE=graphics\color_rgb.ipp
# End Source File
# Begin Source File

SOURCE=graphics\coordinate_system.cpp
# End Source File
# Begin Source File

SOURCE=graphics\coordinate_system.hpp
# End Source File
# Begin Source File

SOURCE=graphics\coordinate_system.ipp
# End Source File
# Begin Source File

SOURCE=graphics\light.cpp
# End Source File
# Begin Source File

SOURCE=graphics\light.hpp
# End Source File
# Begin Source File

SOURCE=graphics\light.ipp
# End Source File
# Begin Source File

SOURCE=graphics\material.cpp
# End Source File
# Begin Source File

SOURCE=graphics\material.hpp
# End Source File
# Begin Source File

SOURCE=graphics\material.ipp
# End Source File
# Begin Source File

SOURCE=graphics\model_viewer.cpp
# End Source File
# Begin Source File

SOURCE=graphics\model_viewer.hpp
# End Source File
# Begin Source File

SOURCE=graphics\model_viewer.ipp
# End Source File
# Begin Source File

SOURCE=.\graphics\ogltools.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics\ogltools.h
# End Source File
# Begin Source File

SOURCE=graphics\text.cpp
# End Source File
# Begin Source File

SOURCE=graphics\text.hpp
# End Source File
# Begin Source File

SOURCE=graphics\view.cpp
# End Source File
# Begin Source File

SOURCE=graphics\view.hpp
# End Source File
# Begin Source File

SOURCE=graphics\view.ipp
# End Source File
# Begin Source File

SOURCE=graphics\viewport.cpp
# End Source File
# Begin Source File

SOURCE=graphics\viewport.hpp
# End Source File
# Begin Source File

SOURCE=graphics\viewport.ipp
# End Source File
# End Group
# Begin Source File

SOURCE=graphics\graphics.cpp
# End Source File
# Begin Source File

SOURCE=graphics\graphics.hpp
# End Source File
# End Group
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=debug\debug.cpp
# End Source File
# Begin Source File

SOURCE=debug\debug.hpp
# End Source File
# End Group
# Begin Group "error"

# PROP Default_Filter ""
# Begin Source File

SOURCE=error\err.cpp
# End Source File
# Begin Source File

SOURCE=error\err.h
# End Source File
# Begin Source File

SOURCE=.\error\err.inl
# End Source File
# Begin Source File

SOURCE=error\error.cpp
# End Source File
# Begin Source File

SOURCE=error\error.hpp
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\math\amat.h
# End Source File
# Begin Source File

SOURCE=.\math\amat.inl
# End Source File
# Begin Source File

SOURCE=.\math\arraymat.h
# End Source File
# Begin Source File

SOURCE=math\math.cpp
# End Source File
# Begin Source File

SOURCE=math\math.hpp
# End Source File
# Begin Source File

SOURCE=math\math.ipp
# End Source File
# Begin Source File

SOURCE=.\math\mathtools.h
# End Source File
# Begin Source File

SOURCE=math\matrix4.cpp
# End Source File
# Begin Source File

SOURCE=math\matrix4.hpp
# End Source File
# Begin Source File

SOURCE=math\matrix4.ipp
# End Source File
# Begin Source File

SOURCE=.\math\mynr.cpp
# End Source File
# Begin Source File

SOURCE=.\math\mynr.h
# End Source File
# Begin Source File

SOURCE=.\math\ran1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=real\real.cpp
# End Source File
# Begin Source File

SOURCE=real\real.hpp
# End Source File
# Begin Source File

SOURCE=real\real.ipp
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=ui\keyboard.cpp
# End Source File
# Begin Source File

SOURCE=ui\keyboard.hpp
# End Source File
# Begin Source File

SOURCE=ui\mouse.cpp
# End Source File
# Begin Source File

SOURCE=ui\mouse.hpp
# End Source File
# Begin Source File

SOURCE=ui\ui.cpp
# End Source File
# Begin Source File

SOURCE=ui\ui.hpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\network\netutil.cpp
# End Source File
# Begin Source File

SOURCE=.\network\netutil.hpp
# End Source File
# Begin Source File

SOURCE=network\network.cpp
# End Source File
# Begin Source File

SOURCE=network\network.hpp
# End Source File
# Begin Source File

SOURCE=network\socket_client.cpp
# End Source File
# Begin Source File

SOURCE=network\socket_client.hpp
# End Source File
# Begin Source File

SOURCE=network\socket_server.cpp
# End Source File
# Begin Source File

SOURCE=network\socket_server.hpp
# End Source File
# End Group
# Begin Group "memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\memory\counter.h
# End Source File
# Begin Source File

SOURCE=.\memory\hirestimer.h
# End Source File
# Begin Source File

SOURCE=.\memory\ptrs.h
# End Source File
# Begin Source File

SOURCE=.\memory\timer.h
# End Source File
# End Group
# Begin Group "plp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\plp\extinction_coef.cpp
# End Source File
# Begin Source File

SOURCE=.\plp\extinction_coef.hpp
# End Source File
# Begin Source File

SOURCE=.\plp\extinction_coef.ipp
# End Source File
# Begin Source File

SOURCE=.\plp\extinction_coef_set.cpp
# End Source File
# Begin Source File

SOURCE=.\plp\extinction_coef_set.hpp
# End Source File
# Begin Source File

SOURCE=.\plp\front.cpp
# End Source File
# Begin Source File

SOURCE=.\plp\front.hpp
# End Source File
# Begin Source File

SOURCE=.\plp\front.ipp
# End Source File
# Begin Source File

SOURCE=.\plp\plp.cpp
# End Source File
# Begin Source File

SOURCE=.\plp\plp.hpp
# End Source File
# Begin Source File

SOURCE=.\plp\plp.ipp
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\util\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\util\timer.hpp
# End Source File
# Begin Source File

SOURCE=.\util\timer.ipp
# End Source File
# Begin Source File

SOURCE=.\util\util.cpp
# End Source File
# Begin Source File

SOURCE=.\util\util.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=gtb.cpp
# ADD CPP /Yc"gtb/gtb.hpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=common.hpp
# End Source File
# Begin Source File

SOURCE=gtb.hpp
# End Source File
# Begin Source File

SOURCE=.\stlext.h
# End Source File
# Begin Source File

SOURCE=unix.hpp
# End Source File
# Begin Source File

SOURCE=windows.hpp
# End Source File
# End Group
# Begin Group "replace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\replace\replace.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
