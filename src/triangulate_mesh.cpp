
#include "common.h"
#include "guidance.h"
#include "triangulator.h"
#include "triangulate_mesh.h"
#include "parallel.h"
#include <sys/time.h>


//#define CLOSEST_POINT_PROJECTION

// Timing utility
static double get_time_seconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}


///////////////////////////////////////////////////////////////////////////////
// Helper for finding curvature of space curves (boundaries)

class Quadratic {
    public:

    Quadratic() {
	coefs[0] = 0;
	coefs[1] = 0;
	coefs[2] = 0;
    }

    Quadratic(real_type A, real_type B, real_type C) {
	coefs[0] = A;
	coefs[1] = B;
	coefs[2] = C;
    }

    real_type eval(real_type x) {
	return (coefs[0]*x*x + coefs[1]*x + coefs[2]);
    }

    real_type eval1(real_type x) {
	return (2*coefs[0]*x + coefs[1]);
    }

    real_type eval2(real_type x) {
	return (2*coefs[0]);
    }


    bool fit(const vector<real_type> &xi, const vector<real_type> &yi, const vector<real_type> &weights) {

	const int N = (int)xi.size();
	const int NCF = 3;
	const int DEG = 2;

	double cc[NCF];
	gtb::AMatc<double> A(NCF, NCF);
	gtb::AVec<double> b(NCF);
	A.set(0);
	b.set(0);

	real_type max_w = weights[0];
	for (int i=1; i<N; i++) {
	    if (weights[i] > max_w)
		max_w = weights[i];
	}
	double invmaxw = 1.0 / max_w;

	/*--------- Prepare the matrix A and vector b ----------*/
	for (int k = 0; k < N; ++k)
	    {
		double w = weights[k] * invmaxw;

		//
		// Speedup?
		//   - if our function is a*f_a(x,y) + b*f_b(x,y) + c*f_c(x,y) +...
		//     compute f_a(x,y), f_b(x,y), ...
		//     i.e. computes the "coefficients" of a,b,c...
		//   - Polynoms (old) pre-compute X^i, y^i, i=0:deg
		//
		{
		    int cidx = 0;
		    for (int p = 0; p <= DEG; ++p)
			{
			    cc[cidx] = gtb::ipow((double)xi[k], p);
			    ++cidx;
			}
		}

		double *pr, *pc;
		for (int i = 0; i < NCF; ++i)
		    {
			pc = pr = A.flatten() + i + i*NCF;
			for (int j = i; j < NCF; ++j)
			    {
				*pr = *pc += cc[i]*cc[j] *w;
				pr += NCF;
				++pc;
			    }

			// xi[2] is the height of the function above the 
			// plane. (i.e. y_i)
			b[i] += yi[k] * cc[i] * w;
		    }
	    }

	/*--------- Solve Ax=b ----------*/
	{
	    if (!isfinite(A(0,0)))
		{
		    //            DebugBreak();
		    return false;
		}
	    else
		{
		    gtb::AVec<double> x(NCF);
	           
		    SVDSolve(A, b, x);

		    /*
		     * Copy result to the coeeficients of the polynom
		     */
		    coefs[0] = x[2];
		    coefs[1] = x[1];
		    coefs[2] = x[0];
		}
	    return true;
	}
    }



    real_type coefs[3];
};


real_type edge_curvature(const vector<Point3> &points, const vector<real_type> &weights) {

    vector<real_type> ss;
    vector<real_type> xs, ys, zs;
    for (unsigned i=0; i<points.size(); i++) {

	if (i==0)
	    ss.push_back(0);
	else
	    ss.push_back(ss.back() + Point3::distance(points[i-1], points[i]));

	xs.push_back(points[i][0]);
	ys.push_back(points[i][1]);
	zs.push_back(points[i][2]);
    }

    for (unsigned i=0; i<ss.size(); i++) {
	ss[i] /= ss.back();
    }


    Quadratic pcurves[3];

    pcurves[0].fit(ss, xs, weights);
    pcurves[1].fit(ss, ys, weights);
    pcurves[2].fit(ss, zs, weights);


    real_type s = ss[points.size()/2];

    Vector3 qd(pcurves[0].eval1(s), pcurves[1].eval1(s), pcurves[2].eval1(s));
    Vector3 qdd(pcurves[0].eval2(s), pcurves[1].eval2(s), pcurves[2].eval2(s));

    real_type k = qd.cross(qdd).length() / gtb::ipow(qd.length(), 3);
    return k;
}




