#ifndef _VIEWER_H
#define _VIEWER_H

#include <ThreadLib/threadslib.h>
#include <vector>
#include <iostream>

class Viewer {

public:

	Viewer();
    virtual ~Viewer();

	int AddWindow(const char *title, int width=-1, int height=-1, int xpos=-1, int ypos=-1);
	void MakeWindow(int win);
	void Start();


	// the main functionality for the user to add - drawing and input handling
	virtual void Init(int win) = 0;
	virtual void Draw(int win) = 0;
	virtual void Key(int win, unsigned char key) = 0;
	virtual void Mouse(int win, int button, int motion, int wx, int wy, float depth, float x, float y, float z) = 0;
        virtual void PassiveMotion(int win, int wx, int wy, float x, float y, float z) = 0;
	virtual void BoundingSphere(int win, float &x, float &y, float &z, float &r) = 0;

	void Redraw(int win=-1);
	void ToggleImmediateMode(int win=-1);
	void ToggleOrtho(int win=-1);
	void WaitForKey(int key=0,int win=-1);
	void WaitForKeyOrClick(int key=0, int kwin=-1, int button=-1, int bwin=-1);
	int LastKey();
	void SetCamSyncIn(int win=-1);
	void SetCamSyncOut(int win=-1);
	bool SaveViewInfo(char *filename, int win=0);
	bool LoadViewInfo(char *filename, int win=0);



	template <typename T>	void LastClick(T &x, T &y, T &z, int &win, int &button) {
		input_cs.enter();
		x = (T)last_click_point[0];
		y = (T)last_click_point[1];
		z = (T)last_click_point[2];
		win=last_click_win;
		button = last_click_button;
		last_click_win = -1;
		last_click_button=-1;
		input_cs.leave();
	}



	void ViewAll(int win=-1);
	void LookAt(int win, float x, float y, float z);
	void SetMessage(int win, const char *msg);

        float get_object_space(int x, int y, double &objx, double &objy, double &objz);

	int WindowWidth(int win) { return windows[win]->width; }
	int WindowHeight(int win) { return windows[win]->height; }

    thlib::Thread *Thread() { return _thread; }

    static Viewer *GetViewer() { return theViewer; };
    thlib::CSObject &get_critical_section() {
	std::cerr << "Old interface, please switch to CamelCase naming" << std::endl;
	return _critical_section;
    };
    thlib::CSObject &GetCriticalSection() { return _critical_section; };

    // Run GLUT main loop - must be called on main thread on macOS
    // Never returns!
    void Go();

protected:
    thlib::CSObject _critical_section;

private:

	// all the glut callbacks, use theViewer to hook back into the object
	static Viewer *theViewer;
	static void s_gc_display();
	static void s_gc_reshape(int width, int height);
	static void s_gc_keyboard(unsigned char key, int x, int y);
	static void s_gc_mouse(int button, int state, int x, int y);
	static void s_gc_motion(int x, int y);
	static void s_gc_passive_motion(int x, int y);
	static void s_gc_idle();
	void gc_display();
	void gc_reshape(int width, int height);
	void gc_keyboard(unsigned char key, int x, int y);
	void gc_mouse(int button, int state, int x, int y);
	void gc_motion(int x, int y);
	void gc_passive_motion(int x, int y);
	void gc_idle();


    static void *start_stub(void *arg);
    thlib::Thread *_thread;

	static void *cam_stub(void *arg);
	void CamSyncLoop();
    thlib::Thread *cam_thread;
	void SaveViewInfo(FILE *f, int win=0);
	bool LoadViewInfo(FILE *f, int win=0);
	bool ReadViewInfo(FILE *f, float pan[3], float &dolly, float quat[4], bool &ortho);
	void LoadViewInfo(float pan[3], float dolly, float quat[4], bool ortho, int win);

    static volatile bool glut_has_initialized;


    thlib::CSObject input_cs;	// protect the last_key/last_click stuff
	volatile int last_key;
	volatile int last_key_win;
	volatile double last_click_point[3];
	volatile int last_click_win;
	volatile int last_click_button;



	class Window {
	public:

		Window(const char *_title, int _ixpos, int _iypos, int _iwidth, int _iheight) :
		  cs(),
		  ixpos(_ixpos), iypos(_iypos), iwidth(_iwidth), iheight(_iheight),
		  glut_id(-1), ortho(false), immediatemode(false), redraw(true), display_list(-1), mouse_x(0), mouse_y(0), mouse_button(-1), width(0), height(0),
		   panning(false), pan_depth(0), pan_x(0), pan_y(0), pan_z(0), dollying(false), dolly(1), tracking(false), sync_cam_in(false), sync_cam_out(false) {
#ifdef NDEBUG
		    strcpy(title, "");
#else
		    strcpy(title, "[debug] ");
#endif
		    strcat(title, _title);
			quat[3]=1;
			quat[0]=quat[1]=quat[2]=0;
			status_msg[0]=0;
		}

		thlib::CSObject cs;	// protect everything in here


		char title[1024];
		int ixpos, iypos;
		int iwidth, iheight;

		int glut_id;
		bool immediatemode;		// draw with display lists or immediate mode?
		bool ortho;				// orthographic projection
		bool redraw;			// recompile the display list
		int display_list;		// gl display list
		int mouse_x, mouse_y;	// for view manipulation
		int mouse_button;
		int width,height;

		char status_msg[1024];

		bool panning;
		double pan_depth;
		double   pan_x;
		double   pan_y;
		double   pan_z;

		bool dollying;
		double   dolly;

		bool tracking;
		float quat[4];

		bool sync_cam_in;
		bool sync_cam_out;
	};

	std::vector<Window*> windows;

	int GetCurrentWindowIndex();
};

#endif
