#include "common.h"
#include <stdlib.h>
#include <ThreadLib/threadslib.h>
#include <gtb/ui/commandline.h>
#include <viewer/viewer.h>
#include "uheap.h"
#include "front.h"
#include "guidance.h"
#include "triangulator.h"
#include "triangulate_mesh.h"
#include "triangulate_mls.h"
#include "triangulate_iso.h"
#include "triangulate_tet.h"
#include "triangulate_csg.h"

#include "output_controller_obj.h"
#include "output_controller_reeb.h"
#include "output_controller_hhm.h"
#include "output_controller_gui.h"
#include "edgeflipper.h"
#include "crease.h"

#include "FLF_io.h"
#include "PC_io.h"

#include "parallel.h"


using namespace std;

#ifdef __x86_64__
// this used to be 4, but it appears to be about 10-15% faster with 8
// would we be better off with 2 theads on a uniprocessor?
int idealNumThreads = 8;
#else
int idealNumThreads = 1;
#endif


// Reeb graph stuff
OutputControllerReeb *reeb = NULL;
SmoothMLSGuidanceField::VectorField vector_field = NULL;
float reeb_epsilon = 0.1;


OutputControllerGui *gui=NULL;
static char command_line[2048];


// can be modified by the worker thread, but only inside of the draw mutex.  Draw thread will never modify it
static TriangleMesh meshes[2];
static RegularVolume volume;
static surfel_set points;
static TetMesh tetmesh;
static TriangleMesh tetshell;

// const versions for the drawing thread so it can't accidently screw it up
const TriangleMesh *cmeshes[2] = { &meshes[0], &meshes[1] };
const RegularVolume *cvolume = &volume;
const surfel_set *cpoints = &points;
const TetMesh *ctetmesh = &tetmesh;
const TriangleMesh *ctetshell = &tetshell;

static char *save_field = NULL;
static char *load_field = NULL;
char *outname = "outmesh.m";
static char *reebname = "reeb_out.txt";
static real_type rho = (real_type)(M_PI * 2.0 / 32.0); // ~ 0.2
static real_type sharp = -2.0; // if cos dihedralangle < sharp, treat it as crease
static real_type min_step = (real_type) 0;
static real_type max_step = (real_type) 1000000;
static real_type reduction = (real_type)0.8;
static real_type radius_factor = 2.0f;
static bool csg_guidance_blend = true;
static bool failsafe = true;	
static int small_crease = 20;
static int adamson = 0;
static int nielson = 0;
static bool draw_messages = true;
real_type fence_scale = 1.0;
int curvature_sub = 4;
int eval_sub = 3;
bool trim_guidance = true;
real_type proj_tol = 1e-3;
int trim_bin_size = 1000000;
int grid_intersect_overestimate = 0;
real_type bad_connect_priority = 1e34;
real_type boundary_dist = 0;
real_type noise_threshold = 0;
bool rmls = true;
int rmls_knn = 120;

real_type saliency = 0;

GuidanceField *guidance = NULL;
static ControllerWrapper *controller = NULL;
static Triangulator *triangulator = NULL;

thlib::CSObject *critical_section;

OutputController *output_controller_head = NULL;

bool allow_outside=false;

Vector3 constant_x(const Point3 &)
{
    return Vector3(1,0,0);
}

Vector3 constant_y(const Point3 &)
{
    return Vector3(0,1,0);
}

Vector3 constant_z(const Point3 &)
{
    return Vector3(0,0,1);
}

Vector3 radial_x(const Point3 &p)
{
    Vector3 result = p - Point3::ZERO;
    result[0] = 0.0;
    if (result.length() == 0.0)
	return Vector3(0,1,0);
    result.normalize();
    return result;
}

void redrawAll(){
    if (gui) {
	gui->Extract(triangulator);
	gui->Redraw();
    }
}

void redrawAndWait(int key, bool force){
    if(!gui) return;
    redrawAll();

    if (force || gui->GetWaitForUser()) {

	char message[1024];
	sprintf(message, "waiting for key <%c>\n", key);
	if (draw_messages)
	    gui->SetMessage(0, message);
	redrawAll();
	gui->WaitForKey(key);
	gui->SetMessage(0, NULL);
    }
}

void waitForKey(int key) {
    if(!gui) return;
    if (gui->GetWaitForUser())
	gui->WaitForKey(key);
}


int GetUILastKey() {
    if (!gui) return 0;
    return gui->LastKey();
}

bool getUISelectPoint(Point3 &p, int &win) {

    if (!gui) return false;

    int button;
    gui->LastClick(p[0], p[1], p[2], win, button);
    return (win>=0 && button==GLUT_RIGHT_BUTTON);
}




void PerpVectors(const Vector3 &n, Vector3 &udir, Vector3 &vdir) {

    int m = (fabs(n[0])<fabs(n[1])) ? 0 : 1;
    if (fabs(n[2])<fabs(n[m])) m=2;

    udir=Vector3(0,0,0);
    udir[m] = 1;

    vdir = udir.cross(n); vdir.normalize();
    udir = vdir.cross(n); udir.normalize();
}


bool endswith(const char *s, const char *end) {
    int slen = strlen(s);
    int endlen = strlen(end);
    if (slen<endlen) return false;
    return (!stricmp(&s[slen-endlen], end));
}


/////////////////////////////////////////////////////////////////////////////////

void do_gui() {

    if (gui) {
	cerr<<"gui already started!"<<endl;
	return;
    }

    gui = new OutputControllerGui(command_line);
    gui->Start();

    critical_section = &gui->GetCriticalSection();

    redrawAll();
    gui->ViewAll();
}


void do_quit(){
    exit(0);
}

//////////////////////////////////////////////
// the main functions that get things going

void extract_boundary_loops(const TriangleMesh &m, vector< vector<int> > &loops) {
    loops.resize(0);

    vector<bool> needadd(m.verts.size());
    for (unsigned i=0; i<needadd.size(); i++)
	needadd[i] = false;

    for (unsigned v=0; v<m.verts.size(); v++) {
	TriangleMesh::VertexVertexIteratorI vvi(m, v);
	needadd[v] = vvi.isonedge();
    }


    while (1) {

	int start = -1;
	for (unsigned i=0; i<needadd.size(); i++) {
	    if (needadd[i]) {
		start = i;
		break;
	    }
	}

	if (start == -1) break;


	vector<int> tloop;
	tloop.push_back(start);
	needadd[start] = false;

	while (1) {
	    TriangleMesh::VertexVertexIteratorI vvi(m, tloop.back());
	    int next = *vvi;
	    if (next == start) break;
	    needadd[next] = false;
	    tloop.push_back(next);
	}

	loops.push_back(tloop);
    }

	
}

void compute_pointset_normals(surfel_set &ss, CProjection &proj) {

    ss.normals().resize(ss.size());

    for (unsigned i=0; i<ss.size(); i++) {
	Point3 r1;
	Vector3 n;
	proj.PowellProject(ss.vertex(i), r1, n);
	ss.normal(i) = n;
    }

    // orient the normals
    vector<bool> set(ss.size());
    for (unsigned i=0; i<ss.size(); i++) set[i]=false;

    gtb::fast_pq< std::pair<real_type, std::pair<int,int> > > pq;

    pq.push(std::pair<real_type, std::pair<int,int> > (0, std::pair<int,int>(-1,0) ));

    while (!pq.empty()) {

	std::pair<real_type, std::pair<int,int> > top = pq.top();
	pq.pop();

	if (!set[top.second.second]) {
	    set[top.second.second] = true;

	    if (top.second.first >= 0) {
		if (ss.normal(top.second.first).dot(ss.normal(top.second.second)) < 0)
		    ss.normal(top.second.second).flip();
	    }

	    surfelset_view nbhd(ss);
	    proj.extract(ss.vertex(top.second.second), ((real_type)1)*proj.point_radius(ss.vertex(top.second.second)), nbhd);

	    for (unsigned i=0; i<nbhd.size(); i++) {
		if (set[nbhd.get_index(i)]) continue;

		pq.push(std::pair<real_type, std::pair<int,int> > (top.first - Point3::distance(ss.vertex(top.second.second),nbhd.vertex(i)), std::pair<int,int>(top.second.second,nbhd.get_index(i)) ));
	    }

	}

    }
}

int do_reeb_name(int argc, char *argv[])
{
    reebname = argv[1];
    return 2;
}

int do_reeb_eps(int argc, char *argv[])
{
    reeb_epsilon = atof(argv[1]);
    return 2;
}

int do_reeb(int argc, char *argv[])
{
    char fun = argv[1][0];
    OutputControllerReeb::Function f;
    switch (toupper(fun)) {
    case 'X': f = OutputControllerReeb::FunctionX; vector_field = constant_x; break;
    case 'Y': f = OutputControllerReeb::FunctionY; vector_field = constant_y; break;
    case 'Z': f = OutputControllerReeb::FunctionZ; vector_field = constant_z; break;
    case 'R': f = OutputControllerReeb::RadialX;   vector_field = radial_x; break;
    default: {
	cerr << "function \'" << argv[1] << "\' not recognized. " << endl;
	exit(1);
    }
    }

    OutputController::AddControllerToBack(output_controller_head, new OutputControllerReeb(f, reebname));
    return 2;
}


