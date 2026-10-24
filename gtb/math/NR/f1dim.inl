
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
