#ifndef __CRITICALSECTION_H
#define __CRITICALSECTION_H

#include "threadsall.h"

/*
 * Critical section class
 * Sample usage:
 *
 * CSObject list_guard;
 * ...
 * in each thread do:
 *  {
 *     CS cs(list_guard); // Enter the critical section
 *     ...
 *  } // Releases the critical section
 */

BEGIN_THREADLIB_NAMESPACE

//
// A critical section object, used to guard
// an object. Must declare one for the object that
// is being guarded
//
class CSObject
{
public:
    CSObject();
    ~CSObject(); // NOTE: not virtual

    void enter();
    void leave();
private:
    CriticalSection _cs;
};

//
// Auto release critical section class
//
class CS
{
public:
    CS(CSObject& csobject);
    ~CS();

    CSObject& _csobject;
};

END_THREADLIB_NAMESPACE


#endif // __CRITICALSECTION_H
