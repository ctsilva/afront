
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


inline unsigned RtpiGrid::num_rows() const
{
	return _num_rows;
}


inline unsigned RtpiGrid::num_cols() const
{
	return _num_cols;
}


inline const Rtpi &RtpiGrid::sample(unsigned i, unsigned j) const
{
	assert(_num_rows > 0);
	assert(_num_cols > 0);
	assert(i < _num_rows);
	assert(j < _num_cols);
	return _data[i][j];
}


GTB_END_NAMESPACE
