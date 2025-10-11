
#include <vector>
#include <iostream>

#include <gtb/graphics/ogltools.h>

#include <math.h>

#include "viewer.h"
#include "trackball.h"

// GLUT_ACTIVE_SUPER is Command/Meta key (may not be defined in older GLUT)
#ifndef GLUT_ACTIVE_SUPER
#define GLUT_ACTIVE_SUPER 0x0008
#endif


void *Viewer::start_stub(void *arg)
{
    theViewer->Go();
    return 0;
}


void Viewer::Start() {
	// IMPORTANT: On macOS, GLUT initialization and window creation
	// must happen on the main thread. Do it here synchronously.
	// Caller MUST call Go() on the main thread afterwards.
	_critical_section.enter();

	int argc=1; char *argv[1] = { "blah" };
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	for (unsigned i=0; i<windows.size(); i++) {
	    MakeWindow(i);
	}
	glut_has_initialized = true;
	_critical_section.leave();

	// DON'T spawn thread - caller will run Go() on main thread
	// The old code spawned a thread here, but macOS now requires
	// glutMainLoop to run on the main thread.
}




void Viewer::Go() {
    // GLUT initialization already done in Start() on main thread
    // Just run the event loop here
    glutMainLoop();	// never returns!
}

void *Viewer::cam_stub(void *arg)
{
	theViewer->CamSyncLoop();
	return 0;
}

void Viewer::CamSyncLoop() {

	while (1) {

		float pan[3];
		float d;
		float quat[4];
		bool o;

		if (ReadViewInfo(stdin, pan, d, quat, o)) {
			for (unsigned i=0; i<windows.size(); i++) {
				if (windows[i]->sync_cam_in)
					LoadViewInfo(pan, d, quat, o, i);
			}
		}

		thlib::Thread::yield();
	}
}



Viewer* Viewer::theViewer = NULL;
volatile bool Viewer::glut_has_initialized = false;

Viewer::Viewer() :
_critical_section(),
_thread(NULL),
cam_thread(NULL),
input_cs(),
last_key(-1),
last_key_win(-1),
last_click_win(-1),
last_click_button(-1)
{
	if (theViewer) {
		std::cerr<<"can only create one viewer, exiting!"<<std::endl;
		exit(-1);
	}
	theViewer=this;
}

Viewer::~Viewer() {
#ifndef WIN32
    if (_thread != NULL) {
	pthread_kill(*(pthread_t*)_thread, SIGKILL);

    }
#endif
}


int Viewer::AddWindow(const char *title, int width, int height, int xpos, int ypos) {
	windows.push_back(new Window(title, xpos, ypos, width, height));
	return ((int)windows.size()-1);
}

void Viewer::MakeWindow(int win)
{
    Window &w = *windows[win];
    
    glutInitWindowSize(w.iwidth, w.iheight);
    glutInitWindowPosition(w.ixpos, w.iypos);
    w.glut_id = glutCreateWindow(w.title);

    // register callbacks
    glutDisplayFunc(s_gc_display);
    glutReshapeFunc(s_gc_reshape);
    glutKeyboardFunc(s_gc_keyboard);
    glutMouseFunc(s_gc_mouse);
    glutMotionFunc(s_gc_motion);
    glutPassiveMotionFunc(s_gc_passive_motion);
    glutIdleFunc(s_gc_idle);
    
    Init(win);
}


void Viewer::Redraw(int win) {
	int begin=(win<0) ? 0 : win;
	int end = (win<0) ? (int)windows.size() : win+1;
	
	for (int i=begin; i<end; i++) {
		windows[i]->cs.enter();
		windows[i]->redraw=true;
		windows[i]->cs.leave();
	}
}


void Viewer::ToggleImmediateMode(int win) {

	int begin=(win<0) ? 0 : win;
	int end = (win<0) ? (int)windows.size() : win+1;
	
	for (int i=begin; i<end; i++) {
		windows[i]->cs.enter();
		windows[i]->immediatemode = !windows[i]->immediatemode;
		windows[i]->redraw=true;
		windows[i]->cs.leave();
	}
}


