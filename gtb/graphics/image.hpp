#ifndef GTB_IMAGE_INCLUDED
#define GTB_IMAGE_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/ogltools.h>
#include <gtb/graphics/color_rgb.hpp>


GTB_BEGIN_NAMESPACE


class Image {
public:
	Image(GLsizei width, GLsizei height, GLenum format, GLenum type);
	virtual ~Image();

	GLsizei width() const;
	GLsizei height() const;
	GLenum format() const;
	GLenum type() const;
	unsigned num_pixels() const;
	GLubyte *pixels();
	const GLubyte *pixels() const;

	bool is_valid() const;
	unsigned num_components() const;
	unsigned bits_per_pixel() const;

	void draw() const;
	void draw(unsigned x, unsigned y, unsigned w, unsigned h, unsigned sx = 0, unsigned sy = 0) const;

	void read_from_frame_buffer(GLint x = 0, GLint y = 0);
	void draw_to_frame_buffer();

	void resize(GLsizei width, GLsizei height);

  	virtual ColorRgb pixel_rgb(int x, int y) const;
  	virtual void set_pixel_rgb(int x, int y, const ColorRgb &c);

    ColorRgb operator() (int x, int y) const;

    void clear();

protected:
	bool is_format_valid() const;
	bool is_type_valid() const;

	Image();
	GLsizei _width;
	GLsizei _height;
	GLenum _format;		// components
	GLenum _type;		// type of each component
	mutable GLubyte *_pixels;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/image.ipp>
#endif

#endif // GTB_IMAGE_INCLUDED
