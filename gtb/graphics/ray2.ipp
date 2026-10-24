
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


#include <gtb/graphics/line2.hpp>


GTB_BEGIN_NAMESPACE


template<class T>
inline tRay2<T> &tRay2<T>::operator=(const tRay2 &r)
{
	if (&r != this) {
		_source = r._source;
		_direction = r._direction;
	}
	return *this;
}


template<class T>
inline bool tRay2<T>::operator==(const tRay2 &r) const
{
	return (_source == r._source) && (_direction == r._direction);
}


template<class T>
inline bool tRay2<T>::operator!=(const tRay2 &r) const
{
	return !(*this == r);
}


template<class T>
inline tRay2<T> tRay2<T>::operator-() const
{
	return tRay2(_source, -_direction);
}


template<class T>
inline const tPoint2<T> &tRay2<T>::source() const
{
	return _source;
}


template<class T>
inline const tVector2<T> &tRay2<T>::direction() const
{
	return _direction;
}


template<class T>
inline bool tRay2<T>::is_degenerate() const
{
	return !real::is_equal(_direction.length(), 1.0);
}


template<class T>
inline const tPoint2<T> tRay2<T>::point(value_type a) const
{
	return _source + (a * _direction);
}


template<class T>
inline typename tRay2<T>::value_type tRay2<T>::t(const tPoint2<T> &p) const
{
	return (p - _source).dot(_direction);
}


template<class T>
inline tLine2<T> tRay2<T>::supporting_line() const
{
//	return tLine2<T>(*this);
    return tLine2<T>(source(), direction());
}


template<class T>
inline bool tRay2<T>::contains(const tPoint2<T> &p) const
{
	return (p - _source).cross(_direction).is_zero() && (t(p) >= 0.0);
}


template<class T>
inline tPoint2<T> tRay2<T>::projection(const tPoint2<T> &p) const
{
	return _source + t(p) * _direction;
}


GTB_END_NAMESPACE
