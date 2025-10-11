GTB_BEGIN_NAMESPACE

inline ImageFloat::ImageFloat(const Image& image) : 
    Image(image.width(), image.height(), GL_RGB, GL_FLOAT)
{
    //
    // copy image (format convert)
    //
    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            set_pixel_rgb(x, y, image(x, y));
        }
    }
}

inline ImageFloat::ImageFloat(GLsizei width, GLsizei height) :
    Image(width, height, GL_RGB, GL_FLOAT)
{
}

inline ColorRgb& ImageFloat::operator() (int x, int y)
{
    ColorRgb* ppixel = (ColorRgb*)_pixels;
    ppixel += x + y*_width;
    return *ppixel;
}

inline const ColorRgb& ImageFloat::operator() (int x, int y) const
{
    const ColorRgb* ppixel = (const ColorRgb*)_pixels;
    ppixel += x + y*_width;
    return *ppixel;
}


GTB_END_NAMESPACE
