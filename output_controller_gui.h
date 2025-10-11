#ifndef __OUTPUT_CONTROLLER_GUI_H
#define __OUTPUT_CONTROLLER_GUI_H

#include <viewer/viewer.h>

class OutputControllerGui : public Viewer, public OutputController {
private:
	// the draw data must be protected by the critical section,
	// but must also be read only since the work thread reads it outside the critical section!
    //    thlib::CSObject &critical_section;



public:

	OutputControllerGui(const char *fnames);

	// viewer functions that need to be implemented
	void Init(int win);
	void Draw(int win);
	void Key(int win, unsigned char key);
	void Mouse(int win, int button, int motion, int wx, int wy, float depth, float x, float y, float z);
	void BoundingSphere(int win, float &x, float &y, float &z, float &r);
        void PassiveMotion(int win, int wx, int wy, float x, float y, float z);


	// output functions that need to be implemented
    void AddVertex(int index, const Point3 &p, const Vector3 &n, bool boundary);
    void AddTriangle(int index, int v1, int v2, int v3);
    void Finish() { if (child) child->Finish(); }


	void Extract(Triangulator *t);

	bool GetWaitForUser() const { return WaitForUser; }
	void MeshEditMode(TriangleMesh meshes[2]);


private:

	typedef gtb::tMatrix4<double> Matrix4d;	// use doubles because there can be a lot of accumulation

	void DrawMesh(const TriangleMesh &mesh);
	void DrawTetMesh(const TetMesh &mesh);


	int drawMesh;
	bool drawNormals;
	bool drawBoundaries;
	bool drawGuidance;
	bool drawFront;
	bool drawFence;
	int drawOutput;
	bool WaitForUser;
	int drawPointSize;

	volatile int UIEditMesh;
	Matrix4d meshEditTrans;


	IndexedTriangleSet triangulation;	// the triangulation completed so far


	vector< vector<Point3> > drawfronts_p;
	vector< vector<Vector3> > drawfronts_n;
	vector< vector<int> > drawfronts_s;
	vector< vector<real_type> > drawfronts_f;


	Point3 MatPointMult(const Matrix4d &m, const Point3 &p);
};


#endif
