#include "common.h"
#include "guidance.h"
#include "triangulator.h"
#include "triangulate_mls.h"

#include <cstdio>
#include <algorithm>

#ifdef WIN32
#define NO_RMLS
#endif

#ifndef NO_RMLS
#include <rmlslib/Primitive_Functions.h>
#endif

using namespace std;

SmoothMLSProjector::SmoothMLSProjector(surfel_set &surfels, int adamson):
    _wf(1.0f),
    _radius_wf(1.0f),
    _projector(surfels, 8, &_wf, &_radius_wf, 2, NULL),
    _adamson(adamson)
{
    _projector.compute_points_radius();
    _projector.set_radius_factor(2.0f);
}

SmoothMLSProjector::~SmoothMLSProjector()
{
}

int SmoothMLSProjector::ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const
{
    return (_projector.PowellProject(fp, tp, tn) ? PROJECT_SUCCESS : PROJECT_FAILURE);
}

int SmoothMLSProjector::ProjectPoint
    (const FrontElement &base1, const FrontElement &base2, 
     const Point3 &fp, const Vector3 &fn,
     Point3 &tp, Vector3 &tn) const
{
    extern bool rmls;
    if (rmls) {
#ifndef NO_RMLS

	real_type radius = _projector.point_radius(fp);
	gtb::surfelset_view nbhd(_projector.get_points());

	extern int rmls_knn;
	_projector.extract2(fp, rmls_knn, nbhd);

	// copy into a vector for rmls
	vector<Point> neighbors;
	for (unsigned i=0; i<nbhd.size(); i++) {
	    const Point3 &p = nbhd.vertex(i);
	    neighbors.push_back(Point(p[0], p[1], p[2]));
	}

	Point rmls_fp(fp[0], fp[1], fp[2]);
	extern real_type noise_threshold;
	Vertex res = rmls_projection(rmls_fp, neighbors, noise_threshold);
	tp = Point3(res.point_[0], res.point_[1], res.point_[2]);
	tn = Vector3(res.normal_.x(), res.normal_.y(), res.normal_.z());

	// FIXME - is it possible to fail??
#else
		cerr<<"no robust mls on windows right now"<<endl;
#endif
    } else
		if (_adamson != 0) {
	if (!_projector.adamson_projection(fp, tp, tn, _adamson))
	    return PROJECT_FAILURE;
    } else {
	if (!_projector.PowellProject(fp, tp, tn))
	    return PROJECT_FAILURE;
    }

    if (fn.dot(tn) < 0)
	tn.flip();
    if (fn.dot(tn) < 0.7) {
	cerr << "dUuuuuh" << endl;
	tn = fn;
    }
    return PROJECT_SUCCESS;
}


void SmoothMLSGuidanceField::curvatures(const Point3 &ref, Vector3 &normal, real_type &k1, real_type &k2)
{
    Point3 result;
    real_type radius = _projector.point_radius(ref);
    surfelset_view nbhd(_projector.get_points());
    _projector.extract2(ref, radius, nbhd);

    if (_adamson != 0) {
	_projector.adamson_projection(nbhd, ref, result, normal, _adamson, &k1, &k2);
    } else {
	mls::Poly2<real_type> poly;
	plane_transformation T;
	surfel_set std_points;
	if (_projector.PowellProject(nbhd, ref, result, normal, T, &poly, std_points))
	    poly.curvatures(0, 0, k1, k2);
	else
	    k1=k2=1e-5;
    }

    //	cerr<<k1<<" "<<k2<<endl;
}

SmoothMLSGuidanceField::SmoothMLSGuidanceField
    (CProjection &projector,
     real_type rho, real_type min_step, real_type max_step, real_type reduction, 
     int adamson, 
     VectorField field, float field_epsilon):
	GuidanceField(rho, min_step, max_step, reduction),
	_projector(projector),
	_kdOrderedTraverse(NULL),
	_adamson(adamson)
{
    ideal_length.resize(projector.get_points().size());
    cerr << "Point count: ";
    cerr << projector.get_points().size() << endl;


    for (unsigned i=0; i<_projector.get_points().size(); ++i) {
	real_type k1, k2;
	Vector3 normal;
	Point3 p = _projector.get_points().vertex(i);
	curvatures(p, normal, k1, k2);
	ideal_length[i] = MaxCurvatureToIdeal(k2);
	if (field) {
	    Vector3 grad_f = field(p).normalized();
	    float s = 1 - normal.dot(grad_f)*normal.dot(grad_f); // s = sin^2(angle(normal, grad_f))
	    ideal_length[i] *= max(field_epsilon * field_epsilon, s);
	}
	if ((int)(i * 100.0 / _projector.get_points().size()) != (int)((i+1) * 100.0 / _projector.get_points().size())) {
	    cerr<<"\r          \r"<<((int)((i+1) * 100.0 / _projector.get_points().size()))<<"%";
	    cerr.flush();
	}
    }
    cerr << endl;
}

SmoothMLSGuidanceField::SmoothMLSGuidanceField
    (CProjection &projector,
     real_type rho, real_type min_step, real_type max_step, real_type reduction, int adamson, const char *filename):
	GuidanceField(rho, min_step, max_step, reduction),
	_projector(projector),
	_kdOrderedTraverse(NULL),
	_adamson(adamson)
{
    //    curv_radius.resize(projector.get_points().size());
    //    cerr << projector.get_points().size() << endl;

    LoadField(filename);

    if (ideal_length.size() != projector.get_points().size()) {
	cerr << "Guidance field size mismatch" << endl;
	cerr << "There are " << projector.get_points().size() << " points and ";
	cerr << ideal_length.size() << " entries." << endl;
	exit(1);
    } else {
	cerr << "Loaded curvature field." << endl;
    }
}

SmoothMLSGuidanceField::~SmoothMLSGuidanceField()
{
}

int SmoothMLSGuidanceField::ClosestPoint(const Point3 &p) 
{
    return _projector.get_kdtree().tree->FindMin(p);
}


void SmoothMLSGuidanceField::OrderedPointTraverseStart(const Point3 &p)
{
    if (_kdOrderedTraverse)
	delete _kdOrderedTraverse;
    _kdOrderedTraverse = new gtb::ss_kdtree<surfel_set>::t_surfel_tree::OrderedIncrementalTraverse(*_projector.get_kdtree().tree, p);
}

int SmoothMLSGuidanceField::OrderedPointTraverseNext(real_type &dist) {
    if (_kdOrderedTraverse->empty()) return -1;
    return _kdOrderedTraverse->GetNext(dist);
}

void SmoothMLSGuidanceField::OrderedPointTraverseEnd() {
    delete _kdOrderedTraverse;
    _kdOrderedTraverse = NULL;
}

const Point3& SmoothMLSGuidanceField::PointLocation(int i) const
{
    return _projector.get_points().vertex(i);
}

void SmoothMLSGuidanceField::Extract(vector<Point3> &pts, vector<Vector3> &norms, vector<real_type> &rad)
{
    pts = _projector.get_points().vertices();
    norms = _projector.get_points().normals();
    rad = ideal_length;
}
