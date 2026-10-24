
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


inline std::list<ImageTile *> &ImageTileSet::active_tiles()
{
	return m_active_tiles;
}


inline void ImageTileSet::activate_all()
{
	m_active_tiles.clear();
	for (unsigned i = 0; i < m_tiles.size(); i++) {
		m_active_tiles.push_back(m_tiles[i]);
	}
}


inline void ImageTileSet::deactivate(std::list<ImageTile *>::iterator pos)
{
	m_active_tiles.erase(pos);
}


inline void ImageTileSet::read_active()
{
	std::list<ImageTile *>::iterator i;
	for (i = m_active_tiles.begin(); i != m_active_tiles.end(); i++) {
		(*i)->read();
	}
}


inline void ImageTileSet::read_all()
{
	for (unsigned i = 0; i < m_tiles.size(); i++) {
		m_tiles[i]->read();
	}
}


GTB_END_NAMESPACE
