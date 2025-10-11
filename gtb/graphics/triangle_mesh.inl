GTB_BEGIN_NAMESPACE

template <class T>
inline unsigned tTriangleMesh<T>::num_vertices() const 
{
    return verts.size(); 
}

template <class T>
inline unsigned tTriangleMesh<T>::num_faces() const
{ 
    return faces.size(); 
}

template <class T>
inline typename tTriangleMesh<T>::TriangleMeshVertex& tTriangleMesh<T>::vertex(unsigned idx)
{
    assert(idx < num_vertices());
    return verts[idx];
}

template <class T>
inline const typename tTriangleMesh<T>::TriangleMeshVertex& tTriangleMesh<T>::vertex(unsigned idx) const
{
    assert(idx < num_vertices());
    return verts[idx];
}

template <class T>
inline TriangleMeshFace& tTriangleMesh<T>::face(unsigned idx)
{
    assert(idx < num_faces());
    return faces[idx];

}

template <class T>
inline const TriangleMeshFace& tTriangleMesh<T>::face(unsigned idx) const
{
    assert(idx < num_faces());
    return faces[idx];

}


template <class T>
inline bool tTriangleMesh<T>::VertOnBoundary(int v) const 
{
   
	int f = verts[v].someface;
	if (f < 0) return false;	// no faces touching the vert!
    assert (f < (int)faces.size());
	do {

		int vfi = faces[f].VertIndex(v);

		if (faces[f].nbrs[vfi] < 0)
			return true;

		f = faces[f].nbrs[vfi];

	} while (f != verts[v].someface);

	return false;
}


template <class T>
inline void tTriangleMesh<T>::GetBoundaries(std::vector< std::vector<int> > &boundaries) const {

  boundaries.resize(0);

  std::vector<bool> vb(verts.size());
  for (unsigned i=0;i<verts.size(); i++) {
    vb[i] = VertOnBoundary(i);
  }

  while (1) {
    int startv=-1;
    for (unsigned i=0; i<vb.size(); i++) {
      if (vb[i]) {
	startv=i; break;
      }
    }

    if (startv<0) break;

    std::vector<int> b;
    b.push_back(startv);
    do {
      vb[b.back()] = false;
      b.push_back(*(VertexVertexIteratorI(*this, b.back())));
    } while (b.back()!=startv);
    b.pop_back();
    boundaries.push_back(b);
  }
}



template <class T>
inline bool tTriangleMesh<T>::Vert1Ring(int v, std::vector<int> &ring) const 
{
	ring.clear();

	VertexVertexIteratorI vi(*this, v);
	for ( ; !vi.done(); ++vi) 
    {
		ring.push_back(*vi);
	}
	return vi.isonedge();
}


GTB_END_NAMESPACE
