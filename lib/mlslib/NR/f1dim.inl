#define NRANSI
#include "nrutil.h"

//extern double *pcom,*xicom;

template <class REAL, class F>
struct Cf1dim
{
    const F& nrfunc;
    int ncom;
    REAL *pcom, *xicom;
    REAL *xt;

    Cf1dim(const F& fop, int n, REAL* pc, REAL* xi, REAL* _xt) : 
        nrfunc(fop), ncom(n), pcom(pc), xicom(xi), xt(_xt) {}

    REAL operator() (REAL x) const
    {
        int j;
        REAL f;

        for (j=1;j<=ncom;j++) xt[j]=pcom[j]+x*xicom[j];
        f=nrfunc(xt);
        return f;
    }
};

template <class REAL, class F> 
Cf1dim<REAL, F> Gen_f1dim(const F& fop, int ncom, REAL* pcom, REAL* xicom, REAL* xt)
{
    return Cf1dim<REAL, F>(fop, ncom, pcom, xicom, xt);
}

#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