void Viewer::ToggleOrtho(int win) {

	int begin=(win<0) ? 0 : win;
	int end = (win<0) ? (int)windows.size() : win+1;
	
	for (int i=begin; i<end; i++) {
		windows[i]->cs.enter();
		windows[i]->ortho = !windows[i]->ortho;
		windows[i]->redraw=true;
		windows[i]->cs.leave();

		if (windows[i]->sync_cam_out)
			SaveViewInfo(stdout, i);
	}
}


void Viewer::WaitForKey(int key, int win) {

	input_cs.enter();
	last_key=-1;
	input_cs.leave();


	while (1) {

		thlib::Thread::yield();
		input_cs.enter();
		if (last_key >= 0) {	// key was hit
			if ((win<0 || win==last_key_win) && // want any win, or the win matches
				(key<=0 || key==last_key)) {		// want any key, or the key matches
				input_cs.leave();
				Redraw(win);
			    break;
			}
		}
		input_cs.leave();
	}

}


void Viewer::WaitForKeyOrClick(int key, int kwin, int button, int bwin) {

	input_cs.enter();
	last_key=-1;
	input_cs.leave();


	while (1) {

		thlib::Thread::yield();
		input_cs.enter();
		if (last_key >= 0) {	// key was hit
			if ((kwin<0 || kwin==last_key_win) && // want any win, or the win matches
				(key<=0 || key==last_key)) {		// want any key, or the key matches
				input_cs.leave();
				Redraw(kwin);
			    break;
			}
		}

		if (last_click_button >= 0) { // mouse was clicked
			if ((bwin<0 || bwin==last_click_win) &&
				(button<0 || button==last_click_button)) {
				input_cs.leave();
				Redraw(bwin);
			    break;
			}
		}


		input_cs.leave();
	}

}


int Viewer::LastKey() {

	input_cs.enter();

	int ret = last_key;
	last_key = -1;

	input_cs.leave();

	return ret;
}


void Viewer::SetCamSyncIn(int win) {
	int begin=(win<0) ? 0 : win;
	int end = (win<0) ? (int)windows.size() : win+1;
	
	for (int i=begin; i<end; i++) {
		windows[i]->cs.enter();
		windows[i]->sync_cam_in=true;
		windows[i]->cs.leave();
	}

	if (!cam_thread) {
		cam_thread = new thlib::Thread(cam_stub, NULL, 0);
	}

}

void Viewer::SetCamSyncOut(int win) {
	int begin=(win<0) ? 0 : win;
	int end = (win<0) ? (int)windows.size() : win+1;
	
	for (int i=begin; i<end; i++) {
		windows[i]->cs.enter();
		windows[i]->sync_cam_out=true;
		windows[i]->cs.leave();
	}
}


bool Viewer::SaveViewInfo(char *filename, int win) {
	FILE *f = fopen(filename, "w");
	if (!f) return false;
	SaveViewInfo(f, win);
	fclose(f);
	return true;
}

void Viewer::SaveViewInfo(FILE *f, int win) {

	Window &w = *windows[win];
	w.cs.enter();
	fprintf(f, "pan: %f %f %f\n", w.pan_x, w.pan_y, w.pan_z);
	fprintf(f, "dolly: %f\n", w.dolly);
	fprintf(f, "quat: %f %f %f %f\n", w.quat[0], w.quat[1], w.quat[2], w.quat[3]);
	fprintf(f, "ortho: %i\n", w.ortho);
	fflush(f);
	w.cs.leave();
}


bool Viewer::LoadViewInfo(char *filename, int win) {

	FILE *f = fopen(filename, "r");
	if (!f) return false;

	if (!LoadViewInfo(f, win)) {
		fclose(f);
		return false;
	}
	fclose(f);
	return true;
}


