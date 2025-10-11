

#include "common.h"

#include <ThreadLib/threadslib.h>

#include <viewer/viewer.h>

#include "uheap.h"
#include "front.h"
#include "guidance.h"
#include "triangulator.h"
#include "triangulate_mesh.h"
#include "triangulate_tet.h"
#include "triangulate_mls.h"
#include "triangulate_csg.h"
#include "triangulate_iso.h"

#include "output_controller_gui.h"



void PerpVectors(const Vector3 &n, Vector3 &udir, Vector3 &vdir);

extern const TriangleMesh *cmeshes[2];
extern const RegularVolume *cvolume;
extern const surfel_set *cpoints;
extern const TetMesh *ctetmesh;
extern const TriangleMesh *ctetshell;



OutputControllerGui::OutputControllerGui(const char *fnames) :
    Viewer(),
    drawMesh(1),
    drawNormals(false),
    drawBoundaries(true),
    drawGuidance(false),
    drawFront(true),
    drawFence(false),
    drawOutput(1),
    drawPointSize(1),
    WaitForUser(true),
    UIEditMesh(-1)
{
    meshEditTrans = Matrix4d::MATRIX4_IDENTITY;

    char title[1024];
    strcpy(title, "");

    AddWindow(strcat(title, fnames), 600, 600);
}



void OutputControllerGui::MeshEditMode(TriangleMesh meshes[2]) {

    while (1) {

	cerr<<"select mesh to orient"<<endl;

	waitForKey();
	int lastkey = LastKey();
	if (lastkey=='1') UIEditMesh=0;
	else if (lastkey=='2') UIEditMesh=1;
	else if (lastkey==' ') { UIEditMesh=-1; break; }
	else continue;

	cerr<<"orienting mesh "<<UIEditMesh<<endl;


	int m = UIEditMesh;
	while (UIEditMesh!=-1) { }

	// bake the transformation in
	_critical_section.enter();
	for (unsigned i=0; i<meshes[m].verts.size(); i++) {
	    meshes[m].verts[i].point = MatPointMult(meshEditTrans, meshes[m].verts[i].point);
	}
	meshEditTrans = Matrix4d::MATRIX4_IDENTITY;

	meshes[m].SetNormals();
	meshes[m].invalidate();

	_critical_section.leave();

	Redraw();
    }
}


// viewer functions that need to be implemented

