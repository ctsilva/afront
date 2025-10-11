#ifndef GTB_MATRIX4_INCLUDED
#define GTB_MATRIX4_INCLUDED

#include <gtb/real/real.hpp>
#include <iostream>
#include <gtb/graphics/point3.hpp>

GTB_BEGIN_NAMESPACE


template <class T>
class tMatrix4 {
public:
    typedef T value_type;
	tMatrix4();

	explicit tMatrix4(value_type m[4][4]);
	explicit tMatrix4(value_type m[16]);

	tMatrix4(value_type m00, value_type m01, value_type m02, value_type m03,
		value_type m10, value_type m11, value_type m12, value_type m13,
		value_type m20, value_type m21, value_type m22, value_type m23,
		value_type m30, value_type m31, value_type m32, value_type m33);

	tMatrix4(const tMatrix4 &m);
	tMatrix4 &operator=(const tMatrix4 &m);

	bool operator==(const tMatrix4 &m) const;
	bool operator!=(const tMatrix4 &m) const;

	tMatrix4 &make_identity();
	tMatrix4 &negate();

	tMatrix4 transpose() const;
	tMatrix4 inverse() const;
	value_type det() const;

	value_type *operator[](unsigned i);
	const value_type *operator[](unsigned i) const;

	const value_type *as_array() const;
	value_type *as_array();

	void print(FILE *fp) const;
	void scan(FILE *fp);

	tMatrix4 operator-();
	tMatrix4 &operator+=(const tMatrix4 &m);
	tMatrix4 &operator-=(const tMatrix4 &m);
	tMatrix4 &operator*=(const tMatrix4 &m);
	tMatrix4 &operator*=(value_type a);

#if 0
	friend tMatrix4 operator+(const tMatrix4 &m1, const tMatrix4 &m2);
	friend tMatrix4 operator-(const tMatrix4 &m1, const tMatrix4 &m2);
	friend tMatrix4 operator*(const tMatrix4 &m1, const tMatrix4 &m2);
	friend tMatrix4 operator*(const value_type a, const tMatrix4 &m);
	friend tMatrix4 operator*(const tMatrix4 &m, value_type a);

	friend std::ostream &operator<<(std::ostream &out, const tMatrix4 &m);
#endif

    void load(); // To current openGL matrix stack
    void ogl_multiply();

    static const tMatrix4 MATRIX4_ZERO;
    static const tMatrix4 MATRIX4_IDENTITY;
protected:
	bool LU_decomposition(int p[4]);

	void LU_back_substitution(const int p[4],
				  tMatrix4 &b,
				  tMatrix4 &x);

	value_type _m[4][4];
};


template<class T>
tMatrix4<T> TranslationMatrix(T Tx, T Ty, T Tz);
template<class T>
tMatrix4<T> TranslationMatrix(const tPoint3<T>& Tv);
template<class T>
tMatrix4<T> ScaleMatrix(T Sx, T Sy, T Sz);

//extern const tMatrix4 MATRIX4_ZERO;
//extern const tMatrix4 MATRIX4_IDENTITY;

typedef tMatrix4<float> Matrix4f;
typedef tMatrix4<double> Matrix4d;

#ifdef REAL_IS_FLOAT
typedef Matrix4f Matrix4;
#else
typedef Matrix4d Matrix4;
#endif

// Forward declare explicit specializations to avoid instantiation-before-specialization errors
template<> const tMatrix4<float> tMatrix4<float>::MATRIX4_IDENTITY;
template<> const tMatrix4<double> tMatrix4<double>::MATRIX4_IDENTITY;

GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/math/matrix4.ipp>
#endif

#endif // GTB_MATRIX4_INCLUDED
