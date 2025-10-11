#ifndef GTB_TIFF_RGBA_IMAGE_INCLUDED
#define GTB_TIFF_RGBA_IMAGE_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/image.hpp>
#include <tiffio.h>


GTB_BEGIN_NAMESPACE


class TiffRgbaImage : public Image {
public:
	explicit TiffRgbaImage(const char *name);
	TiffRgbaImage(GLsizei width, GLsizei height);
	~TiffRgbaImage();

	ColorRgb pixel_rgb(int x, int y) const;
	void set_pixel_rgb(int x, int y, const ColorRgb &c);

	void write(const char *name) const;

protected:
	void reverse_components() const;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/tiff_rgba_image.ipp>
#endif

#endif // GTB_TIFF_RGBA_IMAGE_INCLUDED
