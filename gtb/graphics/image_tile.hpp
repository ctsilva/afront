#ifndef GTB_IMAGE_TILE_INCLUDED
#define GTB_IMAGE_TILE_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/image.hpp>

GTB_BEGIN_NAMESPACE


class ImageTile {
public:
	ImageTile(GLint x,
		  GLint y,
		  GLsizei width,
		  GLsizei height,
		  GLenum format,
		  GLenum type);

	~ImageTile();

	void read();
	const Image &image() const;
	Image &image();

protected:
	GLint m_x;
	GLint m_y;
	Image *m_img;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/image_tile.ipp>
#endif

#endif // GTB_IMAGE_TILE_INCLUDED
