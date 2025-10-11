#define NRANSI
#include "nrutil.h"
#define TOL 2.0e-2f // BUGBUG SHACHAR: originaly 1e-4

//double *pcom,*xicom,(*nrfunc)(double []);
//double *pcom,*xicom;

template <class F>
void linmin(double p[], double xi[], int n, double *fret, F& func)
{
//      double brent(double ax, double bx, double cx, double (*f)(double), double tol, double *xmin);
//      double f1dim(double x, F& func);
//      void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc, F);
    int j;
    double xx,xmin,fx,fb,fa,bx,ax;

    double *pcom,*xicom;

    pcom=dvector(1,n);
    xicom=dvector(1,n);

    for (j=1;j<=n;j++)
    {
        pcom[j]=p[j];
        xicom[j]=xi[j];
    }

    ax=0.0;
    xx=1.0; // BUGBUG: 1.0, this should be data dependent!

    double* xt=dvector(1,n);
    Cf1dim<F> f1dim(Gen_f1dim(func, n, pcom, xicom, xt));
    mnbrak(&ax,&xx,&bx,&fa,&fx,&fb, f1dim);
    *fret=brent(ax,xx,bx, f1dim,TOL,&xmin, fx);
    free_dvector(xt,1,n);

    for (j=1;j<=n;j++)
    {
        xi[j] *= xmin;
        p[j] += xi[j];
    }

    free_dvector(xicom,1,n);
    free_dvector(pcom,1,n);
}
#undef TOL
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
