
#ifndef _TRIANGULATE_MESH
#define	_TRIANGULATE_MESH

#include <viewer/debug_draw.h>
#include "common.h"
#include "guidance.h"
#include "triangulator.h"

class MeshProjector : public SurfaceProjector {
    public:
    // stuff for the kdtree
    typedef ::Box3 Box3;
    typedef ::Point3 Point3;

    MeshProjector(const TriangleMesh &mesh, const vector<int> *pointsides=NULL);
    virtual ~MeshProjector();
    int ProjectPoint(const FrontElement &base1, const FrontElement &base2, 
		     const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const;


    Box3 bounding_box(int face) const;
    real_type distance(int face, const Point3 &from) const;


    // this is a simple closest point projection
    int ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const;
    typedef gtb::BoxKDTree<int, MeshProjector> kdtree_type;
    kdtree_type *GetKdTree();

    
    void FindSoupBoundaries(vector< vector<int> > &boundaries, real_type d) const;

    private:

    const TriangleMesh &mesh;
    kdtree_type *kdtree;
};





class MeshGuidanceField : public GuidanceField {
    public:

    MeshGuidanceField(int curv_sub, const TriangleMesh &mesh, real_type rho, real_type min_step, real_type max_step, real_type reduction);
    virtual ~MeshGuidanceField();

    int ClosestPoint(const Point3 &p);
    void OrderedPointTraverseStart(const Point3 &p);
    int OrderedPointTraverseNext(real_type &squared_dist);
    void OrderedPointTraverseEnd();
    const Point3& PointLocation(int i) const;
    int NumPoints() const;

    void Extract(vector<Point3> &pts, vector<Vector3> &norms, vector<real_type> &rad);

    class GetPointMesh {
	public:
	const TriangleMesh& mesh;
	GetPointMesh(const TriangleMesh& m) : mesh(m) {}
	const Point3& operator()(unsigned idx) const {
	    return mesh.verts[idx].point;
	}
    };
    typedef gtb::KDTree<int, real_type, GetPointMesh> kdtree_type;
    kdtree_type *GetKdTree();


    private:

    void MeanCurvaturesToSaliency(real_type rad);
    void MeanCurvaturesToSaliencyParallel(int nt, int id, real_type rad, const real_type rads[], vector<real_type> ave_curvatures[][2]);

    kdtree_type *kdtree;
    GetPointMesh kdGetPoint;
    kdtree_type::OrderedIncrementalTraverse *kdOrderedTraverse;
};



// compute curvature of a space curve by fitting a quadratic polynomial
real_type edge_curvature(const vector<Point3> &points, const vector<real_type> &weights);



#endif

