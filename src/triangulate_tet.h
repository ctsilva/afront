
#ifndef _TRIANGULATE_TET
#define	_TRIANGULATE_TET

#include <viewer/debug_draw.h>
#include "common.h"
#include "guidance.h"
#include "triangulator.h"
#include "triangulate_mesh.h"



class TetMesh {

    public:
    TetMesh() { }
    ~TetMesh() { }

    bool ReadOFF(const char *filename);

    void Clear() { verts.resize(0); tets.resize(0); }


    class Vert {
	public:
	Vert() { }
	Vert(const Point3 &p, real_type s) : point(p), scalar(s) { }
	Point3		point;
	real_type	scalar;
    };

    class Tet {
	public:
	Tet() { }
	Tet(int i0, int i1, int i2, int i3) { vi[0]=i0; vi[1]=i1; vi[2]=i2; vi[3]=i3; }
	int vi[4];
    };


    void GetShell(TriangleMesh &tm, vector<real_type> &scalars) const;
    void SmoothScalars(int passes);

    // compute the incircle / circumcircle radius ratio for the tet
    real_type radius_ratio(int tet) const;


    vector<Vert> verts;
    vector<Tet>  tets;

};



class TetMeshProjector : public SurfaceProjector {
    public:
    typedef ::Box3 Box3;
    typedef ::Point3 Point3;

    TetMeshProjector(const TetMesh &m, real_type iso);
    virtual ~TetMeshProjector() { }
    virtual bool EvalAtPoint(const Point3 &p, real_type &f, vector<int> *nbrs) const = 0;
    virtual bool NormalAtPoint(const Point3 &p, Vector3 &n) const = 0;
    virtual bool CurvatureAtPoint(const Point3 &p, real_type &k1, real_type &k2) const = 0;
    virtual int ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const = 0;
    int ProjectPoint(const FrontElement &base1, const FrontElement &base2, 
		     const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const;


    const TetMesh& GetMesh() const { return mesh; }
    real_type GetIsoValue() const { return isovalue; }

    Box3 bounding_box(int tet) const;
    real_type distance(int tet, const Point3 &from) const;

    bool PointInTet(const Point3 &p, int t) const;

    void TetBaryCoords(int t, const Point3 &x, real_type b[4]) const;
    real_type TetBaryInterp(int t, const Point3 &x, real_type f0, real_type f1, real_type f2, real_type f3) const;


    const TetMesh &mesh;
    real_type isovalue;


    // stuff for the tet kdtree
    typedef gtb::BoxKDTree<int, TetMeshProjector> kdtree_type;
    kdtree_type *kdtree;

    int GetPointTet(const Point3 &p) const;
	

    class RingEval {

	public:
	RingEval(const TetMeshProjector &t, const Point3 &c, const Vector3 &u, const Vector3 &v, vector<int> &_nbrs) :
	hit_boundary(false), tmp(t), center(c), udir(u), vdir(v), nbrs(_nbrs) { }

	real_type operator()(real_type theta) {
	    Point3 x = center + (real_type)cos(theta)*udir + (real_type)sin(theta)*vdir;
	    real_type e;
	    if (!tmp.EvalAtPoint(x, e, &nbrs)) {
		hit_boundary = true;
		return 1e34;
	    }
	    return e - tmp.isovalue;
	}

	bool hit_boundary;
	const TetMeshProjector &tmp;
	vector<int> &nbrs;
	Point3 center;
	Vector3 udir, vdir;
    };

    virtual void StartRingProjection(const Point3 &p, vector<int> &nbrs) const = 0;


    enum proj_type { TET_MESH_PROJECTOR_NIELSON, TET_MESH_PROJECTOR_MLS };
    virtual proj_type GetProjectorType() const = 0;
	
};



class TetMeshProjectorNielson : public TetMeshProjector {
    public:

    TetMeshProjectorNielson(const TetMesh &mesh, real_type iso);
    virtual ~TetMeshProjectorNielson();

    bool EvalAtPoint(const Point3 &p, real_type &f, vector<int> *nbrs) const;
    bool NormalAtPoint(const Point3 &p, Vector3 &n) const;
    bool CurvatureAtPoint(const Point3 &p, real_type &k1, real_type &k2) const;
    int ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const;

    template <typename T>
	void EvalInTet(int t, const gtb::tPoint3<T> &x, T &fx) const;

    proj_type GetProjectorType() const { return TET_MESH_PROJECTOR_NIELSON; }

    void StartRingProjection(const Point3 &p, vector<int> &nbrs) const { }


    private:

    vector<Vector3> gradients;


    template <typename T>
	static void HermiteCoefs(const T &f0, const T &d0, const T &fs, const T &ds, const T &s, T coefs[4]);

