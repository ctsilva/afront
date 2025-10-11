#ifndef GTB_IMAGE_FLOAT_INCLUDED
#define GTB_IMAGE_FLOAT_INCLUDED

//
// "Specialized" image class for real_t type
//
#include <gtb/common.hpp>
#include <gtb/graphics/ogltools.h>
#include <gtb/graphics/color_rgb.hpp>


GTB_BEGIN_NAMESPACE


class ImageFloat : public Image {
public:
    ImageFloat(const Image& image);
    ImageFloat(GLsizei width, GLsizei height);

    ColorRgb& operator() (int x, int y);
    const ColorRgb& operator() (int x, int y) const;

    void add(const ImageFloat& rhs, double weight);
    void add(double value);
    void multiply(double value);
    void copy(const ImageFloat& rhs, int x0, int y0, int w, int h, int x, int y);

protected:
    ImageFloat();
    unsigned _row_size;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/image_float.inl>
#endif

#endif // GTB_IMAGE_FLOAT_INCLUDED