int do_tri_mesh(int argc, char* argv[]) {
	
    int ret=1;
    int csubdiv=0;
    //	char *command = argv[1];
    if (argc>1 && argv[1][0]!='-') {
	csubdiv = atoi(argv[1]);
	ret++;
    }



    if (triangulator)	delete triangulator;	triangulator=NULL;
    if (guidance)		delete guidance;		guidance=NULL;
    if (controller)		delete controller;		controller=NULL;

    // Use meshes[0] directly instead of const pointer to avoid any aliasing issues
    TriangleMesh &mesh = meshes[0];
    MeshProjector projector(mesh);
    guidance = new MeshGuidanceField(csubdiv, mesh, rho, min_step, max_step, reduction);
    OutputController::AddControllerToBack(output_controller_head, new OutputControllerHHM(outname));
    if (gui)
	OutputController::AddControllerToBack(output_controller_head, gui);
    controller = new ControllerWrapper(guidance, &projector, output_controller_head);


    vector< vector<int> > boundaries;
    vector< Point3 > crease_points;
    vector< Vector3 > crease_onormals;
    vector< vector<int> > crease_indices;
    vector< vector<Vector3> > crease_normals;
    vector< int > corner_triangles;

#if 1  // this kills pensatore for some reason

    if (boundary_dist != 0) {
	projector.FindSoupBoundaries(boundaries, boundary_dist);
    } else {
	extract_boundary_loops(mesh, boundaries);
    }

    CreaseExtractor extractor(mesh, (MeshGuidanceField *)guidance,
			      &projector,
			      crease_points, crease_onormals,
			      crease_indices, crease_normals,
			      corner_triangles,
			      sharp,
			      small_crease);
    cerr << crease_indices.size() << endl;
    extractor.Extract();
    cerr << crease_indices.size() << endl;
    //	creases = extractor.Extract();
#endif	
    vector< vector<Point3> > ipts;
    vector< vector<Vector3> > inorms;


    if (boundaries.size()) {
	cerr << (boundaries.size()) << " boundaries." << endl;
	for (unsigned b=0; b<boundaries.size(); b++) {
	    cerr << "Resampling boundary " << b << endl;
	    vector<Point3> ploop;
	    vector< vector<Vector3> > nloop(1);
	    for (unsigned i=0; i<boundaries[b].size(); i++) {
		ploop.push_back(mesh.verts[boundaries[b][i]].point);
		nloop[0].push_back(mesh.verts[boundaries[b][i]].normal);
	    }

	    vector<Point3> plooprs;
	    vector< vector<Vector3> > nlooprs(1);
	    guidance->ResampleCurve(ploop, nloop, plooprs, nlooprs);

	    ipts.push_back(plooprs);
	    inorms.push_back(nlooprs[0]);
	}
    } else if (!crease_indices.size()) {
	unsigned startvert = (unsigned) 0;

	// let the user select the starting location
	if (0 && gui) {
	    int win; Point3 p;
	    while (!getUISelectPoint(p, win)) { }
	    for (unsigned i=1; i<mesh.verts.size(); i++) {
		if (Point3::distance(mesh.verts[i].point, p) < Point3::distance(mesh.verts[startvert].point, p))
		    startvert = i;
	    }
	}

	cerr<<"starting at point "<<startvert<<endl;


	real_type size = guidance->MaxStepLength(mesh.verts[startvert].point);
	Vector3 udir, vdir;
	PerpVectors(mesh.verts[startvert].normal, udir, vdir);

	ipts.resize(1);
	inorms.resize(1);
	ipts[0].resize(2); inorms[0].resize(2);

	ipts[0][0]	 = mesh.verts[startvert].point;
	inorms[0][0] = mesh.verts[startvert].normal;

	projector.ProjectPoint(ipts[0][0] + size * udir, ipts[0][1], inorms[0][1]);
    }


    triangulator = new Triangulator(*controller);
    if (crease_indices.size()) {
	
	triangulator->Go(ipts, inorms, failsafe,
			 crease_points, crease_onormals,
			 crease_indices, crease_normals,
			 corner_triangles);
    } else
	triangulator->Go(ipts, inorms, failsafe);

    return ret;
}


int do_csg(int argc, char* argv[]) {


    int ret=2;
    int csubdiv=0;
    assert(argc>1 && argv[1][0]!='-');
    char *command = argv[1];
    if (argc>2 && argv[2][0]!='-') {
	csubdiv = atoi(argv[2]);
	ret++;
    }


    if (triangulator)	delete triangulator;	triangulator=NULL;
    if (guidance)		delete guidance;		guidance=NULL;
    if (controller)		delete controller;		controller=NULL;


    // set the direction of the loops the way we need them
    bool reverse0, reverse1;
    if (stricmp(command, "union")==0) {
	reverse0 = false;
	reverse1 = false;
    } else if (stricmp(command, "int")==0) {
	reverse0 = true;
	reverse1 = true;
    } else if (stricmp(command, "sub")==0) {
	reverse0 = false;
	reverse1 = true;
    } else {
	cerr<<"unknown csg operation: "<<command<<endl;
	return 4;
    }


    vector< vector<Point3> > points0, points1;
    vector< vector<Vector3> > normals0, normals1;
    vector<int> pointsides[2];
    for (int m=0; m<2; m++) {
	pointsides[m].resize(cmeshes[m]->verts.size());
	for (unsigned i=0; i<pointsides[m].size(); i++)
	    pointsides[m][i] = 0;	// unknown
    }


    if (cmeshes[0]->num_vertices() && cmeshes[1]->num_vertices()) {

	cerr<<"Doing mesh/mesh csg"<<endl;

	cerr<<"getting intersection loops...";
	MeshCSGGuidanceField::GetIntersectionLoops(*cmeshes[0], *cmeshes[1], points0, normals0, normals1, pointsides);
	cerr<<"OK"<<endl;

	if (points0.size() == 0) {
	    cerr<<"surfaces don't intersect!"<<endl;
	    return 4;
	}

	// figure out which points of the models we need to keep
	cerr<<"flooding point sides...";
	MeshCSGGuidanceField::FloodPointSides(*cmeshes[0], pointsides[0]);
	MeshCSGGuidanceField::FloodPointSides(*cmeshes[1], pointsides[1]);
	cerr<<"OK"<<endl;

	if (!reverse0) {
	    for (unsigned i=0; i<pointsides[0].size(); i++)
		pointsides[0][i] = -pointsides[0][i];
	}
	if (reverse1) {
	    for (unsigned i=0; i<pointsides[1].size(); i++)
		pointsides[1][i] = -pointsides[1][i];
	}


	guidance = new MeshCSGGuidanceField(csubdiv, *cmeshes[0], *cmeshes[1], pointsides, points0, rho, min_step, max_step, reduction);

	cerr<<"trimming point sides...";
	((MeshCSGGuidanceField*)guidance)->TrimPointSides(*cmeshes[0], pointsides[0]);
	((MeshCSGGuidanceField*)guidance)->TrimPointSides(*cmeshes[1], pointsides[1]);
	cerr<<"OK"<<endl;


	cerr<<"blending curvature...";
	((MeshCSGGuidanceField*)guidance)->BlendGuidance(csg_guidance_blend, cmeshes, pointsides);
	cerr<<"OK"<<endl;


	if (0) {
	    dbgClear();
	    for (int m=0; m<2; m++) {
		for (unsigned i=0; i<cmeshes[m]->verts.size(); i++) {
		    if (pointsides[m][i] == -1) {
			DbgPoints::add(cmeshes[m]->verts[i].point, 0.5, 0, 0);
		    } else if (pointsides[m][i] == -2) {
			DbgPoints::add(cmeshes[m]->verts[i].point, 1, 0, 0);
		    } else if (pointsides[m][i] == 1) {
			DbgPoints::add(cmeshes[m]->verts[i].point, 0, 0.5, 0);
		    } else if (pointsides[m][i] == 2) {
			DbgPoints::add(cmeshes[m]->verts[i].point, 0, 1, 0);
		    }
		}
	    }
	    redrawAndWait(' ');
	}



	// resample the intersection curves
	cerr<<"resampling intersection curves...";
	for (unsigned l=0; l<points0.size(); l++) {
	    vector<Point3> op;
	    vector< vector<Vector3> > inorms, onorms;
	    inorms.push_back(normals0[l]);
	    inorms.push_back(normals1[l]);
	    guidance->ResampleCurve(points0[l], inorms, op, onorms);

	    points0[l] = op;
	    normals0[l] = onorms[0];
	    normals1[l] = onorms[1];
	}
	cerr<<"OK"<<endl;

	points1 = points0;
	for (unsigned l=0; l<points0.size(); l++) {
	    reverse(points1[l]);
	    reverse(normals1[l]);
	}

	cerr<<"computing initial fronts...";
	MeshCSGGuidanceField::FrontsFromSides(*cmeshes[0], pointsides[0], points0, normals0);
	MeshCSGGuidanceField::FrontsFromSides(*cmeshes[1], pointsides[1], points1, normals1);
	cerr<<"OK"<<endl;


	// set the direction of the loops the way we need them
	for (unsigned l=0; l<points0.size(); l++) {
	    if (reverse0) {
		reverse(points0[l]);
		reverse(normals0[l]);
	    }
	}

	for (unsigned l=0; l<points1.size(); l++) {
	    if (!reverse1) {
		reverse(points1[l]);
		reverse(normals1[l]);
	    }
	}

	if (gui)
	    OutputController::AddControllerToBack(output_controller_head, gui);

	OutputControllerHHM *hhmout = new OutputControllerHHM(outname);
	OutputController::AddControllerToBack(output_controller_head, hhmout);
	controller = new ControllerWrapper(guidance, NULL, output_controller_head);
	triangulator = new Triangulator(*controller);


	MeshProjector projector1(*cmeshes[0], &pointsides[0]);
	controller->SetProjector(&projector1);
	triangulator->SetFlipOutput(reverse0);
	hhmout->SetVertexKey("{matid=0}");
	triangulator->InsertSubMesh(*cmeshes[0], pointsides[0], 1);
	hhmout->SetVertexKey("{matid=1}");
	triangulator->Go(points0, normals0, failsafe);

	MeshProjector projector2(*cmeshes[1], &pointsides[1]);
	controller->SetProjector(&projector2);
	triangulator->SetFlipOutput(reverse1);
	hhmout->SetVertexKey("{matid=0}");
	triangulator->InsertSubMesh(*cmeshes[1], pointsides[1], 1);
	hhmout->SetVertexKey("{matid=1}");
	triangulator->Go(points1, normals1, failsafe);

    } else if (cmeshes[0]->num_vertices() && points.size()) {
	cerr<<"Doing mesh/pointset csg"<<endl;


	MeshProjector mp(*cmeshes[0], NULL);
	SmoothMLSProjector psp(points, adamson);

	compute_pointset_normals(points, psp._projector);

	cerr<<"flip pointset normal orientation? [y/n]"<<endl;
	cerr<<"defaulting n"<<endl;	//for (unsigned i=0; i<points.size(); i++) { points.normal(i).flip(); }
#if 0
	while (1) {
	    int key = GetUILastKey();
	    if (key=='y' || key=='Y') {
		for (unsigned i=0; i<points.size(); i++)
		    points.normal(i).flip();
	    }
	    break;
	} else if (key=='n' || key=='N') {
	    break;
	}
#endif



	cerr<<"computing intersection curves"<<endl;
	vector< vector<Point3> > points0, points1;
	vector< vector<Vector3> > normals0, normals1;
	MeshPSCSGGuidanceField::GetIntersectionLoops(mp, psp, points0, normals0, normals1);
	cerr<<"OK"<<endl;


	guidance = new MeshPSCSGGuidanceField(csubdiv, *cmeshes[0], points, psp._projector, points0, rho, min_step, max_step, reduction, adamson);

	// computing mesh pointsides
	vector<int> pointsides;
	((MeshPSCSGGuidanceField*)guidance)->HackPointSides(*cmeshes[0], pointsides);
	MeshCSGGuidanceField::FixPointSides(*cmeshes[0], pointsides);



	// resample the intersection curves
	cerr<<"resampling intersection curves...";
	for (unsigned l=0; l<points0.size(); l++) {
	    vector<Point3> op;
	    vector< vector<Vector3> > inorms, onorms;
	    inorms.push_back(normals0[l]);
	    inorms.push_back(normals1[l]);
	    guidance->ResampleCurve(points0[l], inorms, op, onorms);

	    points0[l] = op;
	    normals0[l] = onorms[0];
	    normals1[l] = onorms[1];
	}
	cerr<<"OK"<<endl;

	points1 = points0;
	for (unsigned l=0; l<points0.size(); l++) {
	    reverse(points0[l]);
	    reverse(normals0[l]);
	}

	cerr<<"computing initial fronts...";
	MeshCSGGuidanceField::FrontsFromSides(*cmeshes[0], pointsides, points0, normals0);
	cerr<<"OK"<<endl;



	// set the direction of the loops the way we need them
	for (unsigned l=0; l<points0.size(); l++) {
	    if (reverse0) {
		reverse(points0[l]);
		reverse(normals0[l]);
	    }
	}

	for (unsigned l=0; l<points1.size(); l++) {
	    if (reverse1) {
		reverse(points1[l]);
		reverse(normals1[l]);
	    }
	}


	if (gui)
	    OutputController::AddControllerToBack(output_controller_head, gui);
	OutputControllerHHM *hhmout = new OutputControllerHHM(outname);
	OutputController::AddControllerToBack(output_controller_head, hhmout);
	controller = new ControllerWrapper(guidance, NULL, output_controller_head);
	triangulator = new Triangulator(*controller);


	controller->SetProjector(&mp);
	triangulator->SetFlipOutput(reverse0);
	hhmout->SetVertexKey("{matid=0}");
	triangulator->InsertSubMesh(*cmeshes[0], pointsides, 1);
	hhmout->SetVertexKey("{matid=1}");
	triangulator->Go(points0, normals0, failsafe);

	controller->SetProjector(&psp);
	triangulator->SetFlipOutput(reverse1);
	hhmout->SetVertexKey("{matid=2}");
	//		triangulator->InsertSubMesh(*cmeshes[1], pointsides[1], 1);
	triangulator->Go(points1, normals1, failsafe);

    }

    return ret;
}


