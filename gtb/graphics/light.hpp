#ifndef GTB_LIGHT_INCLUDED
#define GTB_LIGHT_INCLUDED

#include <gtb/graphics/ogltools.h>
#include <gtb/common.hpp>

GTB_BEGIN_NAMESPACE


class Light {
public:
	Light(GLenum light);

	void enable();
	void disable();
	bool is_enabled();

	void set_ambient(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void set_diffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void set_specular(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void set_position(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void load() const;

protected:
	GLenum _light;
	bool _is_enabled;
	GLfloat _ambient[4];
	GLfloat _diffuse[4];
	GLfloat _specular[4];
	GLfloat _position[4];
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/light.ipp>
#endif

#endif // GTB_LIGHT_INCLUDED
