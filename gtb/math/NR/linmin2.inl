#define NRANSI
#include "nrutil.h"
//#define TOL (sizeof(REAL)==4? 1.0e-4f : 1e-8)
#define TOL 1e-4

//double *pcom,*xicom,(*nrfunc)(double []);
//double *pcom,*xicom;

//
// Shachar: added initial guess and step size guesses
//
template <class F>
void linmin2(double p[], double xi[], int n, double initial_guess, double stepsize, double *fret, F& func)
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

    ax=initial_guess-stepsize;
    xx=initial_guess+stepsize;

    double* xt=dvector(1,n);
    Cf1dim<double, F> f1dim(Gen_f1dim(func, n, pcom, xicom, xt));
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
