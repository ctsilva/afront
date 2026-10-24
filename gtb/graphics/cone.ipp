
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


inline bool Cone::operator==(const Cone &c) const
{
	return ((_apex == c._apex)
		&& (_base == c._base));
}


inline bool Cone::operator!=(const Cone &c) const
{
	return !(c == *this);
}


inline const Point3 &Cone::apex() const
{
	return _apex;
}


inline const Circle3 &Cone::base() const
{
	return _base;
}


inline real_t Cone::height() const
{
	return (real_t)((double)Point3::distance(_apex, _base.center()));
}


GTB_END_NAMESPACE
