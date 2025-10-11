GTB_BEGIN_NAMESPACE


inline int OctreeNode::id() const
{
	return _id;
}


inline OctreeNode::Octant OctreeNode::octant() const
{
	return _octant;
}


inline unsigned OctreeNode::depth() const
{
	return _depth;
}


inline unsigned  OctreeNode::num_vertices() const
{
	return _num_vertices;
}


inline unsigned  OctreeNode::num_triangles() const
{
	return _num_triangles;
}


inline OctreeNode *OctreeNode::parent() const
{
	return _parent;
}


inline bool OctreeNode::has_model() const
{
	return _model != NULL;
}


inline const IndexedTriangleSet &OctreeNode::model() const
{
	assert(_model != NULL);
	return *_model;
}


inline IndexedTriangleSet &OctreeNode::model()
{
	assert(_model != NULL);
	return *_model;
}


inline bool OctreeNode::is_leaf() const
{
	return _is_leaf;
}


inline const OctreeNode *OctreeNode::child(unsigned i) const
{
	assert(i < NUM_CHILDREN);
	return _children[i];
}


inline OctreeNode *OctreeNode::child(unsigned i)
{
	assert(i < NUM_CHILDREN);
	return _children[i];
}


inline OctreeNode::Octant OctreeNode::octant(
	unsigned i, unsigned j, unsigned k) const
{
	assert((i == 0) || (i == 1));
	assert((j == 0) || (j == 1));
	assert((k == 0) || (k == 1));
	//unsigned o = i + (j << 1) + (k << 2);
	unsigned o = k + (j << 1) + (i << 2);
	assert(o <= 7);
	return (Octant) o;
}


inline OctreeNode::Octant OctreeNode::octant(const Point3 &p) const
{
	Point3 c = centroid();
	unsigned i = (p[0] > c[0]) ? 1 : 0;
	unsigned j = (p[1] > c[1]) ? 1 : 0;
	unsigned k = (p[2] > c[2]) ? 1 : 0;
	return octant(i, j, k);
}


inline unsigned OctreeNode::max_num_vertices()
{
	return _max_num_vertices;
}


inline void OctreeNode::set_max_num_vertices(
	unsigned arg_max_num_vertices)
{
	assert(arg_max_num_vertices > 0);
	_max_num_vertices = arg_max_num_vertices;
}


inline unsigned OctreeNode::max_depth()
{
	return _max_depth;
}


inline void OctreeNode::set_max_depth(unsigned arg_max_depth)
{
	assert(arg_max_depth > 0);
	_max_depth = arg_max_depth;
}


inline Box3 OctreeNode::bounding_box() const
{
	return *this;
}


//  inline const Point3 &OctreeNode::centroid() const
//  {
//  	return _centroid;
//  }


inline void OctreeNode::insert_neighbor(OctreeNode *node, Direction dir)
{
	_neighbors.push_back(node);
	_neighbor_directions.push_back(dir);
}


inline bool OctreeNode::is_neighbor(const OctreeNode *node) const
{
	std::vector<OctreeNode *>::const_iterator ni =
		std::find(_neighbors.begin(), _neighbors.end(), node);
	return ni != _neighbors.end();
}


inline void OctreeNode::clear_neighbors()
{
	_neighbors.clear();
}


inline const std::vector<OctreeNode *> &OctreeNode::neighbors() const
{
	return _neighbors;
}


inline const std::vector<OctreeNode::Direction> &OctreeNode::
neighbor_directions() const
{
	return _neighbor_directions;
}


inline real_t OctreeNode::initial_solidity() const
{
	return _initial_solidity;
}


inline void OctreeNode::set_solidity(real_t arg_solidity)
{
	_current_solidity = arg_solidity;
}


inline real_t OctreeNode::current_solidity() const
{
	return _current_solidity;
}


inline bool OctreeNode::compare_solidity(const OctreeNode *a,
					 const OctreeNode *b)
{
  	assert(a != NULL);
  	assert(b != NULL);
	if (a->_current_solidity < b->_current_solidity) {
		return true;
	} else if (a->_current_solidity > b->_current_solidity) {
		return false;
	} else {
		if (a->_time_stamp < b->_time_stamp) {
			return true;
		} else {
			return false;
		}
	}
}


inline bool OctreeNode::CompareSolidity::operator()(const OctreeNode *a,
						    const OctreeNode *b)
{
	assert(a != NULL);
	assert(b != NULL);
	return compare_solidity(a, b);
}


inline unsigned OctreeNode::time_stamp() const
{
	return _time_stamp;
}


inline void OctreeNode::set_time_stamp(unsigned arg_time_stamp)
{
	_time_stamp = arg_time_stamp;
}


inline unsigned OctreeNode::layer() const
{
	return _layer;
}


inline void OctreeNode::set_layer(unsigned arg_layer)
{
	_layer = arg_layer;
}


inline const CoordinateSystem &OctreeNode::coordinate_system() const
{
	return _cs;
}


//  inline bool operator<(const OctreeNode &a, const OctreeNode &b)
//  {
//  	if (a._current_solidity < b._current_solidity) {
//  		return true;
//  	} else if (a._current_solidity > b._current_solidity) {
//  		return false;
//  	} else {
//  		if (a._time_stamp < b._time_stamp) {
//  			return true;
//  		} else {
//  			return false;
//  		}
//  	}
//  }


inline unsigned OctreeNode::num_new_vertices() const
{
	return _vertices.size();
}


inline unsigned OctreeNode::num_new_triangles() const
{
	return _triangles.size();
}


inline void OctreeNode::increment_global_ray_id()
{
	if (_global_ray_id == UINT_MAX) {
		GTB_ERROR("max ray id reached");
	}
	_global_ray_id++;
}


inline OctreeNode::hit::hit(OctreeNode *a_node, real_t a_t1, real_t a_t2)
	: m_node(a_node), m_t1(a_t1), m_t2(a_t2)
{
	assert(m_node != NULL);
	assert(m_t2 >= m_t1);
}


inline OctreeNode *OctreeNode::hit::node() const
{
	return m_node;
}


inline real_t OctreeNode::hit::t1() const
{
	return m_t1;
}


inline real_t OctreeNode::hit::t2() const
{
	return m_t2;
}


inline bool operator<(const OctreeNode::hit &h1, const OctreeNode::hit &h2)
{
	assert(h1.m_node->id() != h2.m_node->id());
//  	fprintf(stderr, ">: %d %f %f\n", h1.m_node->id(), h1.m_t1, h1.m_t2);
//  	fprintf(stderr, "   %d %f %f\n", h2.m_node->id(), h2.m_t1, h2.m_t2);
	if (h1.m_t1 < h2.m_t1) {
		return true;
	} else if (h1.m_t2 < h2.m_t2) {
		return true;
	} else if (h1.m_node->id() < h2.m_node->id()) {
		return true;
	} else {
		return false;
	}
}


GTB_END_NAMESPACE
