#include <assert.h>


GTB_BEGIN_NAMESPACE


inline ColorRgb TiffRgbaImage::pixel_rgb(int x, int y) const
{
	assert((x >= 0) && (x < _width));
	assert((y >= 0) && (y < _height));
	uint32 pixel = *((uint32 *) _pixels + (y * _width) + x);
	real_t r = TIFFGetR(pixel) / 255.0;
	real_t g = TIFFGetG(pixel) / 255.0;
	real_t b = TIFFGetB(pixel) / 255.0;
	return ColorRgb(r, g, b);
}


inline void TiffRgbaImage::set_pixel_rgb(int x, int y, const ColorRgb &c)
{
	assert((x >= 0) && (x < _width));
	assert((y >= 0) && (y < _height));
	GLuint *p = (GLuint *) _pixels + (y * _width) + x;
	unsigned char r = (unsigned char) (c.r() * 255);
	unsigned char g = (unsigned char) (c.g() * 255);
	unsigned char b = (unsigned char) (c.b() * 255);
	unsigned char a = 255;
	*p = (a << 24) | (b << 16) | (g << 8) | r;
}


GTB_END_NAMESPACE
