#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/image_tile.hpp>
#endif // WIN32

#ifdef OUTLINE
#define inline
#include <gtb/graphics/image_tile.ipp>
#undef inline
#endif

GTB_BEGIN_NAMESPACE


ImageTile::ImageTile(GLint x,
		     GLint y,
		     GLsizei width,
		     GLsizei height,
		     GLenum format,
		     GLenum type)
	: m_x(x),
	  m_y(y)
{
	m_img = new Image(width, height, format, type);
}


ImageTile::~ImageTile()
{
	delete m_img;
	m_img = NULL;
}


GTB_END_NAMESPACE
