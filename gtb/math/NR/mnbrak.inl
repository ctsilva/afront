
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


#include <math.h>
#define NRANSI
#include "nrutil.h"
#define GOLD 1.618034
#define GLIMIT 100.0
#define TINY 1.0e-20
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d)

template <class REAL, class F>
void mnbrak(REAL *ax, REAL *bx, REAL *cx, REAL *fa, REAL *fb, REAL *fc, F& func)
{
    REAL ulim,u,r,q,fu,dum;

#ifdef DBG_NR_EVAL
    printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
    *fa=func(*ax);
#ifdef DBG_NR_EVAL
    printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
    *fb=func(*bx);
    if (*fb > *fa) {
        SHFT(dum,*ax,*bx,dum);
        SHFT(dum,*fb,*fa,dum);
    }

    *cx=(*bx)+GOLD*(*bx-*ax);
#ifdef DBG_NR_EVAL
	printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
    *fc=func(*cx);
    while (*fb > *fc) {

        r=(*bx-*ax)*(*fb-*fc);
        q=(*bx-*cx)*(*fb-*fa);
        u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/
            (2.0*SIGN(FMAX(gtb::absv(q-r),(REAL)TINY),q-r));
        ulim=(*bx)+GLIMIT*(*cx-*bx);
        if ((*bx-u)*(u-*cx) > 0.0) {
#ifdef DBG_NR_EVAL
			printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fu=func(u);
            if (fu < *fc) {
                *ax=(*bx);
                *bx=u;
                *fa=(*fb);
                *fb=fu;
                return;
            } else if (fu > *fb) {
                *cx=u;
                *fc=fu;
                return;
            }
            u=(*cx)+GOLD*(*cx-*bx);
#ifdef DBG_NR_EVAL
			printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fu=func(u);
        } else if ((*cx-u)*(u-ulim) > 0.0) {
#ifdef DBG_NR_EVAL
			printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fu=func(u);
            if (fu < *fc) {
                SHFT(*bx,*cx,u,*cx+GOLD*(*cx-*bx));
#ifdef DBG_NR_EVAL
				printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
                SHFT(*fb,*fc,fu,func(u));
            }
        } else if ((u-ulim)*(ulim-*cx) >= 0.0) {
            u=ulim;
            fu=func(u);
        } else {
            u=(*cx)+GOLD*(*cx-*bx);
#ifdef DBG_NR_EVAL
			printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fu=func(u);
        }
        SHFT(*ax,*bx,*cx,u);
        SHFT(*fa,*fb,*fc,fu);
    }
}
#undef GOLD
#undef GLIMIT
#undef TINY
#undef SHFT
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
