
#ifndef __OUTPUT_CONTROLLER_EDGEFLIPPER_H
#define __OUTPUT_CONTROLLER_EDGEFLIPPER_H

#include "triangulator.h"



class EdgeFlipperVertex {
    public:
    
    bool boundary;
    std::set<int> incidentTriangles;
    std::set<int> incidentVerticesLocal;
    std::set<int> incidentVerticesGlobal;
    int index;  // global index

    Point3 position;
    Vector3 normal;

    bool OnFront() const {
	if (boundary) {
	    return false;
	} else {
	    return (incidentVerticesGlobal.size() != incidentTriangles.size());
	}
    }
};


class EdgeFlipperTriangle {
    public:
    int vi[3];  // local indices
    int index;  // global index
};





class OutputControllerEdgeFlipper : public OutputController
{
public:
    virtual ~OutputControllerEdgeFlipper() { }

    virtual void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary);
    virtual void AddTriangle(int index, int v1, int v2, int v3);
    virtual void Finish();

    static bool DoFlips;


protected:

 
    vector<EdgeFlipperVertex> verts;
    vector<EdgeFlipperTriangle> tris;

    std::map<int,int> global_to_local_vi;

    vector<int> available_verts;
    vector<int> available_tris;
    int AllocVert();
    int AllocTri();
    void FreeVert(int v);
    void FreeTri(int t);

    void FinishVertex(int v);
    void FinishTriangle(int t);

    bool VertexRemovable(int vi) const;
    bool TriangleFlippable(int ti) const;

    bool TryFlip(int v1, int v2);
    bool ShouldFlip(int va, int vb, int vc, int vd) const;

    void DoEdgeFlips(const vector<int> &rv);

    void Draw();


};


#endif
