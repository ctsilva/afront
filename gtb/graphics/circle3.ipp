
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
