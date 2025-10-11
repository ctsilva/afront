#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/color_rgb.hpp>
#include <gtb/io/io.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/color_rgb.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


const ColorRgb COLOR_RGB_BLACK(0.0, 0.0, 0.0);
const ColorRgb COLOR_RGB_RED(1.0, 0.0, 0.0);
const ColorRgb COLOR_RGB_GREEN(0.0, 1.0, 0.0);
const ColorRgb COLOR_RGB_BLUE(0.0, 0.0, 1.0);
const ColorRgb COLOR_RGB_YELLOW(1.0, 1.0, 0.0);
const ColorRgb COLOR_RGB_CYAN(0.0, 1.0, 1.0);
const ColorRgb COLOR_RGB_MAGENTA(1.0, 0.0, 1.0);
const ColorRgb COLOR_RGB_WHITE(1.0, 1.0, 1.0);
const ColorRgb COLOR_RGB_GRAY25(0.25, 0.25, 0.25);
const ColorRgb COLOR_RGB_GRAY50(0.5, 0.5, 0.5);
const ColorRgb COLOR_RGB_GRAY75(0.75, 0.75, 0.75);


GTB_END_NAMESPACE
