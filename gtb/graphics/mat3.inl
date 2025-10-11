template <class T>
inline tmat3<T>::tmat3() {}

template <class T>
inline tmat3<T>::tmat3(const tVector3<T>& v0, const tVector3<T>& v1, const tVector3<T>& v2)
{ v[0] = v0; v[1] = v1; v[2] = v2; }

template <class T>
inline tmat3<T>::tmat3(const double d)
{ v[0] = v[1] = v[2] = tVector3<T>(d); }

template <class T>
inline tmat3<T>::tmat3(const tmat3& m)
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; }

template <class T>
inline tmat3<T> identity2D()
{   return tmat3<T>(tVector3<T>(1.0, 0.0, 0.0),
                tVector3<T>(0.0, 1.0, 0.0),
                tVector3<T>(0.0, 0.0, 1.0)); }

// ASSIGNMENT OPERATORS

template <class T>
inline tmat3<T>& tmat3<T>::operator = ( const tmat3<T>& m )
{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; return *this; }

template <class T>
inline tmat3<T>& tmat3<T>::operator += ( const tmat3<T>& m )
{ v[0] += m.v[0]; v[1] += m.v[1]; v[2] += m.v[2]; return *this; }

template <class T>
inline tmat3<T>& tmat3<T>::operator -= ( const tmat3<T>& m )
{ v[0] -= m.v[0]; v[1] -= m.v[1]; v[2] -= m.v[2]; return *this; }

template <class T>
inline tmat3<T>& tmat3<T>::operator *= ( const double d )
{ v[0] *= d; v[1] *= d; v[2] *= d; return *this; }

template <class T>
inline tmat3<T>& tmat3<T>::operator /= ( const double d )
{ v[0] /= d; v[1] /= d; v[2] /= d; return *this; }

template <class T>
inline tVector3<T>& tmat3<T>::operator [] ( int i) {
	assert((i>=0) && (i<=2));
    return v[i];
}

template <class T>
inline const tVector3<T>& tmat3<T>::operator [] ( int i) const
{
	assert((i>=0) && (i<=2));
    return v[i];
}


// SPECIAL FUNCTIONS
template <class T>
inline tmat3<T> tmat3<T>::transpose() const
{
    return tmat3(tVector3<T>(v[0][0], v[1][0], v[2][0]),
                tVector3<T>(v[0][1], v[1][1], v[2][1]),
                tVector3<T>(v[0][2], v[1][2], v[2][2]));
}

template <class T>
inline tmat3<T> tmat3<T>::inverse() // Gauss-Jordan elimination with partial pivoting
{
    tmat3<T> a(*this),    // As a evolves from original mat into identity
    b(identity2D<T>());    // b evolves from identity into inverse(a)
    int  i, j, i1;

    // Loop over cols of a from left to right, eliminating above and below diag
    for (j=0; j<3; j++) {   // Find largest pivot in column j among rows j..2
        i1 = j;             // Row with largest pivot candidate
        for (i=j+1; i<3; i++)
            if (fabs(a.v[i][j]) > fabs(a.v[i1][j]))
                i1 = i;

        // Swap rows i1 and j in a and b to put pivot on diagonal
        swap(a.v[i1], a.v[j]);
        swap(b.v[i1], b.v[j]);

        // Scale row j to have a unit diagonal
#ifndef NO_EXCEPTIONS
        if (a.v[j][j]==0.) throw CErr("tmat3: matrix is not invertible");
#endif
        b.v[j] /= a.v[j][j];
        a.v[j] /= a.v[j][j];

        // Eliminate off-diagonal elems in col j of a, doing identical ops to b
        for (i=0; i<3; i++)
            if (i!=j) {
                b.v[i] -= a.v[i][j]*b.v[j];
                a.v[i] -= a.v[i][j]*a.v[j];
            }
    }
    return b;
}

template <class T>
inline typename tmat3<T>::value_type tmat3<T>::MINOR(int r0, int r1, int c0, int c1) const
{
    return v[r0][c0] * v[r1][c1] - v[r1][c0] * v[r0][c1];
}

template <class T>
inline typename tmat3<T>::value_type tmat3<T>::determinant() const
{
    return v[0][0] * MINOR(1, 2, 1, 2) -
            v[0][1] * MINOR(1, 2, 0, 2) +
            v[0][2] * MINOR(1, 2, 0, 1);
}

