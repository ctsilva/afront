GTB_BEGIN_NAMESPACE


inline void ModelViewer::set_window(int window_a)
{
	assert(window_a > 0);
	_window = window_a;
}


inline int ModelViewer::window() const
{
	assert(_window > 0);
	return _window;
}


inline Mouse &ModelViewer::mouse()
{
	return _mouse;
}


inline const Mouse &ModelViewer::mouse() const
{
	return _mouse;
}


inline View &ModelViewer::view()
{
	return _view;
}


inline const View &ModelViewer::view() const
{
	return _view;
}


inline Octree &ModelViewer::octree()
{
	assert(_plp != NULL);
	return _plp->octree();
}


inline const Octree &ModelViewer::octree() const
{
	assert(_plp != NULL);
	return _plp->octree();
}


inline bool ModelViewer::has_model() const
{
	return _plp != NULL;
}


inline Plp &ModelViewer::plp()
{
	assert(_plp != NULL);
	return *_plp;
}


inline const Plp &ModelViewer::plp() const
{
	assert(_plp != NULL);
	return *_plp;
}


inline void ModelViewer::enable(Capability cap)
{
	assert(cap < NUM_CAPS);
	_caps[cap] = true;
}


inline void ModelViewer::disable(Capability cap)
{
	assert(cap < NUM_CAPS);
	_caps[cap] = false;
}


inline void ModelViewer::toggle(Capability cap)
{
	assert(cap < NUM_CAPS);
	_caps[cap] = !_caps[cap];
}


inline bool ModelViewer::is_enabled(Capability cap) const
{
	assert(cap < NUM_CAPS);
	return _caps[cap];
}


inline void ModelViewer::add_point(const Point3 &p)
{
	_points.push_back(p);
}


inline unsigned ModelViewer::num_points() const
{
	return _points.size();
}


inline const Point3 &ModelViewer::point(unsigned i) const
{
	assert(i < _points.size());
	return _points[i];
}


inline Point3 &ModelViewer::point(unsigned i)
{
	assert(i < _points.size());
	return _points[i];
}


inline const std::vector<Point3> &ModelViewer::points() const
{
	return _points;
}


inline std::vector<Point3> &ModelViewer::points()
{
	return _points;
}


inline void ModelViewer::set_mode(unsigned m)
{
	assert(m < NUM_MODES);
	_mode = m;
}


inline unsigned ModelViewer::mode() const
{
	return _mode;
}


inline bool ModelViewer::has_picked_point() const
{
	return _has_picked_point;
}


inline const Point3 &ModelViewer::picked_point() const
{
	assert(_has_picked_point);
	return _picked_point;
}


inline const Ray3 &ModelViewer::world_ray() const
{
	return _world_ray;
}


inline const OctreeNode::hit_set &ModelViewer::hits() const
{
	return _hits;
}


inline void ModelViewer::post_redisplay() const
{
	glutSetWindow(_window);
	glutPostRedisplay();
}


GTB_END_NAMESPACE
