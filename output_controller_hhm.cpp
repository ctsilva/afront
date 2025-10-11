#include "common.h"
#include "output_controller_hhm.h"

void OutputControllerHHM::AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary)
{
    if (fout) {
		// 0 index not allowed? increase all indiced by 1 for .m files
		(*fout) << "Vertex  "<<(index+1)<<" "<<p[0]<<" "<<p[1]<<" "<<p[2];
		if (vkey)
			(*fout) <<" "<<vkey;

		(*fout)<<endl;
    }

    if (child)
	child->AddVertex(index, p, n, boundary);
}

void OutputControllerHHM::AddTriangle(int index, int v1, int v2, int v3)
{

    if (v1==v2 || v1==v3 || v2==v3)
	cerr<<"bogus triangle"<<endl;
    if (fout) {
	// 0 index not allowed? increase all indiced by 1 for .m files
	(*fout) << "Face  "<<(index+1)<<" "<<(v1+1)<<" "<<(v2+1)<<" "<<(v3+1)<<endl;
    }

    if (child)
	child->AddTriangle(index, v1, v2, v3);
}