int do_tri_vol(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    real_type isoval = atof(argv[1]);
    bool bspline=false;
    if (argc>2 && !stricmp(argv[2], "bspline"))
	bspline=true;

    if (triangulator)	delete triangulator;	triangulator=NULL;
    if (guidance)		delete guidance;		guidance=NULL;
    if (controller)		delete controller;		controller=NULL;



    // get the marching cubes mesh
    // wrap it in a local scope so it gets destructed before doing the triangulation
    // so that huge volumes (512^3) don't destroy our computers
    cerr<<"running marching cubes"<<endl;
    vector< vector<Point3> > cc_seeds;
    vector< vector<int> > boundaries;
    vector< vector<Vector3> > nloops;
    vector< vector<Point3> > ploops;
    vector< vector<Point3> > ipts;
    vector< vector<Vector3> > inorms;
    IsoSurfaceProjector projector(*cvolume, isoval, bspline);
	    
    {
	TriangleMesh mc_mesh;
	{
	    bool oldflips = OutputControllerEdgeFlipper::DoFlips;
	    OutputControllerEdgeFlipper::DoFlips = false;

	    real_type *orig_values = volume.CopyValues();
		
	    if (bspline) {
		real_type *bvals = cvolume->GetBSplineValues();
		critical_section->enter();
		volume.SetValues(bvals);
		critical_section->leave();
		delete [] bvals;
	    }


	    OutputControllerITS *oc_its = new OutputControllerITS();
	    controller = new ControllerWrapper(NULL, NULL, oc_its);	// to catch the mc output
	    MarchingCubes(*cvolume, *controller, isoval);

	    ITS2TM(oc_its->triangulation, mc_mesh);
	    delete oc_its;  oc_its=NULL;
	    delete controller; controller = NULL;

	    volume.SetValues(orig_values);
	    delete [] orig_values;

	    OutputControllerEdgeFlipper::DoFlips = oldflips;

	}


	critical_section->enter();
	meshes[0] = mc_mesh;
	meshes[0].BuildStrips();
	critical_section->leave();

	redrawAndWait(' ');


	cerr<<"adding boundaries as initial fronts"<<endl;
	mc_mesh.GetBoundaries(boundaries);


	IndexedTriangleSet tribox;
	Box3 bbox = cvolume->bounding_box();
	tribox.add_vertex(Point3(bbox.x_min(), bbox.y_min(), bbox.z_min()));
	tribox.add_vertex(Point3(bbox.x_max(), bbox.y_min(), bbox.z_min()));
	tribox.add_vertex(Point3(bbox.x_max(), bbox.y_max(), bbox.z_min()));
	tribox.add_vertex(Point3(bbox.x_min(), bbox.y_max(), bbox.z_min()));
	tribox.add_vertex(Point3(bbox.x_min(), bbox.y_min(), bbox.z_max()));
	tribox.add_vertex(Point3(bbox.x_max(), bbox.y_min(), bbox.z_max()));
	tribox.add_vertex(Point3(bbox.x_max(), bbox.y_max(), bbox.z_max()));
	tribox.add_vertex(Point3(bbox.x_min(), bbox.y_max(), bbox.z_max()));
	tribox.insert_triangle(0,1,2,false,0);
	tribox.insert_triangle(0,2,3,false,0);
	tribox.insert_triangle(4,6,5,false,0);
	tribox.insert_triangle(4,7,6,false,0);
	tribox.insert_triangle(0,4,5,false,0);
	tribox.insert_triangle(0,5,1,false,0);
	tribox.insert_triangle(3,2,6,false,0);
	tribox.insert_triangle(3,6,7,false,0);
	tribox.insert_triangle(1,5,6,false,0);
	tribox.insert_triangle(1,6,2,false,0);
	tribox.insert_triangle(0,3,7,false,0);
	tribox.insert_triangle(0,7,4,false,0);

	TriangleMesh boxmesh;
	ITS2TM(tribox, boxmesh);
	for (unsigned i=0; i<boxmesh.faces.size(); i++) {
	    for (int j=0; j<3; j++) {
		boxmesh.faces[i].nbrs[j] = -1;
	    }
	}

	MeshProjector box_mp(boxmesh);




	guidance = new IsoSurfaceGuidanceField(projector, *cvolume, bspline, rho, min_step, max_step, reduction);
	OutputController::AddControllerToBack(output_controller_head, new OutputControllerHHM(outname));
	if (gui)
	    OutputController::AddControllerToBack(output_controller_head, gui);
	controller = new ControllerWrapper(guidance, &projector, output_controller_head);

	redrawAndWait(' ');




	ploops.resize(boundaries.size());
	nloops.resize(boundaries.size());

	for (unsigned i=0 ;i<boundaries.size(); i++) {
	    reverse(boundaries[i]);
	    for (unsigned j=0; j<boundaries[i].size(); j++) {
		ploops[i].push_back(mc_mesh.verts[boundaries[i][j]].point);
	    }
	}

	for (unsigned l=0; l<boundaries.size(); l++) {
	  
	    nloops[l].resize(ploops[l].size());


	    // project the points onto the isosurface / get the normals
	    for (unsigned p=0; p<ploops[l].size(); p++) {
		DbgPoints::add(ploops[l][p], 1,0,0);
		((IsoSurfaceGuidanceField*)guidance)->NormalAtPoint(ploops[l][p], nloops[l][p]);
		DbgPoints::add(ploops[l][p], 0,1,0);
	    
		vector<Point3> line(2);
		line[0] = ploops[l][p];
		line[1] = line[0] + (real_type)0.1 * nloops[l][p];
		DbgPLines::add(line, 0);
	    
	    }
	    redrawAndWait(' ');


	    // resample
	    vector< vector<Vector3> > nloop2(1);	nloop2[0] = nloops[l];
	    vector<Point3> plooprs;
	    vector< vector<Vector3> > nlooprs(1);
	    guidance->ResampleCurve(ploops[l], nloop2, plooprs, nlooprs);


	    cerr<<"resampled loop "<<l<<endl;
	    dbgClear();
	    for (unsigned i=0; i<plooprs.size(); i++) {
		DbgPoints::add(plooprs[i], 1,0,0);
	    }
	    redrawAndWait(' ');


	    vector<Point3> oplooprs = plooprs;
	    for (unsigned p=0; p<plooprs.size(); p++) {
	    
		Vector3 meshnorm;
		Vector3 oldnorm = nlooprs[0][p];
		allow_outside=true;
		if (!ProjectToIntersection(box_mp, projector, plooprs[p], meshnorm, nlooprs[0][p], 1e-4)) {
		    nlooprs[0][p] = oldnorm;
		}
		allow_outside=false;
	    }
	  
	    if (plooprs.size() > 2) {
		cerr<<"adding loop "<<l<<endl;
	    
		dbgClear();
		for (unsigned i=0; i<plooprs.size(); i++) {
		    DbgPoints::add(plooprs[i], 1,0,0);
		}
		redrawAndWait(' ');
	    
		ipts.push_back(plooprs);
		inorms.push_back(nlooprs[0]);
	    }
	}



	// find the connected components to use as seeds 
	cerr<<"finding connected components"<<endl;
	if (1){
	    vector<bool> flooded(mc_mesh.verts.size());
	    for (unsigned i=0;i<mc_mesh.verts.size(); i++) {
		flooded[i]=false;
	    }
	    
	    
	    while (1) {
	    
		int startv=-1;
		for (unsigned i=0; i<flooded.size(); i++) {
		    if (!flooded[i]) {
			if (mc_mesh.verts[startv].someface < 0) {
			    flooded[i] = true;
			} else {
			    startv=i; break;
			}
		    }
		}
		if (startv<0) break;

		if (mc_mesh.verts[startv].someface < 0)
		    cerr<<"someface < 0"<<endl;


		vector<int> component_points;
		component_points.push_back(startv);

	    
		vector<int> toflood;
		toflood.push_back(startv);
		flooded[startv] = true;
		bool boundary = mc_mesh.VertOnBoundary(startv);
	    
		while (!toflood.empty()) {
		    int tf = toflood.back();
		    toflood.pop_back();
		    for (TriangleMesh::VertexVertexIteratorI vi(mc_mesh, tf); !vi.done(); ++vi) {
			int tf2 = *vi;
			if (flooded[tf2]) continue;
			flooded[tf2]=true;
			component_points.push_back(tf2);
			boundary |= mc_mesh.VertOnBoundary(tf2);
			toflood.push_back(tf2);
		    }
		}

		if (!boundary) {
		    // since seed point projections sometimes fail, we'll try up to 10 different seeds per connected component
		    cc_seeds.push_back(vector<Point3>());
		    for (int i=0; i<10; i++) {
			int j = i*component_points.size() / 10;
			cc_seeds.back().push_back(mc_mesh.verts[component_points[j]].point);
		    }
		} else {
		    cerr<<"skipped connected component with boundary"<<endl;
		}
	    }
	}

	cerr<<cc_seeds.size()<<" cc seeds"<<endl;
	redrawAndWait(' ');
    }


    if (cc_seeds.size()==0  && ipts.size()==0) {
	cerr<<"no seeds found!?!"<<endl;
	exit(0);
    }


    dbgClear();
    for (unsigned i=0; i<cc_seeds.size(); i++) {
	DbgPoints::add(cc_seeds[i].front(), 1, 0, 0);
    }
    redrawAndWait(' ');

    for (unsigned i=0; i<cc_seeds.size(); i++) {
	Point3 startpoint;
	Vector3 startnorm;

	bool success = false;

	for (int attempt=0; attempt<cc_seeds[i].size(); attempt++) {
	    if (projector.ProjectPoint(cc_seeds[i][attempt], startpoint, startnorm) != PROJECT_SUCCESS) {
		cerr<<"seed point projection failed"<<endl;
		continue;
	    }

	    if (!((IsoSurfaceGuidanceField*)guidance)->NormalAtPoint(startpoint, startnorm)) {
		cerr<<"couldn't get normal"<<endl;
		continue; //exit(0);
	    }
	    real_type size = guidance->MaxStepLength(startpoint);
	      
	    Vector3 udir, vdir;
	    PerpVectors(startnorm, udir, vdir);
	  
 
	    Point3 proj_p;
	    Vector3 proj_n;

	    if (projector.ProjectPoint(startpoint + size * udir, proj_p, proj_n) != PROJECT_SUCCESS) {
		cerr<<"initial edge point projection failed"<<endl;
		allow_outside = false;
		continue;
	    }

	    ipts.push_back(vector<Point3>());
	    inorms.push_back(vector<Vector3>());
	    ipts.back().resize(2);
	    inorms.back().resize(2);

	    ipts.back()[0]   = startpoint;
	    inorms.back()[0] = startnorm;
	    ipts.back()[1]   = proj_p;
	    inorms.back()[1] = proj_n;

	    success = true;
	    break;
	}

	if (!success)
	    cerr<<"couldn't get initial front on connected component!"<<endl;

    }
    redrawAndWait(' ');


    triangulator = new Triangulator(*controller);
    triangulator->SetFlipOutput(true);
    triangulator->Go(ipts, inorms, failsafe);


    if (bspline)
	return 3;
    else
	return 2;
}


