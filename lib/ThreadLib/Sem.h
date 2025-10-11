#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "threadsall.h"

BEGIN_THREADLIB_NAMESPACE

class Semaphore
{
public:    
    Semaphore(int initial_value);
    ~Semaphore();
    void v();     void post() { v(); };
    void p();     void wait() { p(); };


protected:
    PrimitiveSemaphore _semaphore;
};

END_THREADLIB_NAMESPACE

#endif
