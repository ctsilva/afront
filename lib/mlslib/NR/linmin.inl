#define NRANSI
#include "nrutil.h"
//#define TOL (sizeof(REAL)==4? 1.0e-4f : 1e-8)
#define TOL 1e-4

//double *pcom,*xicom,(*nrfunc)(double []);
//double *pcom,*xicom;

template <class REAL, class F>
void linmin(REAL p[], REAL xi[], int n, REAL *fret, F& func)
{
//      REAL brent(REAL ax, REAL bx, REAL cx, REAL (*f)(REAL), REAL tol, REAL *xmin);
//      REAL f1dim(REAL x, F& func);
//      void mnbrak(REAL *ax, REAL *bx, REAL *cx, REAL *fa, REAL *fb, REAL *fc, F);
    int j;
    REAL xx,xmin,fx,fb,fa,bx,ax;

    REAL *pcom,*xicom;

    pcom=avector<REAL>(1,n);
    xicom=avector<REAL>(1,n);

    for (j=1;j<=n;j++)
    {
        pcom[j]=p[j];
        xicom[j]=xi[j];
    }

    ax=(REAL)0;
    xx=(REAL)1;

    REAL* xt=avector<REAL>(1,n);
    Cf1dim<REAL,F> f1dim(Gen_f1dim(func, n, pcom, xicom, xt));
    mnbrak(&ax,&xx,&bx,&fa,&fx,&fb, f1dim);
    *fret=brent(ax,xx,bx, f1dim,(REAL)TOL,&xmin, fx);
    free_avector(xt,1,n);

    for (j=1;j<=n;j++)
    {
        xi[j] *= xmin;
        p[j] += xi[j];
    }

    free_avector(xicom,1,n);
    free_avector(pcom,1,n);
}
#undef TOL
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
