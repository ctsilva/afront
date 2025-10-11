#include <gtb/math/math.hpp>


GTB_BEGIN_NAMESPACE


template<class T>
inline const typename tCamera<T>::Point3 &tCamera<T>::origin() const
{
    return _cs.origin();
}

template<class T>
inline const typename tCamera<T>::Point3 &tCamera<T>::get_origin() const
{
    return _cs.origin();
}


template<class T>
inline const typename tCamera<T>::Vector3 &tCamera<T>::backwards() const
{
    return _cs.z();
}


template<class T>
inline typename tCamera<T>::Vector3 tCamera<T>::towards() const
{
    return -_cs.z();
}


template<class T>
inline const typename tCamera<T>::Vector3 &tCamera<T>::up() const
{
    return _cs.y();
}


template<class T>
inline const typename tCamera<T>::Vector3 &tCamera<T>::get_up() const
{
    return _cs.y();
}


template<class T>
inline typename tCamera<T>::Vector3 tCamera<T>::down() const
{
    return -_cs.y();
}


template<class T>
inline const typename tCamera<T>::Vector3 &tCamera<T>::right() const
{
    return _cs.x();
}


template<class T>
inline const typename tCamera<T>::Vector3 &tCamera<T>::get_right() const
{
    return _cs.x();
}


template<class T>
inline typename tCamera<T>::Vector3 tCamera<T>::left() const
{
    return -_cs.x();
}


template<class T>
inline typename tCamera<T>::value_type tCamera<T>::x_fov() const
{
    return _x_fov;
}


template<class T>
inline typename tCamera<T>::value_type tCamera<T>::y_fov() const
{
    return _y_fov;
}


template<class T>
inline typename tCamera<T>::value_type tCamera<T>::aspect() const
{
    // value_type w = 2.0 * _near_distance * tan(_x_fov);
    // value_type h = 2.0 * _near_distance * tan(_y_fov);
    // return w / h;
    return tan(_x_fov) / tan(_y_fov);
}


template<class T>
inline typename tCamera<T>::value_type tCamera<T>::near_distance() const
{
    return _near_distance;
}


template<class T>
inline typename tCamera<T>::value_type tCamera<T>::far_distance() const
{
    return _far_distance;
}


template<class T>
inline const tCoordinateSystem<T> &tCamera<T>::coordinate_system() const
{
    return _cs;
}


template<class T>
inline typename tCamera<T>::Matrix4 tCamera<T>::matrix() const
{
    return _cs.matrix();
}


template<class T>
inline typename tCamera<T>::Matrix4 tCamera<T>::inverse_matrix() const
{
    return _cs.inverse_matrix();
}


template<class T>
inline bool tCamera<T>::sees(const Point3 &p) const
{
    for (unsigned i = 0; i < 6; i++) {
        if (_planes[i].signed_distance(p) <= 0.0) {
            return false;
        }
    }
    return true;
}


template<class T>
inline void tCamera<T>::pitch(value_type angle)
{
    rotate(tLine3<T>(origin(), right()), angle);
}


template<class T>
inline void tCamera<T>::yaw(value_type angle)
{
    rotate(tLine3<T>(origin(), up()), angle);
}


template<class T>
inline void tCamera<T>::roll(value_type angle)
{
    rotate(tLine3<T>(origin(), towards()), angle);
}


GTB_END_NAMESPACE
