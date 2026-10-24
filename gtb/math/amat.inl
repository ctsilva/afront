
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


template<class T>
inline AVec<T>::AVec(const AVec& rhs) : N(rhs.N)
{
    allocate();
    copy(rhs.v);
}

template<class T>
inline AVec<T>::~AVec()
{
    cleanup();
}

template<class T>
inline typename AVec<T>::reference AVec<T>::operator[](int n)
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline typename AVec<T>::const_reference AVec<T>::operator[](int n) const
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline typename AVec<T>::reference AVec<T>::operator()(int n)
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline typename AVec<T>::const_reference AVec<T>::operator()(int n) const
{
    assert((n>=0) && (n<N));
    return v[n];
}

template<class T>
inline void AVec<T>::cleanup()
{
    delete[] v;
}


/* ---------------------- amat ---------------------- */
template<class T>
inline typename AMat<T>::reference AMat<T>::operator() (int i, int j)
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i][j];
}

template<class T>
inline typename AMat<T>::const_reference AMat<T>::operator() (int i, int j) const
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i][j];
}

