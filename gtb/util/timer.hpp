#ifndef GTB_TIMER_INCLUDED
#define GTB_TIMER_INCLUDED

#include <gtb/common.hpp>
//#include <stdlib.h>

#ifdef WIN32
//#include <winsock2.h>
#else
#include <sys/time.h>
#endif

GTB_BEGIN_NAMESPACE


class timer {
public:
	timer();
	void start();
	void stop();
	double elapsed_seconds() const;
	double elapsed_milliseconds() const;
protected:
	struct timeval m_t1;
	struct timeval m_t2;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/util/timer.ipp>
#endif

#endif // GTB_TIMER_INCLUDED
