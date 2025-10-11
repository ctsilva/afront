#include <assert.h>


GTB_BEGIN_NAMESPACE

template<class T>
inline bool tMatrix4<T>::operator==(const tMatrix4 &m) const
{
	return (real::is_equal(_m[0][0], m._m[0][0]) &&
		real::is_equal(_m[0][1], m._m[0][1]) &&
		real::is_equal(_m[0][2], m._m[0][2]) &&
		real::is_equal(_m[0][3], m._m[0][3]) &&
		
		real::is_equal(_m[1][0], m._m[1][0]) &&
		real::is_equal(_m[1][1], m._m[1][1]) &&
		real::is_equal(_m[1][2], m._m[1][2]) &&
		real::is_equal(_m[1][3], m._m[1][3]) &&
		
		real::is_equal(_m[2][0], m._m[2][0]) &&
		real::is_equal(_m[2][1], m._m[2][1]) &&
		real::is_equal(_m[2][2], m._m[2][2]) &&
		real::is_equal(_m[2][3], m._m[2][3]) &&
		
		real::is_equal(_m[3][0], m._m[3][0]) &&
		real::is_equal(_m[3][1], m._m[3][1]) &&
		real::is_equal(_m[3][2], m._m[3][2]) &&
		real::is_equal(_m[3][3], m._m[3][3]));
}

template<class T>
inline bool tMatrix4<T>::operator!=(const tMatrix4 &m) const
{
	return !(*this == m);
}

template<class T>
inline tMatrix4<T> tMatrix4<T>::transpose() const
{
	return tMatrix4(_m[0][0], _m[1][0], _m[2][0], _m[3][0],
		       _m[0][1], _m[1][1], _m[2][1], _m[3][1],
		       _m[0][2], _m[1][2], _m[2][2], _m[3][2],
		       _m[0][3], _m[1][3], _m[2][3], _m[3][3]);
}

template<class T>
inline const typename tMatrix4<T>::value_type *tMatrix4<T>::as_array() const
{
	return (const value_type *) _m;
}

template<class T>
inline typename tMatrix4<T>::value_type *tMatrix4<T>::as_array()
{
	return (value_type *) _m;
}

template<class T>
inline typename tMatrix4<T>::value_type *tMatrix4<T>::operator[](unsigned i)
{
	assert(i < 4);
	return _m[i];
}

template<class T>
inline const typename tMatrix4<T>::value_type *tMatrix4<T>::operator[](unsigned i) const
{
	assert(i < 4);
	return _m[i];
}

template<class T>
inline tMatrix4<T> tMatrix4<T>::operator-()
{
	return tMatrix4(-_m[0][0], -_m[0][1], -_m[0][2], -_m[0][3],
		       -_m[1][0], -_m[1][1], -_m[1][2], -_m[1][3],
		       -_m[2][0], -_m[2][1], -_m[2][2], -_m[2][3],
		       -_m[3][0], -_m[3][1], -_m[3][2], -_m[3][3]);
}

template<class T>
inline tMatrix4<T> &tMatrix4<T>::operator+=(const tMatrix4 &m)
{
	_m[0][0] += m._m[0][0];
	_m[0][1] += m._m[0][1];
	_m[0][2] += m._m[0][2];
	_m[0][3] += m._m[0][3];

	_m[1][0] += m._m[1][0];
	_m[1][1] += m._m[1][1];
	_m[1][2] += m._m[1][2];
	_m[1][3] += m._m[1][3];

	_m[2][0] += m._m[2][0];
	_m[2][1] += m._m[2][1];
	_m[2][2] += m._m[2][2];
	_m[2][3] += m._m[2][3];

	_m[3][0] += m._m[3][0];
	_m[3][1] += m._m[3][1];
	_m[3][2] += m._m[3][2];
	_m[3][3] += m._m[3][3];

	return *this;
}

