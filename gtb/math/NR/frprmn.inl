#include <math.h>
#define NRANSI
#include "nrutil.h"
#define ITMAX 50
#define EPS 1.0e-10
#define FREEALL {free_avector(xi,1,n);free_avector(h,1,n);free_avector(g,1,n);}

/*
 * Congugate gradient minimization from chapter 10-6
 *
 * All matrices / vectors are 1..n and not 0..n-1!!!
 *
 * p  - Initial starting point, The result is here
 *                              ------------------
 * n  - dimension of the function.
 * ftol - Tolerance, "stop criteria": failure to descrease the
 *        function by more than ftol means we've found the
 *        (local) minimal point.
 * iter - (output) Iterations taken
 * fret - The function value at p.
 * func - (obviously) function to minimize
 * dfunc - a function that computes the partial derivatives
 *         first argument - point
 *         second argument - (output) partial derivatives at point
 *
 * TODO:
 *   change ITMAX to a parameter and return a true/false value for failure.
 *
 * bool frprmn(float p[], int n, float ftol, int *iter, float *fret,
 *	float (*func)(float []), void (*dfunc)(float [], float []))
 *
 */
template <class REAL, class F, class DF>
bool frprmn(REAL p[], int n, REAL ftol, int *iter, REAL *fret,
            F& func, DF& dfunc)
{
//	void linmin(REAL p[], REAL xi[], int n, REAL *fret, REAL (*func)(REAL []));
	int j,its;
	REAL gg,gam,fp,dgg;
	REAL *g,*h,*xi;

	g=avector<REAL>(1,n);
	h=avector<REAL>(1,n);
	xi=avector<REAL>(1,n);

#ifdef DBG_NR_EVAL
	printf("f: ");  // Debug for analyzing who evaluates the function so many times.
#endif

    fp=func(p);
	dfunc(p, xi);

    for (j=1;j<=n;j++) 
        {
		g[j] = -xi[j];
		xi[j]=h[j]=g[j];
	}


    for (its=1; its <= ITMAX; its++) 
        {
            *iter=its;
            linmin(p,xi,n,fret,func);
            if ((2.0*gtb::absv(*fret-fp) <= ftol*(gtb::absv(*fret)+gtb::absv(fp)+(REAL)EPS))) 
//                || (gtb::absv(*fret-fp) < (REAL)1e-8)) // HAck added by shachar.
            {
                FREEALL
                return true;
            }
#ifdef DBG_NR_EVAL
	printf("f: ");  // Debug for analyzing who evaluates the function so many times.
#endif
            fp=*fret; // Shachar func(p);
            dfunc(p, xi);
            dgg=gg=0;
            for (j=1;j <= n;j++) {
                gg += g[j]*g[j];
                dgg += (xi[j]+g[j])*xi[j];
            }
            if (gg == 0) {
                FREEALL
        		return false;
            }
            gam=dgg/gg;
            for (j=1; j <= n; j++) {
                g[j] = -xi[j];
                xi[j]=h[j]=g[j]+gam*h[j];
            }
	}
//	nrerror("Too many iterations in frprmn");
        FREEALL
        return false;
}
#undef ITMAX
#undef EPS
#undef FREEALL
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software . */