int do_tri_tet(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    real_type isoval = atof(argv[1]);

    if (triangulator)	delete triangulator;	triangulator=NULL;
    if (guidance)		delete guidance;		guidance=NULL;
    if (controller)		delete controller;		controller=NULL;



    critical_section->enter();
    vector<real_type> scalars;
    ctetmesh->GetShell(tetshell, scalars);
    for (int i=0; i<0; i++) {
	tetshell.LoopSubdivide(true);
    }
    tetshell.BuildStrips();
    critical_section->leave();


    redrawAndWait(' ');

    TetMeshProjector *projector = (nielson!=0 ?
				   (TetMeshProjector*) new TetMeshProjectorNielson(*ctetmesh, isoval) :
				   (TetMeshProjector*) new TetMeshProjectorMLS(*ctetmesh, isoval));


    guidance = new TetMeshGuidanceField(*projector, rho, min_step, max_step, reduction);
    OutputController::AddControllerToBack(output_controller_head, new OutputControllerHHM(outname));
    if (gui)
	OutputController::AddControllerToBack(output_controller_head, gui);
    controller = new ControllerWrapper(guidance, projector, output_controller_head);


    allow_outside = true;


    vector< vector<Point3> > ipts;
    vector< vector<Vector3> > inorms;

    if (nielson == 0) {
	cerr<<"evaluating at shell points"<<endl;
	scalars.resize(ctetshell->verts.size());
	for (unsigned v=0; v<ctetshell->verts.size(); v++) {
	    projector->EvalAtPoint(ctetshell->verts[v].point, scalars[v], NULL);
	    //	    cerr<<scalars[v]<<endl;
	}
    }

    redrawAndWait(' ');

    cerr<<"doing marching triangles"<<endl;
    MeshProjector shell_projector(*ctetshell);
    vector< vector<Point3> > ploops;
    MarchingTris(*ctetshell, scalars, isoval, ploops);

    if (ploops.size()) {

#if 0

	// get the dense sampling
	vector< vector<Point3> > ploops_dense;
	vector< vector<Vector3> > nloops_dense;

	const real_type dense_step = 0.004;
	projector->WalkIntersectionCurve(shell_projector, ploops, dense_step, ploops_dense, nloops_dense);


	// resample them with the guidance field
	for (unsigned l=0; l<ploops_dense.size(); l++) {
	    vector<Point3> ploop_rs;
	    vector< vector<Vector3> > nloop_rs;

	    vector< vector<Vector3> > nloop2(1);    nloop2[0] = nloops_dense[l];
	    guidance->ResampleCurve(ploops_dense[l], nloop2, ploop_rs, nloop_rs);
	    
	    if (ploop_rs.size() > 2) {
		ipts.push_back(ploop_rs);
		inorms.push_back(nloop_rs[0]);
	    }
	}

	  

#else

	vector< vector<Vector3> > nloops(ploops.size());
	for (unsigned l=0; l<ploops.size(); l++) {
	    nloops[l].resize(ploops[l].size());


	    if (nielson != 0) {
		// move the points so we're not trying to evaluate on teh edges of the tet!
		vector<Point3> pl(ploops[l].size());
		for (unsigned p=0; p<ploops[l].size(); p++) {
		    pl[p] = Point3::inbetween(ploops[l][p], ploops[l][(p+1)%ploops[l].size()], 0.01);
		}
		ploops[l] = pl;
	    }


	    // project the points onto the isosurface / get the normals
	    for (unsigned p=0; p<ploops[l].size(); p++) {
		DbgPoints::add(ploops[l][p], 1,0,0);
		/*
		  if (projector.ProjectPoint(ploops[l][p], ploops[l][p], nloops[l][p]) != PROJECT_SUCCESS) {
		  cerr<<"couldn't project boundary point!"<<endl; return 2;
		  }
		*/

		projector->NormalAtPoint(ploops[l][p], nloops[l][p]);
		DbgPoints::add(ploops[l][p], 0,1,0);

		vector<Point3> line(2);
		line[0] = ploops[l][p];
		line[1] = line[0] + (real_type)0.1 * nloops[l][p];
		DbgPLines::add(line, 0);

	    }
	    redrawAndWait(' ');


	    // resample
	    vector< vector<Vector3> > nloop2(1);	nloop2[0] = nloops[l];
	    vector<Point3> plooprs;
	    vector< vector<Vector3> > nlooprs(1);
	    guidance->ResampleCurve(ploops[l], nloop2, plooprs, nlooprs);


	    cerr<<"resampled loop "<<l<<endl;
	    dbgClear();
	    for (unsigned i=0; i<plooprs.size(); i++) {
		DbgPoints::add(plooprs[i], 1,0,0);
	    }
	    redrawAndWait(' ');


	    vector<Point3> oplooprs = plooprs;
	    for (unsigned p=0; p<plooprs.size(); p++) {

		if (p>0) {
		    Vector3 lastmove = plooprs[p-1] - oplooprs[p-1];
		    //			    plooprs[p] += lastmove;
		}

		Vector3 meshnorm;
		Vector3 oldnorm = nlooprs[0][p];
		if (!ProjectToIntersection(shell_projector, *projector, plooprs[p], meshnorm, nlooprs[0][p], 1e-4)) {
		    nlooprs[0][p] = oldnorm;
		}
	    }

	    if (plooprs.size() > 2) {
		cerr<<"adding loop "<<l<<endl;

		dbgClear();
		for (unsigned i=0; i<plooprs.size(); i++) {
		    DbgPoints::add(plooprs[i], 1,0,0);
		}
		redrawAndWait(' ');

		ipts.push_back(plooprs);
		inorms.push_back(nlooprs[0]);
	    }
	}
#endif


    } else {


	int startvert = 0;
	if (1 && gui) {
	    int win; Point3 p;
	    while (!getUISelectPoint(p, win)) { }
	    for (unsigned i=1; i<guidance->NumPoints(); i++) {
		if (Point3::distance(guidance->PointLocation(i), p) <
		    Point3::distance(guidance->PointLocation(startvert), p))
		    startvert = i;
	    }
	}

	Point3 startpoint = guidance->PointLocation(startvert);
	Vector3 startnorm;
	if (!projector->NormalAtPoint(startpoint, startnorm)) {
	    cerr<<"couldn't get normal"<<endl;
	    exit(0);
	}
	real_type size = guidance->MaxStepLength(startpoint);

	Vector3 udir, vdir;
	PerpVectors(startnorm, udir, vdir);

	ipts.resize(1);
	inorms.resize(1);
	ipts[0].resize(2); inorms[0].resize(2);

	ipts[0][0]	 = startpoint;
	inorms[0][0] = startnorm;
	projector->ProjectPoint(ipts[0][0] + size * udir, ipts[0][1], inorms[0][1]);;
    }


    allow_outside = false;


    triangulator = new Triangulator(*controller);
    triangulator->SetFlipOutput(true);
    triangulator->Go(ipts, inorms, failsafe);

    delete projector;

    return 2;
}


