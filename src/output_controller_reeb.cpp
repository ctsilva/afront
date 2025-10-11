#include "common.h"
#include "output_controller_reeb.h"
#include <utility>
#include <algorithm>

using namespace std;

static float function_x(const Point3 &p)
{
    return p[0];
}
static float function_y(const Point3 &p)
{
    return p[1];
}
static float function_z(const Point3 &p)
{
    return p[2];
}
static float radial_x(const Point3 &p)
{
    Point3 c=p;
    c[0]=0;
    return (c-Point3::ZERO).length();
}


OutputControllerReeb::OutputControllerReeb(Function f, const char *filename)
{
    switch (f) {
    case FunctionX: _function = function_x; break;
    case FunctionY: _function = function_y; break;
    case FunctionZ: _function = function_z; break;
    case RadialX:   _function = radial_x; break;
    }
    _filename = string(filename);
}

OutputControllerReeb::~OutputControllerReeb()
{
}

void OutputControllerReeb::AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary)
{
    uint idx = _graph.addVertex(index, p[0], p[1], p[2], _function(p));
    _points[idx] = p;
    _remap[index] = idx;

    if (child)
	child->AddVertex(index, p, n, boundary);
}

void OutputControllerReeb::AddTriangle(int index, int v1, int v2, int v3)
{
    pair<float, unsigned> v[3] = { 
	make_pair(_function(_points[_remap[v1]]), _remap[v1]), 
	make_pair(_function(_points[_remap[v2]]), _remap[v2]),
 	make_pair(_function(_points[_remap[v3]]), _remap[v3])
    };
    sort(v, v+3);

    unsigned
	n0 = v[0].second,
	n1 = v[1].second,
	n2 = v[2].second;

    uint64 Label01=((uint64)n0) | (((uint64)n1)<<32);
    uint64 Label12=((uint64)n1) | (((uint64)n2)<<32);
    uint64 Label02=((uint64)n0) | (((uint64)n2)<<32);		

    if (!_graph.findUpLabel(n0,Label01))
	{uint N01[]={n0,n1};_graph.addPath(2,N01,Label01);}
    
    if (!_graph.findUpLabel(n1,Label12))
	{uint N12[]={n1,n2};_graph.addPath(2,N12,Label12);}
    
    if (!_graph.findUpLabel(n0,Label02))
	{uint N02[]={n0,n2};_graph.addPath(2,N02,Label02);}
    
    _graph.collapse(n0,n1,Label01,Label02);
    _graph.collapse(n1,n2,Label12,Label02);


    if (child)
	child->AddTriangle(index, v1, v2, v3);
}

void OutputControllerReeb::Dump()
{
    map<uint, Point3>::iterator
	b = _points.begin(),
	e = _points.end();
    for (;b!=e;++b) {
	_graph.endVertex(b->first);
    }
    FILE *f = fopen(_filename.c_str(), "w");
    _graph.terminate();
    _graph.print(f);
    fclose(f);
}
