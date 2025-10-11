#include <math.h>
#define NRANSI
#include "nrutil.h"
#define ITMAX 200

/*
 * Powel's multi-dimensional function minimization algorithm
 * commented by Shachar
 *
 * All matrices / vectors are 1..n and not 0..n-1!!!
 *
 * p  - Initial starting point, The result is here
 *                              ------------------
 * xi - initial set of directions. Usually initialized to
 *      the unit basis vector.
 * n  - dimension of the function.
 * ftol - Tolerance, "stop criteria": failure to descrease the
 *        function by more than ftol means we've found the
 *        (local) minimal point.
 * iter - Iterations taken
 * fret - The function value at p.
 * func - (obviously) function to minimize
 *
 * TODO:
 *   change ITMAX to a parameter and return a true/false value for failure.
 */
template <class REAL, class F>
bool powell(REAL p[], REAL **xi, int n, REAL ftol, int *iter, REAL *fret,
            F& func)
{
    //    void linmin(REAL p[], REAL xi[], int n, REAL *fret,REAL (*func)(REAL []));
    int i,ibig,j;
    REAL del,fp,fptt,t,*pt,*ptt,*xit;

    pt=avector<REAL>(1,n);
    ptt=avector<REAL>(1,n);
    xit=avector<REAL>(1,n);
    *fret=func(p);
    for (j=1;j<=n;j++) pt[j]=p[j];
    for (*iter=1;;++(*iter))
    {
        fp=(*fret);
        ibig=0;
        del=0.0;
        for (i=1;i<=n;i++) 
        {
            for (j=1;j<=n;j++) xit[j]=xi[j][i];
            fptt=(*fret);
            linmin(p,xit,n,fret,func);
            if (gtb::absv(fptt-(*fret)) > del)
            {
                del=gtb::absv(fptt-(*fret));
                ibig=i;
            }
        }
        /*
         * Test for end condition
         */
        if (2.0*gtb::absv(fp-(*fret)) <= ftol*(gtb::absv(fp)+gtb::absv(*fret))) {
            free_avector(xit,1,n);
            free_avector(ptt,1,n);
            free_avector(pt,1,n);
            return true;
        }
        if (*iter == ITMAX)
        {
            printf("powell exceeding maximum iterations.\n");
            free_avector(xit,1,n);
            free_avector(ptt,1,n);
            free_avector(pt,1,n);
            return false;
            //nrerror("powell exceeding maximum iterations.");
        }

        for (j=1;j<=n;j++) {
            ptt[j]=2.0*p[j]-pt[j];
            xit[j]=p[j]-pt[j];
            pt[j]=p[j];
        }

        fptt=func(ptt);
        if (fptt < fp) {
            t=2.0*(fp-2.0*(*fret)+fptt)*SQR(fp-(*fret)-del)-del*SQR(fp-fptt);
            if (t < 0.0) {
                linmin(p,xit,n,fret,func);
                for (j=1;j<=n;j++) {
                    xi[j][ibig]=xi[j][n];
                    xi[j][n]=xit[j];
                }
            }
        }
    }
    return false;
}
#undef ITMAX
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
