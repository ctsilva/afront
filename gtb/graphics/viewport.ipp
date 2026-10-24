
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


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
