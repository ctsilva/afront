#ifndef GTB_CAMERA_PATH_INCLUDED
#define GTB_CAMERA_PATH_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/camera.hpp>

GTB_BEGIN_NAMESPACE


class CameraPath {
public:
	CameraPath();

	void add_camera(const Camera &c);
	void clear_cameras();
	unsigned num_cameras() const;
	const Camera &camera(unsigned i) const;
	const Camera &current_camera() const;

	void set_current_frame(unsigned i);
	void advance_frame();
	unsigned current_frame() const;

	void read(const char *file_name);
	void write(const char *file_name) const;

protected:
	int _current_frame;
	std::vector<Camera> _cameras;
};


GTB_END_NAMESPACE

#endif // GTB_CAMERA_PATH_INCLUDED