bool Viewer::LoadViewInfo(FILE *f, int win) {

	float pan[3];
	float d;
	float quat[4];
	bool o;

	if (!ReadViewInfo(f, pan, d, quat, o)) return false;
	LoadViewInfo(pan, d, quat, o, win);

	return true;
}

bool Viewer::ReadViewInfo(FILE *f, float pan[3], float &dolly, float quat[4], bool &ortho) {
	char line[1024];

	if (!fgets(line, 1000, f)) return false;
	if (3 != sscanf(line, "pan: %f %f %f", &pan[0], &pan[1], &pan[2])) {
		std::cerr<<"ReadViewInfo: couldn't parse camera sting: "<<line<<std::endl;
		return false;
	}

	if (!fgets(line, 1000, f)) return false;
	if (1 != sscanf(line, "dolly: %f", &dolly)) {
		std::cerr<<"ReadViewInfo: couldn't parse camera sting: "<<line<<std::endl;
		return false;
	}

	if (!fgets(line, 1000, f)) return false;
	if (4 != sscanf(line, "quat: %f %f %f %f", &quat[0], &quat[1], &quat[2], &quat[3])) {
		std::cerr<<"ReadViewInfo: couldn't parse camera sting: "<<line<<std::endl;
		return false;
	}

	if (!fgets(line, 1000, f)) return false;
	if (1 != sscanf(line, "ortho: %i", &ortho)) {
		std::cerr<<"ReadViewInfo: couldn't parse camera sting: "<<line<<std::endl;
		return false;
	}

	return true;
}


void Viewer::LoadViewInfo(float pan[3], float dolly, float quat[4], bool ortho, int win) {
	Window &w = *windows[win];
	w.cs.enter();

	w.pan_x = pan[0];
	w.pan_y = pan[1];
	w.pan_z = pan[2];

	w.dolly = dolly;

	w.quat[0] = quat[0];
	w.quat[1] = quat[1];
	w.quat[2] = quat[2];
	w.quat[3] = quat[3];

	w.ortho = ortho;

	w.redraw = true;
	w.cs.leave();
}


void Viewer::ViewAll(int win) {
    int begin = (win<0) ? 0 : win;
    int end = (win<0) ? (int)windows.size() : win+1;

	for (int i=begin; i<end; ++i)  {
		Window &w = *windows[i];

		float x,y,z,r;
		BoundingSphere(i, x,y,z,r);

		w.cs.enter();
		w.dolly = 2*r;
		w.pan_x = -x;
		w.pan_y = -y;
		w.pan_z = -z;
		w.quat[3]=1;
		w.quat[0]=w.quat[1]=w.quat[2] = 0;
		w.cs.leave();

		Redraw(i);

		if (w.sync_cam_out)
			SaveViewInfo(stdout, i);
	}

}

void Viewer::LookAt(int win, float x, float y, float z) {
	Window &w = *windows[win];
	w.cs.enter();
	w.pan_x = -x;
	w.pan_y = -y;
	w.pan_z = -z;
	w.cs.leave();

	if (w.sync_cam_out)
		SaveViewInfo(stdout, win);
}

void Viewer::SetMessage(int win, const char *msg) {
	Window &w = *windows[win];

	w.cs.enter();
	if (!msg) {
		w.status_msg[0] = 0;
	} else {
		strcpy(w.status_msg, msg);
	}
	w.cs.leave();
}


int Viewer::GetCurrentWindowIndex() {
	int id = glutGetWindow();

	for (int i=0; i<(int)windows.size(); i++) {
		if (windows[i]->glut_id == id)
			return i;
	}

	std::cerr<<"unknown current window, exiting!!"<<std::endl;
	exit(-1);
	return -1;
}


