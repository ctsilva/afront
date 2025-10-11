#define NRANSI
#include "nrutil.h"

//extern double *pcom,*xicom;

template <class F>
struct Cf1dim
{
    const F& nrfunc;
    int ncom;
    double *pcom, *xicom;
	double *xt;

    Cf1dim(const F& fop, int n, double* pc, double* xi, double* _xt) : 
        nrfunc(fop), ncom(n), pcom(pc), xicom(xi), xt(_xt) {}

    double operator() (double x) const
    {
        int j;
        double f;

        for (j=1;j<=ncom;j++) xt[j]=pcom[j]+x*xicom[j];
        f=nrfunc(xt);
        return f;
    }
};

template <class F> 
Cf1dim<F> Gen_f1dim(const F& fop, int ncom, double* pcom, double* xicom, double* xt)
{
    return Cf1dim<F>(fop, ncom, pcom, xicom, xt);
}

#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
