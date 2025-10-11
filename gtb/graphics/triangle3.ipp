#include <gtb/graphics/vector3.hpp>
#include <gtb/graphics/circle3.hpp>


GTB_BEGIN_NAMESPACE


template <class T>
inline tTriangle3<T>::tTriangle3()
{
}


template <class T>
inline tTriangle3<T>::tTriangle3(const Point3 &a,
		     const Point3 &b,
		     const Point3 &c)
	: m_A(a),
	  m_B(b),
	  m_C(c)
{
}


template <class T>
inline tTriangle3<T>::tTriangle3(const tTriangle3 &t)
	: m_A(t.m_A),
	  m_B(t.m_B),
	  m_C(t.m_C)
{
}


template <class T>
inline tTriangle3<T> &tTriangle3<T>::operator=(const tTriangle3 &t)
{
    if (&t != this) {
        m_A = t.m_A;
        m_B = t.m_B;
        m_C = t.m_C;
    }
    return *this;
}


template <class T>
inline bool tTriangle3<T>::operator==(const tTriangle3 &t) const
{
    return ((m_A == t.m_A) &&
            (m_B == t.m_B) &&
            (m_C == t.m_C));
}


template <class T>
inline bool tTriangle3<T>::operator!=(const tTriangle3 &t) const
{
    return !(*this == t);
}


template <class T>
inline const typename tTriangle3<T>::Point3 &tTriangle3<T>::A() const
{
    return m_A;
}


template <class T>
inline const typename tTriangle3<T>::Point3 &tTriangle3<T>::get_A() const
{
    return m_A;
}


template <class T>
inline const typename tTriangle3<T>::Point3 &tTriangle3<T>::B() const
{
    return m_B;
}


template <class T>
inline const typename tTriangle3<T>::Point3 &tTriangle3<T>::get_B() const
{
    return m_B;
}


template <class T>
inline const typename tTriangle3<T>::Point3 &tTriangle3<T>::C() const
{
	return m_C;
}


template <class T>
inline const typename tTriangle3<T>::Point3 &tTriangle3<T>::get_C() const
{
	return m_C;
}


template <class T>
inline const typename tTriangle3<T>::Point3 &tTriangle3<T>::operator[](int i) const 
{
	assert(i>=0 && i<3);

	switch (i) {
	case 0:	return m_A; break;
	case 1:	return m_B; break;
	case 2:	return m_C; break;
	}

	return m_A;
}



template <class T>
inline typename tTriangle3<T>::Vector3 tTriangle3<T>::normal() const
{
	return (m_B - m_A).cross(m_C - m_A).normalize();
}


template <class T>
inline typename tTriangle3<T>::value_type tTriangle3<T>::area() const
{
	return 0.5 * (m_B - m_A).cross(m_C - m_A).length();
}


template <class T>
inline typename tTriangle3<T>::Circle3 tTriangle3<T>::circumcircle() const
{
	return Circle3(m_A, m_B, m_C);
}


template <class T>
inline typename tTriangle3<T>::Box3 tTriangle3<T>::bounding_box() const
{
	return Box3::bounding_box(m_A, m_B, m_C);
}


GTB_END_NAMESPACE
