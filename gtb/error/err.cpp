#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/error/err.h>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/error/err.inl>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


#if 0
void DbgOut(const std::string& msg)
{
#ifndef NO_ERR_DEBUG_OUTPUT
#ifdef WIN32
  OutputDebugString(msg.c_str());
#else
  std::cerr << msg << std::endl;
#endif // WIN32
#endif // NO_ERR_DEBUG_OUTPUT
}
#endif

GTB_END_NAMESPACE
