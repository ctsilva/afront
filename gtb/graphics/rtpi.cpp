#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/rtpi.hpp>
#include <gtb/io/io.hpp>
#include <gtb/math/math.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/rtpi.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


Rtpi::Rtpi()
	: _r(0.0),
	  _t(0.0),
	  _p(0.0),
	  _i(0)
{
}


Rtpi::Rtpi(real_t arg_r, real_t arg_t, real_t arg_p, int arg_i)
	: _r(arg_r),
	  _t(arg_t),
	  _p(arg_p),
	  _i(arg_i)
{
}


Point3 Rtpi::point() const
{
//  	real_t x = _r * cos(_p) * cos(_t);
//  	real_t y = _r * cos(_p) * sin(_t);
//  	real_t z = _r * sin(_p);

	// x points to the right, y points up, and the scanner is
	// looking down the negative z axis.  Theta is the rotation
	// around y, and phi is the rotation around x.
  	real_t x = -_r * cos(_p) * sin(_t);
  	real_t y = _r * sin(_p);
  	real_t z = -_r * cos(_p) * cos(_t);

	return Point3(x, y, z);
}


GTB_END_NAMESPACE
