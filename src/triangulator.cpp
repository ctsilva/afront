#include "common.h"
#include "front.h"
#include "guidance.h"
#include "triangulator.h"

// set this to 0 if you want projections to happen immediately instead of in a different thread
// WARNING: this will only work for projectors that don't have any state!
extern int idealNumThreads;
#define TRIANGULATOR_NUM_PROJECTORS (idealNumThreads/2) //(std::max(1,idealNumThreads-1))


//static real_type edge_split_ratio = 1.2;unused
static real_type snap_distance = 0.01;


// comparison operator for sorting edge priorities
bool operator<(const FrontElement::priority_type &l, const FrontElement::priority_type &r) {
    if (l.first < r.first) return true;
    if (l.first > r.first) return false;
    return (l.second < r.second);
}



void Triangulator::CreateVertex(const Point3 &p, const Vector3 &n, FrontElement &fe, bool boundary) {
    fe = FrontElement(p, n, numVertsAdded, controller.MaxStepLength(p));
    controller.AddVertex(numVertsAdded, p, flipOutput?-n:n, boundary);
    numVertsAdded++;
}


void Triangulator::CreateTriangle(int v1, int v2, int v3) {
    if (flipOutput)		controller.AddTriangle(numFacesAdded, v2, v1, v3);
    else				controller.AddTriangle(numFacesAdded, v1, v2, v3);
    numFacesAdded++;
}

void Triangulator::CreateTriangle(const feli v1, const feli v2, const feli v3) {
    CreateTriangle(v1->vertindex, v2->vertindex, v3->vertindex);
}



// return the cos of the angle, but shifted to preserve the angle ordering
real_type ccwAngleAlmost(const Vector3 &norm, const Vector3 &_v1, const Vector3 &_v2) {

    Vector3 v1 = _v1 - norm.dot(_v1)*norm;	v1.normalize();
    Vector3 v2 = _v2 - norm.dot(_v2)*norm;	v2.normalize();

    Vector3 perp = norm.cross(v1);


    if (perp.dot(v2) > 0) {
	return 1-v1.dot(v2);
    } else {
	return 3+v1.dot(v2);
    }
}


void Triangulator::RequestProjection(feli e) {
    e->proj_res.result = PROJECT_INCOMPLETE;

    Point3 tentative_p;
    Vector3 tentative_n;
    feli n = Front::NextElement(e);
    GetTentativePoint(*e, *n, tentative_p, tentative_n);


    if (TRIANGULATOR_NUM_PROJECTORS <= 0) {
	// just do it immediately
	e->proj_res.result = controller.ProjectPoint(*e, *n,
						     tentative_p, tentative_n,
						     e->proj_res.position, e->proj_res.normal);
    } else {
	// put it in the work queue
	work_cs.enter();
	real_type priority = PrioritizeEdgeGrow(*e, *n);
	work_queue.push(ProjectionWork(priority, &e->proj_res, tentative_p, tentative_n, &*e, &*n));
	work_cs.leave();
    }
}

void Triangulator::WaitForProjection(feli e) {
    while (e->proj_res.result == PROJECT_INCOMPLETE) {
	thlib::Thread::yield();
    }
}


void* Triangulator::ProjectorThreadMain(void* arg) {

    Triangulator *tri = (Triangulator*)arg;

    while (!tri->work_quit) {

	ProjectionWork pw;
	bool havework = false;
    
	tri->work_cs.enter();
	if (tri->work_queue.size()) {
	    pw = tri->work_queue.top();
	    tri->work_queue.pop();
	    havework=true;
	}
	tri->work_cs.leave();

	if (havework) {
	    pw.pr->result = tri->controller.ProjectPoint(*pw.base1, *pw.base2,
							 pw.fp, pw.fn,
							 pw.pr->position, pw.pr->normal);
	} else {
	    thlib::Thread::yield();
	    //    sleep(0);
	}
    }
    return NULL;
}




void Triangulator::GetTentativePoint(const FrontElement &p1, const FrontElement &p2, Point3 &tentative_p, Vector3 &tentative_n) {


    real_type elen = Point3::distance(p1.position, p2.position);

    real_type step1 = p1.max_step;
    real_type step2 = p2.max_step;

    /*
    real_type step1 = clamp(p1.max_step, elen*reduction, elen/reduction);
    real_type step2 = clamp(p2.max_step, elen*reduction, elen/reduction);


    if (step1+step2 < elen) {
	step1=step2=elen*reduction;
    }
    */

    real_type b = (elen*elen + step2*step2 - step1*step1) / (2*elen);
    b = clamp<real_type>(b, 0, elen);


    real_type a = elen-b;

    real_type h1 = (real_type)sqrt(step1*step1 - a*a);


    Vector3 ndir = p1.normal+p2.normal;
    ndir.normalize();

    Vector3 udir = p2.position-p1.position;
    udir.normalize();

    Vector3 vdir = ndir.cross(udir);
    vdir.normalize();



    tentative_p = p1.position + (a)*udir + h1*vdir;
    tentative_n = ndir;

}



///////////////////////////////////////////////////////////////////////////////
// Prioritize edges based on their triangles


void circumradius_inradius(real_type a, real_type b, real_type c, real_type &circumrad, real_type &inrad) {

    real_type s = (a+b+c) * 0.5;

    real_type den = s*(a+b-s)*(a+c-s)*(b+c-s);
    if (den<=0) {
	circumrad = 1;
	inrad = 0;
	return;
    }


    den = 4*sqrt(den);

    circumrad = (a*b*c) / den;
    inrad = (a*b*c) / (4*circumrad*s);
}


void circumradius_inradius(const Point3 &p0, const Point3 &p1, const Point3 &p2, real_type &circumrad, real_type &inrad) {
    real_type a = Point3::distance(p0,p1);
    real_type b = Point3::distance(p1,p2);
    real_type c = Point3::distance(p2,p0);
    circumradius_inradius(a, b, c, circumrad, inrad);
}




