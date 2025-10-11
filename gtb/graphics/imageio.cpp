#include <gtb/gtb.hpp>

GTB_BEGIN_NAMESPACE

void write_ppm(const char* name, const Image& image)
{
    int format = image.format();
    int type = image.type();
    int format_width;

    afree<FILE*> f(fopen(name, "wb"), fclose);
    if (f == 0)
#ifndef NO_EXCEPTIONS
      throw CErr("write_ppm error opening file");
#else
      return;
#endif

    switch(format)
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

    int w = image.width();
    int h = image.height();
    fprintf(f, "P6\n%d %d\n255\n", w, h);

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            int offset = (w * y + x);

            switch(type)
            {
            case GL_UNSIGNED_BYTE:
            {
                unsigned char* p = (unsigned char*)image.pixels() + format_width * offset;
                fwrite(p, 3, 1, f);
            }
            break;
            case GL_FLOAT:
            {
                float* p = (float*)image.pixels() + format_width * offset;
                unsigned char rgb[3];
                rgb[0] = (unsigned char)(p[0]*255.0);
                rgb[1] = (unsigned char)(p[1]*255.0);
                rgb[2] = (unsigned char)(p[2]*255.0);
                fwrite(rgb, 3, 1, f);
            }
            break;
            default:
                assert(0);
#ifndef NO_EXCEPTIONS
                throw -1;
#endif
            }
        }
    }
}

GTB_END_NAMESPACE
