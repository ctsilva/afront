#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/segment2.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/segment2.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE

template class tSegment2<float>;
template class tSegment2<double>;

GTB_END_NAMESPACE
