#ifndef __RSTPLANE
#define __RSTPLANE

#include <gtb/common.hpp>
#include <gtb/real/real.hpp>
#include <gtb/math/math.hpp>
#include <gtb/graphics/vector3.hpp>

GTB_BEGIN_NAMESPACE

/*
 * This class represents a plane in 3D
 * using two angles and a distance from the origin
 */
template <class T>
struct tCPlaneRST
{
    typedef T value_type;

    tCPlaneRST() {}
    tCPlaneRST(value_type v_r, value_type v_s, value_type v_t) : r(v_r), s(v_s), t(v_t) {}

    value_type r,s; // Angles
    value_type t;   // Distance from the origin
};

typedef tCPlaneRST<float> CPlaneRSTf;
typedef tCPlaneRST<double> CPlaneRSTd;
#if defined(REAL_IS_FLOAT)
typedef CPlaneRSTf CPlaneRST;
#else
typedef CPlaneRSTd CPlaneRST;
#endif


GTB_END_NAMESPACE

#endif // __RSTPLANE
