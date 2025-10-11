GTB_BEGIN_NAMESPACE


inline unsigned OctreeNodeCache::size() const
{
	return _size;
}


inline void OctreeNodeCache::double_size()
{
	set_size(std::max(size(), size() * 2));
}


inline void OctreeNodeCache::halve_size()
{
	set_size(std::max((unsigned) 1, size() / 2));
}


inline const std::list<OctreeNode *> &OctreeNodeCache::nodes() const
{
	return _nodes;
}


inline void OctreeNodeCache::reset_counters()
{
	_num_hits = 0;
	_num_misses = 0;
	_num_prefetches = 0;
}


inline unsigned OctreeNodeCache::num_hits() const
{
	return _num_hits;
}


inline unsigned OctreeNodeCache::num_misses() const
{
	return _num_misses;
}


inline unsigned OctreeNodeCache::num_prefetches() const
{
	return _num_prefetches;
}


inline void OctreeNodeCache::read_node_data(OctreeNode *node) const
{
	assert(node != NULL);
	assert(!node->has_model());
	assert(node->num_vertices() > 0);
	node->read_data(_file_name_prefix.c_str());
	assert(node->has_model());
}


GTB_END_NAMESPACE
