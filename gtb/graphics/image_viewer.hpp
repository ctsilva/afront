#ifndef GTB_IMAGE_VIEWER_INCLUDED
#define GTB_IMAGE_VIEWER_INCLUDED

#include <gtb/common.hpp>
#include <gtb/ui/ui.hpp>
#include <gtb/graphics/image.hpp>
#include <gtb/graphics/view.hpp>
#include <map>


GTB_BEGIN_NAMESPACE


class ImageViewer {
public:
	enum Capability { SHOW_IMAGE, NUM_CAPS };

	enum Mode { LOOK_MODE, PICK_MODE, NUM_MODES };

	ImageViewer();
	~ImageViewer();

	void set_image(const Image &img);
	const Image &image() const;

	void set_window(int win);
	int window() const;

	Mouse &mouse();
	const Mouse &mouse() const;

	View &view();
	const View &view() const;

	void enable(Capability cap);
	void disable(Capability cap);
	void toggle(Capability cap);
	bool is_enabled(Capability cap) const;

	void add_point(const Point2 &p);
	unsigned num_points() const;
	const Point2 &point(unsigned i) const;
	Point2 &point(unsigned i);
	const std::vector<Point2> &points() const;
	std::vector<Point2> &points();
	void load_points(const char *file_name);
	void save_points(const char *file_name) const;

	void post_redisplay() const;

	void set_mode(unsigned mode);
	unsigned mode() const;

	void set_zoom_factor(float zf);
	float zoom_factor() const;

	void pick(int x, int y);
	bool has_picked_point() const;
	Point2 &picked_point();
	const Point2 &picked_point() const;
	int picked_point_index() const;
	void go_to_next_point();
	void go_to_previous_point();
	void center_on(const Point2 &p);

	void draw_image() const;
	void draw_points() const;
	void draw_point_indices() const;

	Point2 image_to_viewport(const Point2 &p) const;
	Point2 viewport_to_image(const Point2 &p) const;

	void on_mouse(int button, int state, int x, int y);
	void on_motion(int x, int y);

protected:
	void clamp_origin();
	void clamp_point(Point2 &p) const;

	const Image *_image;
	int _window;
	Mouse _mouse;
	View _view;
	bool _caps[NUM_CAPS];
	std::vector<Point2> _points;
	int _mode;
	float _zoom_factor;
	int _picked_point_index;
	Point2 _origin;	// what we really needed is point_2<int>
	unsigned _pick_radius;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/image_viewer.ipp>
#endif

#endif // GTB_IMAGE_VIEWER_INCLUDED
