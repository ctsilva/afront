GTB_BEGIN_NAMESPACE


template <class T>
inline bool tCoordinateSystem<T>::operator==(const tCoordinateSystem &cs)
{
    return ((m_x == cs.m_x)
            && (m_y == cs.m_y)
            && (m_z == cs.m_z));
}


template <class T>
inline bool tCoordinateSystem<T>::operator!=(const tCoordinateSystem &cs)
{
    return !(*this == cs);
}


template <class T>
inline const typename tCoordinateSystem<T>::Point3 &tCoordinateSystem<T>::origin() const
{
    return m_origin;
}


template <class T>
inline const typename tCoordinateSystem<T>::Vector3 &tCoordinateSystem<T>::x() const
{
    return m_x;
}


template <class T>
inline const typename tCoordinateSystem<T>::Vector3 &tCoordinateSystem<T>::y() const
{
    return m_y;
}


template <class T>
inline const typename tCoordinateSystem<T>::Vector3 &tCoordinateSystem<T>::z() const
{
    return m_z;
}


template <class T>
inline tCoordinateSystem<T> &tCoordinateSystem<T>::reset(const Point3 &a_origin,
						 const Vector3 &a_x,
						 const Vector3 &a_y,
						 const Vector3 &a_z)
{
    m_origin = a_origin;
    m_x = a_x;
    m_y = a_y;
    m_z = a_z;
    return *this;
}


GTB_END_NAMESPACE