///////////////////////////////////////////////////////////////////////////////
// MeshProjector
MeshProjector::kdtree_type *MeshProjector::GetKdTree()
{
    return kdtree;
}

MeshProjector::MeshProjector(const TriangleMesh &m, const vector<int> *pointsides) : mesh(m) {

    vector<int> fps;
    for (unsigned f=0; f<mesh.faces.size(); f++) {
	// for csg operations, we can skip a bunch of the faces that never see the light of day
	if (!pointsides ||
	    (*pointsides)[m.faces[f].verts[0]]>0 || 
	    (*pointsides)[m.faces[f].verts[1]]>0 || 
	    (*pointsides)[m.faces[f].verts[2]]>0)
	    fps.push_back(f);
    }

    kdtree = new kdtree_type(fps, *this);
}

MeshProjector::~MeshProjector() {
    delete kdtree;
}



int MeshProjector::ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const {
	
    kdtree_type::OrderedTraverse ot(*kdtree, fp, *this);
    int fi; ot.next(fi);
    const TriangleMeshFace &f = mesh.faces[fi];
    Triangle3 tri(mesh.verts[f.verts[0]].point, mesh.verts[f.verts[1]].point, mesh.verts[f.verts[2]].point);
    tp = tri.closest_point(fp);

    real_type b[3];
    tri.get_barycentric_coordinates(tp, b[0], b[1], b[2]);

    tn = Vector3(0,0,0);
    tn += b[0] * mesh.verts[f.verts[0]].normal;
    tn += b[1] * mesh.verts[f.verts[1]].normal;
    tn += b[2] * mesh.verts[f.verts[2]].normal;
    tn.normalize();

    return PROJECT_SUCCESS;
}



