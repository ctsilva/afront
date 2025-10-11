#ifndef __OUTPUT_CONTROLLER_OBJ_H
#define __OUTPUT_CONTROLLER_OBJ_H

#include "triangulator.h"
#include <fstream>

class OutputControllerOBJ : public OutputController
{
public:
    OutputControllerOBJ(const char *filename):
      fout(filename?new std::ofstream(filename):NULL) {};
    virtual ~OutputControllerOBJ() { delete fout; };
    virtual void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary);
    virtual void AddTriangle(int index, int v1, int v2, int v3);
    virtual void Finish() { if (child) child->Finish(); }


protected:
    std::ofstream *fout;
};

#endif
