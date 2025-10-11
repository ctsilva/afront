#ifndef __POLY2_H
#define __POLY2_H

#include "mlslibdefs.h"
#include "poly.h"

MLSLIB_BEGIN_NAMESPACE

/*
 * 2d polynomial of degree 2
 */
template <class REAL>
class Poly2 : public Poly<REAL>
{
    typedef typename Poly<REAL>::areal areal;
    typedef typename Poly<REAL>::Vector3 Vector3;
public:
    areal coefficients[6];

    Poly2() {}
    Poly2(const Poly2& rhs);

    Poly<REAL>* clone() const;
    void assign(const Poly<REAL>* rhs);
    areal get_coefficient(int idx) const;

    void SetCoefficients(areal* p_coeff);
    areal eval0() const { return coefficients[0];}
    void Normal0(Vector3& N) const;

    areal eval(areal x, areal y) const;
    void Normal(areal x, areal y, Vector3& N) const;
    void curvatures(areal x, areal y, areal& k1, areal& k2);
    void minpoint(areal& x, areal& y);
    void tangent_plane(areal x, areal y, Vector3& tx, Vector3& ty);

    void curvatures_at_minimum(areal& k1, areal& k2);

    int degree() const;

    void write(FILE* f) const;
    void read(FILE* f);
};

MLSLIB_END_NAMESPACE

#endif // __POLY2_H
