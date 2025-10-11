#include "Sem.h"

#include <iostream>
#include <cassert>
using namespace std;

#ifdef WIN32
#endif

#ifdef __gnu_linux_
Semaphore::Semaphore(int initial_value)
{
    if (sem_init(&_semaphore, 0, initial_value)) {
	cerr << "Semaphore::Semaphore(): sem_init failed" << endl;
	assert(0);
    };
}

Semaphore::~Semaphore()
{
    if (sem_destroy(&_semaphore)) {
	cerr << "Semaphore::~Semaphore(): sem_destroy failed" << endl;
	assert(0);
    };
}

void Semaphore::v()
{
    if (sem_post(&_semaphore)) {
	cerr << "Semaphore::v(): sem_post failed" << endl;
	assert(0);
    }
}

void Semaphore::p()
{
    if (sem_wait(&_semaphore)) {
	cerr << "Semaphore::p(): sem_wait failed" << endl;
	assert(0);
    }
}
#endif

#ifdef WIN32
Semaphore::Semaphore(int initial_value)
{
    _semaphore = CreateSemaphore(NULL, initial_value, 20000000000, NULL);
}

Semaphore::~Semaphore()
{
    CloseHandle(_semaphore);
}

void Semaphore::v()
{
    ReleaseSemaphore(_semaphore);
}

void Semaphore::p()
{
    WaitForSingleObject(_semaphore, INFINITE);
}
#endif