real_type Triangulator::PrioritizeConnection(const FrontElement &p1, const FrontElement &p2, const FrontElement &p3) {

    // get the ratio of the longest edge to the shortest
    real_type elen[3] = { Point3::distance(p1.position, p2.position),
			  Point3::distance(p2.position, p3.position),
			  Point3::distance(p3.position, p1.position) };

    real_type cir_rad, in_rad;
    circumradius_inradius(elen[0], elen[1], elen[2], cir_rad, in_rad);
    return (cir_rad/in_rad);
}


real_type Triangulator::PrioritizeEdgeGrow(const FrontElement &p1, const FrontElement &p2) {

    // we want to add triangles that are closest to equilateral as possible
    real_type elen = Point3::distance(p1.position, p2.position);


    real_type cir_rad, in_rad;
    circumradius_inradius(elen, p1.max_step, p2.max_step, cir_rad, in_rad);
    return (cir_rad/in_rad);
}


void Triangulator::PrioritizeEdgeGrow(feli e1)
{
    feli e2 = Front::NextElement(e1);

    if (e1->vertindex == e2->vertindex) {
	cerr<<"same vertex on both sides of edge"<<endl;
	BREAK;
    }

    if (Point3::distance(e1->position, e2->position) == 0) {
	cerr<<"zero length edge"<<endl;
	BREAK;
    }


    e1->priority.first = PRIORITY_GROW_EDGE;
    e1->priority.second = PrioritizeEdgeGrow(*e1, *e2);

    if ((Point3::distance(workingAreaCenter, e1->position)>workingAreaRadius &&
	 Point3::distance(workingAreaCenter, e2->position)>workingAreaRadius)) {
	e1->priority.first += PRIORITY_OWA;
    }

    heap.update_position(e1->heap_position);
}



void Triangulator::PrioritizeEdgeConnect(feli e1)
{
    feli e2 = Front::NextElement(e1);

    if (e1->vertindex == e2->vertindex) {
	cerr<<"same vertex on both sides of edge"<<endl;
	BREAK;
    }

    if (Point3::distance(e1->position, e2->position) == 0) {
	cerr<<"zero length edge"<<endl;
	BREAK;
    }


    feli across;
    if (!GetConnection(e1, across)) {
	PrioritizeEdgeFailsafe(e1);
	return;
    }

    e1->priority.first = PRIORITY_CONNECT;
    e1->priority.second = PrioritizeConnection(*e1, *e2, *across);

    if ((Point3::distance(workingAreaCenter, e1->position)>workingAreaRadius &&
	 Point3::distance(workingAreaCenter, e2->position)>workingAreaRadius)) {
	e1->priority.first += PRIORITY_OWA;
    }

    heap.update_position(e1->heap_position);
}


void Triangulator::PrioritizeEdgeFailsafe(feli e) 
{

    if (e->flags & FRONT_FLAG_FROM_GROW) {
	e->priority.first = PRIORITY_FAILSAFE_WILLFAIL;
	e->priority.second= 0;
    } else {

	feli prev = Front::PrevElement(e);
	feli next = Front::NextElement(e);

	feli pprev = Front::PrevElement(prev);

	if (pprev->flags&FRONT_FLAG_FROM_GROW && pprev == Front::NextElement(next)) {
	    e->priority.first = PRIORITY_FAILSAFE_WILLFAIL;
	    e->priority.second= 0;
	} else {
	    e->priority.first = PRIORITY_FAILSAFE;
	    e->priority.second = ccwAngleAlmost(e->normal, next->position-e->position, prev->position-e->position);
	}
    }

    heap.update_position(e->heap_position);
}


///////////////////////////////////////////////////////////////////////////////
// determine if triangles are legal

bool Triangulator::IntersectsFence(const feli &e1, const Point3 &p1, const Point3 &p2) const {

    feli e2 = Front::NextElement(e1);

    Point3 points[8];

    points[0] = e1->position;
    points[1] = e2->position;

    real_type scalefac = controller.GetFenceScale();
    real_type len1 = e1->max_step * scalefac;
    real_type len2 = e1->max_step * scalefac;

    points[2] = points[0] + len1 * e1->normal;
    points[4] = points[1] + len2 * e2->normal;
    points[3] = points[2] + (real_type)0.5 * (points[4]-points[2]);

    points[5] = points[0] - len1 * e1->normal;
    points[7] = points[1] - len2 * e2->normal;
    points[6] = points[5] + (real_type)0.5 * (points[7]-points[5]);


    for (int i=0; i<6; i++) {

	Triangle3 tri;
	switch (i) {
	case 0:
	    tri = Triangle3(points[0], points[3], points[1]);
	    break;
	case 1:
	    tri = Triangle3(points[1], points[6], points[0]);
	    break;
	case 2:
	    tri = Triangle3(points[0], points[2], points[3]);
	    break;
	case 3:
	    tri = Triangle3(points[1], points[3], points[4]);
	    break;
	case 4:
	    tri = Triangle3(points[1], points[7], points[6]);
	    break;
	case 5:
	    tri = Triangle3(points[0], points[6], points[5]);
	    break;
	}


	real_type t;
	if (tri.intersect_segment(p1, p2, t)) {
	    return true;
	}
    }

    return false;
}


