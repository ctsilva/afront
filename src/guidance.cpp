

#include "common.h"
#include "guidance.h"
#include "parallel.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <numeric>

// why aren't these dependent on the reduction factor?
static const float cos_ear_cuttable_angle_max = cosf(M_PI * 100.0f / 180.0f);
static const float cos_ear_cuttable_angle_min = cosf(M_PI * 0.0f / 180.0f);





///////////////////////////////////////////////////////////////////////////////
// GuidanceField


real_type GuidanceField::StepRequired(real_type dist, int to) const
{
    return (1-reduction)*dist + reduction*ideal_length[to];
}


real_type GuidanceField::MaxCurvatureToIdeal(real_type curvature) const {
    return 2*sin(rho*0.5) / fabs(curvature);
}


real_type GuidanceField::MaxStepLength(const Point3 &p, int ignore) 
{
    real_type len = max_step;
    real_type checked_rad=0;

    OrderedPointTraverseStart(p);
    do {

	int checkp = OrderedPointTraverseNext(checked_rad);
	if (checkp < 0) break;

	if (checkp == ignore) continue;

	// apparently the distance returned by the ordered traverse isn't the actual distance!
	checked_rad = sqrt(checked_rad);
	len = std::min(len, StepRequired(checked_rad, checkp));
	if (len<min_step) {
	    len = min_step;
	    break;
	}

    } while (checked_rad < (len/(1-reduction)));


    OrderedPointTraverseEnd();
    return len;
}



real_type GuidanceField::MaxStepLengthD(const Point3 &p, int &stepto) 
{
    real_type len = max_step;
    real_type checked_rad=0;
    stepto = -1;

    OrderedPointTraverseStart(p);
    do {

	int checkp = OrderedPointTraverseNext(checked_rad);
	if (checkp < 0) break;

	// apparently the distance returned by the ordered traverse isn't the actual distance!
	checked_rad = sqrt(checked_rad);

	real_type sr = StepRequired(checked_rad, checkp);
	if (sr < len) {
	    len = sr;
	    stepto = checkp;
	}

	if (len<min_step) {
	    len = min_step;
	    break;
	}

    } while (checked_rad < (len/(1-reduction)));


    OrderedPointTraverseEnd();
    return len;
}


void GuidanceField::SaveField(const char *filename) const
{
    std::ofstream out(filename);
    if (!out) {
	std::cerr << "Error opening file " << filename << " for writing." << std::endl;
	exit(1);
    }
    std::copy(ideal_length.begin(),
	      ideal_length.end(),
	      std::ostream_iterator<float>(out, "\n"));
}

void GuidanceField::LoadField(const char *filename)
{
    std::ifstream in(filename);
    if (!in) {
	cerr << "Error opening file " << filename << " for reading." << std::endl;
	exit(1);
    }
    std::copy(std::istream_iterator<float>(in),
	      std::istream_iterator<float>(),
	      std::back_inserter(ideal_length));
}


void GuidanceField::ResampleCurve(vector<Point3> &ip, vector< vector<Vector3> > &in,
				  vector<Point3> &op, vector< vector<Vector3> > &on,
				  bool is_not_loop) {

    real_type totallength=0;
    real_type steplengths=0;
    for (unsigned i=0; i<ip.size() - (int)is_not_loop; i++) {
	totallength += Point3::distance(ip[i], ip[(i+1)%ip.size()]);
    }

    vector<real_type> steps;

    for (int pass=0; pass<2; pass++) {

	op.clear();
	op.push_back(ip[0]);

	on.resize(in.size());
	for (unsigned i=0; i<in.size(); i++) {
	    on[i].clear();
	    on[i].push_back(in[i][0]);
	}


	int lastpassed = 0;
	bool done = false;
	while (!done) {

	    if (pass==1 && op.size()==steps.size())
		break;

	    Point3 lp = op.back();

	    real_type dist_to_go;
	    if (pass==0) {
		steps.push_back(MaxStepLength(lp));
		steplengths += steps[op.size()-1];
		dist_to_go = steps[op.size()-1];
	    } else {
		dist_to_go = steps[op.size()-1] * totallength / steplengths;
	    }


	    while (dist_to_go > 0) {

		int nextpoint = (lastpassed+1) % ip.size();

		real_type pdist = Point3::distance(lp, ip[nextpoint]);

		if (pdist >= dist_to_go) {

		    // don't go all the way to the endpoint

		    Vector3 edir = ip[nextpoint] - ip[(nextpoint+ip.size()-1)%ip.size()];
		    Vector3 edirn = edir.normalized();
		    op.push_back(lp + dist_to_go * edirn);

		    real_type alpha = Point3::distance(op.back(), ip[nextpoint]) / edir.length();

		    for (unsigned i=0; i<in.size(); i++) {
			on[i].push_back(alpha*in[i][(nextpoint+ip.size()-1)%ip.size()] + (1-alpha)*in[i][nextpoint]);
			on[i].back().normalize();
		    }

		    dist_to_go = 0;

		} else {

		    dist_to_go -= pdist;
		    lp = ip[nextpoint];
		    lastpassed = nextpoint;
		    if ((is_not_loop && lastpassed==ip.size()-1) ||
			(!is_not_loop && lastpassed == 0)) {
			dist_to_go = 0;
			done = true;
		    }
		}
	    }
	}
	if (is_not_loop) {
	    op.push_back(ip.back());
	    for (unsigned i=0; i<in.size(); i++) {
		on[i].push_back(in[i].back());
	    }
	}
    }
}



