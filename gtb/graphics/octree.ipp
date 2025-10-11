GTB_BEGIN_NAMESPACE


inline void Octree::collect_leaves(
	std::vector<const OctreeNode *> &leaves) const
{
	assert(_root != NULL);
	_root->collect_leaves(leaves);
}


inline void Octree::collect_leaves(std::vector<OctreeNode *> &leaves) const
{
	assert(_root != NULL);
	_root->collect_leaves(leaves);
}


inline void Octree::collect_visible_leaves(
	const Camera &cam,
	std::vector<OctreeNode *> &nodes)
{
	assert(_root != NULL);
	_root->collect_visible_leaves(cam, nodes);
}


inline void Octree::collect_visible_leaves(
	const Camera &cam,
	std::vector<const OctreeNode *> &nodes) const
{
	assert(_root != NULL);
	_root->collect_visible_leaves(cam, nodes);
}


inline void Octree::collect_nodes(std::vector<OctreeNode *> &nodes)
{
	assert(_root != NULL);
	_root->collect_nodes(nodes);
}


inline unsigned Octree::num_leaves() const
{
	assert(_root != NULL);
	return _root->num_leaves();
}


inline unsigned Octree::num_nodes() const
{
	assert(_root != NULL);
	return _root->num_nodes();
}


inline const OctreeNode &Octree::root() const
{
	assert(_root != NULL);
	return *_root;
}


inline Box3 Octree::bounding_box() const
{
	assert(_root != NULL);
	return *_root;
}


inline Point3 Octree::centroid() const
{
	assert(_root != NULL);
	return _root->centroid();
}


inline bool Octree::contains(const Point3 &p) const
{
	assert(_root != NULL);
	return _root->contains(p);
}


inline OctreeNode *Octree::find_leaf_containing(const Point3 &p)
{
	assert(_root != NULL);
	return _root->find_leaf_containing(p);
}


inline OctreeNode *Octree::find_node(int id)
{
	assert(_root != NULL);
	return _root->find_node(id);
}


//  inline const std::vector<OctreeNode *> &Octree::
//  outside_nodes(OctreeNode::Direction i) const
//  {
//  	assert(i < 6);
//  	return _outside_nodes[i];
//  }


inline int Octree::make_id()
{
	_max_id++;
	return _max_id;
}


GTB_END_NAMESPACE
