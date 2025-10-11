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
template <class F>
bool powell(double p[], double **xi, int n, double ftol, int *iter, double *fret,
            F& func)
{
    //    void linmin(double p[], double xi[], int n, double *fret,double (*func)(double []));
    int i,ibig,j;
    double del,fp,fptt,t,*pt,*ptt,*xit;

    pt=dvector(1,n);
    ptt=dvector(1,n);
    xit=dvector(1,n);
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
            if (fabs(fptt-(*fret)) > del)
            {
                del=fabs(fptt-(*fret));
                ibig=i;
            }
        }
        /*
         * Test for end condition
         */
        if (2.0*fabs(fp-(*fret)) <= ftol*(fabs(fp)+fabs(*fret))) {
            free_dvector(xit,1,n);
            free_dvector(ptt,1,n);
            free_dvector(pt,1,n);
            return true;
        }
        if (*iter == ITMAX)
        {
            printf("powell exceeding maximum iterations.\n");
            free_dvector(xit,1,n);
            free_dvector(ptt,1,n);
            free_dvector(pt,1,n);
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
    return true;
}
#undef ITMAX
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