bool Triangulator::InLegalFenceCorner(const feli &e, const Point3 &p) const {

    feli prev = Front::PrevElement(e);
    feli next = Front::NextElement(e);

    real_type scalefac = controller.GetFenceScale();
    real_type len = e->max_step * scalefac;
    real_type plen = prev->max_step * scalefac;
    real_type nlen = next->max_step * scalefac;


    Point3 corner[6];
    corner[0] = next->position - nlen*next->normal;
    corner[1] = next->position + nlen*next->normal;
    corner[2] = e->position - len*e->normal;
    corner[3] = e->position + len*e->normal;
    corner[4] = prev->position - plen*prev->normal;
    corner[5] = prev->position + plen*prev->normal;


    Vector3 ring[8];
    ring[0] = prev->position - e->position;
    ring[1] = Point3::midpoint(corner[5], corner[3]) - e->position;
    ring[2] = corner[3] - e->position;
    ring[3] = Point3::midpoint(corner[3], corner[1]) - e->position;
    ring[4] = next->position - e->position;
    ring[5] = Point3::midpoint(corner[0], corner[2]) - e->position;
    ring[6] = corner[2] - e->position;
    ring[7] = Point3::midpoint(corner[2], corner[4]) - e->position;

    for (int r=0; r<8; r++)
	ring[r].normalize();


    // use a plane that will for sure intersect with the ring at least once

    for (int r=-1; r<8; r++) {

	Vector3 udir = p - e->position;
	udir.normalize();
	Vector3 vdir;
		
	if (r==-1) {

	    vdir=Vector3(0,0,0);
	    for (int i=0; i<8; i++) {
		vdir += ring[i].cross(ring[(i+1)%8]);
	    }
	    vdir.normalize();

	} else {
	    vdir = (real_type)0.5*ring[r] + (real_type)0.5*ring[(r+1)%8];
	}


	Vector3 norm = udir.cross(vdir);
	norm.normalize();

	real_type dists[8];
	for (int i=0; i<8; i++) {
	    dists[i] = norm.dot(ring[i]);
	}

	real_type largest_dot = -1e34;
	bool onfront = false;

	for (int i=0; i<8; i++) {

	    int j = (i+1) % 8;

	    if (dists[i] * dists[j] > 0) continue;	// same side

	    real_type alpha = dists[i] / (dists[i]-dists[j]);

	    Vector3 intersection = alpha*ring[j] + (1-alpha)*ring[i];
	    intersection.normalize();

	    real_type tdot = udir.dot(intersection);

	    if (tdot > largest_dot) {
		largest_dot = tdot;

		Vector3 tnorm = (ring[i]).cross(ring[j]);
		onfront = (tnorm.dot(udir) > 0);
	    }
	}


	if (largest_dot > -1)
	    return onfront;
	else 
	    continue;

    }

    return false;
}


real_type Triangulator::DistanceToFence(const feli &e1, const Point3 &p) const {

    feli e2 = Front::NextElement(e1);

    Point3 points[8];

    points[0] = e1->position;
    points[1] = e2->position;

    real_type len = Point3::distance(points[0], points[1]);
    real_type len1 = Point3::distance(points[0], Front::PrevElement(e1)->position);
    real_type len2 = Point3::distance(points[1], Front::NextElement(e2)->position);
    len1 = (len+len1) / 2;
    len2 = (len+len2) / 2;


    points[2] = points[0] + len1 * e1->normal;
    points[4] = points[1] + len2 * e2->normal;

    points[5] = points[0] - len1 * e1->normal;
    points[7] = points[1] - len2 * e2->normal;

    real_type ret = Point3::distance(Triangle3(points[5], points[2], points[4]).closest_point(p), p);
    ret = std::min(ret, Point3::distance(Triangle3(points[5], points[4], points[7]).closest_point(p), p));

    return ret;
}


real_type Triangulator::DistanceToFence(const Point3 &ep1, const Point3 &ep2, const Vector3 &en1, const Vector3 &en2, const Point3 &p) const {

    Point3 points[8];

    points[0] = ep1;
    points[1] = ep2;

    real_type len1 = Point3::distance(points[0], ep1);
    real_type len2 = Point3::distance(points[1], ep2);
    len1 = Point3::distance(ep1,ep2);
    len2 = len1;


    points[2] = points[0] + len1 * en1;
    points[4] = points[1] + len2 * en2;

    points[5] = points[0] - len1 * en1;
    points[7] = points[1] - len2 * en2;

    real_type ret = Point3::distance(Triangle3(points[5], points[2], points[4]).closest_point(p), p);
    ret = std::min(ret, Point3::distance(Triangle3(points[5], points[4], points[7]).closest_point(p), p));

    return ret;

}


bool Triangulator::GrowthFromEdgeLegal(const feli e1, const Point3 &p) const {

    feli e2 = Front::NextElement(e1);

    Point3 points[8];

    points[0] = e1->position;
    points[1] = e2->position;

    real_type len = Point3::distance(points[0], points[1]);
    real_type len1 = Point3::distance(points[0], Front::PrevElement(e1)->position);
    real_type len2 = Point3::distance(points[1], Front::NextElement(e2)->position);
    len1 = (len+len1) / 2;
    len2 = (len+len2) / 2;


    points[2] = points[0] + len1 * e1->normal;
    points[4] = points[1] + len2 * e2->normal;
    points[3] = points[2] + (real_type)0.5 * (points[4]-points[2]);

    points[5] = points[0] - len1 * e1->normal;
    points[7] = points[1] - len2 * e2->normal;
    points[6] = points[5] + (real_type)0.5 * (points[7]-points[5]);


    Vector3 edir = e2->position - e1->position;
    edir.normalize();

    if (ccwAngleAlmost(edir, p-e1->position, points[3]-e1->position) > ccwAngleAlmost(edir, p-e1->position, points[6]-e1->position))
	return false;

    // FIXME - more checks for the other corner triangles??

    return true;
}


real_type PointSegmentDistance(const Point3 &p, const Point3 &e1, const Point3 &e2) {

    Vector3 edir = e2 - e1;
    real_type elen = edir.length();

    real_type pdist = edir.dot(p-e1) / elen;

    if (pdist < 0)
	return Point3::distance(p, e1);
    if (pdist > elen)
	return Point3::distance(p, e2);
    return Point3::distance(p, e1 + (pdist/elen)*edir);
}


