
#ifndef _GUIDANCE_H
#define _GUIDANCE_H

#include "front.h"

#include <gtb/graphics/point3.hpp>

#include "conekdtree.h"

class GuidanceField {
    public:

    GuidanceField(real_type _rho, real_type _min, real_type _max, real_type _reduction)
	: rho(_rho), min_step(_min), max_step(_max), reduction(_reduction) { };
    virtual ~GuidanceField() {};


    virtual const Point3& PointLocation(int i) const = 0;
    virtual void OrderedPointTraverseStart(const Point3 &p) = 0;
    virtual int OrderedPointTraverseNext(real_type &squared_dist) = 0;
    virtual void OrderedPointTraverseEnd() = 0;

    real_type MaxStepLength(const Point3 &p, int ignore=-1);

    void ResampleCurve(vector<Point3> &ip, vector< vector<Vector3> > &in,
		       vector<Point3> &op, vector< vector<Vector3> > &on,
		       bool is_not_loop=false);
    void SaveField(const char *filename) const;
    void LoadField(const char *filename);

    int NumPoints() const { return ideal_length.size(); };

    real_type GetFenceScale() {
	extern real_type fence_scale;
	return (real_type)(fence_scale * 4.0*((1-sqrt((1+2*cos(rho))/3)) / (2*sin(rho*0.5))));
    }


    real_type MaxCurvatureToIdeal(real_type curvature) const;
    real_type IdealLength(int i) const { return ideal_length[i]; }



    // stuff for the cone kd-tree
    Box4 bounding_box(int v) const {
	real_type r = StepRequired(0, v);
	Point3 p = PointLocation(v);
	return Box4(Box3(p,p), r, r);
    }


    void Trim(vector<int> &marked) const;



    protected:

    // must be initialized by child class
    vector<real_type> ideal_length;

    real_type rho;
    real_type min_step;
    real_type max_step;
    real_type reduction;


    
    private:

    real_type StepRequired(real_type dist, int to) const;
    real_type MaxStepLengthD(const Point3 &p, int &stepto);

    void RecursiveTrim(const vector<int> &ipts, vector<int> &marked) const;  // writes into ipts!
    void ParallelTrim(int nt, int id, 
		      std::vector< std::pair<real_type,int> > &points,
		      ConeBoxKDTree<int, GuidanceField> &kd,
		      vector<int> &marked) const;
};




#endif

