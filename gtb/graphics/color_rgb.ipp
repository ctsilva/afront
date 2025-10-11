#include <assert.h>
#include <gtb/graphics/ogltools.h>
#include <gtb/io/io.hpp>


GTB_BEGIN_NAMESPACE


inline ColorRgb::ColorRgb()
{
    _c[0] = 0.0;
    _c[1] = 0.0;
    _c[2] = 0.0;
}


inline ColorRgb::ColorRgb(const ColorRgb &c)
{
    _c[0] = c._c[0];
    _c[1] = c._c[1];
    _c[2] = c._c[2];
}


inline ColorRgb::ColorRgb(GLfloat red, GLfloat green, GLfloat blue)
{
    _c[0] = red; 
    _c[1] = green; 
    _c[2] = blue;
}


inline ColorRgb::ColorRgb(const GLfloat a[3])
{
    _c[0] = a[0]; 
    _c[1] = a[1]; 
    _c[2] = a[2];
}


inline ColorRgb &ColorRgb::operator=(const ColorRgb &c)
{
    if (&c != this) {
        _c[0] = c._c[0];
        _c[1] = c._c[1];
        _c[2] = c._c[2];
    }
    return *this;
}


inline bool ColorRgb::operator==(const ColorRgb &c) const
{
    return (real::is_equal(_c[0], c._c[0]) &&
            real::is_equal(_c[1], c._c[1]) &&
            real::is_equal(_c[2], c._c[2]));
}


inline bool ColorRgb::operator!=(const ColorRgb &c) const
{
    return !(*this == c);
}


inline ColorRgb::operator const GLfloat*() const
{
    return _c;
}


inline ColorRgb::operator GLfloat*()
{
    return _c;
}


inline GLfloat ColorRgb::r() const
{
    return(_c[0]);
}


inline GLfloat ColorRgb::g() const
{
    return(_c[1]);
}


inline GLfloat ColorRgb::b() const
{
    return(_c[2]);
}


inline GLfloat ColorRgb::luminance() const
{
    return (r() * 76 +
            g() * 150 +
            b() * 29) / 256;
}

/*
 * Colorspace FAQ:
 HSL
 |X| |0.606881 0.173505 0.200336| |Red | 
 |Y| |0.298912 0.586611 0.114478| |Green| 
 |Z| |0.000000 0.066097 1.116157| |Blue | 

 D65
 |X| |0.430574 0.341550 0.178325| |Red | 
 |Y| |0.222015 0.706655 0.071330| |Green| 
 |Z| |0.020183 0.129553 0.939180| |Blue |
 * Y - achromatic component
 * X,Z related to chromatic components.
 *
 * Return luminance in [0,1]
 */
inline GLfloat ColorRgb::f_lum() const
{
    return r()*0.298912  + g()*0.586611 + b()*0.114478;
}

inline void ColorRgb::set_r(GLfloat red)
{
    _c[0] = red;
}


inline void ColorRgb::set_g(GLfloat green)
{
    _c[1] = green;
}


inline void ColorRgb::set_b(GLfloat blue)
{
    _c[2] = blue;
}


inline void ColorRgb::reset(GLfloat red, GLfloat green, GLfloat blue)
{
    _c[0] = red;
    _c[1] = green;
    _c[2] = blue;
}


inline ColorRgb &ColorRgb::operator+=(const ColorRgb &c)
{
    _c[0] += c._c[0];
    _c[1] += c._c[1];
    _c[2] += c._c[2];
    return *this;
}


inline ColorRgb &ColorRgb::operator-=(const ColorRgb &c)
{
    _c[0] -= c._c[0];
    _c[1] -= c._c[1];
    _c[2] -= c._c[2];
    return *this;
}


inline ColorRgb &ColorRgb::operator*=(const ColorRgb &c)
{
    _c[0] *= c._c[0];
    _c[1] *= c._c[1];
    _c[2] *= c._c[2];
    return *this;
}


inline ColorRgb &ColorRgb::operator*=(GLfloat a)
{
    _c[0] *= a;
    _c[1] *= a;
    _c[2] *= a;
    return *this;
}


inline ColorRgb &ColorRgb::operator/=(GLfloat a)
{
    assert(!real::is_zero(a));
    _c[0] /= a;
    _c[1] /= a;
    _c[2] /= a;
    return *this;
}


inline ColorRgb operator+(const ColorRgb &c1, const ColorRgb &c2)
{
    return ColorRgb(c1._c[0] + c2._c[0], 
                    c1._c[1] + c2._c[1], 
                    c1._c[2] + c2._c[2]);
}


inline ColorRgb operator-(const ColorRgb &c1, const ColorRgb &c2)
{
    return ColorRgb(c1._c[0] - c2._c[0], 
                    c1._c[1] - c2._c[1], 
                    c1._c[2] - c2._c[2]);
}


inline ColorRgb operator*(const ColorRgb &c1, const ColorRgb &c2)
{
    return ColorRgb(c1._c[0] * c2._c[0], 
                    c1._c[1] * c2._c[1], 
                    c1._c[2] * c2._c[2]);
}


inline ColorRgb operator*(GLfloat a, const ColorRgb &c)
{
    return ColorRgb(a * c._c[0], 
                    a * c._c[1], 
                    a * c._c[2]);
}


inline ColorRgb operator*(const ColorRgb &c, GLfloat a)
{
    return a * c;
}


inline ColorRgb operator/(const ColorRgb &c, GLfloat a)
{
    return ColorRgb(c._c[0] / a, 
                    c._c[1] / a, 
                    c._c[2] / a);
}


inline void ColorRgb::load() const
{
#ifdef REAL_IS_FLOAT
    glColor3f(_c[0], _c[1], _c[2]);
#else
    glColor3d(_c[0], _c[1], _c[2]);
#endif
}


inline void ColorRgb::read(FILE *fp)
{
#ifdef REAL_IS_FLOAT
    read_float(&_c[0], fp);
    read_float(&_c[1], fp);
    read_float(&_c[2], fp);
#else
    float cx, cy, cz;
    read_float(&cx, fp);
    read_float(&cy, fp);
    read_float(&cz, fp);
    reset(cx, cy, cz);
#endif
}


inline void ColorRgb::write(FILE *fp) const
{
    write_float(_c[0], fp);
    write_float(_c[1], fp);
    write_float(_c[2], fp);
}

inline GLfloat dist(const ColorRgb& l, const ColorRgb& r)
{
    double dr = l.r() - r.r();
    double dg = l.g() - r.g();
    double db = l.b() - r.b();
    return sqrt(dr*dr + dg*dg + db*db);
}

/*------------------ Tools ------------------*/
GTB_END_NAMESPACE

