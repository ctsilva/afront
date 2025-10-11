#ifndef __WCLASS_H_777
#define __WCLASS_H_777

#ifdef WIN32

#include <gtb/common.hpp>

/*
 * Q&D hack to open a window that I can draw openGL on
 * Used for offline rendering with glut applications.
 */
GTB_BEGIN_NAMESPACE

class GLWClass
{
public:
    GLWClass(const char* name, int x, int y, int width, int height);
    ~GLWClass();

    void SwapBuffers();
    void PrepareForRendering();
    void Show();
    void Hide();

protected:
    static int lastid;
    char classname[1000];

    HWND hWin;                  // Window handler
    HDC hDC;                    // Device context
    HGLRC hRC;                  // Rendering context

    static LRESULT WINAPI wproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

    void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
    void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
};

GTB_END_NAMESPACE

#endif // WIN32

#endif // __WCLASS_H_777