void Viewer::s_gc_display() {
	theViewer->gc_display();
}
void Viewer::gc_display() {
	int win = GetCurrentWindowIndex();
	Window &w = *windows[win];

	w.cs.enter();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use peterson's algorithm to protect the status message
	glColor3f(0,0,0);
	glDisable(GL_LIGHTING);

	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,w.width,0,w.height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRasterPos3f(35,75,0);

		for(char *c=w.status_msg; (*c && *c!='\n'); c++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*c);
	}


    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w.ortho) {
		glOrtho(-(w.dolly), (w.dolly), -(GLfloat)w.height / (GLfloat)w.width*(w.dolly), (GLfloat)w.height / (GLfloat)w.width*(w.dolly), fabs((-w.pan_z+w.dolly))/100, (-w.pan_z + w.dolly+2)*2);
	} else {
		gluPerspective(45, (GLfloat)w.width / (GLfloat)w.height, fabs((-w.pan_z+w.dolly))/100, (-w.pan_z + w.dolly+2)*2);
	}


    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float light_dir[4] = { 0.2f, 0.2f, 1, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_dir);

	glTranslatef(0,0,-w.dolly);
  	GLfloat m[4][4];
	build_rotmatrix(m, w.quat);
	glMultMatrixf(&m[0][0]);
	glTranslatef(w.pan_x, w.pan_y, w.pan_z);


// 	if (w.immediatemode) {
		Draw(win);
		w.redraw=false;
// 	} else {
// 		if (w.redraw || w.display_list<0) {
// 			if (w.display_list<0)
// 				w.display_list = glGenLists(1);
// 			w.redraw=false;

// 			glNewList(w.display_list, GL_COMPILE_AND_EXECUTE);
// 			Draw(win);
// 			glEndList();
// 		} else {
// 			glCallList(w.display_list);
// 		}
// 	}

	w.cs.leave();

	glutSwapBuffers();
}


void Viewer::s_gc_reshape(int width, int height) {
	theViewer->gc_reshape(width, height);
}
void Viewer::gc_reshape(int width, int height) {
	int win = GetCurrentWindowIndex();
	windows[win]->cs.enter();
	windows[win]->width = width;
	windows[win]->height= height;
	windows[win]->cs.leave();
	glViewport(0, 0, width, height);
	glutPostRedisplay();
}


void Viewer::s_gc_keyboard(unsigned char key, int x, int y) {
	theViewer->gc_keyboard(key, x, y);
}
void Viewer::gc_keyboard(unsigned char key, int x, int y) {
	int win = GetCurrentWindowIndex();
	Key(win, key);
	input_cs.enter();
	last_key = key;
	last_key_win = win;
	input_cs.leave();
	glutPostRedisplay();
}


void Viewer::s_gc_mouse(int button, int state, int x, int y) {
	theViewer->gc_mouse(button, state, x, y);
}
void Viewer::gc_mouse(int button, int state, int x, int y) {

	int win = GetCurrentWindowIndex();
	Window &w = *windows[win];

	w.cs.enter();

	w.mouse_x = x;
	w.mouse_y = y;

	double objx, objy, objz;
	float depth; 
	{
		GLdouble model[16], proj[16];
		GLint view[4];

		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);

		glReadBuffer(GL_FRONT);
		glReadPixels(x,w.height-y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,(void*)&depth);
		gluUnProject((GLdouble)x, (GLdouble)(w.height-y), depth, model, proj, view, &objx, &objy, &objz);
		if(depth>.9999) objz = 1e34;
	}


	// all we handle here is the left button
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			w.pan_depth = depth;
//			std::cerr<<depth<<std::endl;
			if (w.pan_depth==1) w.pan_depth=0.999;

			int modifiers = glutGetModifiers();

			if (modifiers == GLUT_ACTIVE_SHIFT || modifiers == GLUT_ACTIVE_CTRL || modifiers == GLUT_ACTIVE_SUPER)
				w.dollying = true;  // Zoom: Shift, Ctrl, or Command (macOS)
			else if (modifiers == 0)
				w.tracking = true;
			else if (modifiers == GLUT_ACTIVE_ALT)
				w.panning = true;
		}

		if (state == GLUT_UP) {
			w.tracking=w.panning=w.dollying=false;
		}
	}


	if (state==GLUT_DOWN)
		w.mouse_button = button;
	else
		w.mouse_button = -1;

	w.cs.leave();

	if (state == GLUT_DOWN) {
		input_cs.enter();
		last_click_point[0] = objx;
		last_click_point[1] = objy;
		last_click_point[2] = objz;
		last_click_win = win;
		last_click_button = button;
		input_cs.leave();

	}

	// pass all clicks on
	Mouse(win, button, (state==GLUT_DOWN)?-1:1, x, y, depth, objx, objy, objz);

	if (w.sync_cam_out)
		SaveViewInfo(stdout, win);

	glutPostRedisplay();
}