int do_tet_smooth(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    int passes = atoi(argv[1]);

    critical_section->enter();
    tetmesh.SmoothScalars(passes);
    critical_section->leave();

    return 2;
}


int do_marchingcubes(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    real_type isoval = atof(argv[1]);
    bool bspline=false;
    if (argc>2 && !stricmp(argv[2], "bspline"))
	bspline=true;

  
    extern real_type disoval;
    disoval=isoval;
    if (triangulator)	delete triangulator;	triangulator=NULL;
    if (guidance)		delete guidance;		guidance=NULL;
    if (controller)		delete controller;		controller=NULL;

    if (bspline) {
	real_type *bvals = cvolume->GetBSplineValues();
	critical_section->enter();
	volume.SetValues(bvals);
	critical_section->leave();
	delete [] bvals;
    }

    bool oldflips = OutputControllerEdgeFlipper::DoFlips;
    OutputControllerEdgeFlipper::DoFlips = false;

    OutputController::AddControllerToBack(output_controller_head, new OutputControllerHHM(outname));
    if (gui)
	OutputController::AddControllerToBack(output_controller_head, gui);
    controller = new ControllerWrapper(NULL, NULL, output_controller_head);	// for the output
    MarchingCubes(*cvolume, *controller, isoval);

    OutputControllerEdgeFlipper::DoFlips = oldflips;

    if (bspline)
	return 3;
    else
	return 2;
}


int do_marchingtets(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    real_type isoval = atof(argv[1]);


    if (triangulator)	delete triangulator;	triangulator=NULL;
    if (guidance)		delete guidance;		guidance=NULL;
    if (controller)		delete controller;		controller=NULL;


    bool oldflips = OutputControllerEdgeFlipper::DoFlips;
    OutputControllerEdgeFlipper::DoFlips = false;

    OutputController::AddControllerToBack(output_controller_head, new OutputControllerHHM(outname));
    if (gui)
	OutputController::AddControllerToBack(output_controller_head, gui);
    controller = new ControllerWrapper(NULL, NULL, output_controller_head);	// for the output
    MarchingTets(*ctetmesh, *controller, isoval);

    OutputControllerEdgeFlipper::DoFlips = oldflips;

    return 2;
}


int do_tri_smoothmls(int argc, char* argv[]) {
    //    assert(argc>1 && argv[1][0]!='-');

    int ret=1;

    if (triangulator)	delete triangulator;	triangulator=NULL;
    if (guidance)		delete guidance;		guidance=NULL;
    if (controller)		delete controller;		controller=NULL;

    //    read_obj(argv[1], points);


    SmoothMLSProjector projector(points, adamson);
    projector.SetRadiusFactor(radius_factor);

    // compute the normals for the adamson projection
    if (adamson==1 && !points.has_normals()) {
	compute_pointset_normals(points, projector._projector);
    }

    if (load_field)
	guidance = new SmoothMLSGuidanceField(projector._projector, rho, min_step, max_step, reduction, adamson, load_field);
    else {
	guidance = new SmoothMLSGuidanceField(projector._projector, rho, min_step, max_step, reduction, adamson, vector_field, reeb_epsilon);
    }

    if (save_field) {
	guidance->SaveField(save_field);
	cerr << "Saved guidance field." << endl;
    }

    if (gui)
	OutputController::AddControllerToBack(output_controller_head, gui);
    OutputController::AddControllerToBack(output_controller_head, new OutputControllerHHM(outname));
    controller = new ControllerWrapper(guidance, &projector, output_controller_head);


    vector< vector<Point3> > ipts;
    vector< vector<Vector3> > inorms;


    if (argc >= 2 && endswith(argv[1], ".flf")) {
	ret++;

	// read Joel/Linh's feature file and use it as initial fronts
	vector<InfoPoint> pts;
	vector< vector<InfoIndex> > loops;
	FLF_read(argv[1], pts, loops);


	for (unsigned i=0; i<loops.size(); i++) {

	    int start_corner = 0;
	    for (unsigned j=0; j<loops[i].size(); j++) {
		if (pts[loops[i][j].index_].isCorner_) {
		    start_corner = j; break;
		}
	    }

	    ipts.push_back(vector<Point3>());
	    inorms.push_back(vector<Vector3>());


	    int first_corner = start_corner;

	    while (1) {

		vector<Point3> points;
		vector< vector<Vector3> > normals(1);
		vector<Point3> rpoints;
		vector< vector<Vector3> > rnormals(1);


		int end_corner = start_corner;
		while (1) {
		    points.push_back(Point3(pts[loops[i][end_corner].index_].x_,
					    pts[loops[i][end_corner].index_].y_,
					    pts[loops[i][end_corner].index_].z_));
		    normals[0].push_back(Vector3(loops[i][end_corner].x_,
						 loops[i][end_corner].y_,
						 loops[i][end_corner].z_));

		    end_corner = (end_corner+1) % loops[i].size();
		    
		    if (end_corner == start_corner) break;
		    if (pts[loops[i][end_corner].index_].isCorner_) break;
		}
		points.push_back(Point3(pts[loops[i][end_corner].index_].x_,
					pts[loops[i][end_corner].index_].y_,
					pts[loops[i][end_corner].index_].z_));
		normals[0].push_back(Vector3(loops[i][end_corner].x_,
					     loops[i][end_corner].y_,
					     loops[i][end_corner].z_));


		// see if we should reverse the ordering just for resampling
		bool doreverse = (loops[i][first_corner].index_ > loops[i][end_corner].index_);
		if (doreverse) {
		    reverse(points);
		    reverse(normals[0]);
		}

		guidance->ResampleCurve(points, normals, rpoints, rnormals, true);

		// check if we have to flip the normals back over
		if (rnormals[0][0].dot(normals[0][0]) < 0) {
		    cerr<<"flipping normals of loop "<<i<<endl;
		    for (unsigned j=0; j<rnormals[0].size(); j++) {
			rnormals[0][j] *= -1;
		    }
		}

		// reverse the order back
		if (doreverse) {
		    reverse(rpoints);
		    reverse(rnormals[0]);
		}

		for (unsigned j=0; j<rpoints.size()-1; j++) {
		    ipts.back().push_back(rpoints[j]);
		    inorms.back().push_back(rnormals[0][j]);
		}
		
		start_corner = end_corner;
		if (start_corner == first_corner) break;
	    }
	}

    } else {
	/*! Compute an initial front
	 * This is simply a random point + another adjacent point
	 * which forms an edge
	 */
	//	Point3 x0 = points.vertex((int)(gtb::nrran1f()*points.size()));
	Point3 x0 = points.vertex(0);

	Vector3 n0;
	projector.ProjectPoint(x0, x0, n0);
	real_type len = guidance->MaxStepLength(x0);
	Vector3 udir, vdir;
	PerpVectors(n0, udir, vdir);
	Point3 x1 = x0 + udir*len;
	Vector3 n1;
	projector.ProjectPoint(x1, x1, n1);
	if (n1.dot(n0) < 0.0f) n1.flip();

	ipts.resize(1);
	inorms.resize(1);
	ipts[0].resize(2); inorms[0].resize(2);
	ipts[0][0] = x0;
	ipts[0][1] = x1;
	inorms[0][0] = n0;
	inorms[0][1] = n1;
    }

    triangulator = new Triangulator(*controller);
    triangulator->Go(ipts, inorms, failsafe);
    if (reeb)
	reeb->Dump();

    return ret;
}



