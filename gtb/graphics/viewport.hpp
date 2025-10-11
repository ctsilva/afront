#ifndef GTB_VIEWPORT_INCLUDED
#define GTB_VIEWPORT_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/point2.hpp>

GTB_BEGIN_NAMESPACE


class Viewport {
public:
	Viewport();
	Viewport(int x_min, int y_min, int width, int height);
	Viewport(const Viewport &vp);
	Viewport &operator=(const Viewport &vp);

	int x_min() const;
	int y_min() const;

	int x_max() const;
	int y_max() const;

	int x_center() const;
	int y_center() const;
	Point2 center() const;

	int width() const;
	int height() const;

	void resize(int x_min, int y_min, int width, int height);

	void load() const;

protected:
	int _x_min, _y_min;
	int _width, _height;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/viewport.ipp>
#endif

#endif // GTB_VIEWPORT_INCLUDED
