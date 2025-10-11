GTB_BEGIN_NAMESPACE


inline Viewport &Viewport::operator=(const Viewport &vp)
{
	if (&vp != this) {
		_x_min = vp._x_min;
		_y_min = vp._y_min;
		_width = vp._width;
		_height = vp._height;
	}
	return *this;
}


inline int Viewport::x_min() const
{
	return _x_min;
}


inline int Viewport::y_min() const
{
	return _y_min;
}


inline int Viewport::x_max() const
{
	return _x_min + _width - 1;
}


inline int Viewport::y_max() const
{
	return _y_min + _height - 1;
}


inline int Viewport::x_center() const
{
	return _x_min + (_width / 2);
}


inline int Viewport::y_center() const
{
	return _y_min + (_height / 2);
}


inline Point2 Viewport::center() const
{
	return Point2(x_center(), y_center());
}


inline int Viewport::width() const
{
	return _width;
}


inline int Viewport::height() const
{
	return _height;
}


inline void Viewport::resize(int xmin, int ymin, int w, int h)
{
	_x_min = xmin;
	_y_min = ymin;
	_width = w;
	_height = h;
}


inline void Viewport::load() const
{
  	glViewport(_x_min, _y_min, _width, _height);
}


GTB_END_NAMESPACE
