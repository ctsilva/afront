
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


// Map a tuple of triangular (pyramidal, etc) coordinates 
// to a 1D layout

template <int D>
inline int triangular_map_sum(const mp::Tuple<int, D> &x);
template <int D>
inline int triangular_map(const mp::Tuple<int, D> &x);
template <int D>
inline int triangular_map(const int *x);
template <int D>
inline int triangular_map_sum(const int *x);

template <>
inline int triangular_map_sum(const mp::Tuple<int, 1> &x)
{
    return x._value;
}
template <int D>
inline int triangular_map_sum(const mp::Tuple<int, D> &x)
{
    return x._value + triangular_map_sum(x._rest);
}

template <>
inline int triangular_map_sum<1>(const int *x)
{
    return *x;
}
template <int D>
inline int triangular_map_sum(const int *x)
{
    return *x + triangular_map_sum<D-1>(x+1);
}

template <>
inline int triangular_map(const mp::Tuple<int, 1> &x)
{
    return x._value;
}

template <int D>
inline int triangular_map(const mp::Tuple<int, D> &x)
{
    mp::ChooseK<D> p;
    return triangular_map(x._rest) + 
	p(triangular_map_sum(x)+D-1);
}

template <>
inline int triangular_map<1>(const int *x) { return *x; }

template <int D>
inline int triangular_map(const int *x) 
{ 
    mp::ChooseK<D> p;
    return triangular_map<D-1>(x+1) + p(triangular_map_sum<D>(x)+D-1);
}
