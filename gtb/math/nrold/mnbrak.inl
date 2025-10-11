#include <math.h>
#define NRANSI
#include "nrutil.h"
#define GOLD 1.618034f
#define GLIMIT 100.0f
#define TINY 1.0e-20f
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d)

template <class F>
void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc, F& func)
{
    double ulim,u,r,q,fu,dum;

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
            (2.0*SIGN(FMAX(fabs(q-r),TINY),q-r));
        ulim=(*bx)+GLIMIT*(*cx-*bx);
        if ((*bx-u)*(u-*cx) > 0.0) {
#ifdef DBG_NR_EVAL
			printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fu=func(u);
            if (!isfinite(fu)) DebugBreak();

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
            if (!isfinite(fu)) DebugBreak();
        } else if ((*cx-u)*(u-ulim) > 0.0) {
#ifdef DBG_NR_EVAL
			printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fu=func(u);
            if (!isfinite(fu)) DebugBreak();
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
            if (!isfinite(fu)) DebugBreak();
        } else {
            u=(*cx)+GOLD*(*cx-*bx);
#ifdef DBG_NR_EVAL
			printf("l: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fu=func(u);
            if (!isfinite(fu)) DebugBreak();
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
