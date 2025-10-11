GTB_BEGIN_NAMESPACE


template <class T>
inline unsigned tIndexedTriangleSet<T>::num_vertices() const
{
	return _vertices.size();
}


template <class T>
inline const typename  tIndexedTriangleSet<T>::Point3 &tIndexedTriangleSet<T>::vertex(unsigned i) const
{
	assert(i < _vertices.size());
	return _vertices[i];
}

template <class T>
inline typename  tIndexedTriangleSet<T>::Point3 &tIndexedTriangleSet<T>::vertex(unsigned i)
{
	assert(i < _vertices.size());
	return _vertices[i];
}


template <class T>
inline const typename tIndexedTriangleSet<T>::vertex_list &tIndexedTriangleSet<T>::vertices() const
{
    return _vertices;
}

template <class T>
inline typename tIndexedTriangleSet<T>::vertex_list &tIndexedTriangleSet<T>::vertices()
{
    return _vertices;
}


template <class T>
inline bool tIndexedTriangleSet<T>::has_vertex_colors() const
{
	return _vertex_colors.size() > 0;
}


template <class T>
inline unsigned tIndexedTriangleSet<T>::num_vertex_colors() const
{
	return _vertex_colors.size();
}


template <class T>
inline const ColorRgb &tIndexedTriangleSet<T>::vertex_color(unsigned i) const
{
    assert(i < _vertex_colors.size());
    return _vertex_colors[i];
}


template <class T>
inline const std::vector<ColorRgb> &tIndexedTriangleSet<T>::vertex_colors() const
{
    return _vertex_colors;
}


    
template <class T>
inline const ColorRgb &tIndexedTriangleSet<T>::face_color(unsigned i) const
{
	assert(i < _face_colors.size());
	return _face_colors[i];
}
    
template <class T>
inline void tIndexedTriangleSet<T>::set_face_color(unsigned idx, const ColorRgb& c)
{
	assert(idx < _face_colors.size());
	_face_colors[idx] = c;
}
    
template <class T>
inline unsigned tIndexedTriangleSet<T>::num_triangles() const
{
    return _triangles.size();
}


template <class T>
inline const IndexedTriangle &tIndexedTriangleSet<T>::indexed_triangle(
    unsigned i) const
{
    assert(i < _triangles.size());
    return _triangles[i];
}


template <class T>
inline typename  tIndexedTriangleSet<T>::Triangle3 tIndexedTriangleSet<T>::triangle(unsigned i) const
{
    return Triangle3(_vertices[_triangles[i].A()],
                     _vertices[_triangles[i].B()],
                     _vertices[_triangles[i].C()]);
}


template <class T>
inline const typename tIndexedTriangleSet<T>::face_list &tIndexedTriangleSet<T>::triangles() const
{
    return _triangles;
}


template <class T>
inline bool tIndexedTriangleSet<T>::has_vertex_normals() const
{
    return _vertex_normals.size() > 0;
}


template <class T>
inline unsigned tIndexedTriangleSet<T>::num_vertex_normals() const
{
    return _vertex_normals.size();
}


template <class T>
inline const typename tIndexedTriangleSet<T>::Vector3 &tIndexedTriangleSet<T>::vertex_normal(unsigned i) const
{
    assert(i < _vertex_normals.size());
    return _vertex_normals[i];
}

template <class T>
inline typename tIndexedTriangleSet<T>::Vector3 &tIndexedTriangleSet<T>::vertex_normal(unsigned i)
{
	assert(i < _vertex_normals.size());
	return _vertex_normals[i];
}


template <class T>
inline const  typename tIndexedTriangleSet<T>::normal_list &tIndexedTriangleSet<T>::vertex_normals() const
{
	return _vertex_normals;
}


//  inline Triangle3 tIndexedTriangleSet<T>::triangle(unsigned i) const
//  {
//  	return Triangle3(_vertices[_triangles[i].A],
//  			 _vertices[_triangles[i].B],
//  			 _vertices[_triangles[i].C]);
//  }


template <class T>
inline void tIndexedTriangleSet<T>::insert_vertex(const Point3& v)
{
	_vertices.push_back(v);
}

template <class T>
inline void tIndexedTriangleSet<T>::insert_vertex_normal(const Vector3& n)
{
    _vertex_normals.push_back(n);
}

template <class T>
inline void tIndexedTriangleSet<T>::set_vertex_normal(unsigned idx, const Vector3& n)
{
    assert(idx < _vertex_normals.size());
    _vertex_normals[idx] = n;
}

template <class T>
inline void tIndexedTriangleSet<T>::insert_vertex_color(const ColorRgb& c)
{
    _vertex_colors.push_back(c);
}

template <class T>
inline void tIndexedTriangleSet<T>::flip_face(unsigned index)
{
    _triangles[index].flip();
}

GTB_END_NAMESPACE
