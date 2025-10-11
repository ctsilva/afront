GTB_BEGIN_NAMESPACE


template <class T>
inline bool tSphere<T>::operator==(const tSphere &s) const
{
	return ((_center == s._center) &&
		(real::is_equal(_radius, s._radius)));
}


template <class T>
inline bool tSphere<T>::operator!=(const tSphere &s) const
{
	return !(*this == s);
}


template <class T>
inline tSphere<T> &tSphere<T>::operator=(const tSphere<T> &s)
{
	if (&s != this) {
		_center = s._center;
		_radius = s._radius;
	}
	return *this;
}


template <class T>
inline const typename tSphere<T>::Point3 &tSphere<T>::center() const
{
	return _center;
}

template <class T>
inline typename tSphere<T>::Point3 tSphere<T>::get_center() const
{
	return _center;
}


template <class T>
inline typename tSphere<T>::value_type tSphere<T>::radius() const
{
	return _radius;
}

template <class T>
inline typename tSphere<T>::value_type tSphere<T>::get_radius() const
{
	return _radius;
}

template <class T>
inline void tSphere<T>::set_radius(typename tSphere<T>::value_type v)
{
	_radius = v;
}

template <class T>
inline void tSphere<T>::set_center(const typename tSphere<T>::Point3 &c)
{
	_center = c;
}

template <class T>
inline bool tSphere<T>::contains(const typename tSphere<T>::Point3 &p) const
{
	return (p - _center).squared_length() <= _radius * _radius;
}

template <class T>
inline bool tSphere<T>::contains_approx(const typename tSphere<T>::Point3 &p) const
{
	const value_type EPSILON = (value_type).0000000001;
	tVector3<T> ray = (p - _center);
	value_type length = ray.squared_length();
	value_type rad_sq = _radius * _radius;
	bool b1 = (length < rad_sq);
	bool b2 = (length < (rad_sq + EPSILON)) && (length >(rad_sq - EPSILON));
	return b1 && !b2;
}

GTB_END_NAMESPACE