void GuidanceField::ParallelTrim(int nt, int id, 
				 vector< std::pair<real_type,int> > &points,
				 ConeBoxKDTree<int, GuidanceField> &kd,
				 vector<int> &marked) const {
    real_type t = 1-reduction;
    t *= t;
    real_type angle_dot = sqrt(t / (t+1));

    unsigned count = points.size();
    for (unsigned i=id; i<count; i+=nt) {
	if (marked[points[i].second])
	    continue;
	Cone c;
	c.p = PointLocation(points[i].second);
	c.r = StepRequired(0,points[i].second);
	c.angle_dot = angle_dot;
	kd.MarkIntersected(*this, c, marked);
    }

}



void GuidanceField::RecursiveTrim(const vector<int> &ipts, vector<int> &marked) const {

    extern int trim_bin_size;
    if ((int)ipts.size() < trim_bin_size) {

	cerr<<"trimming bin with "<<ipts.size()<<" points"<<endl;

	vector< std::pair<real_type,int> >  sorted(ipts.size());
	for (unsigned i=0; i<ipts.size(); ++i) {
	    sorted[i].first = StepRequired(0,ipts[i]);
	    sorted[i].second = ipts[i];
	}
	sort(sorted.begin(), sorted.end());

	ConeBoxKDTree<int, GuidanceField> kd(ipts, *this);
	ParallelExecutor(idealNumThreads, makeClassFunctor(this, &GuidanceField::ParallelTrim), sorted, kd, marked);

    } else {
	// compute the bounding box
	Box3 bbox(PointLocation(ipts[0]), PointLocation(ipts[0]));
	for (unsigned i=1; i<ipts.size(); i++) {
	    bbox.update(PointLocation(ipts[i]));
	}

	vector<int> sides[2];
	sides[0].reserve((int)(ipts.size()*0.6));
	sides[1].reserve((int)(ipts.size()*0.6));

	int axis = 0;
	if (bbox.y_length() > bbox.x_length() && bbox.y_length() > bbox.z_length()) axis=1;
	if (bbox.z_length() > bbox.x_length() && bbox.z_length() > bbox.y_length()) axis=2;

	real_type split = bbox.centroid()[axis];

	for (unsigned i=0; i<ipts.size(); i++) {

	    if (PointLocation(ipts[i])[axis] < split)
		sides[0].push_back(ipts[i]);
	    else
		sides[1].push_back(ipts[i]);
	}

	RecursiveTrim(sides[0], marked);
	RecursiveTrim(sides[1], marked);
    }

}


void GuidanceField::Trim(vector<int> &marked) const {

    marked = vector<int>(NumPoints(),0);

    extern bool trim_guidance;
    if (!trim_guidance) {
	cerr<<"Skipping trimming guidance field"<<endl;
	return; 
    }

    cerr << "Trimming guidance field." << endl;


    vector<int> ipts(NumPoints());
    for (unsigned i=0; i<ipts.size(); i++) {
	ipts[i] = i;
    }


    RecursiveTrim(ipts, marked);

    int nmarked = marked.size() - std::accumulate(marked.begin(), marked.end(), 0);
    cerr<<"Pass 1"<<endl;
    cerr<<"Num points kept: "<<nmarked<<endl;
    cerr<<"total points: "<<marked.size()<<endl;
    



    ipts.resize(0);
    for (int i=0; i<NumPoints(); i++) {
	if (!marked[i])
	    ipts.push_back(i);
    }

    RecursiveTrim(ipts, marked);

    nmarked = marked.size() - std::accumulate(marked.begin(), marked.end(), 0);
    cerr<<"Pass 2"<<endl;
    cerr<<"Num points kept: "<<nmarked<<endl;
    cerr<<"total points: "<<marked.size()<<endl;
    

}














