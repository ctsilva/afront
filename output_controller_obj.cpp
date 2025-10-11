#include "common.h"
#include "output_controller_obj.h"

void OutputControllerOBJ::AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary)
{
    if (fout) {
	// 0 index not allowed? increase all indiced by 1 for .m files
	(*fout) << "v "<<p[0]<<" "<<p[1]<<" "<<p[2]<<endl;
	(*fout) << "vn "<<n[0]<<" "<<n[1]<<" "<<n[2]<<endl;
    }

    if (child)
	child->AddVertex(index, p, n, boundary);
}

void OutputControllerOBJ::AddTriangle(int index, int v1, int v2, int v3)
{
    if (fout) {
	// 0 index not allowed? increase all indiced by 1 for .m files
	(*fout) << "f "<<v1<<" "<<v2<<" "<<v3<<endl;
    }

    if (child)
	child->AddTriangle(index, v1, v2, v3);
}

