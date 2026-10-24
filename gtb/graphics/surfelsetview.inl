
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
inline const tPoint3<T>& tsurfelset_view<T>::vertex(unsigned i) const
{
    assert(i < _view.size());
    return _surfelset.vertex(_view[i]);
}

template <class T>
inline const tPoint3<T>& tsurfelset_view<T>::vertex(rit i) const
{
    assert(*i < _view.size());
    return _surfelset.vertex(_view[*i]);
}

template <class T>
inline const tPoint3<T>& tsurfelset_view<T>::vertex(const_rit i) const
{
    assert(*i < _view.size());
    return _surfelset.vertex(_view[*i]);
}

template <class T>
inline const tVector3<T>& tsurfelset_view<T>::normal(unsigned i) const
{
    assert(i < _view.size());
    return _surfelset.normal(_view[i]);
}

template <class T>
inline const ColorRgb& tsurfelset_view<T>::vertex_color(unsigned i) const
{
    assert(i < _view.size());
    return _surfelset.vertex_color(_view[i]);
}

template <class T>
inline unsigned tsurfelset_view<T>::get_index(unsigned idx) const
{
    assert(idx < _view.size());
    return _view[idx];
}

template <class T>
inline void tsurfelset_view<T>::set_index(unsigned idx, unsigned pidx)
{
    assert(idx < _view.size());
    _view[idx] = pidx;
}

GTB_END_NAMESPACE
