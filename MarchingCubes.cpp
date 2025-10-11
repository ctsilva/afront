
#include "common.h"
#include "guidance.h"
#include "triangulator.h"
#include "triangulate_iso.h"


#define	MARCH_NOP		0
#define MARCH_INVERT	1
#define MARCH_ROTATE_X	2
#define MARCH_ROTATE_Y	3
#define MARCH_ROTATE_Z	4



int mc_transforms[48][7] =
{

	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		},

	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		},

	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		},

	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		},

	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		},

	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		},
	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		},


	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_INVERT,		},

	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_INVERT,		},

	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_INVERT,		},

	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_Y,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_INVERT,		},

	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_INVERT,		},

	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_NOP,		MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_NOP,		MARCH_INVERT,		},
	{ MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_Z,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_ROTATE_X,	MARCH_INVERT,		},
};


int mc_transform_index(int index, int trans)
{
	int ret = 0;

	switch (trans)
	{
	case MARCH_NOP:
		ret = index;
		break;

	case MARCH_INVERT:
		ret = ~index;
		break;

	case MARCH_ROTATE_X:
		if (index & (1<<0)) ret |= (1<<3);
		if (index & (1<<1)) ret |= (1<<2);
		if (index & (1<<2)) ret |= (1<<6);
		if (index & (1<<3)) ret |= (1<<7);
		if (index & (1<<4)) ret |= (1<<0);
		if (index & (1<<5)) ret |= (1<<1);
		if (index & (1<<6)) ret |= (1<<5);
		if (index & (1<<7)) ret |= (1<<4);
		break;

	case MARCH_ROTATE_Y:
		if (index & (1<<0)) ret |= (1<<4);
		if (index & (1<<1)) ret |= (1<<0);
		if (index & (1<<2)) ret |= (1<<3);
		if (index & (1<<3)) ret |= (1<<7);
		if (index & (1<<4)) ret |= (1<<5);
		if (index & (1<<5)) ret |= (1<<1);
		if (index & (1<<6)) ret |= (1<<2);
		if (index & (1<<7)) ret |= (1<<6);
		break;

	case MARCH_ROTATE_Z:
		if (index & (1<<0)) ret |= (1<<1);
		if (index & (1<<1)) ret |= (1<<2);
		if (index & (1<<2)) ret |= (1<<3);
		if (index & (1<<3)) ret |= (1<<0);
		if (index & (1<<4)) ret |= (1<<5);
		if (index & (1<<5)) ret |= (1<<6);
		if (index & (1<<6)) ret |= (1<<7);
		if (index & (1<<7)) ret |= (1<<4);
		break;

	default:
		cerr<<"mc_transform_index: unknown transform "<<trans<<endl;
		ret = index;
	}

	return ret;
}


void mc_transform_tri(int *tris, int num, int trans)
{
	int i, tmp;
	int xrot[12] = { 8, 5, 0, 4, 11, 9, 1, 3, 10, 6, 2, 7 };
	int yrot[12] = { 5, 9, 6, 1, 0, 8, 10, 2, 4, 11, 7, 3 };
	int zrot[12] = { 3, 0, 1, 2, 7, 4, 5, 6, 11, 8, 9, 10 };


	switch (trans)
	{
	case MARCH_NOP:
		break;

	case MARCH_INVERT:
		for (i=0; i<(num*3); i+=3)
		{
			tmp = tris[i];
			tris[i] = tris[i+1];
			tris[i+1] = tmp;
		}
		break;

	case MARCH_ROTATE_X:
		for (i=0; i<(num*3); i++)
			tris[i] = xrot[tris[i]];
		break;

	case MARCH_ROTATE_Y:
		for (i=0; i<(num*3); i++)
			tris[i] = yrot[tris[i]];
		break;

	case MARCH_ROTATE_Z:
		for (i=0; i<(num*3); i++)
			tris[i] = zrot[tris[i]];
		break;

	default:
		cerr<<"transform_tri: unknown transform "<<trans<<endl;
	}
}


