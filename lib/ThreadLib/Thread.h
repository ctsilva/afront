#ifndef __THREAD_H
#define __THREAD_H

#include "threadsall.h"

BEGIN_THREADLIB_NAMESPACE

class Thread
{
public:
    Thread(void *(*where_to)(void*), void *parameter, int priority);
    ~Thread();

    void join(void **status_p);
    static void exit(void *status);
    static void yield();
    static void sleep();
    static size_t self();
protected:
    PrimitiveThread _thread;
};

END_THREADLIB_NAMESPACE

#endif
