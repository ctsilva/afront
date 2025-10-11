#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/math/math.hpp>
#endif // WIN32

GTB_BEGIN_NAMESPACE

#if 0
template <class T>
void spherical_to_euclidian(T phi, T theta, tVector3<T>& d)
{
    d[0] = (T)cos(phi)*cos(theta);
    d[1] = (T)sin(phi)*cos(theta);
    d[2] = (T)sin(theta);
}
template void spherical_to_euclidian<float>(float phi, float theta, tVector3<float>& d);
template void spherical_to_euclidian<double>(double phi, double theta, tVector3<double>& d);

template <class T>
void euclidian_to_spherical(const tVector3<T>& d, T& phi, T& theta)
{
    theta = (T)(asin(d[2]));
#if 0
    T cos_theta = cos(theta);
    T phi=asin(d[1]/cos_theta);
    T phi=acos(d[0]/cos_theta);
    phi = ac_phi;
    if (as_phi < 0)
    {
       phi = 2*M_PI+as_phi;
    }
    else if (ac_phi < 0) phi = ac_phi + M_PI;
#else
    phi = (T)(atan2(d[1], d[0]));
#endif
}
template void euclidian_to_spherical<float>(const tVector3<float>& d, float& phi, float& theta);
template void euclidian_to_spherical<double>(const tVector3<double>& d, double& phi, double& theta);
#endif

GTB_END_NAMESPACE

#ifdef OUTLINE
#define inline
#include <gtb/math/math.ipp>
#undef inline
#endif
