GTB_BEGIN_NAMESPACE

template <class T>
inline tLine2<T>::tLine2()
{
}

template <class T>
inline tLine2<T>::tLine2(const tLine2 &l)
    : _p(l. _p),
      _d(l. _d)
{
}

template <class T>
inline tLine2<T>::tLine2(const tPoint2<T>&p, const tPoint2<T> &q)
    : _p(p),
      _d(q - p)
{
    _d.normalize();
}

template <class T>
inline tLine2<T>::tLine2(const tPoint2<T> &p, const tVector2<T> &d)
    : _p(p),
      _d(d)
{
    _d.normalize();
}


template<class T>
inline tPoint2<T> tLine2<T>::point(value_type t) const
{
    return _p + _d*t;
}


template<class T>
inline tLine2<T> &tLine2<T>::operator=(const tLine2 &l)
{
    if (&l != this) {
        _p = l._p;
        _d = l._d;
    }
    return *this;
}


template<class T>
inline bool tLine2<T>::contains(const Point2 &p) const
{
    return (p - _p).cross(_d).is_zero();
}


template<class T>
inline bool tLine2<T>::operator==(const tLine2 &l) const
{
    return (_d == l._d) && contains(l._p);
}


template<class T>
inline bool tLine2<T>::operator!=(const tLine2 &l) const
{
    return !(*this == l);
}


template<class T>
inline tLine2<T> tLine2<T>::operator-() const
{
    return tLine2(_p, -_d);
}


template<class T>
inline tVector2<T> &tLine2<T>::direction()
{
    return _d;
}

template<class T>
inline const tVector2<T> &tLine2<T>::direction() const
{
    return _d;
}

template<class T>
inline typename tLine2<T>::Point2& tLine2<T>::origin()
{
    return _p;
}

template<class T>
inline const typename tLine2<T>::Point2 &tLine2<T>::origin() const
{
	return _p;
}

template<class T>
inline bool tLine2<T>::is_degenerate() const
{
	return _d.is_zero();
}


template<class T>
inline typename tLine2<T>::Point2 tLine2<T>::point(int i) const
{
	return _p + ((value_type)i * _d);
}


template<class T>
inline typename tLine2<T>::Point2 tLine2<T>::projection(const Point2 &p) const
{
	return _p + ((p - _p).dot(_d)) * _d;
}

template<class T>
inline typename tLine2<T>::value_type tLine2<T>::distance(const Point2& p) const
{
    return _d.cross(p-origin()).length();
}

template<class T>
inline tVector2<T> tLine2<T>::normal() const
{
    return _d.normal();
}


GTB_END_NAMESPACE
