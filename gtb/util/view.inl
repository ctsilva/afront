GTB_BEGIN_NAMESPACE

template <class T>
inline SequenceView<T>::SequenceView(parent_type& parent) :
_parent(parent)
{
}

// vector like behaviour
template <class T>
inline typename SequenceView<T>::reference SequenceView<T>::operator [](size_type idx)
{
    assert(idx < _indices.size());
    assert(_indices[idx] < _parent.size());
    return _parent[_indices[idx] ];
}

template <class T>
inline typename SequenceView<T>::const_reference SequenceView<T>::operator[](size_type idx) const
{
    assert(idx < _indices.size());
    assert(_indices[idx] < _parent.size());
    return _parent[_indices[idx] ];
}

template <class T>
inline typename SequenceView<T>::reference SequenceView<T>::at(size_type idx)
{
    return (*this)[idx];
}

template <class T>
inline typename SequenceView<T>::const_reference SequenceView<T>::at(size_type idx) const
{
    return (*this)[idx];
}

template <class T>
inline typename SequenceView<T>::reference SequenceView<T>::back( )
{
    return at(_indices.back());
}

template <class T>
inline typename SequenceView<T>::const_reference SequenceView<T>::back( ) const
{
    return at(_indices.back());
}

template <class T>
inline typename SequenceView<T>::const_iterator SequenceView<T>::begin( ) const
{
    return typename SequenceView<T>::const_iterator(_indices.begin(), &_parent);
}

template <class T>
inline typename SequenceView<T>::iterator SequenceView<T>::begin( )
{
    return typename SequenceView<T>::iterator(_indices.begin(), &_parent);
}

template <class T>
inline void SequenceView<T>::clear( )
{
    _indices.clear();
}

template <class T>
inline bool SequenceView<T>::empty( ) const
{
    return _indices.empty();
}

template <class T>
inline typename SequenceView<T>::iterator SequenceView<T>::end( )
{
    return iterator(_indices.end(), &_parent);
}


template <class T>
inline typename SequenceView<T>::const_iterator SequenceView<T>::end( ) const
{
    return const_iterator(_indices.end(), &_parent);
}

template <class T>
inline typename SequenceView<T>::iterator SequenceView<T>::erase(iterator where)
{
    _indices.erase(where);
}

template <class T>
inline typename SequenceView<T>::iterator SequenceView<T>::erase(iterator first, iterator last)
{
    return iterator(_indices.erase(first._idx, last._idx), &_parent);
}

template <class T>
inline typename SequenceView<T>::reference SequenceView<T>::front( )
{
    return at(_indices.front());
}

template <class T>
inline typename SequenceView<T>::const_reference SequenceView<T>::front( ) const
{
    return at(_indices.front());
}

template <class T>
inline typename SequenceView<T>::reverse_iterator SequenceView<T>::rbegin( )
{
    return _indices.rbegin();
}

template <class T>
inline typename SequenceView<T>::const_reverse_iterator SequenceView<T>::rbegin( ) const
{
    return _indices.rbegin();
}

template <class T>
inline typename SequenceView<T>::const_reverse_iterator SequenceView<T>::rend( ) const
{
    return _indices.rend();
}

template <class T>
inline typename SequenceView<T>::reverse_iterator SequenceView<T>::rend( )
{
    return _indices.rend();
}


template <class T>
inline typename SequenceView<T>::size_type SequenceView<T>::size() const
{
    return _indices.size();
}

// special stuff
template <class T>
inline void SequenceView<T>::insert_index(indices_type::value_type idx)
{
    _indices.push_back(idx);
}

template <class T>
inline typename SequenceView<T>::indices_type& SequenceView<T>::get_indices_vector()
{
    return _indices;
}

template <class T>
inline typename SequenceView<T>::parent_type& SequenceView<T>::get_parent( )
{
    return _parent;
}

template <class T>
inline unsigned SequenceView<T>::index(int idx)
{
    return _indices[idx];
}
    


////////////////////// ITERATOR /////////////////////////
template <class T>
SequenceView<T>::iterator::iterator() : const_iterator()
{
}

template <class T>
SequenceView<T>::iterator::iterator(const iterator& rhs) : 
    const_iterator(rhs._idx, rhs._parent)
{
}

template <class T>
SequenceView<T>::iterator::iterator(indices_type::iterator idx, parent_type* parent) :
    const_iterator(idx, parent)
{
}

template <class T>
typename SequenceView<T>::iterator::reference SequenceView<T>::iterator::operator*() const
{	// return designated object
    assert(_parent != 0);
    return (*_parent)[*_idx];
}

template <class T>
typename SequenceView<T>::iterator::pointer SequenceView<T>::iterator::operator->() const
{	// return pointer to class object
    return (&(*this));
}

template <class T>
typename SequenceView<T>::iterator& SequenceView<T>::iterator::operator++()
{	// preincrement
    ++_idx;
    return (*this);
}

template <class T>
typename SequenceView<T>::iterator SequenceView<T>::iterator::operator++(int)
{	// postincrement
    iterator _Tmp = *this;
    ++*this;
    return (_Tmp);
}

template <class T>
typename SequenceView<T>::iterator& SequenceView<T>::iterator::operator--()
{	// predecrement
    --_idx;
    return (*this);
}

template <class T>
typename SequenceView<T>::iterator SequenceView<T>::iterator::operator--(int)
{	// postdecrement
    iterator _Tmp = *this;
    --*this;
    return (_Tmp);
}

template <class T>
typename SequenceView<T>::iterator& SequenceView<T>::iterator::operator+=
(typename SequenceView<T>::iterator::difference_type _Off)
{	// increment by integer
    _idx += _Off;
    return (*this);
}

