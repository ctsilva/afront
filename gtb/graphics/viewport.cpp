#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/viewport.hpp>
#include <gtb/graphics/plane.hpp>
#include <gtb/graphics/polygon3.hpp>
#include <gtb/graphics/ogltools.h>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/viewport.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


Viewport::Viewport()
	: _x_min(0),
	  _y_min(0),
	  _width(512),
	  _height(512)
{
}


Viewport::Viewport(int xmin, int ymin, int w, int h)
	: _x_min(xmin),
	  _y_min(ymin),
	  _width(w),
	  _height(h)
{
}


Viewport::Viewport(const Viewport &vp)
	: _x_min(vp._x_min),
	  _y_min(vp._y_min),
	  _width(vp._width),
	  _height(vp._height)
{
}


GTB_END_NAMESPACE