bool Triangulator::TriangleLegal(const feli e1, const feli e2, const feli *across, const Point3 &across_p, const Vector3 &across_n, vector<feli> *possible_intersects, bool *isclose) const {

    if (isclose)
	*isclose = false;

    // don't make degenerate triangles
    if (across && (e1->vertindex==(*across)->vertindex || e2->vertindex==(*across)->vertindex))
	return false;


    // if we've got a 2 vertex front, it's always legal to put down a new vert
    // this is how we start the triangulation when there aren't any boundaries
    if (Front::NextElement(e2) == e1)
	return true;


    const feli* tri[3] = { &e1, &e2, across };
    Point3 trip[3] = { e1->position, e2->position, across_p };
    Vector3 trin[3] = { e1->normal, e2->normal, across_n };

    Vector3 fnorm = (trip[1] - trip[0]).cross(trip[2] - trip[0]);
    fnorm.normalize();

    // check that the tri's normal jives with the vertex normals

    for (int i=0; i<3; i++) {
	if (fnorm.dot(trin[i]) < 0.5)
	    return false;
    }



    for (int i=0; i<3; i++) {
	if (!tri[i] || !tri[(i+1)%3]) continue;
	if (!(*tri[(i+1)%3] == Front::NextElement(*tri[i]))) continue;

	// we're growing from this edge - make sure we're growing the correct direction
	if (!GrowthFromEdgeLegal(*tri[i], trip[(i+2)%3]))
	    return false;
    }

    // make sure we're in the correct sector
    for (int i=0; i<3; i++) {
	if (!tri[i]) continue;

	if (!tri[(i+1)%3] || *tri[(i+1)%3] != Front::NextElement(*tri[i])) {
	    if (!InLegalFenceCorner(*tri[i], trip[(i+1)%3]))
		return false;
	}

	if (!tri[(i+2)%3] || *tri[(i+2)%3] != Front::PrevElement(*tri[i])) {
	    if (!InLegalFenceCorner(*tri[i], trip[(i+2)%3]))
		return false;
	}
    }



    // look at all the edges that come within some bounding box
    Box3 tribox = Box3::bounding_box(trip[0], trip[1], trip[2]);
    real_type ave_edge_len = Point3::distance(trip[0],trip[1]) + Point3::distance(trip[1],trip[2]) + Point3::distance(trip[2],trip[0]);
    ave_edge_len /= 3;
    for (int i=0; i<3; i++) {
	tribox.update(trip[i] + ave_edge_len * trin[i]);
	tribox.update(trip[i] - ave_edge_len * trin[i]);
    }
    tribox.enlarge(0.4);


    real_type buffer_size;
    real_type mbuffer_size;
    {
	buffer_size = (Point3::distance(trip[0],trip[1]) + Point3::distance(trip[1],trip[2]) + Point3::distance(trip[2],trip[0])) / 3;
	buffer_size /= 2;
	mbuffer_size = buffer_size / 4;
    }


    static_vector(int,new_edges);
    if (!across) {
	new_edges.push_back(1);
	new_edges.push_back(2);
    } else {
	if (*across != Front::NextElement(e2))
	    new_edges.push_back(1);
	if (*across != Front::PrevElement(e1))
	    new_edges.push_back(2);
    }


    // first check the distances to the neighboring edges since we'll skip them later
    for (int i=0; i<3; i++) {
	if (!tri[i]) continue;


	feli nbrs[2] = { Front::NextElement(*tri[i]), Front::PrevElement(*tri[i]) };

	for (int j=1; j<3; j++) {

	    if (tri[(i+j)%3] && *tri[(i+j)%3] == nbrs[j-1]) continue;

	    real_type dist = std::min(PointSegmentDistance(nbrs[j-1]->position, trip[i], trip[(i+j)%3]),
				      PointSegmentDistance(trip[(i+j)%3], trip[i], nbrs[j-1]->position));

	    if (dist < mbuffer_size) {
		return false;
	    }

	    if (isclose && dist < buffer_size) {
		*isclose = true; break;
	    }
	}
    }





    static_vector(feli,_possible_intersects);
    if (!possible_intersects) {
	kdtree.GetIntersectedBoxes(*this, tribox, _possible_intersects);
	possible_intersects = &_possible_intersects;
    }

    bool existing = false;

    for (unsigned i=0; i<possible_intersects->size(); i++) {

	feli pi = (*possible_intersects)[i];
	feli pin = Front::NextElement(pi);

	if (fnorm.dot((pi->normal+pin->normal).normalized()) < -0.7) continue;

	if ((e1->flags & FRONT_FLAG_FEATURE ||
	     e2->flags & FRONT_FLAG_FEATURE) &&
	    e1->front == e2->front &&
	    e1->front != pi->front)
	    continue;


	if (!across && Point3::distance(pi->position, across_p) == 0)
	    existing = true;
	if (!across && Point3::distance(pin->position, across_p) == 0)
	    existing = true;

	if (existing) {
	    cerr<<"projected to already existing point!"<<endl;
	    return false;
	    BREAK;
	}


	// we've already checked all the edges that touch the tri's vertices, so ignore anything with that feature
	{
	    bool is_sharedcorner = false;
	    for (int j=0; j<3; j++) {

		if (tri[j] && (pi->vertindex==(*tri[j])->vertindex || pin->vertindex==(*tri[j])->vertindex)) {
		    is_sharedcorner = true; break;
		}
	    }
	    if (is_sharedcorner) continue;
	}


	// see if the edges are too close
	for (unsigned j=0; j<new_edges.size(); j++) {

	    real_type dist = std::min(DistanceToFence(pi, trip[new_edges[j]]), DistanceToFence(pi, trip[(new_edges[j]+1)%3]));
	    if ((!tri[new_edges[j]] || (*tri[new_edges[j]])->vertindex != pi->vertindex) && 
		(!tri[(new_edges[j]+1)%3] || (*tri[(new_edges[j]+1)%3])->vertindex != pi->vertindex))
		dist = std::min(dist, DistanceToFence(trip[new_edges[j]], trip[(new_edges[j]+1)%3], trin[new_edges[j]], trin[(new_edges[j]+1)%3], pi->position));

	    if ((!tri[new_edges[j]] || (*tri[new_edges[j]])->vertindex != pin->vertindex) && 
		(!tri[(new_edges[j]+1)%3] || (*tri[(new_edges[j]+1)%3])->vertindex != pin->vertindex))
		dist = std::min(dist, DistanceToFence(trip[new_edges[j]], trip[(new_edges[j]+1)%3], trin[new_edges[j]], trin[(new_edges[j]+1)%3], pin->position));

	    if (dist < mbuffer_size) {
		return false;
	    }

	    if (isclose && dist < buffer_size) {
		*isclose = true; break;
	    }

	    if (!across) {
		dist = PointSegmentDistance(across_p, pi->position, pin->position);

		if (dist < mbuffer_size) {
		    return false;
		}

		if (isclose && dist < buffer_size) {
		    *isclose = true; break;
		}

	    }
	}


	if (IntersectsFence(pi, e1->position, across_p))
	    return false;
	if (IntersectsFence(pi, e2->position, across_p))
	    return false;

    }

    return true;
}



