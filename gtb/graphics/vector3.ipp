#include <gtb/error/error.hpp>
#include <gtb/io/io.hpp>
#include <gtb/graphics/ogltools.h>
#include <ctype.h>
#include <math.h>

GTB_BEGIN_NAMESPACE

template <class T>
struct OpenGL; // forward decl

template<class T>
inline tVector3<T>::tVector3()
{
}

template<class T> template <class T2>
inline tVector3<T>::tVector3(const tVector3<T2> &v)
{
	_v[0] = v[0];
	_v[1] = v[1];
	_v[2] = v[2];
}

template<class T>
inline tVector3<T>::tVector3(value_type vx, value_type vy, value_type vz)
{
	_v[0] = vx;
	_v[1] = vy;
	_v[2] = vz;
}

template<class T>
inline tVector3<T>::tVector3(const value_type v[3])
{
	_v[0] = v[0];
	_v[1] = v[1];
	_v[2] = v[2];
}

template<class T>
inline tVector3<T>::tVector3(const value_type v) 
{
	_v[0] = v;
	_v[1] = v;
	_v[2] = v;
}
template<class T>
inline tVector3<T>::tVector3(const tPoint3<T> &p)
{
	_v[0] = p._p[0];
	_v[1] = p._p[1];
	_v[2] = p._p[2];
}
template<class T>
inline tVector3<T> &tVector3<T>::operator=(const tVector3 &v)
{
	_v[0] = v._v[0];
	_v[1] = v._v[1];
	_v[2] = v._v[2];
	return *this;
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::x() const
{
	return _v[0];
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::y() const
{
	return _v[1];
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::z() const
{
	return _v[2];
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::get_x() const
{
	return _v[0];
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::get_y() const
{
	return _v[1];
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::get_z() const
{
	return _v[2];
}


template<class T>
inline void tVector3<T>::set_x(value_type vx)
{
	_v[0] = vx;
}

template<class T>
inline void tVector3<T>::set_y(value_type vy)
{
	_v[1] = vy;
}

template<class T>
inline void tVector3<T>::set_z(value_type vz)
{
	_v[2] = vz;
}

template<class T>
inline tVector3<T> &tVector3<T>::reset(value_type vx, value_type vy, value_type vz)
{
	_v[0] = vx;
	_v[1] = vy;
	_v[2] = vz;
	return *this;
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::operator[](unsigned i) const
{
	assert(i < 3);
	return _v[i];
}

template<class T>
inline typename tVector3<T>::value_type &tVector3<T>::operator[](unsigned i)
{
	assert(i < 3);
	return _v[i];
}

template<class T>
inline bool tVector3<T>::operator==(const tVector3 &v) const
{
	return (treal<T>::is_equal(_v[0], v._v[0]) &&
		treal<T>::is_equal(_v[1], v._v[1]) &&
		treal<T>::is_equal(_v[2], v._v[2]));
}

template<class T>
inline bool tVector3<T>::operator!=(const tVector3 &v) const
{
	return !(*this == v);
}

template<class T>
inline bool tVector3<T>::is_zero() const
{
	return (treal<T>::is_zero(_v[0]) &&
		treal<T>::is_zero(_v[1]) &&
		treal<T>::is_zero(_v[2]));
}

template<class T>
inline tVector3<T> tVector3<T>::operator-() const
{
	return tVector3(-_v[0],
		       -_v[1],
		       -_v[2]);
}

template<class T>
inline tVector3<T> &tVector3<T>::operator+=(const tVector3 &v)
{
	_v[0] += v._v[0];
	_v[1] += v._v[1];
	_v[2] += v._v[2];
	return *this;
}

template<class T>
inline tVector3<T> &tVector3<T>::operator-=(const tVector3 &v)
{
	_v[0] -= v._v[0];
	_v[1] -= v._v[1];
	_v[2] -= v._v[2];
	return *this;
}

template<class T>
inline tVector3<T> &tVector3<T>::operator*=(value_type a)
{
	_v[0] *= a;
	_v[1] *= a;
	_v[2] *= a;
	return *this;
}

template<class T>
inline tVector3<T> &tVector3<T>::operator*=(const tMatrix4<T> &m)
{
	// Do we need vw?
	value_type vx, vy, vz, vw;
	vx = x() * m[0][0] + y() * m[0][1] + z() * m[0][2];
	vy = x() * m[1][0] + y() * m[1][1] + z() * m[1][2];
	vz = x() * m[2][0] + y() * m[2][1] + z() * m[2][2];
	vw = x() * m[3][0] + y() * m[3][1] + z() * m[3][2] + m[3][3];
	assert(!treal<T>::is_zero(vw));
	return reset(vx / vw, vy / vw, vz / vw);
}

template<class T>
inline tVector3<T> &tVector3<T>::operator/=(value_type a)
{
//	if (treal<T>::is_zero(a)) {
//		//GTB_WARNING("division by 0 in tVector3<T>::operator/=");
//	}
	_v[0] /= a;
	_v[1] /= a;
	_v[2] /= a;
	return *this;
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::dot(const tVector3 &v) const
{
	return (_v[0] * v._v[0] +
		_v[1] * v._v[1] +
		_v[2] * v._v[2]);
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::dot(const tPoint3<T> &p) const
{
	return (_v[0] * p._p[0] +
		_v[1] * p._p[1] +
		_v[2] * p._p[2]);
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::operator*(const tVector3 &v) const
{
	return dot(v);
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::operator*(const tPoint3<T> &p) const
{
	return dot(p);
}


template<class T>
inline tVector3<T> operator+(const tVector3<T> &u, const tVector3<T> &v)
{
    return tVector3<T>(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}


template<class T>
inline tVector3<T> operator-(const tVector3<T> &u, const tVector3<T> &v)
{
	return tVector3<T>(u[0] - v[0],
		       u[1] - v[1],
		       u[2] - v[2]);
}


template<class T>
inline tVector3<T> operator*(T a, const tVector3<T> &v)
{
	return tVector3<T>(a * v[0],
		       a * v[1],
		       a * v[2]);
}


template<class T>
inline tVector3<T> operator*(const tVector3<T> &v, T a)
{
	return a * v;
}


template<class T>
inline tVector3<T> operator*(const tMatrix4<T> &m, const tVector3<T> &v)
{
	// Do we need vw?
	T vx, vy, vz, vw;
	vx = v.x() * m[0][0] + v.y() * m[0][1] + v.z() * m[0][2];
	vy = v.x() * m[1][0] + v.y() * m[1][1] + v.z() * m[1][2];
	vz = v.x() * m[2][0] + v.y() * m[2][1] + v.z() * m[2][2];
	vw = v.x() * m[3][0] + v.y() * m[3][1] + v.z() * m[3][2] + m[3][3];
//	assert(!real<T>::is_zero(vw));
	return tVector3<T>(vx / vw, vy / vw, vz / vw);
}


template<class T>
inline tVector3<T> operator/(const tVector3<T> &v, T a)
{
	return tVector3<T>(v[0] / a, v[1] / a, v[2] / a);
}

template<class T>
inline tVector3<T> tVector3<T>::cross(const tVector3<T> &v) const
{
	return tVector3<T>((_v[1] * v._v[2]) - (_v[2] * v._v[1]),
		       (_v[2] * v._v[0]) - (_v[0] * v._v[2]),
		       (_v[0] * v._v[1]) - (_v[1] * v._v[0]));
}

template <>
inline float tVector3<float>::length() const
{
	return sqrtf(this->dot(*this));
}

template<>
inline double tVector3<double>::length() const
{
	return sqrt(this->dot(*this));
}

template<class T>
inline T tVector3<T>::length() const
{
	return sqrt(this->dot(*this));
}

template<class T>
inline typename tVector3<T>::value_type tVector3<T>::squared_length() const
{
	return this->dot(*this);
}

template<class T>
inline tVector3<T> &tVector3<T>::normalize()
{
	*this /= length();
	return *this;
}

template<class T>
inline tVector3<T> tVector3<T>::normalized() const
{
    tVector3 r = *this;
    r.normalize();
    return r;
}
template<class T>
inline bool tVector3<T>::is_normalized() const
{
	return treal<T>::is_equal(length(), 1.0);
}

template<class T>
inline tVector3<T> &tVector3<T>::scale(value_type a)
{
	*this *= a;
	return *this;
}

template<class T>
inline void tVector3<T>::load_as_normal() const
{
    OpenGL<T>::Normal(_v[0], _v[1], _v[2]);
}

template<class T>
inline tVector3<T> &tVector3<T>::transform(const tMatrix4<T> &m)
{
	return *this *= m;
}

template<class T>
inline tVector3<T> &tVector3<T>::affine_transform(const tMatrix4<T> &m)
{
	value_type vx, vy, vz;
	vx = x() * m[0][0] + y() * m[0][1] + z() * m[0][2];
	vy = x() * m[1][0] + y() * m[1][1] + z() * m[1][2];
	vz = x() * m[2][0] + y() * m[2][1] + z() * m[2][2];
	return reset(vx, vy, vz);
}
template<class T>
inline void tVector3<T>::flip()
{
	_v[0] = -_v[0];
	_v[1] = -_v[1];
	_v[2] = -_v[2];
}

template<class T>
inline tVector3<T> tVector3<T>::flipped() const
{
    tVector3 r(*this);
    r.flip();
    return r;
}

template<class T>
inline void swap(tVector3<T> &a, tVector3<T> &b)
{
	tVector3<T> tmp(a);
	a = b;
	b = tmp;
}

template<class T>
inline std::istream &operator>>(std::istream &s, tVector3<T> &v)
{
        return s >> v[0] >> v[1] >> v[2];
}

template<class T>
inline std::ostream &operator<<(std::ostream &s, const tVector3<T> &v)
{
	return s << v[0] << ' ' << v[1] << ' ' << v[2];
}




template<>
inline void tVector3<float>::read(FILE *fp)
{
	read_float(&_v[0], fp);
	read_float(&_v[1], fp);
	read_float(&_v[2], fp);
}

template<>
inline void tVector3<float>::write(FILE *fp) const
{
	write_float(_v[0], fp);
	write_float(_v[1], fp);
	write_float(_v[2], fp);
}


template<>
inline void tVector3<double>::read(FILE *fp)
{
	read_double(&_v[0], fp);
	read_double(&_v[1], fp);
	read_double(&_v[2], fp);
}

template<>
inline void tVector3<double>::write(FILE *fp) const
{
	write_double(_v[0], fp);
	write_double(_v[1], fp);
	write_double(_v[2], fp);
}

GTB_END_NAMESPACE
