
SUBDIRS = ../gtb ../lib/viewer ../lib/ThreadLib ../lib/mlslib ../lib/rmlslib

INCLUDES = -I../ -I../lib
LDFLAGS  = -L../gtb -L../lib/rmlslib -L../lib/mlslib -L../lib/viewer -L../lib/ThreadLib

LDADD = -lviewer -lThreadLib -lgtb -lmlslib -lrmlslib \
	-lGL -lGLU -lglut -lm -lpthread -lz

bin_PROGRAMS = afront


afront_SOURCES =	crease.cpp 			\
			main.cpp			\
			output_controller_reeb.cpp	\
			triangulate_mesh.cpp		\
			front.cpp       		\
			MarchingCubes.cpp		\
		        rg.cpp				\
			generaldef.cpp			\
			output_controller_gui.cpp	\
			triangulate_csg.cpp		\
			triangulate_mls.cpp	\
			guidance.cpp			\
			output_controller_hhm.cpp	\
			triangulate_tet.cpp		\
			lsqr.cpp			\
			output_controller_obj.cpp	\
			triangulate_iso.cpp		\
			triangulator.cpp 		\
			edgeflipper.cpp FLF_io.cpp	\
			PC_io.cpp