real_type cosangle(const Point3 &p0, const Point3 &p1, const Point3 &p2) {
    Vector3 e0 = p0-p1;
    Vector3 e1 = p2-p1;
    return (e0.dot(e1) / (e0.length() * e1.length()));
}
real_type cosangle(const Triangulator::feli &f0, const Triangulator::feli &f1, const Triangulator::feli &f2) {
    return cosangle(f0->position, f1->position, f2->position);
}


Triangulator::feli Triangulator::ClosestPoint(const Point3 &p, feli skip1, feli skip2) {

    kdtree_type::OrderedTraverse ot(kdtree, p, *this);

    while (1) {
	feli closest;
	ot.next(closest);

	if (Point3::distance(closest->position, p) > Point3::distance(Front::NextElement(closest)->position, p))
	    closest = Front::NextElement(closest);

	if (closest->vertindex==skip1->vertindex || closest->vertindex==skip2->vertindex)
	    continue;

	return closest;
    }
}


real_type Triangulator::CosSmallestRemainingAngle(feli e1, feli e2, feli across) {

    real_type ret = gtb::max3(cosangle(e1, e2, across),
			      cosangle(e2, across, e1),
			      cosangle(across, e1, e2));

    feli other;

    other = Front::PrevElement(e1);
    if (other != across)		ret = std::max(ret, cosangle(other, e1, across));

    other = Front::NextElement(e2);
    if (other != across)		ret = std::max(ret, cosangle(across, e2, other));

    other = Front::NextElement(across);
    if (other != e1)			ret = std::max(ret, cosangle(e1, across, other));

    other = Front::PrevElement(across);
    if (other != e2)			ret = std::max(ret, cosangle(other, across, e2));

    return ret;
}


bool Triangulator::GetConnection(feli e1, feli &across) {
    feli e2 = Front::NextElement(e1);

    // check for closing a 3 vert front
    across = Front::NextElement(e2);
    if (Front::NextElement(across) == e1) {
	if (TriangleLegal(e1, e2, &across, across->position, across->normal, NULL, NULL))
	    return true;
    }


    Vector3 n;
    Point3 p = Point3::midpoint(e1->position, e2->position);


    kdtree_type::OrderedTraverse ot(kdtree, p, *this);

    while (1) {

	real_type dist = ot.next(across);
	if (dist<0 || dist>2*Point3::distance(e1->position, e2->position)) break;


	if (Point3::distance(across->position, p) > Point3::distance(Front::NextElement(across)->position, p))
	    across = Front::NextElement(across);


	// don't connect it if it's a really bad triangle
	real_type priority = PrioritizeConnection(*e1, *e2, *across);
	extern real_type bad_connect_priority;
	if (priority > bad_connect_priority)
	    continue;

	bool isclose;
	if (TriangleLegal(e1, e2, &across, across->position, across->normal, NULL, &isclose)) {

	    feli pf[3] = { Front::PrevElement(across), across, Front::NextElement(across) };
	    bool tl[3] = { TriangleLegal(e1, e2, &pf[0], pf[0]->position, pf[0]->normal, NULL, NULL),
			   true,
			   TriangleLegal(e1, e2, &pf[2], pf[2]->position, pf[2]->normal, NULL, NULL) };

	    real_type mincangle[3] = { tl[0] ? CosSmallestRemainingAngle(e1, e2, pf[0]) : 10,
				       CosSmallestRemainingAngle(e1, e2, pf[1]),
				       tl[2] ? CosSmallestRemainingAngle(e1, e2, pf[2]) : 10 };


	    int touse=0;
	    if (mincangle[1] < mincangle[0])		touse=1;
	    if (mincangle[2] < mincangle[touse])	touse=2;

	    if (mincangle[touse] > 1) {
		cerr<<"strange min angle!"<<endl;
		return false;
		BREAK;
	    }


	    across = pf[touse];


	    return true;
	}
    }

    return false;
}