int MeshProjector::ProjectPoint(const FrontElement &base1, const FrontElement &base2, const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const {

#ifdef CLOSEST_POINT_PROJECTION
    return ProjectPointClosest(fp, tp, tn);
#else


    Vector3 axis = base2.position - base1.position;
    axis.normalize();

    Point3 center = base1.position + (axis.dot(fp - base1.position)) * axis;
    real_type rad = Point3::distance(center, fp);


    Vector3 extremes;
    for (int i=0; i<3; i++) {
	extremes[i] = fabs(rad * cos(M_PI/2 - acos(axis[i]))) + 1e-5;
    }

    Box3 ringbox(center-extremes, center+extremes);


    static_vector(int,possible_intersects);
    kdtree->GetIntersectedBoxes(*this, ringbox, possible_intersects);


    real_type closest_intersect = 1e34;

    for (unsigned i=0; i<possible_intersects.size(); i++) {

	const TriangleMeshFace &f = mesh.faces[possible_intersects[i]];
	Triangle3 tri(mesh.verts[f.verts[0]].point, mesh.verts[f.verts[1]].point, mesh.verts[f.verts[2]].point);

	if (tri.area() == 0) continue;

	Vector3 fnorm = tri.normal();

	real_type d = fnorm.dot(center) - fnorm.dot(tri.A());

	if (fabs(d) > rad) continue;

	Vector3 v = fnorm.cross(axis);
	v.normalize();

	real_type x = sqrt(rad*rad - d*d);

	for (int j=0; j<2; j++) {

	    Point3 p = center + -d*fnorm + ((j==0)?-x:x) * v;

	    // see if it's actually inside the triangle
	    bool inside = true;
	    for (int k=0; k<3; k++) {
		if (fnorm.dot((tri[(k+1)%3]-tri[k]).cross(p-tri[k])) < 0) {
		    inside = false; break;
		}
	    }
	    if (!inside) continue;

	    if (axis.dot(fnorm.cross(p-center)) > 0)
		continue;

	    real_type tdist = Point3::distance(p, fp);
	    if (tdist < closest_intersect) {
		closest_intersect = tdist;
		tp = p;

		real_type b[3];
		tri.get_barycentric_coordinates(tp, b[0], b[1], b[2]);

		tn = Vector3(0,0,0);
		tn += b[0] * mesh.verts[f.verts[0]].normal;
		tn += b[1] * mesh.verts[f.verts[1]].normal;
		tn += b[2] * mesh.verts[f.verts[2]].normal;
		tn.normalize();
	    }

	}

    }

    return PROJECT_SUCCESS;

#endif

}


Box3 MeshProjector::bounding_box(int face) const {
    const TriangleMeshFace &f = mesh.faces[face];
    return Box3::bounding_box(mesh.verts[f.verts[0]].point, mesh.verts[f.verts[1]].point, mesh.verts[f.verts[2]].point);
}

real_type MeshProjector::distance(int face, const Point3 &from) const {
    const TriangleMeshFace &f = mesh.faces[face];
    Triangle3 tri(mesh.verts[f.verts[0]].point, mesh.verts[f.verts[1]].point, mesh.verts[f.verts[2]].point);
    return Point3::distance(from, tri.closest_point(from));
}



// try to find the boundaries in a triangle soup
void MeshProjector::FindSoupBoundaries(vector< vector<int> > &boundaries, real_type d) const {

    // get candidate edges:
    // edges that when a point is projected from off to it's side
    // get projected back onto the edge
    std::set< std::pair<int,int> > bedges;
    for (unsigned t=0; t<mesh.faces.size(); t++) {
	for (int i=0; i<3; i++) {
	    if (mesh.faces[t].nbrs[i] < 0) {
		// try projecting a point out to the side onto the mesh
		Point3 v1 = mesh.verts[mesh.faces[t].verts[i]].point;
		Point3 v2 = mesh.verts[mesh.faces[t].verts[(i+1)%3]].point;
		Vector3 n1 = mesh.verts[mesh.faces[t].verts[i]].normal;
		Vector3 n2 = mesh.verts[mesh.faces[t].verts[(i+1)%3]].normal;
		Vector3 norm = (n1+n2).normalized();
		Vector3 perp = (v2-v1).cross(norm).normalized();
		Point3 mid = v1 + (real_type)0.5 * (v2-v1);

		real_type td = d;
		if (td < 0) {
		    td = -td*Point3::distance(v1,v2);
		}

		Point3 fp = mid + td*perp;
		Point3 tp;
		Vector3 tn;

		if (ProjectPoint(fp, tp, tn) == PROJECT_SUCCESS) {
		    if (Point3::distance(mid, tp) < td*0.1) {
			bedges.insert(std::pair<int,int>(mesh.faces[t].verts[i], mesh.faces[t].verts[(i+1)%3]));
		    }
		}
	    }
	}
    }
#if 0
    dbgClear();
    for (std::set<std::pair<int,int> >::iterator i=bedges.begin();
	 i != bedges.end(); ++i) {

	vector<Point3> line(2);
	line[0] = mesh.verts[i->first].point;
	line[1] = mesh.verts[i->second].point;
	DbgPLines::add(line, 0);
    }
    redrawAndWait(' ');
#endif


    // try stitching them together into loops
    while (1) {

	if (bedges.empty())
	    break;


	vector<int> loop;
	loop.push_back(bedges.begin()->first);
	loop.push_back(bedges.begin()->second);
	bedges.erase(bedges.begin());

	bool made_loop = false;
	while (1) {

	    if (bedges.empty())
		break;

	    // look for the edge at the end of the loop
	    real_type dist = 1e34;
	    std::set<std::pair<int,int> >::iterator best = bedges.end();

	    for (std::set<std::pair<int,int> >::iterator i=bedges.begin();
		 i != bedges.end(); ++i) {

		real_type tdist = Point3::distance(mesh.verts[loop.back()].point, mesh.verts[i->first].point);
		if (tdist < dist) {
		    dist = tdist;
		    best = i;
		}
	    }

	    if (best == bedges.end()) {
		cerr<<"no best edge??"<<endl;
		break;
	    }

	    real_type elen = Point3::distance(mesh.verts[best->first].point, mesh.verts[best->second].point);

	    // skipped to an edge too far away
	    if (dist > elen*0.1) {
		cerr<<"dist too far from last point in loop"<<endl;
		break;
	    }


	    // see if we closed the loop
	    real_type clen = Point3::distance(mesh.verts[loop.front()].point, mesh.verts[best->second].point);
	    if (clen < elen*0.1) {
		cerr<<"loop done"<<endl;
		made_loop = true;
		break;
	    }

#if 0
	    vector<Point3> line(2);
	    line[0] = mesh.verts[best->first].point;
	    line[1] = mesh.verts[best->second].point;
	    DbgPLines::add(line, 0);
	    redrawAndWait(' ');
#endif

	    // add to the loop
	    loop.push_back(best->second);
	    bedges.erase(best);
	}


	if (made_loop) {
	    boundaries.push_back(loop);
#if 0
	    dbgClear();
	    vector<Point3> line;
	    for (unsigned i=0; i<loop.size(); i++) {
		line.push_back(mesh.verts[loop[i]].point);
	    }
	    line.push_back(mesh.verts[loop[0]].point);
	    DbgPLines::add(line, 1);
	    redrawAndWait(' ');
#endif
	}
    }
}




///////////////////////////////////////////////////////////////////////////////
// MeshGuidanceField

MeshGuidanceField::kdtree_type *MeshGuidanceField::GetKdTree()
{
    return kdtree;
}

gtb::tPoint3<double> toDouble(const Point3 &p) {
    return gtb::tPoint3<double>(p[0], p[1], p[2]);
}
gtb::tVector3<double> toDouble(const Vector3 &v) {
    return gtb::tVector3<double>(v[0], v[1], v[2]);
}


real_type SubdivCurvature(const Point3 &_center, const vector<Point3> &_ring, bool edge, int niter) {

    assert(niter>0);

    const unsigned N = _ring.size();
    gtb::tPoint3<double> center[2];
    vector< gtb::tPoint3<double> > rings[2];

    rings[0].resize(N);
    rings[1].resize(N);

    int s=0;
    center[0] = toDouble(_center);
    for (unsigned i=0; i<N; i++) {
	rings[0][i] = toDouble(_ring[i]) - center[0];
    }
    center[0] = gtb::tPoint3<double>(0,0,0);


    double beta = (N>3) ? (3.0/(8.0*N)) : (3.0/16.0);


    for (int iter=0; iter<niter; iter++) {

	int os=1-s;

	center[os] = gtb::tPoint3<double>(0,0,0);
	if (edge) {
	    center[os].add_scaled(center[s], 6.0/8.0);
	    center[os].add_scaled(rings[s].front(), 1.0/8.0);
	    center[os].add_scaled(rings[s].back(), 1.0/8.0);
	} else {
	    center[os].add_scaled(center[s], 1.0-N*beta);
	    for (unsigned i=0; i<N; i++) {
		center[os].add_scaled(rings[s][i], beta);
	    }
	}

	for (unsigned i=0; i<N; i++) {
	    rings[os][i] = gtb::tPoint3<double>(0,0,0);
	    if (edge && (i==0 || i==N-1)) {
		rings[os][i].add_scaled(center[s], 4.0/8.0);
		rings[os][i].add_scaled(rings[s][i], 4.0/8.0);
	    } else {
		rings[os][i].add_scaled(center[s], 3.0/8.0);
		rings[os][i].add_scaled(rings[s][i], 3.0/8.0);
		rings[os][i].add_scaled(rings[s][(i+1)%N], 1.0/8.0);
		rings[os][i].add_scaled(rings[s][(i+N-1)%N], 1.0/8.0);
	    }
	}

	s = os;
    }


    // compute the normal
    gtb::tVector3<double> normal(0,0,0);
    for (unsigned i=0; i<((edge)?N-1:N); i++) {
	normal += (rings[s][i] - center[s]).cross(rings[s][(i+1)%N] - center[s]).normalized();
    }
    normal.normalize();


    // stuff it into this surfelset crap so we can fit a polynomial to it
    gtb::tsurfel_set<double> nbhdset;
    gtb::tsurfelset_view<double> nbhdview(nbhdset);
    gtb::tPlane<double> plane(normal, toDouble(Point3(0,0,0)));
    gtb::plane_transformation<double> T(plane, center[s]);
    for (unsigned i=0; i<N; i++) {
	nbhdview.insert(nbhdset.size());
	nbhdset.insert_vertex(rings[0][i]);
	nbhdview.insert(nbhdset.size());
	nbhdset.insert_vertex(rings[1][i]);
    }

    mls::Poly2<double> poly;
    gtb::tsurfel_set<double> stdpts;
    mls::GaussianWeightFunction<double> wf;	wf.set_radius(1);

    mls::CProjection<double>::WeightedPolyFit(nbhdview, T, center[s], &wf, &poly, stdpts);

    double k1, k2;
    poly.curvatures(0,0,k1,k2);

    extern real_type saliency;
    if (saliency==0)
	return k2;
    else
	return 0.5*(k1+k2);
}



void SolveNormalEquations(const AMat &nA, const AVec &nB, gtb::AVec<double> &x) {

    gtb::AMat<double> A(nA.columns(), nA.columns());

    for (int m=0; m<A.rows(); m++) {
	for (int n=0; n<A.columns(); n++) {
	    A(m,n) = 0;
	    for (int i=0; i<nA.rows(); i++) {
		A(m,n) += (double)nA(i,m)*(double)nA(i,n);
	    }
	}
    }

    gtb::AVec<double> b(nA.columns());
    for (int m=0; m<b.size(); m++) {
	b[m] = 0;
	for (int i=0; i<nA.rows(); i++) {
	    b[m] += (double)nB[i] * (double)nA(i,m);
	}
    }

    x.resize(b.size());
    try {
        SVDSolve(A, b, x);
    } catch (const std::runtime_error& e) {
        // SVD failed to converge - use small positive curvature as fallback
        // This gives reasonable edge lengths rather than infinite
        static int warn_count = 0;
        if (warn_count++ < 5) {
            fprintf(stderr, "WARNING: SVD convergence failure, using moderate curvature estimate\n");
        }
        for (int i = 0; i < x.size(); i++) {
            x[i] = 0.0;
        }
        // Set small positive curvature  (x[0] + x[2] terms)
        x[0] = 0.1;  // moderate curvature
        x[2] = 0.1;
    } catch (...) {
        fprintf(stderr, "WARNING: Unknown exception in SVD, using moderate curvature estimate\n");
        for (int i = 0; i < x.size(); i++) {
            x[i] = 0.0;
        }
        x[0] = 0.1;
        x[2] = 0.1;
    }
}


real_type ExtendedQuadricCurvature(const Point3 &center, const vector<Point3> &points, const Vector3 &first_normal) {

    Vector3 normal, lnormal;
    normal = first_normal;

    AMat A(points.size(), 5);
    AVec b(points.size());
    gtb::AVec<double> x(5);

    int iter=0;
    do {
	iter++;

	Plane plane(normal, Point3(0,0,0));
	plane_transformation T(plane, center);

	Point3 _center = T.ToPlane(center);

	for (unsigned p=0; p<points.size(); p++) {
	    Point3 local = T.ToPlane(points[p]);
	    A(p,0) = local[0]*local[0];
	    A(p,1) = local[0]*local[1];
	    A(p,2) = local[1]*local[1];
	    A(p,3) = local[0];
	    A(p,4) = local[1];
	    b[(int)p] = local[2];
	}

	SolveNormalEquations(A, b, x);

	lnormal = normal;
	normal = T.PlaneNormal2World(Vector3(-x[3],-x[4],1).normalized());

    } while (iter<10 && normal.dot(lnormal) < 0.99);



    double descrim = std::max(0.0, (x[0]-x[2])*(x[0]-x[2]) + x[1]*x[1]);

    extern real_type saliency;
    if (saliency==0) {
	if (x[0]+x[2] > 0)
	    return (real_type)(x[0] + x[2] + sqrt(descrim));
	else
	    return (real_type)(x[0] + x[2] - sqrt(descrim));
    } else {
	return ((real_type)(x[0] + x[2] + sqrt(descrim)) +
		(real_type)(x[0] + x[2] - sqrt(descrim))) / 2;
    }
}







MeshGuidanceField::~MeshGuidanceField() {
    if (kdOrderedTraverse)		delete kdOrderedTraverse;
    delete kdtree;
}

MeshGuidanceField::MeshGuidanceField(int curv_sub, const TriangleMesh &mesh, real_type rho, real_type min_step, real_type max_step, real_type reduction)
    : GuidanceField(rho, min_step, max_step, reduction), kdGetPoint(mesh), kdOrderedTraverse(NULL) {


    // setup the kdtree
    kdtree = new kdtree_type(10, mesh.bounding_box(), kdGetPoint);
    for (unsigned i=0; i<mesh.verts.size(); i++)
        kdtree->Insert(i, false);
    kdtree->MakeTree();


    mls::GaussianWeightFunction<double> wf;
    wf.set_radius(1);


    TriangleMesh cmesh = mesh;
    for (int i=0; i<curv_sub; i++) {
	cmesh.LoopSubdivide();
    }

    double curvature_start = get_time_seconds();
    cerr << "[TIMING] Computing curvature for " << mesh.verts.size() << " vertices..." << endl;

    for (unsigned v=0; v<mesh.verts.size(); v++) {


	// fit a polynomial
	const int numrings = 3;
	vector< std::set<int> > rings(numrings+1);
	rings[0].insert(v);

	for (int r=0; r<numrings; r++) {
	    rings[r+1] = rings[r];

	    for (std::set<int>::iterator i=rings[r].begin(); i!=rings[r].end(); ++i) {

		for (TriangleMesh::VertexVertexIteratorI vi(cmesh, *i); !vi.done(); ++vi) {
		    rings[r+1].insert(*vi);
		}
	    }
	}

	vector<Point3> nbrhood;
	Vector3 normest = cmesh.verts[v].normal;
	for (unsigned r=0; r<rings.size(); r++) {
	    for (std::set<int>::iterator i=rings[r].begin(); i!=rings[r].end(); ++i) {
		nbrhood.push_back(cmesh.verts[*i].point);
		normest += cmesh.verts[*i].normal;
	    }
	}
	normest.normalize();

	real_type kmax = ExtendedQuadricCurvature(cmesh.verts[v].point, nbrhood, normest);

	if (cmesh.VertOnBoundary(v)) {

	    const int num =2;

	    vector<int> eptsi;
	    eptsi.push_back(v);
            
	    for (int i=0; i<num; i++) {
		TriangleMesh::VertexVertexIteratorI vi(cmesh, eptsi.back());
		eptsi.push_back(*vi);
	    }

	    reverse(eptsi);


	    for (int i=0; i<num; i++) {
		TriangleMesh::VertexVertexIteratorI vi(cmesh, eptsi.back());
		eptsi.push_back(0);
		while (!vi.done()) {
		    eptsi.back() = *vi;
		    ++vi;
		}
	    }


	    vector<Point3> epts;
	    vector<real_type> weights;
	    for (unsigned i=0; i<eptsi.size(); i++) {
		epts.push_back(cmesh.verts[eptsi[i]].point);
		weights.push_back(1);
	    }


	    double k = edge_curvature(epts, weights);

	    if (fabs(k) > fabs(kmax))
		kmax = k;
	}


	ideal_length.push_back(MaxCurvatureToIdeal(kmax));



	if ((int)(v * 100.0 / mesh.verts.size()) != (int)((v+1) * 100.0 / mesh.verts.size())) {
	    cerr<<"                  \r"<<((int)((v+1) * 100.0 / mesh.verts.size()))<<"%"<<std::flush;
	}

    }
    cerr << std::endl;

    double curvature_elapsed = get_time_seconds() - curvature_start;
    cerr << "[TIMING] Curvature computation completed in " << curvature_elapsed << " seconds ("
         << (mesh.verts.size() / curvature_elapsed) << " verts/sec)" << endl;

    vector<int> marked; // for removal
    Trim(marked);

    // rebuild the kdtree
    delete kdtree;

    kdtree = new kdtree_type(10, mesh.bounding_box(), kdGetPoint);
    for (unsigned i=0; i<mesh.verts.size(); i++) {
	if (!marked[i])
	    kdtree->Insert(i, false);
    }
    kdtree->MakeTree();




#if 0
    extern real_type saliency;
    if (saliency > 0)
	MeanCurvaturesToSaliency(saliency * Point3::distance(kdGetPoint.mesh.bounding_box().min_point(),
							     kdGetPoint.mesh.bounding_box().max_point()));



    if (0) {
	dbgClear();


	real_type max_saliency = max_element(curv_radius);
	real_type min_saliency = min_element(curv_radius);

	if (1) {

	    vector<real_type> sorted = curv_radius;
	    std::sort(sorted.begin(), sorted.end());

	    min_saliency = sorted[100];
	    max_saliency = sorted[sorted.size()-100];

	}


	std::swap(min_saliency, max_saliency);
	min_saliency = 1/min_saliency;
	max_saliency = 1/max_saliency;

	cerr<<"max: "<<max_saliency<<endl;
	cerr<<"min: "<<min_saliency<<endl;


	for (unsigned f=0; f<kdGetPoint.mesh.faces.size(); f++) {
	    Point3 p[3];
	    Point3 c[3];

	    for (int i=0; i<3; i++) {
		p[i] = kdGetPoint.mesh.verts[kdGetPoint.mesh.faces[f].verts[i]].point;
		real_type saliency = 1/curv_radius[kdGetPoint.mesh.faces[f].verts[i]];

		saliency = (saliency-min_saliency) / (max_saliency -min_saliency);
		//		cerr<<"saliency "<<saliency<<endl;
		c[i] = Point3(saliency, 0, 1-saliency);
	    }

	    DbgPoly::add(p[0], c[0], p[1], c[1], p[2], c[2]);

	}

	redrawAndWait(' ', true);
    }

#endif
}



// this has been moved from guidance.cpp here so we can make it parallel
// the standard guidance interface for range queries is not threadsafe
const int num_levels = 5;
void MeshGuidanceField::MeanCurvaturesToSaliencyParallel(int nt, int id, real_type rad, const real_type rads[], vector<real_type> ave_curvatures[][2]) {
#if 0
    for (unsigned i=id; i<curv_radius.size(); i+=nt) {

	real_type wsum[num_levels][2];
	for (int j=0; j<num_levels; j++) {
	    wsum[j][0] = 0;	            wsum[j][1] = 0;
	    ave_curvatures[j][0][i] = 0;    ave_curvatures[j][1][i] = 0;
	}

	kdtree_type::OrderedIncrementalTraverse traverse(*kdtree, PointLocation(i));
	while (1) {

	    real_type squared_dist;
	    if (traverse.empty()) break;
	    int next = traverse.GetNext(squared_dist);

	    if (squared_dist > square(2*rad*rads[num_levels-1])) break;

	    for (int j=0; j<num_levels; j++) {
		real_type weight1 = exp(-squared_dist / (2*square(rad*1*rads[j])));
		real_type weight2 = exp(-squared_dist / (2*square(rad*2*rads[j])));

		ave_curvatures[j][0][i] += weight1 / curv_radius[next];
		ave_curvatures[j][1][i] += weight2 / curv_radius[next];

		wsum[j][0] += weight1;
		wsum[j][1] += weight2;
	    }
	}

	for (int j=0; j<num_levels; j++) {
	    ave_curvatures[j][0][i] /= wsum[j][0];
	    ave_curvatures[j][1][i] /= wsum[j][1];

	    if (ave_curvatures[j][0][i] != ave_curvatures[j][0][i])
		ave_curvatures[j][0][i] = 0;
	    if (ave_curvatures[j][1][i] != ave_curvatures[j][1][i])
		ave_curvatures[j][1][i] = 0;
	}


	if ((int)(i * 100.0 / curv_radius.size()) != (int)((i+1) * 100.0 / curv_radius.size())) {
	    cerr<<"                  \r"<<((int)((i+1) * 100.0 / curv_radius.size()))<<"%"<<std::flush;\
	}
    }
#endif
}



void MeshGuidanceField::MeanCurvaturesToSaliency(real_type rad) {
#if 0
    cerr<<"computing saliency..."<<endl;


    const real_type rads[num_levels] = { 1, 2, 3, 4, 5 };  // * rad

    vector<real_type> ave_curvatures[num_levels][2];
    for (int i=0; i<num_levels; i++) {
	ave_curvatures[i][0].resize(curv_radius.size());
	ave_curvatures[i][1].resize(curv_radius.size());
    }


    ParallelExecutor(idealNumThreads, 
		     makeClassFunctor(this, 
				      &MeshGuidanceField::MeanCurvaturesToSaliencyParallel),
		     rad,
		     rads,
		     ave_curvatures);


    // compute the saliency
    vector<real_type> saliency_levels[num_levels];
    for (int j=0; j<num_levels; j++) {
	saliency_levels[j].resize(curv_radius.size());
	forvec(i, curv_radius) {
	    saliency_levels[j][i] = fabs(ave_curvatures[j][1][i] - ave_curvatures[j][0][i]);
	}
    }



    // weird scales / normalization
    vector<real_type> maximas[num_levels];
    for (unsigned v=0; v<kdGetPoint.mesh.verts.size(); v++ ) {

	bool ismax[num_levels];
	for (int i=0; i<num_levels; i++) ismax[i]=true;

	for (TriangleMesh::VertexVertexIteratorI vi(kdGetPoint.mesh, v); !vi.done(); ++vi) {
	    for (int j=0; j<num_levels; j++) {
		if (saliency_levels[j][v] <= saliency_levels[j][*vi])
		    ismax[j] = false;
	    }
	}

	for (int j=0; j<num_levels; j++) {
	    if (ismax[j])
		maximas[j].push_back(saliency_levels[j][v]);
	}
    }

    for (int j=0; j<num_levels; j++) {
	real_type m = max_element(saliency_levels[j]);
	real_type mbar = 0;
	forvec(i, maximas[j]) {
	    mbar += maximas[j][i];
	}
       	mbar = maximas[j].size()<=1 ? 0 : (mbar-m) / (maximas[j].size()-1);

	real_type scale = square(m-mbar) / m;
	cerr<<"scale: "<<scale<<endl;
	forvec(i, saliency_levels[j]) {
	    saliency_levels[j][i] *= scale;
	}
    }
    


    for (unsigned i=0; i<curv_radius.size(); i++) {
	curv_radius[i] = 0;
	for (int j=0; j<num_levels; j++) {
	    curv_radius[i] += saliency_levels[j][i];
	}
	curv_radius[i] /= num_levels;

	// add small epsilon just so it's not == 0
	curv_radius[i] = fabs(curv_radius[i])+1e-30;
	if (curv_radius[i] == 0)
	    cerr<<"aoeu"<<endl;
	curv_radius[i] = 25.0 * pow(curv_radius[i], -1.0);
    }
#endif
}


int MeshGuidanceField::ClosestPoint(const Point3 &p) {
    return kdtree->FindMin(p);
}


void MeshGuidanceField::OrderedPointTraverseStart(const Point3 &p) {

    if (kdOrderedTraverse)
	delete kdOrderedTraverse;
    kdOrderedTraverse = new kdtree_type::OrderedIncrementalTraverse(*kdtree, p);
}

int MeshGuidanceField::OrderedPointTraverseNext(real_type &squared_dist) {
    if (kdOrderedTraverse->empty()) return -1;
    return kdOrderedTraverse->GetNext(squared_dist);
}

void MeshGuidanceField::OrderedPointTraverseEnd() {
    delete kdOrderedTraverse;
    kdOrderedTraverse = NULL;
}


const Point3& MeshGuidanceField::PointLocation(int i) const {
    return kdGetPoint(i);
}

int MeshGuidanceField::NumPoints() const {
    return (int)kdGetPoint.mesh.verts.size();
}


void MeshGuidanceField::Extract(vector<Point3> &pts, vector<Vector3> &norms, vector<real_type> &rad) {
    pts.resize(kdGetPoint.mesh.verts.size());
    norms.resize(kdGetPoint.mesh.verts.size());
    rad.resize(kdGetPoint.mesh.verts.size());
    for (unsigned i=0; i<kdGetPoint.mesh.verts.size(); i++) {
	pts[i]   = kdGetPoint.mesh.verts[i].point;
	norms[i] = kdGetPoint.mesh.verts[i].normal;
	rad[i]   = 1 / ideal_length[i];
    }
}


