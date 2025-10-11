#include <gtb/error/error.hpp>


GTB_BEGIN_NAMESPACE


inline GLsizei Image::width() const
{
	return _width;
}


inline GLsizei Image::height() const
{
	return _height;
}


inline GLenum Image::format() const
{
	return _format;
}


inline GLenum Image::type() const
{
	return _type;
}


inline unsigned Image::num_pixels() const
{
	return width() * height();
}


inline GLubyte *Image::pixels()
{
	return _pixels;
}


inline const GLubyte *Image::pixels() const
{
	return _pixels;
}


inline ColorRgb Image::pixel_rgb(int x, int y) const
{
	(void) x;
	(void) y;
	GTB_ERROR("not implemented\n");
	return COLOR_RGB_BLACK;
}


inline void Image::set_pixel_rgb(int x, int y, const ColorRgb &c)
{
    int format_width;
    int offset = (_width * y + x);

    switch(_format)
    {
    case GL_RGB:
        format_width = 3;
        break;
    case GL_RGBA:
        format_width = 4;
        break;
    default:
        assert(0);
#ifndef NO_EXCEPTIONS
        throw -1;
#endif
    }

    switch(_type)
    {
    case GL_UNSIGNED_BYTE:
        {
            unsigned char* p = (unsigned char*)_pixels + format_width * offset;
			p[0] = (unsigned char)(c.r()*255.0);
			p[1] = (unsigned char)(c.g()*255.0);
			p[2] = (unsigned char)(c.b()*255.0);
			if (_format == GL_RGBA) p[3] = 255;
        }
		break;
    case GL_FLOAT:
        {
            float* p = (float*)_pixels + format_width * offset;
			p[0] = c.r();
			p[1] = c.g();
			p[2] = c.b();
			if (_format == GL_RGBA) p[3] = 1.0;
        }
		break;
    default:
        assert(0);
#ifndef NO_EXCEPTIONS
        throw -1;
#endif
    }
}

inline ColorRgb Image::operator() (int x, int y) const
{
    int format_width;
    int offset = (_width * y + x);

    switch(_format)
    {
    case GL_RGB:
        format_width = 3;
        break;
    case GL_RGBA:
        format_width = 4;
        break;
    default:
        assert(0);
#ifndef NO_EXCEPTIONS
        throw -1;
#endif
    }

    switch(_type)
    {
    case GL_UNSIGNED_BYTE:
        {
            unsigned char* p = (unsigned char*)_pixels + format_width * offset;
            return ColorRgb(p[0] / 255.0, p[1] / 255.0, p[2] / 255.0);
        }
    case GL_FLOAT:
        {
            float* p = (float*)_pixels + format_width * offset;
            return ColorRgb(p[0], p[1], p[2]);
        }
    default:
        assert(0);
#ifndef NO_EXCEPTIONS
        throw -1;
#endif
    }
}



GTB_END_NAMESPACE
