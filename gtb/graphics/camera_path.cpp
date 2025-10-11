#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/camera_path.hpp>
#include <gtb/io/io.hpp>
#endif // WIN32


GTB_BEGIN_NAMESPACE


CameraPath::CameraPath()
	: _current_frame(-1)
{
}


void CameraPath::add_camera(const Camera &c)
{
	_cameras.push_back(c);
}


void CameraPath::clear_cameras()
{
	_cameras.clear();
	_current_frame = -1;
}


unsigned CameraPath::num_cameras() const
{
	return _cameras.size();
}


const Camera &CameraPath::camera(unsigned i) const
{
	assert(i < _cameras.size());
	return _cameras[i];
}


const Camera &CameraPath::current_camera() const
{
	assert(_cameras.size() > 0);
	assert(_current_frame >= 0);
	return _cameras[_current_frame];
}


void CameraPath::set_current_frame(unsigned i)
{
	assert(i < _cameras.size());
	_current_frame = i;
}


void CameraPath::advance_frame()
{
	assert(_cameras.size() > 0);
	assert(_current_frame >= 0);
	_current_frame = (_current_frame + 1) % _cameras.size();
}


unsigned CameraPath::current_frame() const
{
	assert(_cameras.size() > 0);
	assert(_current_frame >= 0);
	return _current_frame;
}


void CameraPath::read(const char *file_name)
{
	_cameras.clear();
	_current_frame = 0;
	FILE *fp = xfopen(file_name, "rb");
	Camera c;
	while (c.read(fp)) {
		_cameras.push_back(c);
	}
	fclose(fp);
}


void CameraPath::write(const char *file_name) const
{
	FILE *fp = fopen(file_name, "wb");
	assert(fp != NULL);
	for (unsigned i = 0; i < _cameras.size(); i++) {
		_cameras[i].write(fp);
	}
	fclose(fp);
}


GTB_END_NAMESPACE
