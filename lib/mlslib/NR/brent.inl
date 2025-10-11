#include <math.h>
#define NRANSI
#include "nrutil.h"
#define ITMAX 100
#define CGOLD 0.3819660
//#define ZEPS 1.0e-10
#define EPS 3.0e-8
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d)

template<class REAL> REAL ZEPS();
template<> inline float ZEPS() { return 1e-10f; }
template<> inline double ZEPS() { return 1e-12; }


template <class REAL, class F>
REAL brent(
	REAL ax, REAL bx, REAL cx, const F& f, REAL tol, REAL *xmin, 
	REAL ifx	// Shachar, initial value of f(bx);
	)
{
    int iter;
    REAL a,b,d=0.0,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
    REAL e=0.0;

    a=(ax < cx ? ax : cx);
    b=(ax > cx ? ax : cx);
    x=w=v=bx;

    fw=fv=fx=ifx;

    for (iter=1;iter<=ITMAX;iter++) 
    {
        xm=0.5*(a+b);
        tol2=2.0*(tol1=tol*gtb::absv(x)+ZEPS<REAL>());
        if (gtb::absv(x-xm) <= (tol2-0.5*(b-a))) 
        {
            *xmin=x;
            return fx;
        }
        if (gtb::absv(e) > tol1) 
        {
            r=(x-w)*(fx-fv);
            q=(x-v)*(fx-fw);
            p=(x-v)*q-(x-w)*r;
            q=2.0*(q-r);
            if (q > 0.0) p = -p;
            q=gtb::absv(q);
            etemp=e;
            e=d;
            if (gtb::absv(p) >= gtb::absv(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
                d=CGOLD*(e=(x >= xm ? a-x : b-x));
            else {
                d=p/q;
                u=x+d;
                if (u-a < tol2 || b-u < tol2)
                    d=SIGN(tol1,xm-x);
            }
        } else {
            d=CGOLD*(e=(x >= xm ? a-x : b-x));
        }
        u=(gtb::absv(d) >= tol1 ? x+d : x+SIGN(tol1,d));

#ifdef DBG_NR_EVAL
        printf("b: "); // Debug for analyzing who evaluates the function so many times.
#endif
        fu=f(u);
        if (fu <= fx) {
            if (u >= x) a=x; else b=x;
            SHFT(v,w,x,u);
            SHFT(fv,fw,fx,fu);
        } else {
            if (u < x) a=u; else b=u;
            if (fu <= fw || w == x) {
                v=w;
                w=u;
                fv=fw;
                fw=fu;
            } else if (fu <= fv || v == x || v == w) {
                v=u;
                fv=fu;
            }
        }
    }
    nrerror("Too many iterations in brent");
    *xmin=x;
    return fx;
}


//Using Brent’s method, find the root of a function func known to lie between x1 and x2. The
//root, returned as zbrent, will be refined until its accuracy is tol.
template <typename REAL, typename F>
REAL zbrent(F &f, REAL x1, REAL x2, REAL tol)
{
	int iter;
	REAL a=x1,b=x2,c=x2,d,e,min1,min2;
	REAL fa=f(a),fb=f(b),fc,p,q,r,s,tol1,xm;
	if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0))
		nrerror("Root must be bracketed in zbrent");
	fc=fb;
	for (iter=1;iter<=ITMAX;iter++) {
		if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
			c=a; //Rename a, b, c and adjust bounding interval d.
			fc=fa;
				e=d=b-a;
		}
		if (fabs(fc) < fabs(fb)) {
			a=b;
			b=c;
			c=a;
			fa=fb;
			fb=fc;
			fc=fa;
		}
		tol1=2.0*EPS*fabs(b)+0.5*tol; // Convergence check.
		xm=0.5*(c-b);
		if (fabs(xm) <= tol1 || fb == 0.0) return b;
		if (fabs(e) >= tol1 && fabs(fa) > fabs(fb)) {
			s=fb/fa; // Attempt inverse quadratic interpolation.
			if (a == c) {
				p=2.0*xm*s;
				q=1.0-s;
			} else {
				q=fa/fc;
				r=fb/fc;
				p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
				q=(q-1.0)*(r-1.0)*(s-1.0);
			}
			if (p > 0.0) q = -q; // Check whether in bounds.
			p=fabs(p);
			min1=3.0*xm*q-fabs(tol1*q);
			min2=fabs(e*q);
			if (2.0*p < (min1 < min2 ? min1 : min2)) {
				e=d; // Accept interpolation.
				d=p/q;
			} else {
				d=xm; // Interpolation failed, use bisection.
				e=d;
			}
		} else { // Bounds decreasing too slowly, use bisection.
			d=xm;
			e=d;
		}
		a=b; // Move last best guess to a.
		fa=fb;
		if (fabs(d) > tol1) // Evaluate new trial root.
			b += d;
		else
			b += SIGN(tol1,xm);
		fb=f(b);
	}
	nrerror("Maximum number of iterations exceeded in zbrent");
	return 0.0; // Never get here.
}



#undef ITMAX
#undef CGOLD
//#undef ZEPS
#undef EPS
#undef SHFT
#undef NRANSI
/* (C) Copr. 1986-92 Numerical Recipes Software '>'!^,. */
