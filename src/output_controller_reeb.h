#ifndef __OUTPUT_CONTROLLER_REEB_H_1414_20051109
#define __OUTPUT_CONTROLLER_REEB_H_1414_20051109

#include "triangulator.h"
#include <fstream>
#include "rg.h"
#include <map>
#include <string>

typedef float (*MorseFunction)(const Point3&);

class OutputControllerReeb : public OutputController
{
public:
    enum Function { FunctionX, FunctionY, FunctionZ, RadialX };
    OutputControllerReeb(Function f, const char *filename);
    ~OutputControllerReeb();
    virtual void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary);
    virtual void AddTriangle(int index, int v1, int v2, int v3);
    virtual void Finish() { if (child) child->Finish(); }
    virtual void Dump();
    

protected:
    ReebGraph _graph;
    MorseFunction _function;

    std::map<int, uint> _remap;
    std::map<uint, Point3> _points;
    std::string _filename;
};

#endif