int do_tri(int argc, char* argv[]) {

    if (cmeshes[0]->verts.size())
	return do_tri_mesh(argc, argv);
    else if (!cvolume->Empty())
	return do_tri_vol(argc, argv);
    else if (points.size())
	return do_tri_smoothmls(argc, argv);
    else if (tetmesh.verts.size())
	return do_tri_tet(argc, argv);

    cerr<<"nothing loaded to triangulate!"<<endl;
    return 1;
}


int do_volsmooth(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    int width = atoi(argv[1]);

    critical_section->enter();
    volume.Smooth(width);
    critical_section->leave();

    return 2;
}


int do_subdiv(int argc, char* argv[]) {

    // since we need to modify the mesh
    critical_section->enter();

    assert(argc>1 && argv[1][0]!='-');
    int num = atoi(argv[1]);

    for (int i=0; i<num; i++) {
	meshes[0].LoopSubdivide();
	meshes[1].LoopSubdivide();
    }

    if (meshes[0].verts.size())
	meshes[0].BuildStrips(100);
    if (meshes[1].verts.size())
	meshes[1].BuildStrips(100);


    critical_section->leave();

    return 2;
}


void do_flip() {

    critical_section->enter();

    for (int f=0; f<(int)meshes[0].faces.size(); f++) {
	std::swap(meshes[0].faces[f].verts[1], meshes[0].faces[f].verts[2]);
	std::swap(meshes[0].faces[f].nbrs[1], meshes[0].faces[f].nbrs[2]);
	std::swap(meshes[0].faces[f].nbrs[2], meshes[0].faces[f].nbrs[0]);
	std::swap(meshes[0].faces[f].nbrs[0], meshes[0].faces[f].nbrs[1]);
    }

    for (int v=0; v<(int)meshes[0].verts.size(); v++) {
	meshes[0].verts[v].normal.flip();
    }

    meshes[0].BuildStrips(100);

    critical_section->leave();
}


// interactively let the user flip the orientations of the connected components
void do_flip_components() { 

    if (!gui) {
	cerr<<"gui needed for interactive component flipping!"<<endl;
	return;
    }

    vector<int> done(meshes[0].faces.size(), 0);

    while (1) {
	vector<int> toprocess;

	for (int i=0; i<meshes[0].faces.size(); i++) {
	    if (!done[i]) {
		toprocess.push_back(i);
		break;
	    }
	}

	if (toprocess.empty())
	    break;

	vector<int> component;

	while (!toprocess.empty()) {

	    int next = toprocess.back();
	    toprocess.resize(toprocess.size()-1);

	    if (done[next])
		continue;

	    done[next] = true;
	    component.push_back(next);

	    for (int i=0; i<3; i++) {
		if (meshes[0].faces[next].nbrs[i] >= 0 &&
		    !done[meshes[0].faces[next].nbrs[i]])
		    toprocess.push_back(meshes[0].faces[next].nbrs[i]);
	    }

	}


	dbgClear();
	for (int i=0; i<component.size(); i++) {
	    DbgPoly::add(meshes[0].verts[meshes[0].faces[component[i]].verts[0]].point, Point3(1,0,0),
			 meshes[0].verts[meshes[0].faces[component[i]].verts[1]].point, Point3(1,0,0),
			 meshes[0].verts[meshes[0].faces[component[i]].verts[2]].point, Point3(1,0,0));

	}
	gui->Redraw();


	bool stop = false;
	while (!stop) {
	    gui->WaitForKey();
	    int key = gui->LastKey();

	    switch (key) {
	    case 'f':
	    case 'F':
		{

		    std::set<int> verts;
		    for (int i=0; i<component.size(); i++) {
			for (int j=0; j<3; j++) {
			    verts.insert(meshes[0].faces[component[i]].verts[j]);
			}
		    }

		    critical_section->enter();
		    for (int i=0; i<component.size(); i++) {
			int f = component[i];
			std::swap(meshes[0].faces[f].verts[1], meshes[0].faces[f].verts[2]);
			std::swap(meshes[0].faces[f].nbrs[1], meshes[0].faces[f].nbrs[2]);
			std::swap(meshes[0].faces[f].nbrs[2], meshes[0].faces[f].nbrs[0]);
			std::swap(meshes[0].faces[f].nbrs[0], meshes[0].faces[f].nbrs[1]);
		    }
		    
		    
		    for (std::set<int>::iterator v=verts.begin(); v!=verts.end(); ++v) {
			meshes[0].verts[*v].normal.flip();
		    }
		    
		    meshes[0].BuildStrips(100);
		    critical_section->leave();
		    gui->Redraw();
		}
		break;

	    case ' ':
		stop=true;
		break;
	    }

	}



    }


}



void do_trans() {

    if (!gui) {
	cerr<<"gui must be used to transform meshes!"<<endl;
	exit(-1);
    }

    gui->MeshEditMode(meshes);
}


int do_save_pts(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    // gcc 4.1 can't figure out where this function is for some reason
    //	write_obj(argv[1], points);
    cerr<<"CAN'T DO SAVE_PTS!"<<endl;
    return 2;
}


int do_save_mesh0(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    cmeshes[0]->Write(argv[1]);
    return 2;
}

int do_save_mesh1(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    cmeshes[1]->Write(argv[1]);
    return 2;
}

int do_save_vol(int argc, char* argv[]) {
    assert(argc>1 && argv[1][0]!='-');
    volume.Write(argv[1]);
    return 2;
}

int do_rho_N(int argc, char* argv[])
{
    if ((argc < 2) || (argv[1][0] == '-'))
	{
	    cerr<<"Rho requires a parameter"<<endl;
	    return 1;
	}
    rho = (real_type)(M_PI * 2.0 / atof(argv[1]));
    return 2;
}


void do_misc() {
    // miscelaneous function to help debug
}



