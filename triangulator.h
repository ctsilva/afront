#ifndef _TRIANGULATOR_H
#define _TRIANGULATOR_H

#include "front.h"
#include "uheap.h"
#include <gtb/gtb.hpp>
#include "guidance.h"


#define PROJECT_INCOMPLETE     -1       // projector thread hasn't gotten around to it yet
#define PROJECT_SUCCESS		0
#define PROJECT_FAILURE		1
#define PROJECT_BOUNDARY	2	// failed because we tried crossing a boundary


// this interface must be implemented and passed to the triangulator
// this stuff depends on the type of surface you are working on
class TriangulatorController {
    public:
    virtual ~TriangulatorController() {};

    // functions for storing the mesh as it's created
    // may want to save it for debugging drawing, or just write it out as it is created for more streamlike precessing
    virtual void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary) = 0;
    virtual void AddTriangle(int index, int v1, int v2, int v3) = 0;
    virtual void Finish() = 0;

    // how far are we allowed to step from this spot?
    virtual real_type MaxStepLength(const Point3 &p) = 0;

    // project a point onto the surface
    virtual int ProjectPoint(const FrontElement &base1, const FrontElement &base2, const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const = 0;

    // get how big the fence should be to guarantee that there is no front crossing
    virtual real_type GetFenceScale() = 0;

};

// project a point onto the surface
class SurfaceProjector 
{
    public:
    virtual ~SurfaceProjector() {};
    virtual int ProjectPoint(const FrontElement &base1, const FrontElement &base2, const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const = 0;
    virtual int ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const = 0;
};




////////////////////////////////////////////////////////////////////////////////

class OutputController
{
    public:
    OutputController() { child=NULL; }
    virtual ~OutputController() { if (child) delete child; }

    virtual void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary) = 0;
    virtual void AddTriangle(int index, int v1, int v2, int v3) = 0;
    virtual void Finish() = 0;


    static void AddControllerToBack(OutputController *&h, OutputController *c) {
	if (!h) {
	    h = c;
	} else {
	    AddControllerToBack(h->child, c);
	}
    }
    OutputController *child;
};



class OutputControllerITS : public OutputController
{
    public:
    OutputControllerITS() { }
    virtual ~OutputControllerITS() { };

    void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary) {
	triangulation.add_vertex(p);
	triangulation.insert_vertex_normal(n);

	if (child)
	    child->AddVertex(index, p, n, boundary);
    }
  
    void AddTriangle(int index, int v1, int v2, int v3) {
	triangulation.insert_triangle((unsigned)v1, (unsigned)v2, (unsigned)v3, false, 0.0);

	if (child)
	    child->AddTriangle(index, v1, v2, v3);
    }

    void Finish() {
	if (child)
	    child->Finish();
    }

    IndexedTriangleSet triangulation;
};


////////////////////////////////////////////////////////////////////////////////
// wrapper to allow different implementations of output, guidance field, and surface

class ControllerWrapper : public TriangulatorController 
{
    public:
    ControllerWrapper(GuidanceField *g, SurfaceProjector *p, OutputController *oc)
	: TriangulatorController(),
	guidance(g), projector(p), output_c(oc) { }

    virtual ~ControllerWrapper() {
    }

    void SetProjector(SurfaceProjector *p) {
	projector = p;
    }


    void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary) {
	output_c->AddVertex(index, p, n, boundary);
    }

    void AddTriangle(int index, int v1, int v2, int v3) {
	output_c->AddTriangle(index, v1, v2, v3);
    }

    void Finish() {
	output_c->Finish();
    }


    real_type MaxStepLength(const Point3 &p) {
	return guidance->MaxStepLength(p);
    }



    int ProjectPoint(const FrontElement &base1, const FrontElement &base2, const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const {
	return projector->ProjectPoint(base1, base2, fp, fn, tp, tn);
    }



    real_type GetFenceScale() { return guidance->GetFenceScale(); }


    private:

    GuidanceField *guidance;
    SurfaceProjector *projector;
    OutputController* output_c;
};



class Triangulator {

    public:
    // stuff needed by the kd tree
    typedef ::Box3 Box3;
    typedef ::Point3 Point3;


    typedef Front::feli feli;
    typedef gtb::BoxKDTree<feli, Triangulator> kdtree_type;
    typedef UHeap<feli> heap_type;


    Triangulator(TriangulatorController &c);
    ~Triangulator();


