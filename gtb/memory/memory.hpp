#ifndef __GTB_MEMORY_H
#define __GTB_MEMORY_H

#include "ptrs.h"
#include "counter.h"
#include "timer.h"
#include "hirestimer.h"

#ifdef WIN32
/*
 * Overload the default global new / delete operators
 * to use aligned allocation
 */
void * operator new(size_t size);
void operator delete(void * mem);
void * operator new[](size_t size);
void operator delete[](void * mem);

void start_leak_collection();
void stop_leak_collection();
void dump_leaks();
#endif

#endif // __GTB_MEMORY_H