bool mc_testmatch(int index, int trans, bool extra, int *tris, int *ntris)
{
	int i;

	if ((index == 0) || (index == 255))
	{
		(*ntris) = 0;
		return true;
	}

	for (i=0; i<7; i++)
		index = mc_transform_index(index, mc_transforms[trans][i]);

	(*ntris) = 0;


	if (true)
	{
		if (index == 0)
		{
			(*ntris) = 0;
			return true;
		}

		else if (index == (1<<4))
		{
			(*ntris) = 1;

			tris[0] = 4;
			tris[1] = 11;
			tris[2] = 8;
			return true;
		}

		else if (index == ((1<<4) | (1<<5)))
		{
			(*ntris) = 2;

			tris[0] = 4;
			tris[1] = 11;
			tris[2] = 9;

			tris[3] = 4;
			tris[4] = 9;
			tris[5] = 5;
			return true;
		}

		else if (index == ((1<<4) | (1<<6)))
		{
			(*ntris) = 2;

			tris[0] = 4;
			tris[1] = 11;
			tris[2] = 8;

			tris[3] = 6;
			tris[4] = 9;
			tris[5] = 10;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<5)))
		{
			(*ntris) = 3;

			tris[0] = 4;
			tris[1] = 8;
			tris[2] = 9;

			tris[3] = 4;
			tris[4] = 9;
			tris[5] = 3;

			tris[6] = 3;
			tris[7] = 9;
			tris[8] = 1;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<4) | (1<<5)))
		{
			(*ntris) = 2;

			tris[0] = 11;
			tris[1] = 1;
			tris[2] = 3;

			tris[3] = 11;
			tris[4] = 9;
			tris[5] = 1;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<5) | (1<<7)))
		{
			(*ntris) = 4;

			tris[0] = 4;
			tris[1] = 8;
			tris[2] = 9;

			tris[3] = 4;
			tris[4] = 9;
			tris[5] = 3;

			tris[6] = 3;
			tris[7] = 9;
			tris[8] = 1;

			tris[9 ] = 11;
			tris[10] = 7;
			tris[11] = 10;
			return true;
		}

		else if (index == ((1<<1) | (1<<3) | (1<<4) | (1<<6)))
		{
			(*ntris) = 4;

			tris[0] = 11;
			tris[1] = 8;
			tris[2] = 4;

			tris[3] = 0;
			tris[4] = 5;
			tris[5] = 1;

			tris[6] = 2;
			tris[7] = 7;
			tris[8] = 3;

			tris[9 ] = 6;
			tris[10] = 9;
			tris[11] = 10;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<3) | (1<<4)))
		{
			(*ntris) = 4;

			tris[0] = 11;
			tris[1] = 2;
			tris[2] = 7;

			tris[3] = 11;
			tris[4] = 8;
			tris[5] = 2;

			tris[6] = 2;
			tris[7] = 8;
			tris[8] = 1;

			tris[9 ] = 1;
			tris[10] = 8;
			tris[11] = 5;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<3) | (1<<5)))
		{
			(*ntris) = 4;

			tris[0] = 7;
			tris[1] = 4;
			tris[2] = 8;

			tris[3] = 7;
			tris[4] = 8;
			tris[5] = 1;

			tris[6] = 7;
			tris[7] = 1;
			tris[8] = 2;

			tris[9] = 8;
			tris[10] = 9;
			tris[11] = 1;


			return true;
		}

		else if (index == ((1<<2) | (1<<4)))
		{
			(*ntris) = 2;

			tris[0] = 11;
			tris[1] = 8;
			tris[2] = 4;

			tris[3] = 6;
			tris[4] = 2;
			tris[5] = 1;
			return true;
		}

		else if (index == ((1<<2) | (1<<4) | (1<<5)))
		{
			(*ntris) = 3;

			tris[0] = 6;
			tris[1] = 2;
			tris[2] = 1;

			tris[3] = 11;
			tris[4] = 9;
			tris[5] = 4;

			tris[6] = 4;
			tris[7] = 9;
			tris[8] = 5;
			return true;
		}

		else if (index == ((1<<2) | (1<<5) | (1<<7)))
		{
			(*ntris) = 3;

			tris[0] = 6;
			tris[1] = 2;
			tris[2] = 1;

			tris[3] = 10;
			tris[4] = 11;
			tris[5] = 7;

			tris[6] = 8;
			tris[7] = 9;
			tris[8] = 5;
			return true;
		}

		else if (index == ((1<<1) | (1<<2) | (1<<4) | (1<<7)))
		{
			(*ntris) = 4;

			tris[0] = 2;
			tris[1] = 0;
			tris[2] = 6;

			tris[3] = 6;
			tris[4] = 0;
			tris[5] = 5;

			tris[6] = 7;
			tris[7] = 10;
			tris[8] = 4;

			tris[9 ] = 4;
			tris[10] = 10;
			tris[11] = 8;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<2) | (1<<4)))
		{
			(*ntris) = 4;

			tris[0] = 11;
			tris[1] = 8;
			tris[2] = 3;

			tris[3] = 3;
			tris[4] = 8;
			tris[5] = 6;

			tris[6] = 6;
			tris[7] = 8;
			tris[8] = 5;

			tris[9 ] = 3;
			tris[10] = 6;
			tris[11] = 2;
			return true;
		}

