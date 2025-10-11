#ifndef GTB_MATERIAL_INCLUDED
#define GTB_MATERIAL_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/color_rgb.hpp>


GTB_BEGIN_NAMESPACE


class Material {
public:
	Material();
	~Material();

	void set_ambient_color(const ColorRgb &c);
	void set_diffuse_color(const ColorRgb &c);
	void set_specular_color(const ColorRgb &c);
	void set_emissive_color(const ColorRgb &c);
	void set_shininess(real_t shininess);
	void set_transparency(real_t transparency);

	const ColorRgb &ambient_color() const;
	const ColorRgb &diffuse_color() const;
	const ColorRgb &specular_color() const;
	const ColorRgb &emissive_color() const;
	real_t shininess() const;
	real_t transparency() const;

	void load() const;

protected:
	ColorRgb _ambient_color;
	ColorRgb _diffuse_color;
	ColorRgb _specular_color;
	ColorRgb _emissive_color;
	real_t _shininess;
	real_t _transparency;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/material.ipp>
#endif

#endif // GTB_MATERIAL_INCLUDED