template <class T>
typename SequenceView<T>::iterator SequenceView<T>::iterator::operator+
(typename SequenceView<T>::iterator::difference_type _Off) const
{	// return this + integer
    iterator _Tmp = *this;
    return (_Tmp += _Off);
}

template <class T>
typename SequenceView<T>::iterator& SequenceView<T>::iterator:: operator-=
(typename SequenceView<T>::iterator::difference_type _Off)
{	// decrement by integer
    return (*this += -_Off);
}

template <class T>
typename SequenceView<T>::iterator SequenceView<T>::iterator::operator-
(typename SequenceView<T>::iterator::difference_type _Off) const
{	// return this - integer
    iterator _Tmp = *this;
    return (_Tmp -= _Off);
}

template <class T>
typename SequenceView<T>::iterator::difference_type SequenceView<T>::iterator::operator-(const iterator& _Right) const
{	// return difference of iterators
    return _idx - _Right._idx;
}

template <class T>
typename SequenceView<T>::iterator::reference SequenceView<T>::iterator::operator[]
(typename SequenceView<T>::iterator::difference_type _Off) const
{	// subscript
    return (*(*this + _Off));
}

////////////////////// CONST_ITERATOR /////////////////////////
template <class T>
SequenceView<T>::const_iterator::const_iterator()
{
}

template <class T>
SequenceView<T>::const_iterator::const_iterator(const const_iterator& rhs) : 
    _idx(rhs._idx),
    _parent(rhs._parent)
{
}

template <class T>
SequenceView<T>::const_iterator::const_iterator(indices_type::iterator idx, parent_type* parent) : 
    _idx(idx),
    _parent(parent)
{
}

template <class T>
typename SequenceView<T>::const_iterator::reference SequenceView<T>::const_iterator::operator*() const
{	// return designated object
    assert(_parent != 0);
    return (*_parent)[*_idx];
}

template <class T>
typename SequenceView<T>::const_iterator::pointer SequenceView<T>::const_iterator::operator->() const
{	// return pointer to class object
    return (&(*this));
}

template <class T>
typename SequenceView<T>::const_iterator& SequenceView<T>::const_iterator::operator++()
{	// preincrement
    ++_idx;
    return (*this);
}

template <class T>
typename SequenceView<T>::const_iterator SequenceView<T>::const_iterator::operator++(int)
{	// postincrement
    const_iterator _Tmp = *this;
    ++*this;
    return (_Tmp);
}

template <class T>
typename SequenceView<T>::const_iterator& SequenceView<T>::const_iterator::operator--()
{	// predecrement
    --_idx;
    return (*this);
}

template <class T>
typename SequenceView<T>::const_iterator SequenceView<T>::const_iterator::operator--(int)
{	// postdecrement
    const_iterator _Tmp = *this;
    --*this;
    return (_Tmp);
}

template <class T>
typename SequenceView<T>::const_iterator& SequenceView<T>::const_iterator::operator+=
(typename SequenceView<T>::const_iterator::difference_type _Off)
{	// increment by integer
    _idx += _Off;
    return (*this);
}

template <class T>
typename SequenceView<T>::const_iterator SequenceView<T>::const_iterator::operator+
(typename SequenceView<T>::const_iterator::difference_type _Off) const
{	// return this + integer
    const_iterator _Tmp = *this;
    return (_Tmp += _Off);
}

template <class T>
typename SequenceView<T>::const_iterator& SequenceView<T>::const_iterator:: operator-=
(typename SequenceView<T>::const_iterator::difference_type _Off)
{	// decrement by integer
    return (*this += -_Off);
}

template <class T>
typename SequenceView<T>::const_iterator SequenceView<T>::const_iterator::operator-
(typename SequenceView<T>::const_iterator::difference_type _Off) const
{	// return this - integer
    const_iterator _Tmp = *this;
    return (_Tmp -= _Off);
}

template <class T>
typename SequenceView<T>::const_iterator::difference_type SequenceView<T>::const_iterator::operator-(const const_iterator& _Right) const
{	// return difference of const_iterators
    return _idx - _Right._idx;
}

template <class T>
typename SequenceView<T>::const_iterator::reference SequenceView<T>::const_iterator::operator[]
(typename SequenceView<T>::const_iterator::difference_type _Off) const
{	// subscript
    return (*(*this + _Off));
}

template <class T>
bool SequenceView<T>::const_iterator::operator==(const const_iterator& _Right) const
{	// test for iterator equality
    return (_idx == _Right._idx);
}

template <class T>
bool SequenceView<T>::const_iterator::operator!=(const const_iterator& _Right) const
{	// test for iterator inequality
    return (!(*this == _Right));
}

template <class T>
bool SequenceView<T>::const_iterator::operator<(const const_iterator& _Right) const
{	// test if this < _Right
    return (_idx < _Right._idx);
}

template <class T>
bool SequenceView<T>::const_iterator::operator>(const const_iterator& _Right) const
{	// test if this > _Right
    return (_Right < *this);
}

template <class T>
bool SequenceView<T>::const_iterator::operator<=(const const_iterator& _Right) const
{	// test if this <= _Right
    return (!(_Right < *this));
}

template <class T>
bool SequenceView<T>::const_iterator::operator>=(const const_iterator& _Right) const
{	// test if this >= _Right
    return (!(*this < _Right));
}

template <class T>
void SequenceView<T>::use_all()
{
    int N = _parent.size();
    _indices.clear();
    _indices.reserve(N);
    for (int i = 0; i < N; ++i)
    {
        _indices.push_back(i);
    }
}


GTB_END_NAMESPACE
