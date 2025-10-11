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
