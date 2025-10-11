#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/tiff_rgba_image.hpp>
#include <gtb/error/error.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/tiff_rgba_image.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


TiffRgbaImage::TiffRgbaImage(const char *name)
{
	_format = GL_RGBA;
	_type = GL_UNSIGNED_BYTE;

	assert(bits_per_pixel() == 32);
	TIFF *tif = TIFFOpen(name, "rb");
	if (tif == NULL) {
		GTB_ERROR(name);
	}
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &_width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &_height);
	size_t npixels = _width * _height;
	_pixels = (GLubyte *) _TIFFmalloc(
		(tsize_t) (npixels * sizeof(uint32)));
	if (_pixels == NULL) {
		GTB_ERROR("_TIFFmalloc() failed");
	}
	if (!TIFFReadRGBAImage(tif, _width, _height, (uint32 *) _pixels, 0)) {
		GTB_ERROR("_TIFFReadRGBAImage() failed");
	}
	TIFFClose(tif);
}


TiffRgbaImage::TiffRgbaImage(GLsizei w, GLsizei h)
{
	_width = w;
	_height = h;
	_format = GL_RGBA;
	_type = GL_UNSIGNED_INT_8_8_8_8_REV;
	assert(bits_per_pixel() == 32);
	size_t npixels = _width * _height;
	_pixels = (GLubyte *) _TIFFmalloc(
		(tsize_t) (npixels * sizeof(uint32)));
	if (_pixels == NULL) {
		GTB_ERROR("_TIFFmalloc() failed");
	}
}


TiffRgbaImage::~TiffRgbaImage()
{
	_TIFFfree(_pixels);
	_pixels = NULL;
}


void TiffRgbaImage::write(const char *name) const
{
	TIFF *out = TIFFOpen(name, "wb");
	if (out == NULL) {
		GTB_ERROR(name);
	}

	int samples_per_pixel = 4;
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, _width);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, _height);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

	// necessary?
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	// set the strip size of the file to be size of one row of pixels
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
		     TIFFDefaultStripSize(out, _width * samples_per_pixel));

	// writing image to the file one strip at a time
	for (int row = 0; row < _height; row++) {
		uint32 *buf = ((uint32 *) _pixels +
			       (_height - row - 1) * _width);
		if (TIFFWriteScanline(out, buf, row, 0) < 0) {
			GTB_ERROR("TIFFWriteScanline() failed");
		}
	}

	TIFFClose(out);
}


GTB_END_NAMESPACE
