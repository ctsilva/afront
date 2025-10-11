#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/math/mynr.h>
#endif // WIN32

GTB_BEGIN_NAMESPACE

static long seed;

void nrseed(long v)
{
    seed = v;
}

long nrran1()
{
    return abs(ran1(&seed));
}

float nrran1f()
{
    return ran1f(&seed);
}

double nrgasdev()
{
    return gasdev(&seed);
}


GTB_END_NAMESPACE
