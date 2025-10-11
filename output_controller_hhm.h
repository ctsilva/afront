#ifndef __OUTPUT_CONTROLLER_HHM_H
#define __OUTPUT_CONTROLLER_HHM_H

#include "triangulator.h"
#include <fstream>

class OutputControllerHHM : public OutputController
{
public:
    OutputControllerHHM(const char *filename):
      fout(filename?new std::ofstream(filename):NULL), vkey(NULL) {};
    virtual ~OutputControllerHHM() { delete fout; };
    virtual void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary);
    virtual void AddTriangle(int index, int v1, int v2, int v3);
    virtual void Finish() { if (child) child->Finish(); }

	void SetVertexKey(const char *key) { vkey=key; }

protected:
    std::ofstream *fout;

	const char *vkey;
};

#endif