    template <typename T, typename R>
	static void HermiteEval(const T &f0, const T &d0, const T &fs, const T &ds, const T &s, const R &x, R &f);


    template <typename T>
	void EvalOnFace(const Point2 p[3],
			const Vector2 g[3],
			const real_type f[3],
			const gtb::tPoint2<T> &x,
			T &fx) const;

    template <typename T>
	void EvalInTet(const Point3 p[4],
		       const Vector3 g[4],
		       const real_type f[4],
		       const gtb::tPoint3<T> &x,
		       T &fx) const;
};




class TetMeshProjectorMLS : public TetMeshProjector {
    public:

    TetMeshProjectorMLS(const TetMesh &mesh, real_type iso);
    virtual ~TetMeshProjectorMLS();

    bool EvalAtPoint(const Point3 &p, real_type &f, vector<int> *nbrs) const;
    bool NormalAtPoint(const Point3 &p, Vector3 &n) const;
    bool CurvatureAtPoint(const Point3 &p, real_type &k1, real_type &k2) const;
    int ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const;

    proj_type GetProjectorType() const { return TET_MESH_PROJECTOR_MLS; }
    void StartRingProjection(const Point3 &p, vector<int> &nbrs) const;


    private:

    void SetRadiusParallel(int nt, int id);

    class GetPoint {
	public:
	const TetMeshProjectorMLS *mlsproj;
	GetPoint() : mlsproj(NULL) {}
	const Point3& operator()(unsigned idx) const {
	    if (idx < mlsproj->mesh.verts.size())
		return mlsproj->mesh.verts[idx].point;
	    else
		return extra_pts[idx - mlsproj->mesh.verts.size()];
	}

	real_type scalar(unsigned idx) const {
	    if (idx < mlsproj->mesh.verts.size())
		return mlsproj->mesh.verts[idx].scalar;
	    else
		return extra_scalars[idx - mlsproj->mesh.verts.size()];
	}

	real_type radius(unsigned idx) const {
	    if (idx < mlsproj->mesh.verts.size())
		return mlsproj->vert_radius[idx];
	    else
		return extra_radius[idx - mlsproj->mesh.verts.size()];
	}

	int NumPoints() const { 
	    return (mlsproj->mesh.verts.size() + extra_pts.size());
	}


	vector<Point3> extra_pts;
	vector<real_type> extra_scalars;
	vector<real_type> extra_radius;
    };

    template <typename T>
	bool EvalAtPoint(const Point3 &p, T *f, gtb::tVector3<T> *gradient, gtb::tmat3<T> *hessian, const vector<int> *nbrs=NULL) const;

    typedef gtb::KDTree<int, real_type, GetPoint> kdtree_type;
    kdtree_type *kdtree;
    GetPoint kdGetPoint;

    vector<real_type> vert_radius;
};





class TetMeshGuidanceField : public GuidanceField {
    public:

    TetMeshGuidanceField(TetMeshProjector &projector, real_type rho, real_type min_step, real_type max_step, real_type reduction);
    ~TetMeshGuidanceField();

    int ClosestPoint(const Point3 &p);
    void OrderedPointTraverseStart(const Point3 &p);
    int OrderedPointTraverseNext(real_type &squared_dist);
    void OrderedPointTraverseEnd();
    const Point3& PointLocation(int i) const;
    int NumPoints() const;

    void Extract(vector<Point3> &pts, vector<Vector3> &norms, vector<real_type> &rad);


    private:

    void BuildGuidanceParallel(int nt, int id, real_type isovalue, const TetMesh &mesh, const vector<real_type> &scalars);

    TetMeshProjector &projector;

    class GetPoint {
	public:
	vector<Point3> allpoints;
	GetPoint() {}
	const Point3& operator()(unsigned idx) const {
	    return allpoints[idx];
	}
    };

    typedef gtb::KDTree<int, real_type, GetPoint> kdtree_type;
    kdtree_type *kdtree;
    GetPoint kdGetPoint;
    kdtree_type::OrderedIncrementalTraverse *kdOrderedTraverse;

};



void MarchingTets(const TetMesh &mesh, TriangulatorController &tc, real_type isovalue);
void MarchingTris(const TriangleMesh &mesh, const vector<real_type> &scalar, real_type isovalue,
		  vector< vector<Point3> > &paths);

bool ProjectToIntersection(const SurfaceProjector  &surf1, const SurfaceProjector &surf2,
			   Point3 &p, Vector3 &norm1, Vector3 &norm2, real_type tol);

void WalkIntersectionCurve(const SurfaceProjector &p1, const SurfaceProjector &p2, 
			   const vector< vector<Point3> > &startpoints,
			   real_type stepsize,
			   vector< vector<Point3> > &out_points,
			   vector< vector<Vector3> > &out_norms1,
			   vector< vector<Vector3> > &out_norms2);

#endif
