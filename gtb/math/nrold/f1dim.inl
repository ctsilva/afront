
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
