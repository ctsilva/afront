
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


inline void RtpiStrip::add_point(const Rtpi &p)
{
	_data.push_back(p);
}


inline unsigned RtpiStrip::num_points() const
{
	return _data.size();
}


inline const Rtpi &RtpiStrip::operator[](unsigned i) const
{
	assert(i < _data.size());
	return _data[i];
}


inline Rtpi &RtpiStrip::operator[](unsigned i)
{
	assert(i < _data.size());
	return _data[i];
}


GTB_END_NAMESPACE
