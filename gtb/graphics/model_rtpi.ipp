
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


inline unsigned ModelRtpi::num_strips() const
{
	return _strips.size();
}


inline const RtpiStrip &ModelRtpi::strip(unsigned i) const
{
	assert(i < _strips.size());
	assert(_strips[i] != NULL);
	return *(_strips[i]);
}


inline RtpiStrip &ModelRtpi::strip(unsigned i)
{
	assert(i < _strips.size());
	assert(_strips[i] != NULL);
	return *(_strips[i]);
}


inline unsigned ModelRtpi::num_points() const
{
	unsigned n = 0;
	for (unsigned i = 0; i < _strips.size(); i++) {
		n += strip(i).num_points();
	}
	return n;
}


inline unsigned ModelRtpi::max_points() const
{
	return _max_points;
}


inline real_t ModelRtpi::min_range() const
{
	return _min_r;
}


inline real_t ModelRtpi::max_range() const
{
	return _max_r;
}


inline real_t ModelRtpi::min_theta() const
{
	return _min_t;
}


inline real_t ModelRtpi::max_theta() const
{
	return _max_t;
}


inline real_t ModelRtpi::min_phi() const
{
	return _min_p;
}


inline real_t ModelRtpi::max_phi() const
{
	return _max_p;
}


inline int ModelRtpi::min_intensity() const
{
	return _min_i;
}


inline int ModelRtpi::max_intensity() const
{
	return _max_i;
}


GTB_END_NAMESPACE
