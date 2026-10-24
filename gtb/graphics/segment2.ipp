
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


#include <gtb/graphics/vector2.hpp>
#include <gtb/graphics/line2.hpp>
#include <gtb/graphics/box2.hpp>


GTB_BEGIN_NAMESPACE


template<class T>
inline tSegment2<T>::tSegment2()
    //	: _p(0.0, 0.0, 0.0),
    //	  _q(1.0, 0.0, 0.0)
{
}


template<class T>
inline tSegment2<T>::tSegment2(const tSegment2 &s)
	: _p(s._p),
	  _q(s._q)
{
}


template<class T>
inline tSegment2<T>::tSegment2(const Point2 &p, const Point2 &q)
	: _p(p),
	  _q(q)
{
}


template<class T>
inline tSegment2<T> &tSegment2<T>::operator=(const tSegment2 &s)
{
	_p = s._p;
	_q = s._q;
	return *this;
}


template<class T>
inline bool tSegment2<T>::operator==(const tSegment2 &s) const
{
	return (_p == s._p) && (_q == s._q);
}


template<class T>
inline bool tSegment2<T>::operator!=(const tSegment2 &s) const
{
	return !(*this == s);
}


template<class T>
inline const typename tSegment2<T>::Point2& tSegment2<T>::source() const
{
	return _p;
}


template<class T>
inline const typename tSegment2<T>::Point2 &tSegment2<T>::target() const
{
	return _q;
}


template<class T>
inline const typename tSegment2<T>::Point2 &tSegment2<T>::min() const
{
	if (_p.x() < _q.x()) {
		return _p;
	} else if (_p.x() > _q.x()) {
		return _q;
	} else if (_p.y() < _q.y()) {
		return _p;
	} else {
		return _q;
	}
}


template<class T>
inline const typename tSegment2<T>::Point2 &tSegment2<T>::max() const
{
	if (_p.x() > _q.x()) {
		return _p;
	} else if (_p.x() < _q.x()) {
		return _q;
	} else if (_p.y() > _q.y()) {
		return _p;
	} else {
		return _q;
	}
}


template<class T>
inline typename tSegment2<T>::value_type tSegment2<T>::squared_length() const
{
	return (_q - _p).squared_length();
}

template<class T>
inline typename tSegment2<T>::value_type tSegment2<T>::length() const
{
    return sqrt(squared_length());
}

template<class T>
inline typename tSegment2<T>::Vector2 tSegment2<T>::direction() const
{
	return (_q - _p);
}


template<class T>
inline tLine2<T> tSegment2<T>::supporting_line() const
{
	return tLine2<T>(_p, _q);
}


template<class T>
inline tBox2<T> tSegment2<T>::bounding_box() const
{
	return tBox2<T>(_p, _q);
}


template<class T>
inline bool tSegment2<T>::is_degenerate() const
{
	return _p == _q;
}


template<class T>
inline bool tSegment2<T>::collinear_contains(const Point2 &p) const
{
	value_type t = (p - _p).dot(direction());
	return (t >= 0.0) && (t <= squared_length());
}


template<class T>
inline bool tSegment2<T>::contains(const Point2 &p) const
{
	if (supporting_line().contains(p)) {
		return collinear_contains(p);
	} else {
		return false;
	}
}


template<class T>
inline tSegment2<T> tSegment2<T>::operator-() const
{
	return tSegment2(_q, _p);
}


template<class T>
inline T tSegment2<T>::squared_distance(const Point2& p) const
{
    Vector2 d = _p - _q; 
    Vector2 p_p = p-_p;
    if (d.dot(p_p) > 0) return p_p.squared_length();
    Vector2 p_q = p-_q;
    if (d.dot(p_q) < 0) return p_q.squared_length();
    else
    {
        Line2 sl(_q, d);
        T d = sl.distance(p);
        return d*d;
    }
}

template<class T>
inline T tSegment2<T>::distance(const Point2& p) const
{
    Vector2 d = _p - _q; 
    Vector2 p_p = p-_p;
    if (d.dot(p_p) > 0) return p_p.length();
    Vector2 p_q = p-_q;
    if (d.dot(p_q) < 0) return p_q.length();
    else
    {
        Line2 sl(_q, d);
        T d = sl.distance(p);
        return d;
    }
}

template <class T>
inline T tSegment2<T>::D() const
{
    return supporting_line().distance(Point2::ZERO);
}

template <class T>
inline tVector2<T> tSegment2<T>::normal() const
{
    return supporting_line().normal();
}

GTB_END_NAMESPACE