void do_meshinfo() {

    for (int m=0; m<2; m++) {
	int nbl=-1;  // num boundary loops
	int nbe=0;  // num boundary edges
	int ncc=-1;  // num connected components


	if (!cmeshes[m]->verts.size()) continue;

	// check that we're a manifold
	vector<int> vertfaces(cmeshes[m]->verts.size());
	for (unsigned i=0; i<vertfaces.size(); i++) vertfaces[i]=0;

	for (unsigned f=0; f<cmeshes[m]->faces.size(); f++) {
	    for (int i=0; i<3; i++) {
		vertfaces[cmeshes[m]->faces[f].verts[i]]++;
	    }
	}

	bool manifold=true;
	int numdangling=0;

	// this barfs on non-manifold meshes, so skip it for now!
	manifold=false;
	if (0)
	    for (unsigned v=0; v<cmeshes[m]->verts.size(); v++) {

		if (vertfaces[v]==0 || cmeshes[m]->verts[v].someface==-1) {
		    cerr<<"dangling vertex (index: "<<v<<")"<<endl;
		    numdangling++;
		}

		int tvf = 0;
		for (TriangleMesh::VertexFaceIteratorI vfi(*cmeshes[m], v); !vfi.done(); ++vfi) {
		    tvf++;
		}

		if (tvf != vertfaces[v]) {
		    cerr<<"non-manifold vertex (index: "<<v<<")"<<endl;
		    dbgClear();
		    DbgPoints::add(cmeshes[m]->verts[v].point, 1, 0, 0);
		    redrawAndWait(' ');
		    manifold=false;
		}
	    }



	if (!manifold) {
	    cerr<<"skipping info for non-manifold surface!"<<endl;
	} else {

	    nbl = 0;
	    nbe = 0;

	    // count the number of boundary loops
	    {
		vector<bool> vb(cmeshes[m]->verts.size());
		for (unsigned i=0;i<cmeshes[m]->verts.size(); i++) {
		    vb[i] = cmeshes[m]->VertOnBoundary(i);
		    if (vb[i]) nbe++;
		}

		while (1) {

		    int startv=-1;
		    for (unsigned i=0; i<vb.size(); i++) {
			if (vb[i]) {
			    startv=i; break;
			}
		    }
		    if (startv<0) break;

		    int bi=startv;
		    do {
			vb[bi] = false;
			bi = *(TriangleMesh::VertexVertexIteratorI(*cmeshes[m], bi));
		    } while (bi!=startv);

		    nbl++;
		}
	    }


	    // count the number of connected components
	    ncc = 0;
	    {
		vector<bool> flooded(cmeshes[m]->verts.size());
		for (unsigned i=0;i<cmeshes[m]->verts.size(); i++) {
		    flooded[i]=false;
		}


		while (1) {

		    int startv=-1;
		    for (unsigned i=0; i<flooded.size(); i++) {
			if (!flooded[i]) {
			    startv=i; break;
			}
		    }
		    if (startv<0) break;

		    vector<int> toflood;
		    toflood.push_back(startv);
		    flooded[startv] = true;

		    while (!toflood.empty()) {
			int tf = toflood.back();
			toflood.pop_back();
			for (TriangleMesh::VertexVertexIteratorI vi(*cmeshes[m], tf); !vi.done(); ++vi) {
			    int tf2 = *vi;
			    if (flooded[tf2]) continue;
			    flooded[tf2]=true;
			    toflood.push_back(tf2);
			}
		    }

		    ncc++;
		}
		
	    }
	}

	int num_edges=0;
	for (unsigned f=0; f<cmeshes[m]->faces.size(); f++) {
	    for (int i=0; i<3; i++) {
		if (cmeshes[m]->faces[f].nbrs[i] < (int)f)
		    num_edges++;
	    }
	}

	int num_verts = cmeshes[m]->verts.size() - numdangling;
	int num_faces = cmeshes[m]->faces.size();

	int genus = (2*ncc - num_verts + num_edges - num_faces - nbl) / 2;


	// compute the smallest angle
	real_type sangle = -1, langle = 1;
	int sav = -1, lav = -1;
	real_type smallest_total_angle = 0.0, largest_total_angle = 0.0;
	for (unsigned f=0; f<cmeshes[m]->faces.size(); f++) {
	    float mna = -1.0;
	    float mxa = 1.0;
	    for (int i=0; i<3; i++) {

		Vector3 e1 = cmeshes[m]->verts[cmeshes[m]->faces[f].verts[(i+1)%3]].point - cmeshes[m]->verts[cmeshes[m]->faces[f].verts[(i+0)%3]].point;
		Vector3 e2 = cmeshes[m]->verts[cmeshes[m]->faces[f].verts[(i+2)%3]].point - cmeshes[m]->verts[cmeshes[m]->faces[f].verts[(i+0)%3]].point;

		real_type ta = e1.dot(e2) / (e1.length() * e2.length());

		if (ta > sangle) {
		    sangle = ta;
		    sav = cmeshes[m]->faces[f].verts[i];
		}
		if (ta < langle) {
		    langle = ta;
		    lav = cmeshes[m]->faces[f].verts[i];
		}
		if (ta > mna)
		    mna = ta;
		if (ta < mxa)
		    mxa = ta;
	    }
	    smallest_total_angle += mna;
	    largest_total_angle += mxa;
	}
	if (sangle < -1) sangle = -1;
	if (sangle > 1)  sangle = 1;
	if (langle < -1) langle = -1;
	if (langle > 1)  langle = 1;
	sangle = acos(sangle);
	langle = acos(langle);
	smallest_total_angle = acos(smallest_total_angle / (real_type) num_faces);
	largest_total_angle = acos(largest_total_angle / (real_type) num_faces);
	DbgPoints::add(cmeshes[m]->verts[sav].point, 1,0,0);
	DbgPoints::add(cmeshes[m]->verts[lav].point, 1,0,0);


	cerr<<"info for mesh "<<m<<":"<<endl;
	if (!manifold) cerr<<"NON MANIFOLD MESH"<<endl;
	cerr<<"\tnum vertices: "<<num_verts<<endl;
	cerr<<"\tnum edges: "<<num_edges<<endl;
	cerr<<"\tboundary edges: "<<nbe<<endl;
	cerr<<"\tnum faces: "<<num_faces<<endl;
	cerr<<"\tnum boundary loops: "<<nbl<<endl;
	cerr<<"\tnum connected components: "<<ncc<<endl;
	cerr<<"\tgenus: "<<genus<<endl;
	cerr<<"\tsmallest angle: "<<(sangle * 180/M_PI)<<" degrees"<<endl;
	cerr<<"\tlargest angle: "<<(langle * 180/M_PI)<<" degrees"<<endl;
	cerr<<"\tsmallest angle mean: "<<(smallest_total_angle * (180/M_PI))<<" degrees"<<endl;
	cerr<<"\tlargest angle mean: "<<(largest_total_angle * (180/M_PI))<<" degrees"<<endl;

    }

}

#define NUM_HISTOGRAM_BINS 100
void do_histogram(void) {

    int counts[NUM_HISTOGRAM_BINS];
    for (int i=0; i<NUM_HISTOGRAM_BINS; i++)
	counts[i] = 0;



    if (cmeshes[0]->faces.size()) {
	for (unsigned f=0; f<cmeshes[0]->faces.size(); f++) {

	    real_type elen[3] = { Point3::distance(cmeshes[0]->verts[cmeshes[0]->faces[f].verts[0]].point, cmeshes[0]->verts[cmeshes[0]->faces[f].verts[1]].point),
				  Point3::distance(cmeshes[0]->verts[cmeshes[0]->faces[f].verts[1]].point, cmeshes[0]->verts[cmeshes[0]->faces[f].verts[2]].point),
				  Point3::distance(cmeshes[0]->verts[cmeshes[0]->faces[f].verts[2]].point, cmeshes[0]->verts[cmeshes[0]->faces[f].verts[0]].point) };

	    real_type circumrad, inrad;
	    void circumradius_inradius(real_type a, real_type b, real_type c, real_type &circumrad, real_type &inrad);
	    circumradius_inradius(elen[0], elen[1], elen[2], circumrad, inrad);
		
	    int bin = (int)(NUM_HISTOGRAM_BINS * ((inrad / circumrad) * 2.0f));
	    if (bin < 0) bin = 0;
	    if (bin >= NUM_HISTOGRAM_BINS) bin = NUM_HISTOGRAM_BINS-1;
	    counts[bin]++;
	}
    } else if (ctetmesh->tets.size()) {
	for (unsigned t=0; t<ctetmesh->tets.size(); t++) {
	    real_type ratio = ctetmesh->radius_ratio(t);
	    int bin = (int)(NUM_HISTOGRAM_BINS * (ratio * 3.f));
	    if (bin < 0) bin = 0;
	    if (bin >= NUM_HISTOGRAM_BINS) bin = NUM_HISTOGRAM_BINS-1;
	    counts[bin]++;
	}
    }




    int mode = 0;
    for (int i=1; i<NUM_HISTOGRAM_BINS; ++i) {
	if (counts[i] > mode)
	    mode = counts[i];
	counts[i] += counts[i-1];
    }
    double quality = 0.0;

    FILE *f = fopen("histogram.txt", "w");
    for (int i=0; i<NUM_HISTOGRAM_BINS; i++) {
	fprintf(f, "%g\n", (double)counts[i] / (double) counts[NUM_HISTOGRAM_BINS-1]);
	if (i>0)
	    quality += (1.0/99.0) * (counts[i-1]+counts[i])/(2.0 * counts[NUM_HISTOGRAM_BINS-1]);
    }
    fclose(f);

    cerr << "overall mesh quality (1.0 is best): " << 1.0-quality << endl;

    FILE *h = fopen("histogram.ppm", "w");
    fprintf(h, "P3 %d %d\n 1\n", NUM_HISTOGRAM_BINS, NUM_HISTOGRAM_BINS);
    for (int i=0; i<NUM_HISTOGRAM_BINS; ++i) {
	// 0<=th<1
	float th = ((double)NUM_HISTOGRAM_BINS - (double)i) / (double) NUM_HISTOGRAM_BINS;
	for (int j=0; j<NUM_HISTOGRAM_BINS; ++j) {
	    float count = counts[j]/(float)counts[NUM_HISTOGRAM_BINS-1];
	    float singlebin;
	    if (j == 0)
		singlebin = counts[0];
	    else
		singlebin = counts[j]-counts[j-1];
	    singlebin /= (float)mode;

	    fprintf(h, "%d 1 %d\n",th>=count ,th>=singlebin);
	}
    }
    fclose(h);
}


void do_dbg() {
    BREAK;
}


void do_sync_cam_out() {
    if (gui) {
	gui->SetCamSyncOut(0);
    }
}

void do_sync_cam_in() {
    if (gui) {
	gui->SetCamSyncIn(0);
    }
}


void read_pc(const char *filename) {
    vector<InfoPoint> pts;
    PC_read(filename, pts);

    for (unsigned i=0; i<pts.size(); i++) {
	points.insert_vertex(Point3(pts[i].x_, pts[i].y_, pts[i].z_));
    }
}


