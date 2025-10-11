#include <gtb/graphics/ogltools.h>
#include <gtb/graphics/vector3.hpp>
#include <gtb/io/io.hpp>


GTB_BEGIN_NAMESPACE

template<class T>
inline tPoint3<T>::tPoint3()
{
}

template<class T> template <class T2>
inline tPoint3<T>::tPoint3(const tPoint3<T2> &p)
{
	_p[0] = p[0];
	_p[1] = p[1];
	_p[2] = p[2];
}
template<class T>
inline tPoint3<T>::tPoint3(const tVector3<T> &v)
{
	_p[0] = v[0];
	_p[1] = v[1];
	_p[2] = v[2];
}

template<class T>
inline tPoint3<T>::tPoint3(value_type px, value_type py, value_type pz)
{
	_p[0] = px;
	_p[1] = py;
	_p[2] = pz;
}

template<class T>
inline tPoint3<T>::tPoint3(const value_type p[3])
{
	_p[0] = p[0];
	_p[1] = p[1];
	_p[2] = p[2];
}

template<class T>
inline tPoint3<T> &tPoint3<T>::operator=(const tPoint3 &p)
{
	_p[0] = p._p[0];
	_p[1] = p._p[1];
	_p[2] = p._p[2];
	return *this;
}

template<class T>
inline bool tPoint3<T>::operator==(const tPoint3 &p) const
{
	return (real::is_equal(_p[0], p._p[0]) &&
		real::is_equal(_p[1], p._p[1]) &&
		real::is_equal(_p[2], p._p[2]));
}

template<class T>
inline bool tPoint3<T>::operator!=(const tPoint3 &p) const
{
	return !(*this == p);
}

