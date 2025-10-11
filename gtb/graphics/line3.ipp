GTB_BEGIN_NAMESPACE

template<class T>
inline tLine3<T>::tLine3()
//	: _p(tPoint3<T>(0.0, 0.0, 0.0)),
//	  _d(tVector3<T>(1.0, 0.0, 0.0))
{
}


template<class T>
inline tLine3<T>::tLine3(const tLine3 &l)
	: _p(l._p),
	  _d(l._d)
{
}


template<class T>
inline tLine3<T>::tLine3(const tPoint3<T>&p, const tPoint3<T>&q)
	: _p(p),
	  _d(q - p)
{
    _d.normalize();
}


template<class T>
inline tLine3<T>::tLine3(const tPoint3<T>&p, const tVector3<T> &d)
	: _p(p),
	  _d(d)
{
    _d.normalize();
}


#if 0
template<class T>
inline tLine3<T>::tLine3(const tRay3<T> &r)
	: _p(r.source()),
	  _d(r.direction())
{
}
#endif

template<class T>
inline tPoint3<T> tLine3<T>::point(value_type t) const
{
    return _p + _d*t;
}


template<class T>
inline tLine3<T> &tLine3<T>::operator=(const tLine3 &l)
{
    if (&l != this) {
        _p = l._p;
        _d = l._d;
    }
    return *this;
}


template<class T>
inline bool tLine3<T>::contains(const Point3 &p) const
{
    return (p - _p).cross(_d).is_zero();
}


template<class T>
inline bool tLine3<T>::operator==(const tLine3 &l) const
{
    return (_d == l._d) && contains(l._p);
}


template<class T>
inline bool tLine3<T>::operator!=(const tLine3 &l) const
{
    return !(*this == l);
}


template<class T>
inline tLine3<T> tLine3<T>::operator-() const
{
    return tLine3(_p, -_d);
}


template<class T>
inline tVector3<T> &tLine3<T>::direction()
{
    return _d;
}

template<class T>
inline const tVector3<T> &tLine3<T>::direction() const
{
    return _d;
}

template<class T>
inline typename tLine3<T>::Point3& tLine3<T>::origin()
{
    return _p;
}

template<class T>
inline const typename tLine3<T>::Point3 &tLine3<T>::origin() const
{
	return _p;
}

template<class T>
inline bool tLine3<T>::is_degenerate() const
{
	return _d.is_zero();
}


template<class T>
inline typename tLine3<T>::Point3 tLine3<T>::point(int i) const
{
	return _p + ((value_type)i * _d);
}


template<class T>
inline typename tLine3<T>::Point3 tLine3<T>::projection(const Point3 &p) const
{
	return _p + ((p - _p).dot(_d)) * _d;
}

template<class T>
inline typename tLine3<T>::value_type tLine3<T>::distance(const Point3& p) const
{
    return _d.cross(p-origin()).length();
}



GTB_END_NAMESPACE
