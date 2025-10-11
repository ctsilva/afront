/*------------------ surfel_hierarchy iterator ---------------------*/
inline surfel_hierarchy::const_iterator::const_iterator() :
    _p_hierarchy(0)
{
}

inline surfel_hierarchy::const_iterator::const_iterator(const const_iterator& rhs) :
    hierarchy_index(rhs),
    _p_hierarchy(rhs._p_hierarchy)
{
}

inline surfel_hierarchy::const_iterator::const_iterator(const surfel_hierarchy* hierarchy, unsigned a_level, unsigned a_index):
    hierarchy_index(a_level, a_index),
    _p_hierarchy(hierarchy)
{
}

inline surfel_hierarchy::const_iterator& surfel_hierarchy::const_iterator::operator++()
{
    ++index;
    while ((level < _p_hierarchy->num_levels()) && (_p_hierarchy->level(level).size() <= index))
    {
        ++level;
        index = 0;
    }
    return *this;
}

inline bool surfel_hierarchy::const_iterator::operator==(const surfel_hierarchy::const_iterator& rhs)
{
    return (level == rhs.level) && (index == rhs.index);
}

inline bool surfel_hierarchy::const_iterator::operator!=(const surfel_hierarchy::const_iterator& rhs)
{ return !(*this == rhs);
}

/*------------------ surfel_hierarchy ---------------------*/

inline const Point3& surfel_hierarchy::vertex(const hierarchy_index& hi) const
{
    assert(hi.level < _hierarchy.size());
    assert(hi.index < _hierarchy[hi.level]->size());

    return _hierarchy[hi.level]->vertex(hi.index);
}

inline const Point3& surfel_hierarchy::vertex(surfel_hierarchy::const_iterator& i) const
{
    return vertex(static_cast<hierarchy_index>(i));
}

inline const Point3& surfel_hierarchy::vertex(unsigned a_level, unsigned index) const
{
    assert(a_level < _hierarchy.size());
    assert(index < _hierarchy[a_level]->size());

    return _hierarchy[a_level]->vertex(index);
}

inline hierarchy_index surfel_hierarchy::insert_vertex(unsigned a_level, const Point3& v)
{
    assert(a_level < _hierarchy.size());

    _hierarchy[a_level]->insert_vertex(v);
    return hierarchy_index(a_level, _hierarchy[a_level]->size()-1);
}


inline const Vector3& surfel_hierarchy::normal(const hierarchy_index& hi) const
{
    assert(hi.level < _hierarchy.size());
    assert(hi.index < _hierarchy[hi.level]->size());

    return _hierarchy[hi.level]->normal(hi.index);
}

inline const Vector3& surfel_hierarchy::normal(surfel_hierarchy::const_iterator& i) const
{
    return normal(static_cast<hierarchy_index>(i));
}

inline const Vector3& surfel_hierarchy::normal(unsigned a_level, unsigned index) const
{
    assert(a_level < _hierarchy.size());
    assert(index < _hierarchy[a_level]->size());

    return _hierarchy[a_level]->normal(index);
}

inline void surfel_hierarchy::insert_normal(unsigned a_level, const Vector3& n)
{
    assert(a_level < _hierarchy.size());

    _hierarchy[a_level]->insert_normal(n);
}

inline const ColorRgb& surfel_hierarchy::vertex_color(const hierarchy_index& hi) const
{
    assert(hi.level < _hierarchy.size());
    assert(hi.index < _hierarchy[hi.level]->size());

    return _hierarchy[hi.level]->vertex_color(hi.index);
}


inline void surfel_hierarchy::new_level()
{
	_hierarchy.push_back(t_hierarchy::value_type(new surfel_set));
}

inline surfel_hierarchy::const_iterator surfel_hierarchy::begin() const
{
    return const_iterator(this, 0, 0);
}

inline surfel_hierarchy::const_iterator surfel_hierarchy::end() const
{
    return const_iterator(this, num_levels(), 0);
}


/*-------------------- surfel_hierarchy_view -----------------------*/
inline surfel_hierarchy_view::surfel_hierarchy_view(const surfel_hierarchy& hierarchy) :
    _hierarchy(hierarchy)
{
}

inline const Point3& surfel_hierarchy_view::vertex(unsigned i) const
{
    assert(i < _view.size());
    return _hierarchy.vertex(_view[i]);
}

inline const Vector3& surfel_hierarchy_view::normal(unsigned i) const
{
    assert(i < _view.size());
    return _hierarchy.normal(_view[i]);
}

inline const ColorRgb& surfel_hierarchy_view::vertex_color(unsigned i) const
{
    assert(i < _view.size());
    return _hierarchy.vertex_color(_view[i]);
}

inline const Point3& HierarchyGetPoint_f::operator()(const hierarchy_index& idx) const
{
    return _hierarchy.vertex(idx);
}


inline bool HierarchyNormalCompare::operator()(const hierarchy_index& idx) const
{
    double cos_angle = _n.dot(_hierarchy.normal(idx));
    if (cos_angle > _cos_legal_angle) return true;
    else return false;
}
