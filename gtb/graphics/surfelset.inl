GTB_BEGIN_NAMESPACE

template <class T>
inline unsigned tsurfel_set<T>::size() const
{
    return _vertices.size();
}

template <class T>
inline void tsurfel_set<T>::clear()
{
    _vertices.clear();
    _normals.clear();
    _colors.clear();
    _radius.clear();
}

template <class T>
inline tPoint3<T>& tsurfel_set<T>::vertex(unsigned idx)
{
    return _vertices[idx];
}

template <class T>
inline const tPoint3<T>& tsurfel_set<T>::vertex(unsigned idx) const
{
    return _vertices[idx];
}

template <class T>
inline typename tsurfel_set<T>::vertex_list& tsurfel_set<T>::vertices()
{
    return _vertices;
}

template <class T>
inline const typename tsurfel_set<T>::vertex_list& tsurfel_set<T>::vertices() const
{
    return _vertices;
}

template <class T>
inline void tsurfel_set<T>::set_vertex(unsigned i, const tPoint3<T>& v)
{
    assert(i < size());
    _vertices[i] = v;
}

template <class T>
inline void tsurfel_set<T>::update_bounding_box(const tPoint3<T>& v)
{
    Box3 &bb = this->tModel<T>::_bounding_box;
    if (!bb.contains(v))
    {
        tPoint3<T> min_p(
            std::min(bb.min_point().x(), v.x()),
            std::min(bb.min_point().y(), v.y()),
            std::min(bb.min_point().z(), v.z()));
        tPoint3<T> max_p(
            std::max(bb.min_point().x(), v.x()),
            std::max(bb.min_point().y(), v.y()),
            std::max(bb.min_point().z(), v.z()));
        bb.reset(min_p, max_p);
    }
}

//
// Assume v is not already in the _vertices list
//
template <class T>
inline void tsurfel_set<T>::update_centroid(const tPoint3<T>& v)
{
    unsigned N = _vertices.size();
    unsigned N1 = N+1;
    tPoint3<T> &c = this->tModel<T>::_centroid;
    c.set_x((c.x() * N + v.x()) / N1);
    c.set_y((c.y() * N + v.y()) / N1);
    c.set_z((c.z() * N + v.z()) / N1);
}


template <class T>
inline void tsurfel_set<T>::insert_vertex(const tPoint3<T>& v)
{
//    update_bounding_box(v);
//    update_centroid(v);
    tModel<T>::_is_bounding_box_valid = false;
    _vertices.push_back(v);
}

template <class T>
inline void tsurfel_set<T>::insert_vertex(const tPoint3<T>& v, const tVector3<T>& n)
{
//    update_bounding_box(v);
//    update_centroid(v);
    tModel<T>::_is_bounding_box_valid = false;
    _vertices.push_back(v);
    _normals.push_back(n);
}

template <class T>
inline bool tsurfel_set<T>::has_normals() const
{
    return _normals.size() > 0;
}

template <class T>
inline const tVector3<T>& tsurfel_set<T>::normal(unsigned i) const
{
    assert(i < _normals.size());
    return _normals[i];
}

template <class T>
inline tVector3<T>& tsurfel_set<T>::normal(unsigned i)
{
    assert(i < _normals.size());
    return _normals[i];
}

template <class T>
inline const typename tsurfel_set<T>::normal_list& tsurfel_set<T>::normals() const
{
    return _normals;
}

template <class T>
inline typename tsurfel_set<T>::normal_list& tsurfel_set<T>::normals()
{
    return _normals;
}

template <class T>
inline void tsurfel_set<T>::set_normal(unsigned i, const tVector3<T>& n)
{
    assert(i < _normals.size());
    _normals[i] = n;
}

template <class T>
inline void tsurfel_set<T>::insert_normal(const tVector3<T>& n)
{
    _normals.push_back(n);
}

template <class T>
inline bool tsurfel_set<T>::has_colors() const
{
    return  _colors.size() > 0;
}

template <class T>
inline const ColorRgb& tsurfel_set<T>::vertex_color(unsigned i) const
{
    assert(i < _colors.size());
    return _colors[i];
}

template <class T>
inline ColorRgb& tsurfel_set<T>::vertex_color(unsigned i)
{
    assert(i < _colors.size());
    return _colors[i];
}

template <class T>
inline const typename tsurfel_set<T>::color_list &tsurfel_set<T>::vertex_colors() const
{
    return _colors;
}

template <class T>
inline typename tsurfel_set<T>::color_list &tsurfel_set<T>::vertex_colors()
{
    return _colors;
}

template <class T>
inline void tsurfel_set<T>::set_color(unsigned i, const ColorRgb& c)
{
    assert(i < _colors.size());
    _colors[i] = c;
}

template <class T>
inline void tsurfel_set<T>::insert_color(const ColorRgb& c)
{
    _colors.push_back(c);
}

template <class T>
inline void tsurfel_set<T>::allocate_colors()
{
    _colors.reserve(_vertices.size());
    ColorRgb black(0,0,0);
    std::fill_n(std::back_inserter(_colors), _vertices.size(), black);
}


template <class T>
inline void tsurfel_set<T>::erase(unsigned idx)
{
    _vertices.erase(_vertices.begin() + idx);
    if (has_normals()) _normals.erase(_normals.begin() + idx);
    if (has_colors()) _colors.erase(_colors.begin() + idx);
}


template <class T>
inline void tsurfel_set<T>::clear_colors()
{
    _colors.clear();
}

template <class T>
inline void tsurfel_set<T>::clear_normals()
{
    _normals.clear();
}

template <class T>
inline bool tsurfel_set<T>::has_radius() const
{
	return (_radius.size() > 0);
}

template <class T>
inline void tsurfel_set<T>::insert_radius(T r)
{
	_radius.push_back(r);
}

template <class T>
inline void tsurfel_set<T>::set_radius(unsigned idx, T r)
{
	assert(idx < _radius.size());
	_radius[idx] = r;
}

template <class T>
inline T tsurfel_set<T>::radius(unsigned idx) const
{
	assert(idx < _radius.size());
	return _radius[idx];
}

template <class T>
inline void tsurfel_set<T>::clear_radius()
{
	_radius.clear();
}

template <class T>
inline typename tsurfel_set<T>::radius_list& tsurfel_set<T>::radiuses()
{
    return _radius;
}

template <class T>
inline const typename tsurfel_set<T>::radius_list& tsurfel_set<T>::radiuses() const
{
    return _radius;
}

GTB_END_NAMESPACE
