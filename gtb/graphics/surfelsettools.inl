
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

//
// KDTREE helper class for surfelsets
//
template <class T>
inline ss_kdtree<T>::ss_kdtree(T& points) :
    tree(0),
    _points(points)
{
    rebuild();
}

template <class T>
inline void ss_kdtree<T>::rebuild(const tBox3<value_type>* ibbox)
{
    if ((ibbox==0) && (_points.size() == 0)) return;

    tBox3<value_type> bbox;
    const tBox3<value_type>* pbbox;
    
    if (ibbox)
    {
        pbbox = ibbox;
    }
    else
    {
        bbox = _points.bounding_box();
        bbox.scale(1.1);
        pbbox = &bbox;
    }

	tree = new t_surfel_tree(10, *pbbox, gen_GetPoint_f(_points));
    for (unsigned i = 0; i < _points.size(); ++i)
    {
        tree->Insert(i);
    }

    tree->MakeTree();
}

template <class T>
inline void ss_kdtree<T>::extract(const tPoint3<value_type>& x, value_type radius, tsurfelset_view<value_type>& NN) const
{
    tree->Extract(x, KDT_INFINIT_NEIGHBORS, radius, std::back_inserter(NN.get_view()));
}

template <class T>
inline void ss_kdtree<T>::extract(const tPoint3<value_type>& x, unsigned K, tsurfelset_view<value_type>& NN) const
{
    tree->Extract(x, K, 1e22, std::back_inserter(NN.get_view()));
}

template <class T>
inline T& ss_kdtree<T>::get_points()
{
    return _points;
}

GTB_END_NAMESPACE
