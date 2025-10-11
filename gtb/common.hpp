#ifndef GTB_NAMESPACE_INCLUDED
#define GTB_NAMESPACE_INCLUDED

#define GTB_BEGIN_NAMESPACE namespace gtb {
#define GTB_END_NAMESPACE }

#ifdef WIN32
#include <gtb/windows.hpp>
#else
#include <gtb/unix.hpp>
#endif

#ifdef WIN32
#define BREAK __asm int 3
#else
#include <signal.h>
#include <unistd.h>
#define BREAK kill(getpid(), SIGINT)
#endif

#if defined(REAL_IS_FLOAT)
  #define GTB_GENERATE_STD_TYPEDEF(c) typedef c##f c
#else
  #define GTB_GENERATE_STD_TYPEDEF(c) typedef c##d c
#endif

#define GTB_GENERATE_CLASS_TYPEDEFS(c)		\
typedef t##c<float> c##f;			\
typedef t##c<double> c##d;			\
GTB_GENERATE_STD_TYPEDEF(c);

#endif // GTB_NAMESPACE_INCLUDED