template <class T>
inline bool tmat3<T>::IsSingular() const
{
    return fabs(determinant()) < 1e-10;
}

#if 0
template <class T>
inline tmat3<T>& tmat3<T>::apply(V_FCT_PTR fct) {
    v[0].apply(fct);
    v[1].apply(fct);
    v[2].apply(fct);
    return *this;
}
#endif

// FRIENDS

template <class T>
inline tmat3<T> operator - (const tmat3<T>& a)
{ return tmat3<T>(-a[0], -a[1], -a[2]); }

template <class T>
inline tmat3<T> operator + (const tmat3<T>& a, const tmat3<T>& b)
{ return tmat3<T>(a[0] + b[0], a[1] + b[1], a[2] + b[2]); }

template <class T>
inline tmat3<T> operator - (const tmat3<T>& a, const tmat3<T>& b)
{ return tmat3<T>(a[0] - b[0], a[1] - b[1], a[2] - b[2]); }

template <class T>
inline tmat3<T> operator * (const tmat3<T>& a, const tmat3<T>& b) {
#define ROWCOL(i, j) \
    a[i][0]*b[0][j] + a[i][1]*b[1][j] + a[i][2]*b[2][j]
    return tmat3<T>(tVector3<T>(ROWCOL(0,0), ROWCOL(0,1), ROWCOL(0,2)),
                tVector3<T>(ROWCOL(1,0), ROWCOL(1,1), ROWCOL(1,2)),
                tVector3<T>(ROWCOL(2,0), ROWCOL(2,1), ROWCOL(2,2)));
#undef ROWCOL
}

template <class T>
inline tmat3<T> operator * (const tmat3<T>& a, const double d)
{ return tmat3<T>(a[0] * d, a[1] * d, a[2] * d); }

template <class T>
inline tmat3<T> operator * (const double d, const tmat3<T>& a)
{ return a*d; }

template <class T>
inline tmat3<T> operator / (const tmat3<T>& a, const double d)
{ return tmat3<T>(a[0] / d, a[1] / d, a[2] / d); }

template <class T>
inline int operator == (const tmat3<T>& a, const tmat3<T>& b)
{ return (a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]); }

template <class T>
inline int operator != (const tmat3<T>& a, const tmat3<T>& b)
{ return !(a == b); }

template <class T>
inline std::ostream& operator << (std::ostream& s, tmat3<T>& m)
{ return s << m[0] << '\n' << m[1] << '\n' << m[2]; }

template <class T>
inline std::istream& operator >> (std::istream& s, tmat3<T>& m) 
{
    tmat3<T>    m_tmp;

    s >> m_tmp[0] >> m_tmp[1] >> m_tmp[2];
    if (s)
        m = m_tmp;
    return s;
}

#if 0
inline void swap(tmat3<T>& a, tmat3<T>& b)
{ tmat3<T> tmp(a); a = b; b = tmp; }
#endif


/****************************************************************
*                                                               *
*              2D functions and 3D functions                    *
*                                                               *
****************************************************************/

/*
 * Change coordinate system so that [0 0 1] is projected to n
 * Return:
 *   rotation matrix.
 */
template <class T>
inline tmat3<T> cs_change(tVector3<T> n)
{
    n.normalize();

    tVector3<T> right;
    if (fabs(n[2]) > 1e-2)
    {
        right = tVector3<T>(0,-n[2], n[1]);
    }
    else
    {
        right = tVector3<T>(-n[1], n[0], 0);
    }

    right.normalize();

    tVector3<T> up(n.cross(right));
    tmat3<T> r(right, up, n);

    return r.transpose();
}

template <class T>
inline tVector3<T> operator * (const tmat3<T>& a, const tVector3<T>& v)
{
    return tVector3<T> (
        a[0][0] * v[0] + a[0][1] * v[1] + a[0][2] * v[2],
        a[1][0] * v[0] + a[1][1] * v[1] + a[1][2] * v[2],
        a[2][0] * v[0] + a[2][1] * v[1] + a[2][2] * v[2]
        );
}

template <class T>
inline tPoint3<T> operator * (const tmat3<T>& a, const tPoint3<T>& v)
{
    return tPoint3<T> (
        a[0][0] * v[0] + a[0][1] * v[1] + a[0][2] * v[2],
        a[1][0] * v[0] + a[1][1] * v[1] + a[1][2] * v[2],
        a[2][0] * v[0] + a[2][1] * v[1] + a[2][2] * v[2]
        );
}