void Viewer::s_gc_motion(int x, int y) {
	theViewer->gc_motion(x,y);
}
float Viewer::get_object_space(int x, int y, double &objx, double &objy, double &objz) 
{
	int win = GetCurrentWindowIndex();
	Window &w = *windows[win];
	w.cs.enter();
	float depth; 
	{
		GLdouble model[16], proj[16];
		GLint view[4];

		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);

		glReadBuffer(GL_FRONT);
		glReadPixels(x,w.height-y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,(void*)&depth);
		gluUnProject((GLdouble)x, (GLdouble)(w.height-y), depth, model, proj, view, &objx, &objy, &objz);
		if(depth>.9999) objz = 1e34;
	}
	w.cs.leave();
	return depth;
}

void Viewer::s_gc_passive_motion(int x, int y)
{
    theViewer->gc_passive_motion(x, y);
}

void Viewer::gc_passive_motion(int x, int y)
{
	int win = GetCurrentWindowIndex();

	double objx, objy, objz;
	get_object_space(x, y, objx, objy, objz);

	PassiveMotion(win, x, y, objx, objy, objz);
}


void Viewer::gc_motion(int x, int y) {

	int win = GetCurrentWindowIndex();
	Window &w = *windows[win];

	double objx, objy, objz;
	float depth = get_object_space(x, y, objx, objy, objz);

	if (w.dollying) {
		w.dolly *= exp((double)(w.mouse_y-y) * 2. / w.height);
	}


	w.cs.enter();

	if (w.panning) {
		GLdouble model[16], proj[16];
		GLint view[4];
		glGetDoublev(GL_MODELVIEW_MATRIX, model);
                glGetDoublev(GL_PROJECTION_MATRIX, proj);
                glGetIntegerv(GL_VIEWPORT, view);

		GLdouble ox, oy, oz;
		GLdouble nx, ny, nz;
		gluUnProject((GLdouble)w.mouse_x, (GLdouble)(w.height-w.mouse_y), w.pan_depth, model, proj, view, &ox, &oy, &oz);
		gluUnProject((GLdouble)x,		  (GLdouble)(w.height-y),		  w.pan_depth, model, proj, view, &nx, &ny, &nz);
		w.pan_x += nx-ox;		w.pan_y += ny-oy;		w.pan_z += nz-oz;
	}


	if (w.tracking) {

		float quat[4];
		trackball(quat, (2.0 * w.mouse_x - w.width) / w.width,
						(w.height - 2.0 * w.mouse_y) / w.height,
						(2.0 * x - w.width) / w.width,
						(w.height- 2.0 * y) / w.height);

		add_quats(quat, w.quat, w.quat);
	}


	w.mouse_x = x;
	w.mouse_y = y;



	// pass all it on
	Mouse(win, w.mouse_button, 0, x, y, depth, objx, objy, objz);

	w.cs.leave();

	if (w.sync_cam_out)
		SaveViewInfo(stdout, win);


	glutPostRedisplay();
}


void Viewer::s_gc_idle() {
	theViewer->gc_idle();
}
void Viewer::gc_idle() {

	bool didsomething=false;

	for (unsigned i=0; i<windows.size(); i++) {
		windows[i]->cs.enter();
		if (windows[i]->redraw) {
			didsomething=true;
			glutPostWindowRedisplay(windows[i]->glut_id);
		}
		windows[i]->cs.leave();
	}

	if (!didsomething)
		thlib::Thread::yield();
}










