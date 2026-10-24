
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


#include <assert.h>


GTB_BEGIN_NAMESPACE


inline int IndexedTriangle::A() const
{
	return _indices[0];
}


inline int IndexedTriangle::B() const
{
	return _indices[1];
}


inline int IndexedTriangle::C() const
{
	return _indices[2];
}


inline bool IndexedTriangle::operator==(const IndexedTriangle &t) const
{
	return ((A() == t.A()) &&
		(B() == t.B()) &&
		(C() == t.C()));
}


inline bool IndexedTriangle::operator!=(const IndexedTriangle &t) const
{
	return !(*this == t);
}


inline int IndexedTriangle::operator[](unsigned i) const
{
	assert(i < 3);
	return _indices[i];
}


inline int &IndexedTriangle::operator[](unsigned i)
{
	assert(i < 3);
	return _indices[i];
}


inline void IndexedTriangle::reset(int a, int b, int c)
{
	_indices[0] = a;
	_indices[1] = b;
	_indices[2] = c;
}

inline const int* IndexedTriangle::get_indices() const
{
    return _indices;
}

inline int* IndexedTriangle::get_indices()
{
    return _indices;
}

GTB_END_NAMESPACE
