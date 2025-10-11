#include <assert.h>

GTB_BEGIN_NAMESPACE


inline void ImageTile::read()
{
	assert(m_img != NULL);
	m_img->read_from_frame_buffer(m_x, m_y);
}


inline const Image &ImageTile::image() const
{
	assert(m_img != NULL);
	return *m_img;
}


inline Image &ImageTile::image()
{
	assert(m_img != NULL);
	return *m_img;
}


GTB_END_NAMESPACE
