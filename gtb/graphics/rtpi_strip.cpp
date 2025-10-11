#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/rtpi_strip.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/rtpi_strip.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


RtpiStrip::RtpiStrip()
{
}


GTB_END_NAMESPACE