void Triangulator::ConnectTriangle(feli e1, feli across, bool dofailsafe) 
{
    bool failsafe = e1->priority.first >= PRIORITY_FAILSAFE_ANY;

    feli e2 = Front::NextElement(e1);

    if (across==e1 || across==e2) {
	cerr<<"connecting degenerate triangle!! something is wacked out!"<<endl;
	exit(-1);
    }

    across->flags &= ~FRONT_FLAG_FROM_GROW;
    e1->flags &= ~FRONT_FLAG_FROM_GROW;
    Front::NextElement(e1)->flags &= ~FRONT_FLAG_FROM_GROW;


    bool e1ear = (Front::PrevElement(e1) == across);
    bool e2ear = (Front::NextElement(e2) == across);

    if (failsafe) {
	dbgClear();
	DbgPoly::add(0, e1->position, Point3(1,0,0),
		     e2->position, Point3(1,0,0), 
		     across->position, Point3(1,0,0));
	redrawAndWait(' ', true);
    }

    // add this triangle if it is a normal traingle, or it is failsafe and failsafe is allowed
    if ((!failsafe || dofailsafe) &&
	(!(e1->flags&FRONT_FLAG_BOUNDARY) &&
	 !(e2->flags&FRONT_FLAG_BOUNDARY) &&
	 !(across->flags&FRONT_FLAG_BOUNDARY)))
	CreateTriangle(e1, e2, across);

    // see if we have to do a split or merge first
    if (!e1ear && !e2ear) {

	feli i1 = e1;
	feli i2 = across;

	feli i1prev = Front::PrevElement(i1);
	feli i1next = Front::NextElement(i1);
	feli i2prev = Front::PrevElement(i2);
	feli i2next = Front::NextElement(i2);

	// i1 and i2 don't point to the same place anymore, so remove and re-insert them into the kd tree
	WaitForProjection(i1);
	WaitForProjection(i2);
	kdtree.Remove(*this, i1);
	kdtree.Remove(*this, i2);

	// we're reaching across to another vert - either have to split or merge
	feli n1,n2;	// the 2 new verts that will be created (dups of existing ones)
	if (e1->front == across->front) {
	    // split
	    Front *nf = Front::Split(i1, i2, n1, n2);
	    fronts.push_back(nf);
	} else {
	    // merge
	    Front::Merge(i1, i2, n1, n2);
	}


	// add the new edges to the heap and kdtree
	RequestProjection(n1);
	RequestProjection(n2);
	RequestProjection(i1);
	RequestProjection(i2);
	heap.push(n1);
	heap.push(n2);
	kdtree.Insert(*this, n1);
	kdtree.Insert(*this, n2);

	kdtree.Insert(*this, i1);
	kdtree.Insert(*this, i2);



	if (failsafe) {
	    PrioritizeEdgeFailsafe(i1prev);
	    PrioritizeEdgeFailsafe(n1);
	} else {
	    PrioritizeEdgeGrow(i1prev);
	    PrioritizeEdgeGrow(n1);
	}


	// now we have to trim the ear at e1
	e1 = i1;
	e2 = i1next;
	across = n2;

	e1ear = true;
	e2ear = (Front::NextElement(e2) == across);

    }


    // close a front
    if (e1ear && e2ear) {
	WaitForProjection(e1);
	WaitForProjection(e2);
	WaitForProjection(across);
		
	// remove all 3 edges from the kdtree and heap
	heap.remove(e1->heap_position);
	heap.remove(e2->heap_position);
	heap.remove(across->heap_position);

	kdtree.Remove(*this, e1);
	kdtree.Remove(*this, e2);
	kdtree.Remove(*this, across);

	// now remove them from the front - free's the FrontElement structure!
	Front::RemoveElement(e1);
	Front::RemoveElement(e2);
	Front::RemoveElement(across);

	e1ear = e2ear = false;

    }

    // cut an ear
    if (e1ear || e2ear) {
	if (e2ear) {
	    // rotate the edges so it's an e1 ear
	    std::swap(e1, across);
	    std::swap(e1,e2);
	}


	// remove e1 from everything, remove across from the kdtree
	WaitForProjection(e1);
	WaitForProjection(across);
	heap.remove(e1->heap_position);
	kdtree.Remove(*this, e1);
	kdtree.Remove(*this, across);
	Front::RemoveElement(e1);

	// re-add across since e1 has been removed
	RequestProjection(across);
	kdtree.Insert(*this, across);

	if (failsafe) {
	    PrioritizeEdgeFailsafe(across);
	    PrioritizeEdgeFailsafe(Front::PrevElement(across));
	} else {
	    PrioritizeEdgeGrow(across);
	    PrioritizeEdgeGrow(Front::PrevElement(across));
	}

    }
}



void Triangulator::GrowEdge(feli e1, const Point3 &p, const Vector3 &v) {

    WaitForProjection(e1);

    // remove the old edge from the kd tree
    kdtree.Remove(*this, e1);

    // insert the new vertex
    FrontElement fe;
    CreateVertex(p, v, fe, false);
    feli ne = Front::InsertElement(Front::NextElement(e1), fe);

    ne->flags |= FRONT_FLAG_FROM_GROW;
    e1->flags &= ~FRONT_FLAG_FROM_GROW;
    Front::NextElement(ne)->flags &= ~FRONT_FLAG_FROM_GROW;

    // add e1 back (since it now connects to a new vert)
    RequestProjection(e1);
    kdtree.Insert(*this, e1);

    // add the new edge to the kdtree and heap
    RequestProjection(ne);
    kdtree.Insert(*this, ne);
    heap.push(ne);

    // reset the priority on the edges that may be 

    // re-prioritize any edges that are close to the new triangle
    PrioritizeEdgeGrow(e1);
    PrioritizeEdgeGrow(Front::PrevElement(e1));
    PrioritizeEdgeGrow(ne);


    // add the new triangle to the output
    CreateTriangle(e1, Front::NextElement(ne), ne);
}


void Triangulator::SetupInitialCreaseFronts
    (const vector<Point3> &points, // The points on the creases
     const vector<Vector3> &onormals, // The normals for the output
     const vector< vector<int> > &indices, // The front element point indices
     const vector< vector<Vector3> > &normals, // The normals for the
     // front elements
     const vector< int > &corner_triangles) // triangles to be added
// before triangulation starts
 
{
    vector<int> index_map;
    for (unsigned i=0; i<points.size(); ++i) {
	index_map.push_back(numVertsAdded++);
	controller.AddVertex(index_map[i], points[i], 
			     flipOutput?onormals[i]:-onormals[i], true);
    }

    for (unsigned i=0; i<corner_triangles.size(); i+=3)
	controller.AddTriangle(numFacesAdded++, 
			       corner_triangles[i],
			       corner_triangles[i+1],
			       corner_triangles[i+2]);

    for (unsigned i=0; i<indices.size(); ++i) {
	Front *front = new Front();
	for (unsigned j=0; j<indices[i].size(); ++j) {
	    const Point3 &p(points[indices[i][j]]);
	    FrontElement fe(p,
			    normals[i][j],
			    index_map[indices[i][j]],
			    controller.MaxStepLength(p));
	    front->AddElement(fe);
	}
	feli f = front->FirstElement();
	do {
	    RequestProjection(f);
	    heap.push(f);
	    kdtree.Insert(*this, f);
	    f = Front::NextElement(f);
	} while (f != front->FirstElement());
	fronts.push_back(front);
    }
}

