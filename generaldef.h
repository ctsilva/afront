
#ifndef _GENERALDEF_H__
#define _GENERALDEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/* typedef */

typedef unsigned int     uint;

#ifdef WIN32
typedef unsigned __int64 uint64;
#else
typedef unsigned long long uint64;
#endif
typedef unsigned char    uchar;
typedef unsigned char    ubyte;
typedef float            Tvalue;

/* time */

#ifdef WIN32
#include <time.h>
#else 
#include <time.h>
#endif 

#ifdef WIN32
typedef time_t TTime;
#else
// this is missing on OS X
//typedef struct timeb TTime;   
typedef clock_t TTime;
#endif

void getclock(TTime* value);

double getdiffclock(const TTime* c2,const TTime* c1);

/* general purpouse definition */

#ifndef max2
#define max2(a,b) (((a)>=(b))?(a):(b))
#endif

#ifndef min2
#define min2(a,b) (((a)<=(b))?(a):(b))
#endif

#ifndef min3
#define min3(a,b,c) min2(min2(a,b),min2(b,c))
#endif

#ifndef max3
#define max3(a,b,c) max2(max2(a,b),max2(b,c))
#endif

#ifndef CHECKPOINT
#ifdef _DEBUG
#define CHECKPOINT(cond) assert(cond)
#else
#define CHECKPOINT(cond)
#endif
#endif

#ifndef SWAP
#define SWAP(T,a,b) {T tmp;tmp=(a);(a)=(b);(b)=tmp;}
#endif


//-----------------------------------------------
enum
{
	TYPE_BANDED,
	TYPE_BGRY,
	TYPE_BRY,
	TYPE_GAMMA,
	TYPE_HOT1,
	TYPE_HOT2,
	TYPE_ICE,
	TYPE_LIGHTHUES,
	TYPE_RICH,
	TYPE_SMOOTHRICH,
	TYPE_LUT16,
	TYPE_END
};

#define PALETTESIZE 256

//rgba
void setpalette(int type,int* dest);


#ifdef WIN32
#pragma warning (disable:4018 4244 4267)
#endif



#endif