bool read_surface(const char *fname, int i) {
	
    if (endswith(fname, ".m") || endswith(fname, ".off")) {
	meshes[i].Read(fname);
	meshes[i].BuildStrips(100);
    } else if (endswith(fname, ".vol") ||
	       endswith(fname, ".nhdr")) {
	if (i!=0) {
	    cerr<<"can only read one volume!"<<endl;
	    return false;
	}
	volume.Read(fname);
    } else if (endswith(fname, ".obj")) {
	read_obj(fname, points);
    } else if (endswith(fname, ".offt")) {
	tetmesh.ReadOFF(fname);
    } else if (endswith(fname, ".pc")) {
	read_pc(fname);
    } else {
	cerr<<"unknown file type: "<<fname<<endl;
	return false;
    }

    return true;
}



// gcc won't link to these functions if they are not referenced in the main binary
// (they are referenced in gtb)
#ifndef USENR
#include <f2c.h>
extern "C" {
#include <clapack.h>
}
void fix_gcc_linking() {
    ssyev_(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    dsyev_(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    dgesvd_(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    sgesvd_(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}
#endif

// Global variables for worker thread
static gtb::CommandLine *g_commandline = NULL;
static int g_toskip = 0;

// Worker thread function - runs command parsing and triangulation
// This allows the main thread to run glutMainLoop for GUI
void *worker_thread_func(void *arg) {
    if (g_commandline) {
        g_commandline->Parse(g_toskip);
    }
    return NULL;
}





int main(int argc, char* argv[]) {



#ifdef WIN32
    SetPriorityClass(GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);
#endif

    cout.precision(20);
    cerr.precision(20);

    gtb::nrseed(0);


    command_line[0]=0;
    for (int i=1; i<argc; i++) {
	strcat(command_line, argv[i]);
	strcat(command_line, " ");
    }


    gtb::CommandLine cl(argc, argv, "afront <-nogui> <-noedgeflip> mesh1/mls <mesh2> [-commands]");

    CL_ADD_FUN(cl,misc,               ": miscleaneous");
    CL_ADD_FUN(cl,meshinfo,           ": display stats about the mesh");
    CL_ADD_FUN(cl,histogram,          ": write a histogram of the triangle circle ratios to histogram.txt");
    CL_ADD_FUN(cl,quit,               ": duh");
    CL_ADD_FUN(cl,dbg,                ": hit a debug break function");
    CL_ADD_FUN(cl,gui,                ": start the gui");
    CL_ADD_FUN(cl,subdiv,             "num : apply num iterations of loop subdivision");
    CL_ADD_FUN(cl,volsmooth,          "kernelsize : smooth the volume data");
    CL_ADD_FUN(cl,trans,              ": enter mesh transformation mode - for orienting meshes for csg");
    CL_ADD_FUN(cl,flip,               ": flip the ordering of mesh0 faces");
    CL_ADD_FUN(cl,flip_components,    ": allow the user to flip the orientation of each connected componont");
    CL_ADD_FUN(cl,save_mesh0,         "name : save mesh[0] to a file");
    CL_ADD_FUN(cl,save_mesh1,         "name : save mesh[1] to a file");
    CL_ADD_FUN(cl,save_pts,           "name : save pointset to a file");
    CL_ADD_FUN(cl,save_vol,           "name : save volume to a file");
    CL_ADD_VAR(cl,rho,                ": angle subtended on the osculating sphere");
    CL_ADD_FUN(cl,rho_N,              "N: # of edges that a circle is divided to");
    CL_ADD_VAR(cl,draw_messages,      ": draw the debug messages on the sceen?  good for screenshots");
    CL_ADD_VAR(cl,saliency,           ": the radius for the saliency computations");
    CL_ADD_FUN(cl,tet_smooth,         "N: make N passes of smoothing on the tet mesh scalars");

    CL_ADD_VAR(cl,sharp,             "ang : cosine of angle between triangle normals for edge to be considered sharp feature (default, -1 - no sharp features are handled)");
    CL_ADD_VAR(cl,small_crease,       "edge_count : creases with less than <edge_count> edges will be discarded (default 20)");
    CL_ADD_VAR(cl,outname,            ": the file to output the mesh to");
    CL_ADD_VAR(cl,save_field,         ": name of the file to write the guidance field to");
    CL_ADD_VAR(cl,load_field,         ": name of the file to load the guidance field from");
    CL_ADD_VAR(cl,min_step,           ": minimum edge length allowed");
    CL_ADD_VAR(cl,max_step,           ": maximum edge length allowed");
    CL_ADD_VAR(cl,reduction,          ": percent reduction allowed in a single step");
    CL_ADD_VAR(cl,radius_factor,      ": set radius factor for mls triangulation");
    CL_ADD_VAR(cl,csg_guidance_blend, ": blend guidance field into existing edge lengths");
    CL_ADD_VAR(cl,failsafe,           ": close any holes that failed to be triangulated");
    CL_ADD_VAR(cl,adamson,            ": use adamson projection instead of standard mls - 0=standard mls, 1=adamson with weighted ave of normals, 2=adamson with covariance normals from weighted average, 3=adamson with covariance normals from point");
    CL_ADD_VAR(cl,nielson,            ": use nielson tet interpolation");
    CL_ADD_VAR(cl,fence_scale,        ": scale the height of the fence for front intersection tests");

    CL_ADD_FUN(cl,tri_mesh,           "subdiv : triangulate the mesh, applying subdiv iterations of loop subdivision before computing curvature");
    CL_ADD_FUN(cl,tri_smoothmls,      ".obj: triangulate a smooth mls surface");
    CL_ADD_FUN(cl,tri_vol,            "isovalue <bspline>: triangulate an isosurface from the volume");
    CL_ADD_FUN(cl,tri_tet,            "isovalue: triangulate an isosurface from the tet mesh");
    CL_ADD_FUN(cl,marchingcubes,      "isovalue: extract an isosurface from the regular volume");
    CL_ADD_FUN(cl,marchingtets,       "isovalue: extract an isosurface from the tet volume");
    CL_ADD_VAR(cl,curvature_sub,      ": how much to divide cells for the guidance field");
    CL_ADD_VAR(cl,eval_sub,           ": how much to divide cells for evaluating the tetmesh mls isofunction");
    CL_ADD_VAR(cl,trim_guidance,      ": trim the guindance field or use the full point set");
    CL_ADD_VAR(cl,trim_bin_size,      ": when to stop subdivision for guidance field trimming");
    CL_ADD_VAR(cl,proj_tol,           ": the termination condition for newton stepping onto isosurfaces");
    CL_ADD_VAR(cl,grid_intersect_overestimate,           ": how many neighboring cells to check for intersection when building guidance field");
    CL_ADD_VAR(cl,bad_connect_priority, ": if the triangle priority (circumradius/inradius) is < this, don't consider that triangle");
    CL_ADD_VAR(cl,boundary_dist,      ": boundary detection parameter for triangle soups - 0 disables (uses topological boundaries), I recommend -0.2.  Negative is relative, positive is absolute");
    CL_ADD_VAR(cl,rmls,               ": use robust mls instead of standard");
    CL_ADD_VAR(cl,rmls_knn,           ": number of nearest neighbors to use for rmls projection");
    CL_ADD_VAR(cl,noise_threshold,    ": noise parameter for rmls projection and feature detection");


    CL_ADD_FUN(cl,csg,                "command <subdiv> : do a csg operation on the two meshes and retriangulate");
    CL_ADD_FUN(cl,tri,                ": try to smartly decide which triangulation to do");

    CL_ADD_FUN(cl,reeb,               "fun: enable reeb graph computation\n\tfun=={x,y,z}: use x, y or z axis as morse function");
    CL_ADD_FUN(cl,reeb_eps,           "eps: set persistence imprecision to be at most eps*max_triangle_size (default 0.1)");

    CL_ADD_FUN(cl,sync_cam_out,       "write the camera view to stdout continuously");
    CL_ADD_FUN(cl,sync_cam_in,        "read the camera view from stdin continuously");


    CL_ADD_VAR(cl,idealNumThreads,    "num : set the ideal number of execution threads");


    if (argc < 2) {
	cl.Print();
	return 0;
    }

    int toskip=1;

    bool use_gui = true;
    if ((argc > toskip+1) && (stricmp(argv[toskip], "-nogui")==0))
	{
	    toskip++;
	    critical_section = new thlib::CSObject();
	    use_gui = false;
	} else {
        do_gui();
    }


    if ((argc > toskip+1) && (stricmp(argv[toskip], "-noedgeflip")==0))
	{
	    toskip++;
	} else {
        OutputController::AddControllerToBack(output_controller_head,
					      new OutputControllerEdgeFlipper());
	OutputControllerEdgeFlipper::DoFlips = true;
    }


    critical_section->enter();

    if (argc>toskip && argv[toskip][0] != '-')
	{
	    if (!read_surface(argv[toskip], 0)) return 1;
	    toskip++;

	    if (argc>toskip && argv[toskip][0] != '-') {
		if (!read_surface(argv[toskip], 1)) return 1;
		toskip++;
	    }
	}

    critical_section->leave();


    redrawAll();
    if (gui) gui->ViewAll();

    if (use_gui && gui) {
        // GUI mode: Spawn worker thread for triangulation, main thread runs GLUT
        g_commandline = &cl;
        g_toskip = toskip;
        thlib::Thread *worker = new thlib::Thread(worker_thread_func, NULL, 0);

        // Main thread runs GLUT event loop - never returns until quit
        gui->Go();

        // Cleanup (never reached unless user quits)
        delete gui;
    } else {
        // No GUI mode: Run triangulation on main thread
        cl.Parse(toskip);
        redrawAndWait('x', true);
    }

    return 0;
}


