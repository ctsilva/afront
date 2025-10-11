#include <gtb/graphics/ogltools.h>
#include <gtb/graphics/vector2.hpp>

GTB_BEGIN_NAMESPACE

template <class T>
inline tPoint2<T>::tPoint2()
{
}


template <class T> template <class T2>
inline tPoint2<T>::tPoint2(const tPoint2<T2> &p)
{
    _p[0] = p[0];
    _p[1] = p[1];
}


template <class T>
inline tPoint2<T>::tPoint2(T px, T py)
{
    _p[0] = px;
    _p[1] = py;
}


template <class T>
inline tPoint2<T>::tPoint2(const T p[2])
{
    _p[0] = p[0];
    _p[1] = p[1];
}


template <class T>
inline tPoint2<T> &tPoint2<T>::operator=(const tPoint2<T> &p)
{
	_p[0] = p._p[0];
	_p[1] = p._p[1];
	return *this;
}


template <class T>
inline bool tPoint2<T>::operator==(const tPoint2<T> &p) const
{
	return (real::is_equal(_p[0], p._p[0]) &&
		real::is_equal(_p[1], p._p[1]));
}


template <class T>
inline bool tPoint2<T>::operator!=(const tPoint2<T> &p) const
{
	return !(*this == p);
}


template <class T>
inline bool tPoint2<T>::is_zero() const
{
	return (real::is_zero(_p[0]) &&
		real::is_zero(_p[1]));
}

template <class T>
inline void tPoint2<T>::load() const
{
    OpenGL<T>::Vertex(_p[0], _p[1]);
}

template <class T>
inline T tPoint2<T>::x() const
{
	return _p[0];
}

template <class T>
inline T tPoint2<T>::y() const
{
	return _p[1];
}


template <class T>
inline void tPoint2<T>::set_x(T px)
{
	_p[0] = px;
}


template <class T>
inline void tPoint2<T>::set_y(T py)
{
	_p[1] = py;
}


template <class T>
inline tPoint2<T> &tPoint2<T>::reset(T px, T py)
{
	_p[0] = px;
	_p[1] = py;
	return *this;
}


template <class T>
inline T tPoint2<T>::operator[](unsigned i) const
{
	assert(i < 2);
	return _p[i];
}


template <class T>
inline T &tPoint2<T>::operator[](unsigned i)
{
	assert(i < 2);
	return _p[i];
}


template <class T>
inline tPoint2<T> &tPoint2<T>::operator+=(const tVector2<T> &v)
{
	_p[0] += v[0];
	_p[1] += v[1];
	return *this;
}


template <class T>
inline tPoint2<T> &tPoint2<T>::operator-=(const tVector2<T> &v)
{
	_p[0] -= v[0];
	_p[1] -= v[1];
	return *this;
}


template <class T>
inline tPoint2<T> operator+(const tPoint2<T> &p, const tVector2<T> &v)
{
	return tPoint2<T>(p[0] + v[0],
		      p[1] + v[1]);
}


template <class T>
inline tVector2<T> operator-(const tPoint2<T> &p, const tPoint2<T> &q)
{
	return tVector2<T>(p[0] - q[0],
		       p[1] - q[1]);
}


template <class T>
inline tPoint2<T> operator-(const tPoint2<T> &p, const tVector2<T> &v)
{
	return tPoint2<T>(p[0] - v[0],
		      p[1] - v[1]);
}


template <class T>
inline tPoint2<T> &tPoint2<T>::translate(const tVector2<T> &t)
{
	return *this += t;
}


template <class T>
inline tPoint2<T> &tPoint2<T>::translate(T dx, T dy)
{
	_p[0] += dx;
	_p[1] += dy;
	return *this;
}


template <class T>
inline T tPoint2<T>::distance(const tPoint2<T> &p, const tPoint2<T> &q)
{
	return (p - q).length();
}


template <class T>
inline T tPoint2<T>::squared_distance(const tPoint2<T> &p, const tPoint2<T> &q)
{
	return (p - q).squared_length();
}


template <class T>
inline tPoint2<T> tPoint2<T>::midpoint(const tPoint2<T> &A, const tPoint2<T> &B)
{
    return tPoint2((A.x() + B.x()) / 2.0,
                   (A.y() + B.y()) / 2.0);
}

template <class T>
inline void tPoint2<T>::add(const tPoint2& rhs)
{
	_p[0] += rhs._p[0];
	_p[1] += rhs._p[1];
}

template <class T>
inline void tPoint2<T>::multiply(T value)
{
	_p[0] *= value;
	_p[1] *= value;
}


template <class T>
inline std::istream &operator>>(std::istream &s, tPoint2<T> &p)
{
        return s >> p[0] >> p[1];
}

//template
//inline std::istream &operator>>(std::istream &s, tPoint2<float> &p);
//template
//inline std::istream &operator>>(std::istream &s, tPoint2<double> &p);


template <class T>
inline std::ostream &operator<<(std::ostream &s, const tPoint2<T> &p)
{
	return s << p[0] << ' ' << p[1];
}

//template
//inline std::ostream &operator<<(std::ostream &s, const tPoint2<float> &p);
//template
//inline std::ostream &operator<<(std::ostream &s, const tPoint2<double> &p);

template <class T>
inline bool tPoint2<T>::collinear(const tPoint2 &A,
				  const tPoint2 &B,
				  const tPoint2 &C)
{
    return (B-A).cross(C-A).is_zero();
}

template <class T>
inline tVector2<T> tPoint2<T>::normal(const tPoint2 &A,
				     const tPoint2 &B)
{
    return (B-A).normal();
}


/*
 * return alpha*A + (1-alpha)*B
 */
template<class T>
inline tPoint2<T> tPoint2<T>::inbetween(const tPoint2 &A, const tPoint2 &B, value_type alpha)
{
    value_type alpha1 = 1.0 - alpha;
	return tPoint2(
		A.x() * alpha + B.x() * alpha1,
		A.y() * alpha + B.y() * alpha1);
}


GTB_END_NAMESPACE
