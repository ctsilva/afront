GTB_BEGIN_NAMESPACE


inline Point2 ImageViewer::image_to_viewport(const Point2 &p) const
{
	return Point2(p.x() - _origin.x(),
		      _view.viewport().height() - 1 - p.y() + _origin.y());
}


inline Point2 ImageViewer::viewport_to_image(const Point2 &p) const
{
	return Point2((p.x() / _zoom_factor) + _origin.x(),
		      ((_view.viewport().height() - 1 - p.y()) / _zoom_factor)
		      + _origin.y());
}


inline const Image &ImageViewer::image() const
{
	assert(_image != NULL);
	return *_image;
}


inline void ImageViewer::set_window(int window_a)
{
	assert(window_a > 0);
	_window = window_a;
}


inline int ImageViewer::window() const
{
	assert(_window > 0);
	return _window;
}


inline Mouse &ImageViewer::mouse()
{
	return _mouse;
}


inline const Mouse &ImageViewer::mouse() const
{
	return _mouse;
}


inline View &ImageViewer::view()
{
	return _view;
}


inline const View &ImageViewer::view() const
{
	return _view;
}


inline void ImageViewer::post_redisplay() const
{
	glutSetWindow(_window);
	glutPostRedisplay();
}


inline void ImageViewer::enable(Capability cap)
{
	assert(cap < NUM_CAPS);
	_caps[cap] = true;
}


inline void ImageViewer::disable(Capability cap)
{
	assert(cap < NUM_CAPS);
	_caps[cap] = false;
}


inline void ImageViewer::toggle(Capability cap)
{
	assert(cap < NUM_CAPS);
	_caps[cap] = !_caps[cap];
}


inline bool ImageViewer::is_enabled(Capability cap) const
{
	assert(cap < NUM_CAPS);
	return _caps[cap];
}


inline void ImageViewer::add_point(const Point2 &p)
{
	_points.push_back(p);
}


inline unsigned ImageViewer::num_points() const
{
	return _points.size();
}


inline const Point2 &ImageViewer::point(unsigned i) const
{
	assert(i < _points.size());
	return _points[i];
}


inline Point2 &ImageViewer::point(unsigned i)
{
	assert(i < _points.size());
	return _points[i];
}


inline const std::vector<Point2> &ImageViewer::points() const
{
	return _points;
}


inline std::vector<Point2> &ImageViewer::points()
{
	return _points;
}


inline void ImageViewer::set_mode(unsigned m)
{
	assert(m < NUM_MODES);
	_mode = m;
}


inline unsigned ImageViewer::mode() const
{
	return _mode;
}


inline void ImageViewer::set_zoom_factor(float zf)
{
	const Viewport &vp = _view.viewport();
	Point2 p = viewport_to_image(Point2(vp.x_center(), vp.y_center()));
	_zoom_factor = zf;
	center_on(p);
}


inline float ImageViewer::zoom_factor() const
{
	return _zoom_factor;
}


inline bool ImageViewer::has_picked_point() const
{
	return _picked_point_index >= 0;
}


inline Point2 &ImageViewer::picked_point()
{
	assert(has_picked_point());
	return _points[_picked_point_index];
}


inline const Point2 &ImageViewer::picked_point() const
{
	assert(has_picked_point());
	return _points[_picked_point_index];
}


inline int ImageViewer::picked_point_index() const
{
	return _picked_point_index;
}


GTB_END_NAMESPACE
