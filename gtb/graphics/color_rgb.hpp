#ifndef GTB_COLOR_RGB_INCLUDED
#define GTB_COLOR_RGB_INCLUDED

#include <gtb/common.hpp>
#include <gtb/real/real.hpp>


GTB_BEGIN_NAMESPACE


class ColorRgb {
public:
    ColorRgb();
    ColorRgb(const ColorRgb &c);
    ColorRgb(GLfloat red, GLfloat green, GLfloat blue);
    ColorRgb(const GLfloat a[3]);
    ColorRgb &operator=(const ColorRgb &c);

    bool operator==(const ColorRgb &c) const;
    bool operator!=(const ColorRgb &c) const;

    operator const GLfloat*() const;
    operator GLfloat*();

    GLfloat r() const;
    GLfloat g() const;
    GLfloat b() const;
    GLfloat luminance() const;
    GLfloat f_lum() const;

    void set_r(GLfloat red);
    void set_g(GLfloat green);
    void set_b(GLfloat blue);
    void reset(GLfloat red, GLfloat green, GLfloat blue);

    void load() const;

    void read(FILE *fp);
    void write(FILE *fp) const;

    ColorRgb &operator+=(const ColorRgb &c);
    ColorRgb &operator-=(const ColorRgb &c);
    ColorRgb &operator*=(const ColorRgb &c);
    ColorRgb &operator*=(GLfloat a);
    ColorRgb &operator/=(GLfloat a);
 
    friend ColorRgb operator+(const ColorRgb &c1, const ColorRgb &c2);
    friend ColorRgb operator-(const ColorRgb &c1, const ColorRgb &c2);
    friend ColorRgb operator*(const ColorRgb &c1, const ColorRgb &c2);
    friend ColorRgb operator*(GLfloat a, const ColorRgb &c);
    friend ColorRgb operator*(const ColorRgb &c, GLfloat a);
    friend ColorRgb operator/(const ColorRgb &c, GLfloat a);

protected:
    GLfloat _c[3];
};

/*------------------ Tools ------------------*/
GLfloat dist(const ColorRgb& l, const ColorRgb& r);

extern const ColorRgb COLOR_RGB_BLACK;
extern const ColorRgb COLOR_RGB_RED;
extern const ColorRgb COLOR_RGB_GREEN;
extern const ColorRgb COLOR_RGB_BLUE;
extern const ColorRgb COLOR_RGB_YELLOW;
extern const ColorRgb COLOR_RGB_CYAN;
extern const ColorRgb COLOR_RGB_MAGENTA;
extern const ColorRgb COLOR_RGB_WHITE;
extern const ColorRgb COLOR_RGB_GRAY25;
extern const ColorRgb COLOR_RGB_GRAY50;
extern const ColorRgb COLOR_RGB_GRAY75;


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/color_rgb.ipp>
#endif

#endif // GTB_COLOR_RGB_INCLUDED
