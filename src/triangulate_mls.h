#ifndef _TRIANGULATE_MLS_SMOOTH
#define _TRIANGULATE_MLS_SMOOTH

#include <gtb/gtb.hpp>
#include <mlslib/CProjection.h>
#include <vector>
#include "common.h"

class SmoothMLSProjector : public SurfaceProjector 
{
    public:
    SmoothMLSProjector(surfel_set &surfels, int adamson);
    virtual ~SmoothMLSProjector();
    
    mls::GaussianWeightFunction<real_type> _wf, _radius_wf;
    mls::CProjection<real_type> _projector;
    int _adamson;

    int ProjectPoint(const FrontElement &base1, const FrontElement &base2,
		     const Point3 &fp, const Vector3 &fn,
		     Point3 &tp, Vector3 &tn) const;

    int ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const;
    CProjection &get_projector();

    void SaveField(const char *filename);
    void SetRadiusFactor(real_type t) {
	_projector.set_radius_factor(t);
    }
};

class SmoothMLSGuidanceField : public GuidanceField 
{
    public:
    typedef Vector3 (*VectorField)(const Point3 &);

    // Compute from guidance field
    SmoothMLSGuidanceField(CProjection &projector, 
			   real_type rho,
			   real_type min_step, real_type max_step, real_type reduction, int adamson,
			   VectorField field = NULL,
			   float field_epsilon = 0.0f);
    // Load from file
    SmoothMLSGuidanceField(CProjection &projector, 
			   real_type rho,
			   real_type min_step, real_type max_step, real_type reduction, int adamson,
			   const char *filename);
    virtual ~SmoothMLSGuidanceField();


    int ClosestPoint(const Point3 &p);
    void OrderedPointTraverseStart(const Point3 &p);
    int OrderedPointTraverseNext(real_type &dist);
    void OrderedPointTraverseEnd();
    const Point3& PointLocation(int i) const;

    void Extract(vector<Point3> &pts, vector<Vector3> &norms, vector<real_type> &rad);

    void curvatures(const Point3 &ref, Vector3 &normal, real_type &k1, real_type &k2);

    CProjection &_projector;

    gtb::ss_kdtree<surfel_set>::t_surfel_tree::OrderedIncrementalTraverse *_kdOrderedTraverse;

    int _adamson;
};

#endif
