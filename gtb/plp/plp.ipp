GTB_BEGIN_NAMESPACE


inline const plp_front &Plp::front() const
{
	return _front;
}


inline const std::vector<OctreeNode *> &Plp::visible_set() const
{
	return _visible_set;
}


inline const Octree &Plp::octree() const
{
	return _octree;
}


inline Octree &Plp::octree()
{
	return _octree;
}


inline const OctreeNodeCache &Plp::cache() const
{
	return _cache;
}


inline OctreeNodeCache &Plp::cache()
{
	return _cache;
}


inline void Plp::set_visibility_mode(int mode)
{
	assert((mode == VM_AGGRESSIVE) || (mode == VM_CONSERVATIVE));
	_visibility_mode = mode;
}


inline void Plp::toggle_visibility_mode()
{
	assert((_visibility_mode == VM_AGGRESSIVE)
	       || (_visibility_mode == VM_CONSERVATIVE));
	if (_visibility_mode == VM_AGGRESSIVE) {
		_visibility_mode = VM_CONSERVATIVE;
	} else {
		_visibility_mode = VM_AGGRESSIVE;
	}
}


inline int Plp::visibility_mode() const
{
	return _visibility_mode;
}


inline void Plp::set_budget(unsigned a_budget)
{
	assert(a_budget > 0);
	_budget = a_budget;
}


inline unsigned Plp::budget() const
{
	return _budget;
}


inline void Plp::double_budget()
{
	_budget *= 2;
}


inline void Plp::halve_budget()
{
	_budget = std::max((unsigned) 1, _budget / 2);
}


inline void Plp::set_lod(float a_lod)
{
	_lod = a_lod;
}


inline float Plp::lod() const
{
	return _lod;
}


inline void Plp::double_lod()
{
	_lod *= 2.0;
	if (_lod > 1.0) {
		_lod = 1.0;
	}
}


inline void Plp::halve_lod()
{
	_lod /= 2.0;
	if (_lod < 0.001) {
		_lod = 0.001;
	}
}


inline void Plp::set_num_extra_layers(unsigned layers)
{
	_num_extra_layers = layers;
}


inline unsigned Plp::num_extra_layers() const
{
	return _num_extra_layers;
}


inline extinction_coef Plp::get_extinction_coef(OctreeNode *node,
						const Vector3 &v)
{
	// FIXME: extend to point sets
	assert(node != NULL);
	assert((node->id() >= 0)
	       && ((unsigned) node->id() < _node_data.size()));
	if (node->num_triangles() == 0) {
		return extinction_coef(0.0);
	}
	return _node_data[node->id()].extinction_coeffs.closest(v);
}


inline unsigned char Plp::node_state(const OctreeNode *node) const
{
	assert(node != NULL);
	assert((node->id() >= 0)
	       && ((unsigned) node->id() < _node_data.size()));
	return _node_data[node->id()].state;
}


inline void Plp::set_node_state(const OctreeNode *node, node_state_t state)
{
	assert(node != NULL);
	assert((node->id() >= 0)
	       && ((unsigned) node->id() < _node_data.size()));
	_node_data[node->id()].state = state;
}


inline void Plp::reset_node_states()
{
	for (unsigned i = 0; i < _node_data.size(); i++) {
		_node_data[i].state = NS_CLEAN;
	}
}


inline const char *Plp::node_state_str(const OctreeNode *node) const
{
	const char *s = NULL;
	switch(node_state(node)) {
	case NS_CLEAN:
		s = "NS_CLEAN";
		break;
	case NS_PROJECTED:
		s = "NS_PROJECTED";
		break;
	case NS_ENQUEUED:
		s = "NS_ENQUEUED";
		break;
	case NS_UNSEEN:
		s = "NS_UNSEEN";
		break;
	default:
		GTB_ERROR("invalid node state");
		break;
	}
	assert(s != NULL);
	return s;
}


GTB_END_NAMESPACE