//		else
//			return false;
	}

	if (extra)
	{
		if (index == ((1<<0) | (1<<1) | (1<<3) | (1<<4) | (1<<5)))
		{
			(*ntris) = 3;

			tris[0] = 7;
			tris[1] = 11;
			tris[2] = 9;

			tris[3] = 7;
			tris[4] = 9;
			tris[5] = 1;

			tris[6] = 7;
			tris[7] = 1;
			tris[8] = 2;
			return true;
		}

		else if (index == ((1<<0) | (1<<2) | (1<<3) | (1<<4) | (1<<5)))
		{
			(*ntris) = 5;

			tris[0] = 1;
			tris[1] = 6;
			tris[2] = 0;

			tris[3] = 6;
			tris[4] = 11;
			tris[5] = 0;

			tris[6] = 0;
			tris[7] = 11;
			tris[8] = 9;

			tris[9 ] = 0;
			tris[10] = 9;
			tris[11] = 5;

			tris[12] = 7;
			tris[13] = 11;
			tris[14] = 6;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5)))
		{
			(*ntris) = 2;

			tris[0] = 7;
			tris[1] = 11;
			tris[2] = 9;

			tris[3] = 7;
			tris[4] = 9;
			tris[5] = 6;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<3) | (1<<4) | (1<<6)))
		{
			(*ntris) = 5;

			tris[0] = 11;
			tris[1] = 8;
			tris[2] = 7;

			tris[3] = 7;
			tris[4] = 8;
			tris[5] = 2;

			tris[6] = 2;
			tris[7] = 8;
			tris[8] = 5;

			tris[9 ] = 2;
			tris[10] = 5;
			tris[11] = 1;

			tris[12] = 9;
			tris[13] = 10;
			tris[14] = 6;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<3) | (1<<4) | (1<<5) | (1<<6)))
		{
			(*ntris) = 4;

			tris[0] = 7;
			tris[1] = 11;
			tris[2] = 2;

			tris[3] = 2;
			tris[4] = 11;
			tris[5] = 1;

			tris[6] = 1;
			tris[7] = 11;
			tris[8] = 10;

			tris[9 ] = 1;
			tris[10] = 10;
			tris[11] = 6;
			return true;
		}

		else if (index == ((1<<0) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6)))
		{
			(*ntris) = 2;

			tris[0] = 7;
			tris[1] = 11;
			tris[2] = 10;

			tris[3] = 0;
			tris[4] = 1;
			tris[5] = 5;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6)))
		{
			(*ntris) = 1;

			tris[0] = 7;
			tris[1] = 11;
			tris[2] = 10;
			return true;
		}

		else if (index == ((1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7)))
		{
			(*ntris) = 0;
			return true;
		}

	}

	return false;
}


