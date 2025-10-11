// Ultra-minimal GLUT test - just verify window creation works on macOS
#include <viewer/viewer.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <iostream>

using namespace std;

class SimpleViewer : public Viewer {
public:
    SimpleViewer() : Viewer() {
        AddWindow("GLUT Test - Window Opens Successfully!", 800, 600);
    }

    void Init(int win) {
        glClearColor(0.2, 0.3, 0.4, 1.0);
    }

    void Draw(int win) {
        // Draw a simple white triangle to show it's working
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_TRIANGLES);
        glVertex3f(-0.5, -0.5, 0);
        glVertex3f( 0.5, -0.5, 0);
        glVertex3f( 0.0,  0.5, 0);
        glEnd();
    }

    void Key(int win, unsigned char key) {
        if (key == 'q' || key == 27) {
            cout << "Quitting..." << endl;
            exit(0);
        }
        cout << "Key pressed: " << key << endl;
    }

    void Mouse(int win, int button, int motion, int wx, int wy, float depth, float x, float y, float z) {}
    void PassiveMotion(int win, int wx, int wy, float x, float y, float z) {}
    void BoundingSphere(int win, float &x, float &y, float &z, float &r) {
        x = y = z = 0;
        r = 1;
    }
};

int main(int argc, char *argv[]) {
    cout << "========================================" << endl;
    cout << "GLUT Threading Test" << endl;
    cout << "========================================" << endl;
    cout << "This tests if GLUT can initialize and" << endl;
    cout << "run on the main thread successfully." << endl;
    cout << endl;
    cout << "If a window opens with a white triangle," << endl;
    cout << "the GLUT threading is FIXED!" << endl;
    cout << endl;
    cout << "Press 'q' or ESC to quit" << endl;
    cout << "========================================" << endl;

    SimpleViewer *viewer = new SimpleViewer();

    cout << "Initializing GLUT on main thread..." << endl;
    viewer->Start();

    cout << "Starting GLUT main loop on main thread..." << endl;
    cout << "(This should never return)" << endl;

    // This MUST run on main thread on macOS
    viewer->Go();

    return 0;
}
