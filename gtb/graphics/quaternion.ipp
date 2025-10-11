#include <gtb/error/error.hpp>
#include <gtb/io/io.hpp>
#include <gtb/graphics/ogltools.h>
#include <ctype.h>

GTB_BEGIN_NAMESPACE

template <class T>
inline tQuaternion<T>::tQuaternion() {};

template <class T> template <class T2>
inline tQuaternion<T>::tQuaternion(const tQuaternion<T2> &q):
    v_(q.v_), w_(q.w_) {}

template <class T>
inline tQuaternion<T>::tQuaternion(const tVector3<T> &axis, T angle)
{
    w_ = cos(angle/2.0);
    v_ = axis * sin(angle/2.0);
}

template <class T>
void tQuaternion<T>::normalize()
{
    T l = length();
    v_ /= l;
    w_ /= l;
}

template <class T>
tQuaternion<T> tQuaternion<T>::normalized() const
{
    tQuaternion<T> cp = *this;
    cp.normalize();
    return cp;
}

template <class T>
T tQuaternion<T>::length() const
{
    return sqrt(length2());
}

template <class T>
    T tQuaternion<T>::length2() const
{
    return v_.length2() + (w_ * w_);
}

template <class T>
tQuaternion<T>& tQuaternion<T>::operator*=(const tQuaternion<T> &q)
{
    T new_w = w_ * q.w_ - v_.dot(q.v_);
    tVector3<T> new_v = q.v_ * w_ + v_ * q.w_ + v_.cross(q.v_);
    set_v(new_v);
    set_w(new_w);
    return *this;
}

template <class T>
tPoint3<T> tQuaternion<T>::operator*=(const tPoint3<T>& p) const
{
    tQuaternion<T> vq;
    vq.set_w(0.0);
    vq.set_v(p - tPoint3<T>::ZERO);
    return (*this * vq * conjugated()).get_v();
}

template <class T>
tVector3<T> tQuaternion<T>::operator*=(const tVector3<T>& p) const
{
    tQuaternion<T> vq;
    vq.set_w(0.0);
    vq.set_v(p);
    return (*this * vq * conjugated()).get_v();
}

template <class T>
void tQuaternion<T>::conjugate()
{
    v_ = -v_;
}

template <class T>
tQuaternion<T> tQuaternion<T>::conjugated() const
{
    tQuaternion<T> cp = *this;
    cp.conjugate();
    return cp;
}

template <class T>
inline std::ostream &operator<<(std::ostream &os, const tQuaternion<T> &q)
{
    return os << "<< [" << q.get_v() << "] " << q.get_w() << " >>";
}

template <class T>
inline tQuaternion<T> operator*(const tQuaternion<T> &q1, const tQuaternion<T> &q2)
{
    tQuaternion<T> result = q1;
    return result *= q2;
}

template <class T>
inline tPoint3<T> operator*(const tQuaternion<T> &q1, const tPoint3<T> &q2)
{
    return q1 *= q2;
}

template <class T>
inline tVector3<T> operator*(const tQuaternion<T> &q1, const tVector3<T> &q2)
{
    return q1 *= q2;
}

GTB_END_NAMESPACE
