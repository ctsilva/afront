
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


inline real_t Rtpi::r() const
{
	return _r;
}


inline real_t Rtpi::t() const
{
	return _t;
}


inline real_t Rtpi::p() const
{
	return _p;
}


inline int Rtpi::i() const
{
	return _i;
}


inline void Rtpi::set_r(real_t a_r)
{
	_r = a_r;
}


inline void Rtpi::set_t(real_t a_t)
{
	_t = a_t;
}


inline void Rtpi::set_p(real_t a_p)
{
	_p = a_p;
}


inline void Rtpi::set_i(int a_i)
{
	_i = a_i;
}


inline void Rtpi::reset(real_t a_r, real_t a_t, real_t a_p, int a_i)
{
	_r = a_r;
	_t = a_t;
	_p = a_p;
	_i = a_i;
}


inline void Rtpi::read(FILE *fp)
{
#ifdef REAL_IS_FLOAT
	read_float(&_r, fp);
	read_float(&_t, fp);
	read_float(&_p, fp);
	read_int(&_i, fp);
#else
	float pr, pt, pp;
	int pi;
	read_float(&pr, fp);
	read_float(&pt, fp);
	read_float(&pp, fp);
	read_int(&pi, fp);
	reset(pr, pt, pp, pi);
#endif
}


inline void Rtpi::write(FILE *fp) const
{
	write_float(_r, fp);
	write_float(_t, fp);
	write_float(_p, fp);
	write_int(_i, fp);
}


GTB_END_NAMESPACE
