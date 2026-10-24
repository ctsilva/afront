
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


inline timer::timer()
{
}


inline void timer::start()
{
#ifndef WIN32
	gettimeofday(&m_t1, NULL);
#endif
}


inline void timer::stop()
{
#ifndef WIN32
	gettimeofday(&m_t2, NULL);
#endif
}


inline double timer::elapsed_seconds() const
{
	return ((m_t2.tv_usec - m_t1.tv_usec) * 0.000001 +
		(m_t2.tv_sec - m_t1.tv_sec));
}


inline double timer::elapsed_milliseconds() const
{
	return ((m_t2.tv_usec - m_t1.tv_usec) * 0.001 +
		(m_t2.tv_sec - m_t1.tv_sec) * 1000.0);
}


GTB_END_NAMESPACE
