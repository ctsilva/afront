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
