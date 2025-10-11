#ifndef __THREADSALL_H
#define __THREADSALL_H

#if defined(__gnu_linux__) || defined(__APPLE__)
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
extern "C"
WINBASEAPI
BOOL
WINAPI
SwitchToThread(
    VOID
    );

#endif

#define BEGIN_THREADLIB_NAMESPACE namespace thlib {
#define END_THREADLIB_NAMESPACE }

BEGIN_THREADLIB_NAMESPACE

#if defined(__gnu_linux__) || defined(__APPLE__)
typedef pthread_mutex_t CriticalSection;
typedef ::sem_t PrimitiveSemaphore;
typedef pthread_t PrimitiveThread;
#endif

#ifdef WIN32
typedef CRITICAL_SECTION CriticalSection;
typedef HANDLE PrimitiveSemaphore;
typedef HANDLE PrimitiveThread;
#endif

END_THREADLIB_NAMESPACE

#endif // __THREADSALL_H
