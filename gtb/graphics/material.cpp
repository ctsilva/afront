#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/material.hpp>
#include <gtb/graphics/ogltools.h>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/material.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


Material::Material()
	: _ambient_color(ColorRgb(0.2, 0.2, 0.2)),
	  _diffuse_color(ColorRgb(0.8, 0.8, 0.8)),
	  _specular_color(ColorRgb(0.0, 0.0, 0.0)),
	  _emissive_color(ColorRgb(0.0, 0.0, 0.0)),
	  _shininess(0.2),
	  _transparency(0.0)
{
}


Material::~Material()
{
}


void Material::load() const
{
	GLfloat alpha = 1.0 - _transparency;

	GLfloat ambient[4] = {
		_ambient_color.r(),
		_ambient_color.g(),
		_ambient_color.b(),
		alpha
	};

	GLfloat diffuse[4] = {
		_diffuse_color.r(),
		_diffuse_color.g(),
		_diffuse_color.b(),
		alpha
	};

	GLfloat specular[4] = {
		_specular_color.r(),
		_specular_color.g(),
		_specular_color.b(),
		alpha
	};

	GLfloat emissive[4] = {
		_emissive_color.r(),
		_emissive_color.g(),
		_emissive_color.b(),
		alpha
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _shininess);
}


GTB_END_NAMESPACE