template<class T>
inline bool tPoint3<T>::is_zero() const
{
	return (real::is_zero(_p[0]) &&
		real::is_zero(_p[1]) &&
		real::is_zero(_p[2]));
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::x() const
{
	return _p[0];
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::y() const
{
	return _p[1];
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::z() const
{
	return _p[2];
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::get_x() const
{
	return _p[0];
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::get_y() const
{
	return _p[1];
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::get_z() const
{
	return _p[2];
}

template<class T>
inline void tPoint3<T>::set_x(value_type px)
{
	_p[0] = px;
}

template<class T>
inline void tPoint3<T>::set_y(value_type py)
{
	_p[1] = py;
}

template<class T>
inline void tPoint3<T>::set_z(value_type pz)
{
	_p[2] = pz;
}

template<class T>
inline tPoint3<T> &tPoint3<T>::reset(value_type px, value_type py, value_type pz)
{
	_p[0] = px;
	_p[1] = py;
	_p[2] = pz;
	return *this;
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::operator[](unsigned i) const
{
	assert(i < 3);
	return _p[i];
}

template<class T>
inline typename tPoint3<T>::value_type &tPoint3<T>::operator[](unsigned i)
{
	assert(i < 3);
	return _p[i];
}

template<class T>
inline tPoint3<T> &tPoint3<T>::operator*=(const tMatrix4<T> &m)
{
	value_type px, py, pz, pw;
	px = x() * m[0][0] + y() * m[0][1] + z() * m[0][2] + m[0][3];
	py = x() * m[1][0] + y() * m[1][1] + z() * m[1][2] + m[1][3];
	pz = x() * m[2][0] + y() * m[2][1] + z() * m[2][2] + m[2][3];
	pw = x() * m[3][0] + y() * m[3][1] + z() * m[3][2] + m[3][3];
	assert(!real::is_zero(pw));
	return reset(px / pw, py / pw, pz / pw);
}

template<class T>
inline tPoint3<T> &tPoint3<T>::operator+=(const tVector3<T> &v)
{
	_p[0] += v[0];
	_p[1] += v[1];
	_p[2] += v[2];
	return *this;
}

template<class T>
inline tPoint3<T> &tPoint3<T>::operator-=(const tVector3<T> &v)
{
	_p[0] -= v[0];
	_p[1] -= v[1];
	_p[2] -= v[2];
	return *this;
}

// Like += with with template<class T>ponit parameter
template<class T>
inline void tPoint3<T>::add(const tPoint3& p)
{
	_p[0] += p._p[0];
	_p[1] += p._p[1];
	_p[2] += p._p[2];
}

template<class T>
inline void tPoint3<T>::add_scaled(const tPoint3& p, value_type s)
{
	_p[0] += p._p[0] * s;
	_p[1] += p._p[1] * s;
	_p[2] += p._p[2] * s;
}

template<class T>
inline void tPoint3<T>::scalar_scale(value_type s)
{
	_p[0] *= s;
	_p[1] *= s;
	_p[2] *= s;
}

template<class T>
inline tPoint3<T> tPoint3<T>::scalar_scaled(value_type s) const
{
    tPoint3 r(_p[0] * s, _p[1] * s, _p[2] * s);
    return r;
}

template<class T>
inline tPoint3<T> operator*(const tMatrix4<T> &m, const tPoint3<T> &p)
{
	T px, py, pz, pw;
	px = p.x() * m[0][0] + p.y() * m[0][1] + p.z() * m[0][2] + m[0][3];
	py = p.x() * m[1][0] + p.y() * m[1][1] + p.z() * m[1][2] + m[1][3];
	pz = p.x() * m[2][0] + p.y() * m[2][1] + p.z() * m[2][2] + m[2][3];
	pw = p.x() * m[3][0] + p.y() * m[3][1] + p.z() * m[3][2] + m[3][3];
	assert(!real::is_zero(pw));
	return tPoint3<T>(px / pw, py / pw, pz / pw);
}


template<class T>
inline tPoint3<T> operator+(const tPoint3<T> &p, const tVector3<T> &v)
{
	return tPoint3<T>(p[0] + v[0],
		      p[1] + v[1],
		      p[2] + v[2]);
}

template<class T>
inline tVector3<T> operator-(const tPoint3<T> &p, const tPoint3<T> &q)
{
	return tVector3<T>(p[0] - q[0],
		       p[1] - q[1],
		       p[2] - q[2]);
}


template<class T>
inline tPoint3<T> operator-(const tPoint3<T> &p, const tVector3<T> &v)
{
	return tPoint3<T>(p[0] - v[0],
		      p[1] - v[1],
		      p[2] - v[2]);
}

template<class T>
inline tPoint3<T> &tPoint3<T>::translate(const tVector3<T> &t)
{
	return *this += t;
}

template<class T>
inline tPoint3<T> &tPoint3<T>::translate(value_type dx, value_type dy, value_type dz)
{
	_p[0] += dx;
	_p[1] += dy;
	_p[2] += dz;
	return *this;
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::distance(const tPoint3<T> &p, const tPoint3<T> &q)
{
	return (p - q).length();
}

template<class T>
inline typename tPoint3<T>::value_type tPoint3<T>::squared_distance(const tPoint3 &p, const tPoint3 &q)
{
	return (p - q).squared_length();
}

template<class T>
inline bool tPoint3<T>::collinear(const tPoint3 &A,
			      const tPoint3 &B,
			      const tPoint3 &C)
{
	return (B - A).cross(C - A).is_zero();
}

template<class T>
inline tVector3<T> tPoint3<T>::normal(const tPoint3 &A,
			      const tPoint3 &B,
			      const tPoint3 &C)
{
	//assert(!collinear(A, B, C));
	return (B - A).cross(C - A).normalize();
}

template<class T>
inline tPoint3<T> tPoint3<T>::midpoint(const tPoint3 &A, const tPoint3 &B)
{
	return tPoint3((A.x() + B.x()) / 2.0,
		      (A.y() + B.y()) / 2.0,
		      (A.z() + B.z()) / 2.0);
}

/*
 * return alpha*A + (1-alpha)*B
 */template<class T>
inline tPoint3<T> tPoint3<T>::inbetween(const tPoint3 &A, const tPoint3 &B, value_type alpha)
{
    value_type alpha1 = 1.0 - alpha;
	return tPoint3(
		A.x() * alpha + B.x() * alpha1,
		A.y() * alpha + B.y() * alpha1,
		A.z() * alpha + B.z() * alpha1);
}

template<class T>
inline tPoint3<T> tPoint3<T>::centroid(const tPoint3 &A,
			       const tPoint3 &B,
			       const tPoint3 &C)
{
    value_type x = (A.x() + B.x() + C.x()) / 3.0;
    value_type y = (A.y() + B.y() + C.y()) / 3.0;
    value_type z = (A.z() + B.z() + C.z()) / 3.0;
    return tPoint3(x,y,z);
}


template<class T>
inline tPoint3<T> centroid(const tPoint3<T> &A,
		       const tPoint3<T> &B,
		       const tPoint3<T> &C,
		       const tPoint3<T> &D)
{
	return tPoint3<T>((A.x() + B.x() + C.x() + D.x()) / 4.0,
		      (A.y() + B.y() + C.y() + D.y()) / 4.0,
		      (A.z() + B.z() + C.z() + D.z()) / 4.0);
}

template<class T>
inline void tPoint3<T>::load() const
{
    OpenGL<T>::Vertex(_p[0], _p[1], _p[2]);
}

template<class T>
inline void tPoint3<T>::gl_vertex() const
{
    OpenGL<T>::Vertex(_p[0], _p[1], _p[2]);
}

template<class T>
inline tPoint3<T> &tPoint3<T>::transform(const tMatrix4<T> &m)
{
	return *this *= m;
}

template<class T>
inline tPoint3<T> &tPoint3<T>::affine_transform(const tMatrix4<T> &m)
{
	value_type px, py, pz;
	px = x() * m[0][0] + y() * m[0][1] + z() * m[0][2] + m[0][3];
	py = x() * m[1][0] + y() * m[1][1] + z() * m[1][2] + m[1][3];
	pz = x() * m[2][0] + y() * m[2][1] + z() * m[2][2] + m[2][3];
	return reset(px, py, pz);
}

template<class T>
inline std::istream &operator>>(std::istream &s, tPoint3<T> &p)
{
        return s >> p[0] >> p[1] >> p[2];
}


template<class T>
inline std::ostream &operator<<(std::ostream &s, const tPoint3<T> &p)
{
	return s << p[0] << ' ' << p[1] << ' ' << p[2];
}


GTB_END_NAMESPACE