void Triangulator::SetupInitialFronts(const vector< vector<Point3> > &ipts, const vector< vector<Vector3> > &inorms) {

    for (unsigned i=0; i<ipts.size(); i++) {

	Front *front = new Front();

	for (unsigned j=0; j<ipts[i].size(); j++) {

	    FrontElement e;
	    CreateVertex(ipts[i][j], inorms[i][j], e, ipts[i].size()>2);
	    if (ipts[i].size() > 2)
		e.flags |= FRONT_FLAG_FEATURE;
	    front->AddElement(e);
	}


	// go back through and add all the edges to the heap/kdtree
	feli f=front->FirstElement();
	do {
	    RequestProjection(f);
	    heap.push(f);
	    kdtree.Insert(*this, f);

	    f = Front::NextElement(f);

	} while (f != front->FirstElement());


	fronts.push_back(front);

    }


    // go back all the fronts again and setup prioritize them
    for (unsigned fi=0; fi<fronts.size(); fi++) {

	if (!fronts[fi]->empty()) {
	    feli f=fronts[fi]->FirstElement();
	    do {
		RequestProjection(f);
		PrioritizeEdgeGrow(f);
		f = Front::NextElement(f);
	    } while (f != fronts[fi]->FirstElement());
	}
    }

    if (fronts.size())
	UpdateWorkingArea(fronts.back()->FirstElement());
}

void Triangulator::UpdateWorkingArea(feli e) {

    workingAreaCenter = e->position;
    workingAreaRadius = 10 * Point3::distance(e->position, Front::NextElement(e)->position);

    for (unsigned f=0; f<fronts.size(); f++) {
	if (fronts[f]->empty()) continue;

	feli fe = fronts[f]->FirstElement();
	do {
	    feli ne = Front::NextElement(fe);

	    if (fe->priority.first<PRIORITY_FAILSAFE_ANY && fe->priority.first>=PRIORITY_OWA &&
		Point3::distance(workingAreaCenter, fe->position)<=workingAreaRadius &&
		Point3::distance(workingAreaCenter, ne->position)<=workingAreaRadius) {

		fe->priority.first -= PRIORITY_OWA;
		heap.update_position(fe->heap_position);
	    }

	    fe = ne;
	} while (fe != fronts[f]->FirstElement());

    }

    dbgClear();
    DbgSpheres::add(workingAreaCenter, workingAreaRadius, 0, 0, 1, 0.5);

    if (heap.contents().size()) {
	kdtree.ReBuild(heap.contents(), *this);
    }

    redrawAndWait(' ');
}


void Triangulator::VerifyFronts() {

    for (unsigned f=0; f<fronts.size(); f++) {
	fronts[f]->verify();
    }

}


void Triangulator::Go
    (const vector< vector<Point3> > &ipts, const vector< vector<Vector3> > &inorms, bool failsafe,
     const vector<Point3> &crease_points,
     const vector<Vector3> &crease_onormals,
     const vector< vector<int> > &crease_indices,
     const vector< vector<Vector3> > &crease_normals,
     const vector< int > &corner_triangles)

{
    SetupInitialCreaseFronts(crease_points, crease_onormals, 
			     crease_indices, crease_normals,
			     corner_triangles);
    Go(ipts, inorms, failsafe);
}


/*
 * To cope with sharp turns in the fronts and the havoc they wreak
 * around them, we explicitly cut ears to increase angles. We do that
 * if the angle between the two edges is less than 90 degrees. Then
 * we cut the ear with one triangle:
 *

 |\
 e1 | \
 |  \
 ----
 e2
*/

void Triangulator::Go
    (const vector< vector<Point3> > &ipts, const vector< vector<Vector3> > &inorms, bool failsafe)
{
    StartWorkerThreads();
    SetupInitialFronts(ipts, inorms);

    vector< vector<int> > failsafe_holes;

    while (!heap.empty()) {
	feli top = heap.top();


	if (top->priority.first == PRIORITY_GROW_EDGE) {

	    PrioritizeEdgeGrow(top);
	    if (top != heap.top() || top->priority.first != PRIORITY_GROW_EDGE) continue;

	    feli e2 = Front::NextElement(top);
	    WaitForProjection(top);

	    if (top->proj_res.result == PROJECT_BOUNDARY) {
		top->flags |= FRONT_FLAG_BOUNDARY;
		e2->flags |= FRONT_FLAG_BOUNDARY;
	    }

	    if (top->proj_res.result != PROJECT_SUCCESS) {
		PrioritizeEdgeConnect(top);
		continue;
	    }

	    // try snapping
	    bool snapped = false;
	    feli across;
	    kdtree_type::OrderedTraverse ot(kdtree, Point3::midpoint(top->position, e2->position), *this);
	    while (1) {

		real_type dist = ot.next(across);
		if (dist<0) break;

		if (across==top || across==Front::NextElement(top))
		    continue;

		real_type dists[2] = { Point3::distance(across->position, top->proj_res.position),
				       Point3::distance(Front::NextElement(across)->position, top->proj_res.position) };

		if (dists[0] < dists[1]) {
		    dist = dists[0];
		} else {
		    dist = dists[1];
		    across = Front::NextElement(across);
		}

		if (dist < snap_distance*Point3::distance(top->position, e2->position))
		    snapped = true;

		break;
	    }

	    if (snapped) {

		bool isclose;
		bool tl = TriangleLegal(top, e2, &across, across->position, across->normal, NULL, &isclose);
		if (tl && !isclose) {
		    ConnectTriangle(top, across, failsafe);
		} else {
		    PrioritizeEdgeConnect(top);
		}

	    } else {
		bool isclose;
		bool tl = TriangleLegal(top, e2, NULL, top->proj_res.position, top->proj_res.normal, NULL, &isclose);
		if (tl && !isclose) {
		    GrowEdge(top, top->proj_res.position, top->proj_res.normal);
		} else {
		    PrioritizeEdgeConnect(top);
		}
	    }


	} else if (top->priority.first == PRIORITY_CONNECT) {

	    PrioritizeEdgeConnect(top);
	    if (top != heap.top() || top->priority.first != PRIORITY_CONNECT) continue;

	    feli across;
	    if (!GetConnection(top, across)) {
		cerr<<"GetConnection failed?!?"<<endl;
		BREAK;
	    }

	    ConnectTriangle(top, across, failsafe);

	} else if (top->priority.first == PRIORITY_FAILSAFE) {

	    // first check if it's just a 3 vertex hole
	    if (top == Front::NextElement(Front::NextElement(Front::NextElement(top)))) {
		ConnectTriangle(top, Front::PrevElement(top), true);
	    } else {

		if (!failsafe) {
		    failsafe_holes.push_back(vector<int>());
		    Front *front = top->front;
		    while (!front->empty()) {
			feli f = front->FirstElement();

			failsafe_holes.back().push_back(f->vertindex);
				
			WaitForProjection(f);
			heap.remove(f->heap_position);
			kdtree.Remove(*this, f);
			Front::RemoveElement(f);
		    }

		} else {
		    // connect the ear at top, don't check for bad triangles!
		    ConnectTriangle(top, Front::PrevElement(top), failsafe);
		}
	    }

	} else if (top->priority.first == PRIORITY_FAILSAFE_WILLFAIL) {
	    // should never get here!!!
	    cerr<<"failsafe is failing!!!"<<endl;
	    redrawAndWait('x', true);
	    exit(1);

	} else if (top->priority.first >= PRIORITY_OWA) {
	    UpdateWorkingArea(top);
	} else {
	    cerr<<"unknown priority!"<<endl;
	    BREAK;
	}

	int stopevery = 10;
	if (numFacesAdded % stopevery == 0) {
	    fprintf(stderr, "                    \rNF: %d", numFacesAdded);
	    fflush(stdout);
	    redrawAndWait(' ');
	}
    }

    controller.Finish();

    if (failsafe_holes.size() != 0) {
	cerr<<"writing failsafe holes"<<endl;

	extern char *outname;
	char fname[1024];
	strcpy(fname, outname);
	char *period = strrchr(fname, '.');
	if (period) *period = '\0';
	strcat(fname, ".failsafe.txt");
	    
	FILE *fout = fopen(fname, "w");
	if (!fout) {
	    cerr<<"couldn't open failsafe.txt"<<endl;
	} else {
	    for (unsigned h=0; h<failsafe_holes.size(); h++) {
		fprintf(fout, "Hole: %d\n", failsafe_holes[h].size());

		for (unsigned i=0; i<failsafe_holes[h].size(); i++) {
		    fprintf(fout, "%d\n", failsafe_holes[h][i]);
		}
	    }
	    fclose(fout);
	}
    }

    fprintf(stderr, "\n");
    StopWorkerThreads();
}


