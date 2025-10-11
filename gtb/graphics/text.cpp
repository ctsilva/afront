#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/text.hpp>
#include <gtb/graphics/ogltools.h>
#include <stdio.h>
#endif // WIN32


GTB_BEGIN_NAMESPACE



void draw_string(const char *s, int x, int y)
{
	glPushAttrib(GL_CURRENT_BIT);
	glRasterPos2i(x, y);
	while (*s) {
		//glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s);
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s);
		s++;
	}
	glPopAttrib();
}



void draw_int(int n, int x, int y)
{
	char buf[20];
	sprintf(buf, "%d", n);
	draw_string(buf, x, y);
}


void draw_text_begin(int l, int r, int b, int t)
{
	glPushAttrib(GL_ENABLE_BIT | GL_VIEWPORT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glViewport(l, b, r - l, t - b);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(l, r, b, t);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}


void draw_text_end()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}


GTB_END_NAMESPACE