int mc_xyz2index(const RegularVolume &v, int x, int y, int z) {
  return (z*(v.GetDim(0)+1)*(v.GetDim(1)+1) + y*(v.GetDim(0)+1) + x);
}

void MarchingCubes(const RegularVolume &v, TriangulatorController &tc, real_type isovalue) {

    int num_verts=0;
    int num_tris =0;
    int *vertindices = new int[(v.GetDim(0)+1)*(v.GetDim(1)+1)*(v.GetDim(2)+1) * 3];

    // find all the vertices (on the edges) 
    cerr<<"Finding verts and normals...";
    for (int z=v.GetBoundaryCells(); z<v.GetDim(2)-v.GetBoundaryCells()+0; z++) {
	for (int y=v.GetBoundaryCells(); y<v.GetDim(1)-v.GetBoundaryCells()+0; y++) {
	    for (int x=v.GetBoundaryCells(); x<v.GetDim(0)-v.GetBoundaryCells()+0; x++) {
		// x-edge
		if ((x != v.GetDim(0)-1) && (v.GetValue(x,y,z)<isovalue) ^ (v.GetValue(x+1,y,z)<isovalue)) {
		    real_type s = (isovalue - v.GetValue(x,y,z)) / (v.GetValue(x+1,y,z) - v.GetValue(x,y,z));
		    tc.AddVertex(num_verts, Point3(v.GetAspect(0)*(x+s),v.GetAspect(1)*y,v.GetAspect(2)*z), Vector3(0,0,0), false);
		    vertindices[mc_xyz2index(v,x,y,z)*3+0] = num_verts;
		    num_verts++;
		} else {
		    vertindices[mc_xyz2index(v,x,y,z)*3+0] = -1;
		}

		// y-edge
		if ((y != v.GetDim(1)-1) && (v.GetValue(x,y,z)<isovalue) ^ (v.GetValue(x,y+1,z)<isovalue)) {
		    real_type s = (isovalue - v.GetValue(x,y,z)) / (v.GetValue(x,y+1,z) - v.GetValue(x,y,z));
		    tc.AddVertex(num_verts, Point3(v.GetAspect(0)*x,v.GetAspect(1)*(y+s),v.GetAspect(2)*z), Vector3(0,0,0), false);
		    vertindices[mc_xyz2index(v,x,y,z)*3+1] = num_verts;
		    num_verts++;
		} else {
		    vertindices[mc_xyz2index(v,x,y,z)*3+1] = -1;
		}


		// z-edge
		if ((z != v.GetDim(2)-1) && (v.GetValue(x,y,z)<isovalue) ^ (v.GetValue(x,y,z+1)<isovalue)) {
		    real_type s = (isovalue - v.GetValue(x,y,z)) / (v.GetValue(x,y,z+1) - v.GetValue(x,y,z));
		    tc.AddVertex(num_verts, Point3(v.GetAspect(0)*x,v.GetAspect(1)*y,v.GetAspect(2)*(z+s)), Vector3(0,0,0), false);
		    vertindices[mc_xyz2index(v,x,y,z)*3+2] = num_verts;
		    num_verts++;
		} else {
		    vertindices[mc_xyz2index(v,x,y,z)*3+2] = -1;
		}
	    }
	}
    }
    cerr<<"OK"<<endl;

    // find all the triangles
    cerr<<"finding triangles...";
    for (int z=v.GetBoundaryCells(); z<v.GetDim(2)-1-v.GetBoundaryCells(); z++)
	{
	    for (int y=v.GetBoundaryCells(); y<v.GetDim(1)-1-v.GetBoundaryCells(); y++)
		{
		    for (int x=v.GetBoundaryCells(); x<v.GetDim(0)-1-v.GetBoundaryCells(); x++)
			{
			    int index = 0;
			    if (v.GetValue(x  ,y  ,z  )>isovalue)	index |= 1<<0;
			    if (v.GetValue(x+1,y  ,z  )>isovalue)	index |= 1<<1;
			    if (v.GetValue(x+1,y+1,z  )>isovalue)	index |= 1<<2;
			    if (v.GetValue(x  ,y+1,z  )>isovalue)	index |= 1<<3;
			    if (v.GetValue(x  ,y  ,z+1)>isovalue)	index |= 1<<4;
			    if (v.GetValue(x+1,y  ,z+1)>isovalue)	index |= 1<<5;
			    if (v.GetValue(x+1,y+1,z+1)>isovalue)	index |= 1<<6;
			    if (v.GetValue(x  ,y+1,z+1)>isovalue)	index |= 1<<7;


			    // first check the extra cases with no inversion
			    bool vol_march_found = false;

			    int n_tris;
			    int tris[5*3];


			    int trans;
			    for (trans=0; trans<24; trans++)
				{
				    if (mc_testmatch(index, trans, true, tris, &n_tris))
					{
					    vol_march_found=true; break;
					}
				}
			    
			    // check the reglar cases, and do the inversions
			    if (!vol_march_found) {
				for (trans = 0; trans<48; trans++)
				    {
					if (mc_testmatch(index, trans, false, tris, &n_tris))
					    {
						vol_march_found=true; break;
					    }
				    }
			    }

			    if (!vol_march_found)
				cerr<<"didn't find match for cube!"<<endl;



			    // rotate all the tris back
			    for (int t=6; t>=0; t--)
				{
				    mc_transform_tri(tris, n_tris, mc_transforms[trans][t]);
				}

			    for (int t=0; t<(n_tris*3); t++)
				{
				    switch (tris[t])
					{
					case 0:	tris[t] = vertindices[mc_xyz2index(v,x  ,y  ,z  )*3+0];	break;
					case 1:	tris[t] = vertindices[mc_xyz2index(v,x+1,y  ,z  )*3+1];	break;
					case 2:	tris[t] = vertindices[mc_xyz2index(v,x  ,y+1,z  )*3+0];	break;
					case 3:	tris[t] = vertindices[mc_xyz2index(v,x  ,y  ,z  )*3+1];	break;
					case 4:	tris[t] = vertindices[mc_xyz2index(v,x  ,y  ,z  )*3+2];	break;
					case 5:	tris[t] = vertindices[mc_xyz2index(v,x+1,y  ,z  )*3+2];	break;
					case 6:	tris[t] = vertindices[mc_xyz2index(v,x+1,y+1,z  )*3+2];	break;
					case 7:	tris[t] = vertindices[mc_xyz2index(v,x  ,y+1,z  )*3+2];	break;
					case 8:	tris[t] = vertindices[mc_xyz2index(v,x  ,y  ,z+1)*3+0];	break;
					case 9:	tris[t] = vertindices[mc_xyz2index(v,x+1,y  ,z+1)*3+1];	break;
					case 10:tris[t] = vertindices[mc_xyz2index(v,x  ,y+1,z+1)*3+0];	break;
					case 11:tris[t] = vertindices[mc_xyz2index(v,x  ,y  ,z+1)*3+1];	break;
					}
				}

			    for (int t=0; t<n_tris*3; t+=3) {
				tc.AddTriangle(num_tris++, tris[t+0], tris[t+1], tris[t+2]);
			    }
			}
		}
	}
    cerr<<"OK"<<endl;

    delete [] vertindices;

}

