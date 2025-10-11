#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/image.hpp>
#include <gtb/graphics/image_float.hpp>
#include <gtb/graphics/ogltools.h>
#include <cassert>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/image_float.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


/*
 * pixel-wise add rhs*weight to this image
 */
void ImageFloat::add(const ImageFloat& rhs, double weight)
{
    for (int v = 0; v < _height; ++v)
        for (int u = 0; u < _width; ++u)
            (*this)(u,v) += rhs(u,v)*weight;
}

/*
 * multiply every pixel by value
 */
void ImageFloat::add(double value)
{
    for (int v = 0; v < _height; ++v)
        for (int u = 0; u < _width; ++u)
            (*this)(u,v) += ColorRgb(value, value, value);
}


/*
 * multiply every pixel by value
 */
void ImageFloat::multiply(double value)
{
    for (int v = 0; v < _height; ++v)
        for (int u = 0; u < _width; ++u)
            (*this)(u,v) *= value;
}

/*
 * Copy window from rhs(x0,y0,w,h) to this image at x,y
 */
void ImageFloat::copy(const ImageFloat& rhs, int x0, int y0, int w, int h, int x, int y)
{
    assert(x>=0);
    assert(y>=0);
    assert(x+w<=_width);
    assert(y+h<=_height);

    int v = y;
    int v0 = y0;
    for (int i = 0; i < h; ++i, ++v, ++v0)
    {
        int u = x;
        int u0 = x0;
        for (int j = 0; j < w; ++j, ++u, ++u0)
        {
            (*this)(u,v) = rhs(u0,v0);
        }
    }
}

GTB_END_NAMESPACE
