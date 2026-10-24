
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


template <MP_INTEGER N, MP_INTEGER K> struct Choose;

////////////////////////////////////////////////////////////
// Completely compile-time Choose
template <MP_INTEGER N, MP_INTEGER K>
    struct Choose;

template <MP_INTEGER N>
struct Choose<N, 0>
{
    enum { Value = 1 };
};

template <MP_INTEGER N, MP_INTEGER K>
struct Choose
{
    enum { Value = (N * Choose<N-1,K-1>::Value) / K };
};

////////////////////////////////////////////////////////////
// Choose with compile-time k

template <int K> struct ChooseK;
 
template <>
struct ChooseK<0>
{
    int operator()(int) { return 1; }
};

template <int K>
struct ChooseK
{
    // if n >= k, this never truncates!
    // Watch the parentheses order and prove by induction..
    int operator()(int n) {
	ChooseK<K-1> c;
	return (n * c(n-1)) / K;
    };
};

