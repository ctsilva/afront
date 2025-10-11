#ifndef __AMATC_H
#define __AMATC_H

/*
 * A general purpose matrix - duplication of amat
 * except that the data is column major
 */
#include "amat.h"

GTB_BEGIN_NAMESPACE

template <class T> class AMatc;
template <class T> AMatc<T> MtM(const AMatc<T>& M);

template <class T>
class AMatc
{
public:
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const T* const_pointer;

    AMatc(int m, int n, const_pointer v=0, bool ftn=false);
    AMatc(const AMatc& rhs);
    ~AMatc();

    AMatc operator *(const AMatc& rhs) const;
    AVec<T> operator *(const AVec<T>& rhs) const;
    void transpose();
    AMatc transposed() const;

    reference operator() (int i, int j);
    const_reference operator() (int i, int j) const;

    AMatc& operator=(const AMatc& rhs);

    void assign(const_pointer v, bool ftn=false);
    void assign(const_pointer v, int m, int n);

    void assign_diag(const AVec<T>& v);

    int rows() const { return M; }
    int columns() const { return N; }
    void resize(int m, int n);

    void set(const T& value);

    void get_row(int r, AVec<T>& vrow);
    void get_column(int c, AVec<T>& vcol);

    pointer flatten() { return m; }
    const_pointer flatten() const { return m; }

    // verify that the elements of the matrix are
    // legal fp values
    bool is_finite() const;

    friend AMatc MtM<>(const AMatc& M);

protected:
    value_type* m;
    int M,N;

    void allocate();
    void cleanup();
};    


/*
 * Utility I/O functions
 */
template <class T>
std::ostream& operator << (std::ostream& s, const AMatc<T>& m);


template <class T>
void diag(const AMatc<T>& m, AVec<T> v);


/*
 * Commonly used
 */
typedef AMatc<float> AMatcf;
typedef AMatc<double> AMatcd;

/*
 * Return x'*M*x
 */
template <class T>
T xtMx(const AVec<T>& x, const AMatc<T>& M);

/*
 * Compute M'M
 */
template <class T>
AMatc<T> MtM(const AMatc<T>& M);

/*
 * Linear algebra tools
 */
template <class T>
void SVD(
    const AMatc<T>& A,     // matrix to solve
    AMatc<T>& U,           // result U
    AVec<T>& W,           // result diagonal "matrix" W
    AMatc<T>& Vt           // result V^t
    );

// Forward declare explicit specializations
template<> void SVD<double>(const AMatc<double>&, AMatc<double>&, AVec<double>&, AMatc<double>&);
template<> void SVD<float>(const AMatc<float>&, AMatc<float>&, AVec<float>&, AMatc<float>&);

template <class T>
AMatc<T> SVDInvert(const AMatc<T>& A);

template <class T>
void SVDSolve(
    const AMatc<T>& A,     // matrix to solve
    const AVec<T>& b,
    AVec<T>& x            // Result
    );

//
// Compute the eigen vectors and values
// of a square matrix
//
void EigenUpperTriangular(AMatc<float>& A, AVec<float>& evalues);
void EigenUpperTriangular(AMatc<double>& A, AVec<double>& evalues);

#include "amatc.inl"

GTB_END_NAMESPACE

#endif //__AMATC_H
