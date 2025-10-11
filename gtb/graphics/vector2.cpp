#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/vector2.hpp>
#include <gtb/graphics/plane.hpp>
#include <gtb/graphics/polygon3.hpp>
#include <gtb/graphics/ogltools.h>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/vector2.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


template<> tVector2<float> tVector2<float>::ZERO(0.0, 0.0);
template<> tVector2<float> tVector2<float>::POSITIVE_X(1.0, 0.0);
template<> tVector2<float> tVector2<float>::NEGATIVE_X(-1.0, 0.0);
template<> tVector2<float> tVector2<float>::POSITIVE_Y(0.0, 1.0);
template<> tVector2<float> tVector2<float>::NEGATIVE_Y(0.0, -1.0);

template<> tVector2<double> tVector2<double>::ZERO(0.0, 0.0);
template<> tVector2<double> tVector2<double>::POSITIVE_X(1.0, 0.0);
template<> tVector2<double> tVector2<double>::NEGATIVE_X(-1.0, 0.0);
template<> tVector2<double> tVector2<double>::POSITIVE_Y(0.0, 1.0);
template<> tVector2<double> tVector2<double>::NEGATIVE_Y(0.0, -1.0);



template <class T>
tVector3<T> tVector2<T>::cross(const tVector2 &v) const
{
    return tVector3<T>(0.0,
                   0.0,
                   (_v[0] * v._v[1]) - (_v[1] * v._v[0]));
}


template <class T>
tVector2<T> &tVector2<T>::rotate(T theta)
{
    T c = cos(theta);
    T s = sin(theta);
    T vx = _v[0];
    T vy = _v[1];
    _v[0] = (vx * c) - (vy * s);
    _v[1] = (vx * s) + (vy * c);
    return *this;
}

template class tVector2<float>;
template class tVector2<double>;

GTB_END_NAMESPACE
