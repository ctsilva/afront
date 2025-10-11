#include <gtb/error/error.hpp>


GTB_BEGIN_NAMESPACE

template <class T>
inline tVector2<T>::tVector2()
{
}


template <class T>
inline tVector2<T>::tVector2(const tVector2 &v)
{
    _v[0] = v._v[0];
    _v[1] = v._v[1];
}


template <class T>
inline tVector2<T>::tVector2(T vx, T vy)
{
    _v[0] = vx;
    _v[1] = vy;
}


template <class T>
inline tVector2<T>::tVector2(const T v[2])
{
    _v[0] = v[0];
    _v[1] = v[1];
}



template <class T>
inline tVector2<T> &tVector2<T>::operator=(const tVector2<T> &v)
{
	_v[0] = v._v[0];
	_v[1] = v._v[1];
	return *this;
}

template <class T>
inline T tVector2<T>::x() const
{
	return _v[0];
}

template <class T>
inline T tVector2<T>::get_x() const
{
	return _v[0];
}


template <class T>
inline T tVector2<T>::y() const
{
	return _v[1];
}

template <class T>
inline T tVector2<T>::get_y() const
{
	return _v[1];
}

template <class T>
inline void tVector2<T>::set_x(T vx)
{
	_v[0] = vx;
}


template <class T>
inline void tVector2<T>::set_y(T vy)
{
	_v[1] = vy;
}


template <class T>
inline tVector2<T> &tVector2<T>::reset(T vx, T vy)
{
	_v[0] = vx;
	_v[1] = vy;
	return *this;
}


template <class T>
inline T tVector2<T>::operator[](unsigned i) const
{
	assert(i < 2);
	return _v[i];
}


template <class T>
inline T &tVector2<T>::operator[](unsigned i)
{
	assert(i < 2);
	return _v[i];
}


template <class T>
inline bool tVector2<T>::operator==(const tVector2<T> &v) const
{
	return (treal<T>::is_equal(_v[0], v._v[0]) &&
		treal<T>::is_equal(_v[1], v._v[1]));
}


template <class T>
inline bool tVector2<T>::operator!=(const tVector2<T> &v) const
{
	return !(*this == v);
}


template <class T>
inline bool tVector2<T>::is_zero() const
{
	return (treal<T>::is_zero(_v[0]) &&
		treal<T>::is_zero(_v[1]));
}


template <class T>
inline tVector2<T> tVector2<T>::operator-() const
{
	return tVector2(-_v[0],
		       -_v[1]);
}


template <class T>
inline tVector2<T> &tVector2<T>::operator+=(const tVector2<T> &v)
{
	_v[0] += v._v[0];
	_v[1] += v._v[1];
	return *this;
}


template <class T>
inline tVector2<T> &tVector2<T>::operator-=(const tVector2<T> &v)
{
	_v[0] -= v._v[0];
	_v[1] -= v._v[1];
	return *this;
}


template <class T>
inline tVector2<T> &tVector2<T>::operator*=(T a)
{
	_v[0] *= a;
	_v[1] *= a;
	return *this;
}


template <class T>
inline tVector2<T> &tVector2<T>::operator/=(T a)
{
//    if (treal<T>::is_zero(a)) {
//        GTB_WARNING("division by 0 in tVector2<T>::operator/=");
//    }
    _v[0] /= a;
    _v[1] /= a;
    return *this;
}


template <class T>
inline tVector2<T> operator+(const tVector2<T> &u, const tVector2<T> &v)
{
    return tVector2<T>(u[0] + v[0],
                    u[1] + v[1]);
}


template <class T>
inline tVector2<T> operator-(const tVector2<T> &u, const tVector2<T> &v)
{
    return tVector2<T>(u[0] - v[0],
                    u[1] - v[1]);
}

template <class T>
inline tVector2<T> operator*(T a, const tVector2<T> &v)
{
	return tVector2<T>(a * v[0],
		       a * v[1]);
}


template <class T>
inline tVector2<T> operator*(const tVector2<T> &v, T a)
{
	return a * v;
}


template <class T>
inline tVector2<T> operator/(const tVector2<T> &v, T a)
{
	assert(!treal<T>::is_zero(a));
	return tVector2<T>(v[0] / a,
		       v[1] / a);
}


template <class T>
inline T tVector2<T>::dot(const tVector2<T> &v) const
{
	return (_v[0] * v[0] +
		_v[1] * v[1]);
}

template<class T>
inline typename tVector2<T>::value_type tVector2<T>::dot(const tPoint2<T> &p) const
{
	return (_v[0] * p._p[0] +
		_v[1] * p._p[1]);
}


template <class T>
inline T tVector2<T>::length() const
{
	return sqrt(this->dot(*this));
}


template <class T>
inline T tVector2<T>::squared_length() const
{
	return this->dot(*this);
}


template <class T>
inline tVector2<T> &tVector2<T>::normalize()
{
	*this /= length();
	return *this;
}


template <class T>
inline bool tVector2<T>::is_normalized() const
{
	return treal<T>::is_equal(length(), 1.0);
}


template <class T>
inline tVector2<T> &tVector2<T>::scale(T a)
{
	*this *= a;
	return *this;
}


template <class T>
inline std::istream &operator>>(std::istream &s, tVector2<T> &v)
{
        return s >> v[0] >> v[1];
}


template <class T>
inline std::ostream &operator<<(std::ostream &s, const tVector2<T> &v)
{
	return s << v[0] << ' ' << v[1];
}

template<class T>
inline typename tVector2<T>::value_type tVector2<T>::operator*(const tVector2 &v) const
{
	return dot(v);
}

template<class T>
inline typename tVector2<T>::value_type tVector2<T>::operator*(const tPoint2<T> &p) const
{
	return dot(p);
}

template <class T>
inline tVector2<T> tVector2<T>::normal() const
{
    return tVector2<T>(-_v[1], _v[0]);
}
GTB_END_NAMESPACE