    void InsertSubMesh(const TriangleMesh &mesh, const vector<int> &sides, int sidetoadd);
    /*!
     * Triangulate a region
     * Input:
     *    ipts - set of points that form the initial front?
     *    inorms - their normals.
     */
    void Go
	(const vector< vector<Point3> > &ipts, const vector< vector<Vector3> > &inorms, 
	 bool failsafe,
	 const vector<Point3> &crease_points,
	 const vector<Vector3> &crease_onormals,
	 const vector< vector<int> > &crease_indices,
	 const vector< vector<Vector3> > &crease_normals,
	 const vector< int > &corner_triangles);
    void Go(const vector< vector<Point3> > &ipts, const vector< vector<Vector3> > &inorms, bool dofailsafe);
    void ExtractFronts(vector< vector<Point3> > &fpts,
		       vector< vector<Vector3> > &fnorms,
		       vector< vector<int> > &states,
		       vector< vector<real_type> > &fenceheight);


    Box3 bounding_box(const feli &i) const;
    real_type distance(const feli &i, const Point3 &from) const;

    void SetFlipOutput(bool f) { flipOutput=f; }




    private:

    void CreateVertex(const Point3 &p, const Vector3 &n, FrontElement &fe, bool boundary);
    void CreateTriangle(const feli v1, const feli v2, const feli v3);
    void CreateTriangle(int v1, int v2, int v3);

    void PrioritizeEdgeGrow(feli e1);
    void PrioritizeEdgeConnect(feli e1);
    void PrioritizeEdgeFailsafe(feli e);

    feli ClosestPoint(const Point3 &p, feli skip1, feli skip2);
    bool GetConnection(feli e1, feli &across);
    real_type CosSmallestRemainingAngle(feli e1, feli e2, feli across);


    void GetTentativePoint(const FrontElement &p1, const FrontElement &p2, Point3 &tentative_p, Vector3 &tentative_n);
    real_type PrioritizeConnection(const FrontElement &p1, const FrontElement &p2, const FrontElement &p3);
    real_type PrioritizeEdgeGrow(const FrontElement &p1, const FrontElement &p2);


    bool IntersectsFence(const feli &e, const Point3 &p1, const Point3 &p2) const;
    bool InLegalFenceCorner(const feli &e, const Point3 &p) const;
    real_type DistanceToFence(const feli &e1, const Point3 &p) const;
    real_type DistanceToFence(const Point3 &ep1, const Point3 &ep2, const Vector3 &en1, const Vector3 &en2, const Point3 &p) const;
    bool GrowthFromEdgeLegal(const feli e1, const Point3 &p) const;
    bool TriangleLegal(const feli e1, const feli e2, const feli *across, const Point3 &across_p, const Vector3 &across_n, vector<feli> *possible_intersects, bool *isclose) const;

    void ConnectTriangle(feli e1, feli across, bool dofailsafe);
    void GrowEdge(feli e1, const Point3 &p, const Vector3 &v);

    void RequestProjection(feli e);
    void WaitForProjection(feli e);
    void CancelProjection(feli e);
    static void* ProjectorThreadMain(void *arg);

    class ProjectionWork {
	public:
	ProjectionWork() { };
	ProjectionWork(real_type _priority, ProjectionResult *_pr, const Point3 &_fp, const Vector3 &_fn, const FrontElement *_base1, const FrontElement *_base2) :
	    priority(_priority), pr(_pr), fp(_fp), fn(_fn), base1(_base1), base2(_base2) { }
	bool operator<(const ProjectionWork &rhs) const {
	    return (priority < rhs.priority);
	}
	real_type priority;
	ProjectionResult *pr;
	Point3 fp;
	Vector3 fn;
	const FrontElement *base1;
	const FrontElement *base2;
    };

    gtb::fast_pq<ProjectionWork> work_queue;
    volatile bool work_quit;
    thlib::CSObject work_cs;
    vector<thlib::Thread*> work_threads;
	

    void StartWorkerThreads();
    void StopWorkerThreads();


    void SetupInitialCreaseFronts
	(const vector<Point3> &points, // The points on the creases
	 const vector<Vector3> &onormals, // The normals for the output
	 const vector< vector<int> > &indices, // The front element point indices
	 const vector< vector<Vector3> > &normals, // The normals for the front elements
	 const vector< int > &corner_triangles); 

    void SetupInitialFronts(const vector< vector<Point3> > &ipts, const vector< vector<Vector3> > &inorms);

    void UpdateWorkingArea(feli e);
    real_type workingAreaRadius;
    Point3 workingAreaCenter;

    void VerifyFronts();

    kdtree_type kdtree;
    heap_type	heap;
    vector<Front*> fronts;

    TriangulatorController &controller;

    int numVertsAdded;
    int numFacesAdded;
    bool flipOutput;

};





#endif

