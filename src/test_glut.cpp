// Minimal GLUT test - just load and display a mesh
// This tests that GLUT initialization works correctly on macOS

#include <gtb/gtb.hpp>
#include <viewer/viewer.h>
#include <iostream>

using namespace std;
using namespace gtb;

TriangleMesh *test_mesh = NULL;

class TestViewer : public Viewer {
public:
    TestViewer() : Viewer() {
        AddWindow("GLUT Test", 800, 600);
    }

    void Init(int win) {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glEnable(GL_DEPTH_TEST);
    }

    void Draw(int win) {
        if (!test_mesh) return;

        // Simple wireframe rendering
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        for (unsigned i = 0; i < test_mesh->num_faces(); i++) {
            const IndexedTriangle &f = (*test_mesh)[i];
            for (int e = 0; e < 3; e++) {
                int v0 = f.vertex_index(e);
                int v1 = f.vertex_index((e+1)%3);
                const Point3 &p0 = test_mesh->vertex(v0);
                const Point3 &p1 = test_mesh->vertex(v1);
                glVertex3f(p0[0], p0[1], p0[2]);
                glVertex3f(p1[0], p1[1], p1[2]);
            }
        }
        glEnd();
    }

    void Key(int win, unsigned char key) {
        if (key == 'q' || key == 27) { // q or ESC
            cout << "Quitting..." << endl;
            exit(0);
        }
    }

    void Mouse(int win, int button, int motion, int wx, int wy, float depth, float x, float y, float z) {
        // Empty - required by interface
    }

    void PassiveMotion(int win, int wx, int wy, float x, float y, float z) {
        // Empty - required by interface
    }

    void BoundingSphere(int win, float &x, float &y, float &z, float &r) {
        if (!test_mesh || test_mesh->num_vertices() == 0) {
            x = y = z = 0;
            r = 1;
            return;
        }
        Box3 bbox = test_mesh->bounding_box();
        Point3 min_pt = bbox.min_point();
        Point3 max_pt = bbox.max_point();
        // Calculate center manually
        x = (min_pt[0] + max_pt[0]) * 0.5;
        y = (min_pt[1] + max_pt[1]) * 0.5;
        z = (min_pt[2] + max_pt[2]) * 0.5;
        r = bbox.diagonal_length() / 2.0;
    }
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <mesh.off>" << endl;
        return 1;
    }

    cout << "Loading mesh: " << argv[1] << endl;

    // Load mesh
    test_mesh = new TriangleMesh();
    if (!test_mesh->Read(argv[1])) {
        cerr << "Failed to load mesh" << endl;
        return 1;
    }

    cout << "Mesh loaded: " << test_mesh->num_vertices() << " vertices, "
         << test_mesh->num_faces() << " faces" << endl;

    // Create GUI
    TestViewer *viewer = new TestViewer();
    viewer->Start();

    cout << "Starting GLUT main loop on main thread..." << endl;
    cout << "Press 'q' or ESC to quit" << endl;

    // Run GLUT on main thread - this never returns
    viewer->Go();

    return 0;
}
