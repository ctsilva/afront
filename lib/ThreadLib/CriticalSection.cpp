
#include "stdafx.h"

#include "CriticalSection.h"


#include <cassert>
#include <iostream>
using namespace std;

BEGIN_THREADLIB_NAMESPACE

#ifdef WIN32
CSObject::CSObject()
{
    InitializeCriticalSection(&_cs);
}

CSObject::~CSObject()
{
    DeleteCriticalSection(&_cs);
}

void CSObject::enter()
{
    EnterCriticalSection(&_cs);
}

void CSObject::leave()
{
    LeaveCriticalSection(&_cs);
}
#endif

#if defined(__gnu_linux__) || defined(__APPLE__)
#include <errno.h>
CSObject::CSObject()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
#ifdef __APPLE__
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
    pthread_mutex_init(&_cs, &attr);
}

CSObject::~CSObject()
{
    pthread_mutex_destroy(&_cs);
}

void CSObject::enter()
{
    if (pthread_mutex_lock(&_cs)) 
    {
	cerr << "CSObject::enter() failed" << endl;
	assert(0);
    };
}

void CSObject::leave()
{
    int error = 0;
    if ((error=pthread_mutex_unlock(&_cs)))
    {
	cerr << "WHHHAAAT????  CSObject::leave() failed" << endl;
	if (error == EINVAL) {
	    cerr << "invalid mutex" << endl;
	}
	if (error == EPERM) {
	    cerr << "not owner" << endl;
	}
	cerr << "error: " << error << flush;
	assert(0);
    };
}
#endif

CS::CS(CSObject& csobject) :
    _csobject(csobject)
{
    _csobject.enter();
}

CS::~CS()
{
    _csobject.leave();
}

END_THREADLIB_NAMESPACE
