GTB_BEGIN_NAMESPACE


template <class T>
inline tCircle3<T> &tCircle3<T>::operator=(const tCircle3 &c)
{
    if (&c != this) {
        _center = c._center;
        _radius = c._radius;
    }
    return *this;
}


template <class T>
inline bool tCircle3<T>::operator==(const tCircle3 &c) const
{
    return ((_center == c._center) &&
            (_normal == c._normal) &&
            (real::is_equal(_radius, c._radius)));
}


template <class T>
inline bool tCircle3<T>::operator!=(const tCircle3 &c) const
{
    return !(*this == c);
}


template <class T>
inline const typename tCircle3<T>::Point3 &tCircle3<T>::center() const
{
    return _center;
}


template <class T>
inline const typename tCircle3<T>::Vector3 &tCircle3<T>::normal() const
{
    return _normal;
}


template <class T>
inline typename tCircle3<T>::value_type tCircle3<T>::radius() const
{
    return _radius;
}


GTB_END_NAMESPACE
