#ifndef __GTB_META_PROGRAMMING_COMMON_H
#define __GTB_META_PROGRAMMING_COMMON_H

#define GTB_MP_BEGIN_NAMESPACE namespace mp {
#define GTB_MP_END_NAMESPACE }

#ifdef WIN32
#define MP_INTEGER __int64
#else
#define MP_INTEGER long long int
#endif

#endif