template<class T>
inline tMatrix4<T> &tMatrix4<T>::operator-=(const tMatrix4 &m)
{
	_m[0][0] -= m._m[0][0];
	_m[0][1] -= m._m[0][1];
	_m[0][2] -= m._m[0][2];
	_m[0][3] -= m._m[0][3];

	_m[1][0] -= m._m[1][0];
	_m[1][1] -= m._m[1][1];
	_m[1][2] -= m._m[1][2];
	_m[1][3] -= m._m[1][3];

	_m[2][0] -= m._m[2][0];
	_m[2][1] -= m._m[2][1];
	_m[2][2] -= m._m[2][2];
	_m[2][3] -= m._m[2][3];

	_m[3][0] -= m._m[3][0];
	_m[3][1] -= m._m[3][1];
	_m[3][2] -= m._m[3][2];
	_m[3][3] -= m._m[3][3];

	return *this;
}


template<class T>
inline tMatrix4<T> operator*(const tMatrix4<T> &m1, const tMatrix4<T> &m2)
{
	tMatrix4<T> t;

	for (unsigned i = 0; i < 4; i++) {
		for (unsigned j = 0; j < 4; j++) {
			t[i][j] = 0.0;
			for (unsigned k = 0; k < 4; k++) {
				t[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return t;
}

template<class T>
inline tMatrix4<T> &tMatrix4<T>::operator*=(const tMatrix4 &m)
{
	*this = *this * m;
	return *this;
}

template<class T>
inline tMatrix4<T> &tMatrix4<T>::operator*=(value_type a)
{
	_m[0][0] *= a;
	_m[0][1] *= a;
	_m[0][2] *= a;
	_m[0][3] *= a;

	_m[1][0] *= a;
	_m[1][1] *= a;
	_m[1][2] *= a;
	_m[1][3] *= a;

	_m[2][0] *= a;
	_m[2][1] *= a;
	_m[2][2] *= a;
	_m[2][3] *= a;

	_m[3][0] *= a;
	_m[3][1] *= a;
	_m[3][2] *= a;
	_m[3][3] *= a;

	return *this;
}


template<class T>
inline tMatrix4<T> operator+(const tMatrix4<T> &m1, const tMatrix4<T> &m2)
{
	return tMatrix4<T>(m1[0][0] + m2[0][0],
		       m1[0][1] + m2[0][1],
		       m1[0][2] + m2[0][2],
		       m1[0][3] + m2[0][3],

		       m1[1][0] + m2[1][0],
		       m1[1][1] + m2[1][1],
		       m1[1][2] + m2[1][2],
		       m1[1][3] + m2[1][3],

		       m1[2][0] + m2[2][0],
		       m1[2][1] + m2[2][1],
		       m1[2][2] + m2[2][2],
		       m1[2][3] + m2[2][3],

		       m1[3][0] + m2[3][0],
		       m1[3][1] + m2[3][1],
		       m1[3][2] + m2[3][2],
		       m1[3][3] + m2[3][3]);
}


template<class T>
inline tMatrix4<T> operator-(const tMatrix4<T> &m1, const tMatrix4<T> &m2)
{
	return tMatrix4<T>(m1[0][0] - m2[0][0],
		       m1[0][1] - m2[0][1],
		       m1[0][2] - m2[0][2],
		       m1[0][3] - m2[0][3],

		       m1[1][0] - m2[1][0],
		       m1[1][1] - m2[1][1],
		       m1[1][2] - m2[1][2],
		       m1[1][3] - m2[1][3],

		       m1[2][0] - m2[2][0],
		       m1[2][1] - m2[2][1],
		       m1[2][2] - m2[2][2],
		       m1[2][3] - m2[2][3],

		       m1[3][0] - m2[3][0],
		       m1[3][1] - m2[3][1],
		       m1[3][2] - m2[3][2],
		       m1[3][3] - m2[3][3]);
}


template<class T>
inline tMatrix4<T> operator*(const T a, const tMatrix4<T> &m)
{
	return tMatrix4<T>(
		a * m[0][0],
		a * m[0][1],
		a * m[0][2],
		a * m[0][3],

		a * m[1][0],
		a * m[1][1],
		a * m[1][2],
		a * m[1][3],

		a * m[2][0],
		a * m[2][1],
		a * m[2][2],
		a * m[2][3],

		a * m[3][0],
		a * m[3][1],
		a * m[3][2],
		a * m[3][3]);
}


template<class T>
inline tMatrix4<T> operator*(const tMatrix4<T> &m, T a)
{
	return a * m;
}


GTB_END_NAMESPACE
