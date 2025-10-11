//#include <gtb/graphics/vector3.hpp>
//#include <gtb/graphics/line3.hpp>
//#include <gtb/graphics/box3.hpp>


GTB_BEGIN_NAMESPACE


template<class T>
inline tSegment3<T>::tSegment3()
    //	: _p(0.0, 0.0, 0.0),
    //	  _q(1.0, 0.0, 0.0)
{
}


template<class T>
inline tSegment3<T>::tSegment3(const tSegment3 &s)
	: _p(s._p),
	  _q(s._q)
{
}


template<class T>
inline tSegment3<T>::tSegment3(const Point3 &p, const Point3 &q)
	: _p(p),
	  _q(q)
{
}


template<class T>
inline tSegment3<T> &tSegment3<T>::operator=(const tSegment3 &s)
{
	_p = s._p;
	_q = s._q;
	return *this;
}


template<class T>
inline bool tSegment3<T>::operator==(const tSegment3 &s) const
{
	return (_p == s._p) && (_q == s._q);
}


template<class T>
inline bool tSegment3<T>::operator!=(const tSegment3 &s) const
{
	return !(*this == s);
}


template<class T>
inline const typename tSegment3<T>::Point3& tSegment3<T>::source() const
{
	return _p;
}


template<class T>
inline const typename tSegment3<T>::Point3 &tSegment3<T>::target() const
{
	return _q;
}


template<class T>
inline const typename tSegment3<T>::Point3 &tSegment3<T>::min() const
{
	if (_p.x() < _q.x()) {
		return _p;
	} else if (_p.x() > _q.x()) {
		return _q;
	} else if (_p.y() < _q.y()) {
		return _p;
	} else if (_p.y() > _q.y()) {
		return _q;
	} else if (_p.z() < _q.z()) {
		return _p;
	} else {
		return _q;
	}
}


template<class T>
inline const typename tSegment3<T>::Point3 &tSegment3<T>::max() const
{
	if (_p.x() > _q.x()) {
		return _p;
	} else if (_p.x() < _q.x()) {
		return _q;
	} else if (_p.y() > _q.y()) {
		return _p;
	} else if (_p.y() < _q.y()) {
		return _q;
	} else if (_p.z() > _q.z()) {
		return _p;
	} else {
		return _q;
	}
}


template<class T>
inline typename tSegment3<T>::value_type tSegment3<T>::squared_length() const
{
	return (_q - _p).squared_length();
}

template<class T>
inline typename tSegment3<T>::value_type tSegment3<T>::length() const
{
    return sqrt(squared_length());
}

template<class T>
inline typename tSegment3<T>::Vector3 tSegment3<T>::direction() const
{
	return (_q - _p);
}


template<class T>
inline tLine3<T> tSegment3<T>::supporting_line() const
{
	return tLine3<T>(_p, _q);
}


template<class T>
inline tBox3<T> tSegment3<T>::bounding_box() const
{
	return tBox3<T>(_p, _q);
}


template<class T>
inline bool tSegment3<T>::is_degenerate() const
{
	return _p == _q;
}


template<class T>
inline bool tSegment3<T>::collinear_contains(const Point3 &p) const
{
	value_type t = (p - _p).dot(direction());
	return (t >= 0.0) && (t <= squared_length());
}


template<class T>
inline bool tSegment3<T>::contains(const Point3 &p) const
{
	if (supporting_line().contains(p)) {
		return collinear_contains(p);
	} else {
		return false;
	}
}


template<class T>
inline tSegment3<T> tSegment3<T>::operator-() const
{
	return tSegment3(_q, _p);
}


template<class T>
inline T tSegment3<T>::squared_distance(const Point3& p)
{
    Vector3 d = _p - _q; 
    Vector3 p_p = p-_p;
    if (d.dot(p_p) > 0) return p_p.squared_length();
    Vector3 p_q = p-_q;
    if (d.dot(p_q) < 0) return p_q.squared_length();
    else
    {
        Line3 sl(_q, d);
        T d = sl.distance(p);
        return d*d;
    }
}

template<class T>
inline T tSegment3<T>::distance(const Point3& p)
{
    Vector3 d = _p - _q; 
    Vector3 p_p = p-_p;
    if (d.dot(p_p) > 0) return p_p.length();
    Vector3 p_q = p-_q;
    if (d.dot(p_q) < 0) return p_q.length();
    else
    {
        Line3 sl(_q, d);
        T d = sl.distance(p);
        return d;
    }
}

GTB_END_NAMESPACE
