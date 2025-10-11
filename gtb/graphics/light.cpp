#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/light.hpp>
#include <gtb/graphics/ogltools.h>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/light.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


Light::Light(GLenum light)
	: _light(light)
{
}


GTB_END_NAMESPACE