void OutputControllerGui::Init(int win) {

    glClearColor(1,1,1,0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(3.0f, 3.0f);

    

    //	cerr<<glGetString(GL_EXTENSIONS)<<endl;
}


void OutputControllerGui::DrawMesh(const TriangleMesh &mesh) {

    if (!mesh.verts.size())
	return;

    if (drawMesh) {

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

#ifdef REAL_IS_FLOAT
	glVertexPointer(3, GL_FLOAT, (char*)&mesh.verts[1].point - (char*)&mesh.verts[0].point, &mesh.verts[0].point);
	glNormalPointer(GL_FLOAT, (char*)&mesh.verts[1].normal - (char*)&mesh.verts[0].normal, &mesh.verts[0].normal);
#else
	glVertexPointer(3, GL_DOUBLE, (char*)&mesh.verts[1].point - (char*)&mesh.verts[0].point, &mesh.verts[0].point);
	glNormalPointer(GL_DOUBLE, (char*)&mesh.verts[1].normal - (char*)&mesh.verts[0].normal, &mesh.verts[0].normal);
#endif


	LoadGLColor(gtb::LightGoldenrod3);
	const int *index = &mesh.tstrips[0];
	const int *end = &(*mesh.tstrips.end());
	while (index<end) {
	    int len = *index;
	    index++;
	    glDrawElements(GL_TRIANGLE_STRIP, len, GL_UNSIGNED_INT, index);
	    index += len;
	}
	glDisableClientState(GL_NORMAL_ARRAY);


	if (drawMesh & 2) {
	    glDisable(GL_LIGHTING);
	    glColor3f(0,0,0);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	    const int *index = &mesh.tstrips[0];
	    const int *end = &(*mesh.tstrips.end());
	    while (index<end) {
		int len = *index;
		index++;
		glDrawElements(GL_TRIANGLE_STRIP, len, GL_UNSIGNED_INT, index);
		index += len;
	    }


	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	    glEnable(GL_LIGHTING);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (drawBoundaries) {
	glColor3f(0,0,1);
	glLineWidth(2);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (unsigned v=0; v<mesh.verts.size(); v++) {
	    if (!mesh.VertOnBoundary(v)) continue;

	    TriangleMesh::VertexVertexIteratorI vvi(mesh, v);
	    if (!mesh.VertOnBoundary(*vvi)) continue;

	    mesh.verts[v].point.load();
	    mesh.verts[*vvi].point.load();
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glLineWidth(1);
    }
}


void OutputControllerGui::DrawTetMesh(const TetMesh &mesh) {

    if (!mesh.verts.size())
	return;

    if (drawMesh) {

	glEnableClientState(GL_VERTEX_ARRAY);

#ifdef REAL_IS_FLOAT
	glVertexPointer(3, GL_FLOAT, (char*)&mesh.verts[1].point - (char*)&mesh.verts[0].point, &mesh.verts[0].point);
#else
	glVertexPointer(3, GL_DOUBLE, (char*)&mesh.verts[1].point - (char*)&mesh.verts[0].point, &mesh.verts[0].point);
#endif


	for (unsigned i=0; i<mesh.tets.size(); i++) {
			
	    int indices[8] = { mesh.tets[i].vi[0], 
			       mesh.tets[i].vi[1], 
			       mesh.tets[i].vi[2], 
			       mesh.tets[i].vi[0], 
			       mesh.tets[i].vi[3], 
			       mesh.tets[i].vi[1], 
			       mesh.tets[i].vi[3], 
			       mesh.tets[i].vi[2] };


	    glDrawElements(GL_LINE_STRIP, 8, GL_UNSIGNED_INT, indices);

	}
	glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (drawMesh&2) {
	DrawMesh(*ctetshell);
    }


}



real_type disoval=0;
void OutputControllerGui::Draw(int win) {

    _critical_section.enter();

    glEnable(GL_LIGHTING);

    glPointSize(drawPointSize);


    for (int m=0; m<2; m++) {

	glPushMatrix();
	if (UIEditMesh == m) {
	    meshEditTrans.ogl_multiply();
	}

	DrawMesh(*cmeshes[m]);

	glPopMatrix();

    }



    if (drawOutput) {
	LoadGLColor(gtb::LightGoldenrod3);
	glBegin(GL_TRIANGLES);
	for (unsigned ti=0; ti<triangulation.num_triangles(); ti++) {
	    const gtb::IndexedTriangle &t = triangulation.indexed_triangle(ti);

	    Vector3 fnorm = (triangulation.vertex(t[1])-triangulation.vertex(t[0])).cross(triangulation.vertex(t[2])-triangulation.vertex(t[0]));
	    fnorm.normalize();
	    fnorm.load_as_normal();

	    for (int j=0; j<3; j++) {
		triangulation.vertex(t[j]).load();
	    }
	}
	glEnd();
    }

    glDisable(GL_LIGHTING);

    glColor3f(0,0,0);


    DrawTetMesh(*ctetmesh);


    // can't really draw the volume, so just draw a wireframe box around it
    if (!cvolume->Empty()) {
	Box3 volbox = cvolume->bounding_box();

	glBegin(GL_LINE_LOOP);
	volbox.vertex(0).load();
	volbox.vertex(1).load();
	volbox.vertex(3).load();
	volbox.vertex(2).load();
	glEnd();

	glBegin(GL_LINE_LOOP);
	volbox.vertex(4).load();
	volbox.vertex(5).load();
	volbox.vertex(7).load();
	volbox.vertex(6).load();
	glEnd();


	glBegin(GL_LINES);
	volbox.vertex(0).load();
	volbox.vertex(4).load();
	volbox.vertex(1).load();
	volbox.vertex(5).load();
	volbox.vertex(3).load();
	volbox.vertex(7).load();
	volbox.vertex(2).load();
	volbox.vertex(6).load();
	glEnd();

    }


    if (drawMesh && drawNormals) {

	glBegin(GL_LINES);
	for (int m=0; m<2; m++) {

	    if (cmeshes[m]->verts.size() == 0) continue;

	    real_type normal_scale = cmeshes[m]->bounding_box().diagonal_length() * 0.005;

	    for (unsigned v=0; v<cmeshes[m]->verts.size(); v++) {
		cmeshes[m]->verts[v].point.load();
		(cmeshes[m]->verts[v].point + (real_type)normal_scale*cmeshes[m]->verts[v].normal).load();
	    }
	}
	glEnd();
    }

    if (drawOutput && drawNormals) {
	glBegin(GL_LINES);
	for (unsigned ti=0; ti<triangulation.num_vertices(); ti++) {
	    const Vector3 &v = triangulation.vertex_normal(ti);
	    const Point3 &p = triangulation.vertex(ti);

	    p.load();
	    (p + (real_type).01 * v).load();
	}
	    
	glEnd();
    }

    if (drawOutput & 2) {
	for (unsigned ti=0; ti<triangulation.num_triangles(); ti++) {
	    const gtb::IndexedTriangle &t = triangulation.indexed_triangle(ti);
	    glBegin(GL_LINE_LOOP);
	    for (int j=0; j<3; j++) {
		triangulation.vertex(t[j]).load();
	    }
	    glEnd();
	}
    }


    // draw the fronts
    if (drawFront) {
	for (unsigned f=0; f<drawfronts_p.size(); f++) {
	    if (drawfronts_p[f].size()) {
                glLineWidth(3.0);
		glBegin(GL_LINES);
		for (unsigned f2=0; f2<drawfronts_p[f].size(); f2++) {
		    switch (drawfronts_s[f][f2]) {

		    case PRIORITY_GROW_EDGE:
			glColor3f(0,0,1);
			break;
		    case PRIORITY_CONNECT:
			glColor3f(1,0,0);
			break;
		    case PRIORITY_FAILSAFE:
			glColor3f(1,1,0);
			break;
		    default:	// PRIORITY_OWA
			glColor3f(0.8,0.8,1.0);
			break;
		    }
		    drawfronts_p[f][f2].load();
		    drawfronts_p[f][(f2+1)%drawfronts_p[f].size()].load();
		}
		glEnd();
                glLineWidth(1.0);
	    }
	}
    }


    if (drawFence) {

	for (unsigned f=0; f<drawfronts_p.size(); f++) {
	    if (drawfronts_p[f].size()) {

		for (unsigned f2=0; f2<drawfronts_p[f].size(); f2++) {

		    Point3 p1 = drawfronts_p[f][f2];
		    Point3 p2 = drawfronts_p[f][(f2+1)%drawfronts_p[f].size()];
		    Vector3 n1 = drawfronts_n[f][f2];
		    Vector3 n2 = drawfronts_n[f][(f2+1)%drawfronts_n[f].size()];

		    real_type len1 = drawfronts_f[f][f2];
		    real_type len2 = drawfronts_f[f][(f2+1)%drawfronts_f[f].size()];

		    Point3 points[4];
		    points[0] = p2 + len2*n2;
		    points[1] = p2 - len2*n2;
		    points[2] = p1 + len1*n1;
		    points[3] = p1 - len1*n1;


		    glColor4f(0,0,1,0.5);
		    glDepthMask(GL_FALSE);
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		    for (int j=0; j<2; j++) {

			if (j)
			    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBegin(GL_QUADS);

			if (j)	glColor4f(0,0,0,1);
			else	glColor4f(0,0,1,0.5);

			points[0].load();
			points[1].load();
			points[3].load();
			points[2].load();

			glEnd();

		    }

		    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		    glDisable(GL_BLEND);
		    glDepthMask(GL_TRUE);


		    // check that the fence meets our error bound
#if 1
		    real_type rho = 0.5;

		    Vector3 u = n1+n2;				u.normalize();
		    Vector3 v = (p1-p2);			v.normalize();
		    Vector3 axis = v.cross(u);
		    u = axis.cross(v); u.normalize();

		    Point3 center = p1 + ((real_type)0.5)*(p2-p1);
		    center -= (real_type)((Point3::distance(p1,p2)*((real_type)0.5) / tan(rho/2))) * u;

		    float rad = Point3::distance(center, p1);

		    glLineWidth(2);
		    glBegin(GL_LINE_STRIP);
		    for (double theta =-rho/2; theta<=rho/2.0+0.007; theta+=M_PI*0.005) {
			if (theta > rho/2) theta=rho/2;

			real_type frac = (theta + rho/2) / (rho);
			glColor3f(0,frac,frac);
			(center + rad*(real_type)cos(theta)*u + rad*(real_type)sin(theta)*v).load();

		    }
		    glEnd();
		    glLineWidth(1);
#endif



		}

	    }

	}

    }



    if (drawMesh && cpoints->size()) {

	unsigned N = cpoints->size();
	glColor3f(0,0.8,0.4);

	if (cpoints->has_normals()) {
	    glEnable(GL_LIGHTING);
	    glBegin(GL_POINTS);
	    for (unsigned i = 0; i < N; ++i)
		{
		    cpoints->normal(i).load_as_normal();
		    cpoints->vertex(i).load();
		}
	    glEnd();
	    glDisable(GL_LIGHTING);
	} else {
	    glDisable(GL_LIGHTING);
	    glBegin(GL_POINTS);
	    for (unsigned i = 0; i < N; ++i)
		{
		    cpoints->vertex(i).load();
		}
	    glEnd();
	}


		
	if (drawNormals && cpoints->has_normals()) {
	    glColor3f(0,0,0);
	    glBegin(GL_LINES);
	    for (unsigned i=0; i<N; ++i) {
		cpoints->vertex(i).load();
		(cpoints->vertex(i) + ((real_type)0.03)*cpoints->normal(i)).load();
	    }
	    glEnd();
	}
		
	glEnable(GL_LIGHTING);
    }


    // draw the debugging info
    glEnable(GL_LIGHTING);
    DbgPoly::draw(win);
    DbgOPoints::draw(win);
    glDisable(GL_LIGHTING);
    DbgPLines::draw(win);
    DbgPoints::draw(win);
    DbgSpheres::draw(win);


    glEnable(GL_LIGHTING);


    _critical_section.leave();
}


void OutputControllerGui::Key(int win, unsigned char key) {

    if (UIEditMesh != -1) {

	_critical_section.enter();


	static real_type factor=1;
	static int mode = 0;
	real_type amount;
	switch (key) {

	case 'b':
	case 'B':
	    cerr<<"done editing mesh"<<endl;
	    UIEditMesh = -1;
	    break;

	case '+':
	case '=':
	case 'F':
	    factor *= 2;
	    cerr<<"edit factor: "<<factor<<endl;
	    break;

	case '-':
	case '_':
	case 'f':
	    factor /= 2;
	    cerr<<"edit factor: "<<factor<<endl;
	    break;

	case 'r':
	case 'R':
	    factor = 1;
	    mode = 0;
	    cerr<<"edit mode: "<<mode<<endl;
	    break;

	case 't':
	case 'T':
	    factor = 1;
	    mode = 1;
	    cerr<<"edit mode: "<<mode<<endl;
	    break;

	case 's':
	case 'S':
	    factor = 1;
	    mode = 2;
	    cerr<<"edit mode: "<<mode<<endl;
	    break;

	case 'x':
	case 'X':
	    if (key=='x')
		amount = -factor;
	    else
		amount = factor;

	    {
		Matrix4d tmat = Matrix4d::MATRIX4_IDENTITY;
		switch (mode) {
		case 0:	// rotate
		    {
			double s = sin(amount * 0.1);
			double c = cos(amount * 0.1);
			tmat = Matrix4d(1, 0, 0, 0,
					0, c, s, 0,
					0,-s, c, 0,
					0, 0, 0, 1);
		    }
		    break;
		case 1:	// translate
		    tmat = gtb::TranslationMatrix<double>(amount, 0, 0);
		    break;
		case 2:	// scale
		    tmat[0][0] *= pow((double)factor, 0.25);
		    tmat[1][1] *= pow((double)factor, 0.25);
		    tmat[2][2] *= pow((double)factor, 0.25);
		    break;
		}
		meshEditTrans = tmat * meshEditTrans;
	    }
	    break;

	case 'y':
	case 'Y':
	    if (key=='y')
		amount = -factor;
	    else
		amount = factor;

	    {
		Matrix4d tmat = Matrix4d::MATRIX4_IDENTITY;
		switch (mode) {
		case 0:	// rotate
		    {
			double s = sin(amount * 0.1);
			double c = cos(amount * 0.1);
			tmat = Matrix4d(c, 0,-s, 0,
					0, 1, 0, 0,
					s, 0, c, 0,
					0, 0, 0, 1);
		    }
		    break;
		case 1:	// translate
		    tmat = gtb::TranslationMatrix<double>(0, amount, 0);
		    break;
		case 2:	// scale
		    tmat[0][0] *= pow((double)factor, 0.25);
		    tmat[1][1] *= pow((double)factor, 0.25);
		    tmat[2][2] *= pow((double)factor, 0.25);
		    break;
		}
		meshEditTrans = tmat * meshEditTrans;
	    }
	    break;

	case 'z':
	case 'Z':
	    if (key=='z')
		amount = -factor;
	    else
		amount = factor;

	    {
		Matrix4d tmat = Matrix4d::MATRIX4_IDENTITY;
		switch (mode) {
		case 0:	// rotate
		    {
			double s = sin(amount * 0.1);
			double c = cos(amount * 0.1);
			tmat = Matrix4d(c, s, 0, 0,
					-s,c, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
		    }
		    break;
		case 1:	// translate
		    tmat = gtb::TranslationMatrix<double>(0, 0, amount);
		    break;
		case 2:	// scale
		    tmat[0][0] *= pow((double)factor, 0.25);
		    tmat[1][1] *= pow((double)factor, 0.25);
		    tmat[2][2] *= pow((double)factor, 0.25);
		    break;
		}
		meshEditTrans = tmat * meshEditTrans;
	    }
	    break;
	}

	_critical_section.leave();


    } else {

	switch(key) 
	    {

	    case 'm':
	    case 'M':
		drawMesh++;
		if (drawMesh>2) drawMesh=0;
		break;

	    case '+':
		drawPointSize++;
		break;
	    case '-':
		drawPointSize = std::max(drawPointSize-1, 1);
		break;

	    case 'o':
	    case 'O':
		drawOutput++;
		if (drawOutput>2) drawOutput=0;
		break;

	    case 'b':
	    case 'B':
		drawBoundaries = !drawBoundaries;
		break;

	    case 'n':
	    case 'N':
		drawNormals = !drawNormals;
		break;

	    case 'g':
	    case 'G':
		drawGuidance = !drawGuidance;
		break;

	    case 'f':
	    case 'F':
		drawFront = !drawFront;
		break;

	    case 'e':
	    case 'E':
		drawFence = !drawFence;
		break;

	    case 'p':
	    case 'P':
		WaitForUser = !WaitForUser;
		if (WaitForUser) cerr<<"Waiting for user"<<endl;
		else cerr<<"Not waiting for user"<<endl;
		break;

	    case 'a':
	    case 'A':
		ViewAll();
		break;

	    case 'c':
	    case 'C':
		dbgClear();
		break;

	    case 'i':
	    case 'I':
		ToggleImmediateMode();
		break;

	    case '2':
		ToggleOrtho();
		break;

	    case 's':
	    case 'S':
		if (SaveViewInfo("view.txt"))
		    cerr<<"view information saved"<<endl;
		break;

	    case 'l':
	    case 'L':
		if (LoadViewInfo("view.txt"))
		    cerr<<"view information loaded"<<endl;
		break;

	    case 'q':
	    case 'Q':
	    case 27:  // ESC key
		cerr<<"Quitting..."<<endl;
		exit(0);
		break;

	    case '?':
	    case 'h':
	    case 'H':
		cerr<<"\n========== AFRONT GUI KEYBOARD CONTROLS ==========\n";
		cerr<<"Triangulation Control:\n";
		cerr<<"  p/P       - Toggle pause/unpause triangulation\n";
		cerr<<"  Spacebar  - Single-step when paused\n";
		cerr<<"\nDisplay Toggles:\n";
		cerr<<"  m/M       - Cycle mesh drawing mode\n";
		cerr<<"  o/O       - Cycle output drawing mode\n";
		cerr<<"  b/B       - Toggle boundaries display\n";
		cerr<<"  n/N       - Toggle normals display\n";
		cerr<<"  g/G       - Toggle guidance field visualization\n";
		cerr<<"  f/F       - Toggle front display\n";
		cerr<<"  e/E       - Toggle fence display\n";
		cerr<<"  +         - Increase point size\n";
		cerr<<"  -         - Decrease point size\n";
		cerr<<"\nView Controls:\n";
		cerr<<"  a/A       - View all (reset camera)\n";
		cerr<<"  i/I       - Toggle immediate rendering mode\n";
		cerr<<"  2         - Toggle orthographic projection\n";
		cerr<<"\nSave/Load:\n";
		cerr<<"  s/S       - Save view to view.txt\n";
		cerr<<"  l/L       - Load view from view.txt\n";
		cerr<<"\nMouse Controls:\n";
		cerr<<"  Left drag       - Rotate view\n";
		cerr<<"  Shift+Left      - Zoom\n";
		cerr<<"  Command+Left    - Zoom (macOS)\n";
		cerr<<"  Alt+Left        - Pan view\n";
		cerr<<"\nOther:\n";
		cerr<<"  c/C       - Clear debug drawings\n";
		cerr<<"  q/Q/ESC   - Quit\n";
		cerr<<"  ?/h/H     - Show this help\n";
		cerr<<"==================================================\n\n";
		break;

	    }
    }


    Redraw();
}


void OutputControllerGui::PassiveMotion(int win, int wx, int wy, float x, float y, float z) 
{
}

void OutputControllerGui::Mouse(int win, int button, int motion, int wx, int wy, float depth, float x, float y, float z) {
    if (button == GLUT_MIDDLE_BUTTON && motion==-1) {
	if (z < 1e8) {
	    LookAt(win, x, y, z);
	} else {
	    cerr<<"Center not set"<<endl;
	}
    }
}

void OutputControllerGui::BoundingSphere(int win, float &x, float &y, float &z, float &r) {
    _critical_section.enter();

    bool hasbox=false;
    Box3 box;

    for (int m=0; m<2; m++) {
	if (cmeshes[m]->verts.size()) {
	    Box3 tbox = cmeshes[m]->bounding_box();
	    box = hasbox ? Box3::make_union(box, tbox) : tbox;
	    hasbox=true;
	}
    }

    if (triangulation.num_vertices()) {
	Box3 tbox = triangulation.bounding_box();
	box = hasbox ? Box3::make_union(box, tbox) : tbox;
	hasbox=true;
    }

    if (!cvolume->Empty()) {
	Box3 tbox = cvolume->bounding_box();
	box = hasbox ? Box3::make_union(box, tbox) : tbox;
	hasbox=true;
    }

    if (cpoints->size()) {
	Box3 tbox = cpoints->bounding_box();
	box = hasbox ? Box3::make_union(box, tbox) : tbox;
	hasbox=true;
    }

    if (ctetmesh->verts.size()) {

	Point3 p = ctetmesh->verts[0].point;
	real_type min_x = p.x(), min_y = p.y(), min_z = p.z();
	real_type max_x = p.x(), max_y = p.y(), max_z = p.z();
	for (unsigned i = 1; i < ctetmesh->verts.size(); i++) {
	    p = ctetmesh->verts[i].point;
	    min_x = std::min(min_x, p.x());
	    min_y = std::min(min_y, p.y());
	    min_z = std::min(min_z, p.z());
	    max_x = std::max(max_x, p.x());
	    max_y = std::max(max_y, p.y());
	    max_z = std::max(max_z, p.z());
	}
	Box3 tbox(min_x, min_y, min_z, max_x, max_y, max_z);

	box = hasbox ? Box3::make_union(box, tbox) : tbox;
	hasbox=true;
    }


    if (!hasbox)
	box = Box3(Point3(-1,-1,-1), Point3(1,1,1));


    _critical_section.leave();

    x = box.centroid()[0];
    y = box.centroid()[1];
    z = box.centroid()[2];
    r = box.diagonal_length() * 0.6;

}


// copy the data from the worker thread to the drawing thread
void OutputControllerGui::Extract(Triangulator *t) {
    _critical_section.enter();

    if (t && drawFront) {
	t->ExtractFronts(drawfronts_p, drawfronts_n, drawfronts_s, drawfronts_f);
    } else {
	drawfronts_p.clear();
	drawfronts_n.clear();
	drawfronts_s.clear();
	drawfronts_f.clear();
    }

    _critical_section.leave();
}


// output functions that need to be implemented
void OutputControllerGui::AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary) {
    _critical_section.enter();
    triangulation.add_vertex(p);
    triangulation.insert_vertex_normal(n);
    _critical_section.leave();


    if (child)
	child->AddVertex(index, p, n, boundary);
}

void OutputControllerGui::AddTriangle(int index, int v1, int v2, int v3) {
    _critical_section.enter();
    triangulation.insert_triangle((unsigned)v1, (unsigned)v2, (unsigned)v3, false, 0.0);
    _critical_section.leave();

    if (child)
	child->AddTriangle(index, v1, v2, v3);

}



Point3 OutputControllerGui::MatPointMult(const Matrix4d &m, const Point3 &p) {
    Point3 ret;
    for (int i=0; i<3; i++) {
	ret[i] = (real_type)(m[i][0]*p[0] + m[i][1]*p[1] + m[i][2]*p[2] + m[i][3]);
    }
    return ret;
}