void Triangulator::InsertSubMesh(const TriangleMesh &mesh, const vector<int> &sides, int sidetoadd) {

    vector<int> vertmap(sides.size());
    for (unsigned i=0; i<mesh.verts.size(); i++) {
	vertmap[i] = -1;
	if (sides[i] != sidetoadd) continue;
	FrontElement fe;
	CreateVertex(mesh.verts[i].point, mesh.verts[i].normal, fe, true);
	vertmap[i] = fe.vertindex;
    }


    for (unsigned f=0; f<mesh.faces.size(); f++) {

	if (vertmap[mesh.faces[f].verts[0]]!=-1 && 
	    vertmap[mesh.faces[f].verts[1]]!=-1 &&
	    vertmap[mesh.faces[f].verts[2]]!=-1) {

	    CreateTriangle(vertmap[mesh.faces[f].verts[0]],
			   vertmap[mesh.faces[f].verts[1]],
			   vertmap[mesh.faces[f].verts[2]]);

	}

    }


}




void Triangulator::ExtractFronts(vector< vector<Point3> > &fpts, vector< vector<Vector3> > &fnorms, vector< vector<int> > &states, vector< vector<real_type> > &fenceheight) {

    fpts.resize(fronts.size());
    fnorms.resize(fronts.size());
    states.resize(fronts.size());
    fenceheight.resize(fronts.size());

    real_type fencescale = controller.GetFenceScale();

    for (unsigned fi=0; fi<fronts.size(); fi++) {
	fpts[fi].resize(0);
	fnorms[fi].resize(0);
	states[fi].resize(0);
	fenceheight[fi].resize(0);

	if (!fronts[fi]->empty()) {
	    feli f = fronts[fi]->FirstElement();
	    do {

		fpts[fi].push_back(f->position);
		fnorms[fi].push_back(f->normal);
		states[fi].push_back(f->priority.first);
		fenceheight[fi].push_back(fencescale * f->max_step);

		f = Front::NextElement(f);
	    } while (f != fronts[fi]->FirstElement());
	}

    }

}










// functions for the heap
bool operator<(const Triangulator::feli &l, const Triangulator::feli &r) {
    // reverse the comparison since the heap wants high priority at the top
    return (l->priority > r->priority);
}

void heap_position(Triangulator::feli &i, int p) {
    i->heap_position = p;
}



Triangulator::Triangulator(TriangulatorController &c)
    : controller(c), numVertsAdded(0), numFacesAdded(0), flipOutput(false), work_quit(false) {

}

void Triangulator::StartWorkerThreads() {
    for (int i=0; i<TRIANGULATOR_NUM_PROJECTORS; i++) {
	work_threads.push_back(new thlib::Thread(ProjectorThreadMain, this, 0));
    }
}

void Triangulator::StopWorkerThreads() {
    work_quit=true;
    for (unsigned i=0; i<work_threads.size(); i++) {
	int *ret;
	work_threads[i]->join((void**)&ret);
	delete work_threads[i];
    }
}


Triangulator::~Triangulator() {
}


// functions needed by the kd tree
Triangulator::Box3 Triangulator::bounding_box(const feli &i) const {
    Box3 box(i->position, i->position);
    box.update(Front::NextElement(i)->position);
    return box;
}

real_type Triangulator::distance(const Front::feli &i, const Point3 &from) const {

    Point3 p1 = i->position;
    Point3 p2 = Front::NextElement(i)->position;

    Vector3 dir = p2-p1;
    real_type len = dir.length();
    dir *= 1/len;

    Vector3 fdir = from - p1;

    real_type along = dir.dot(fdir);
    if (along<=0)
	return Point3::distance(from, p1);
    if (along>=len)
	return Point3::distance(from, p2);

    return Point3::distance(from, p1 + along*dir);
    
}




