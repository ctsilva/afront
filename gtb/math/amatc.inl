
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
AMatc<T>::AMatc(int row, int col, const_pointer v, bool ftn) : M(row), N(col)
{
    allocate();
    if (v) assign(v, ftn);
}

template<class T>
AMatc<T>::AMatc(const AMatc<T>& rhs) : M(rhs.M), N(rhs.N)
{
    allocate();
    std::copy(rhs.m, rhs.m+M*N, m);
}

template<class T>
AMatc<T>::~AMatc()
{
    cleanup();
}


template<class T>
void AMatc<T>::set(const T& value)
{
    pointer p = m;
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            *p = value;
            ++p;
        }
    }
}


template<class T>
inline typename AMatc<T>::reference AMatc<T>::operator() (int i, int j)
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i+j*M];
}

template<class T>
inline typename AMatc<T>::const_reference AMatc<T>::operator() (int i, int j) const
{
    assert ((i>=0) && (i<M));
    assert ((j>=0) && (j<N));
    return m[i+j*M];
}

