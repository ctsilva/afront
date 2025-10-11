GTB_BEGIN_NAMESPACE


inline void Material::set_ambient_color(const ColorRgb &c)
{
	_ambient_color = c;
}


inline void Material::set_diffuse_color(const ColorRgb &c)
{
	_diffuse_color = c;
}


inline void Material::set_specular_color(const ColorRgb &c)
{
	_specular_color = c;
}


inline void Material::set_emissive_color(const ColorRgb &c)
{
	_emissive_color = c;
}


inline void Material::set_shininess(real_t arg_shininess)
{
	_shininess = arg_shininess;
}


inline void Material::set_transparency(real_t arg_transparency)
{
	_transparency = arg_transparency;
}


inline const ColorRgb &Material::ambient_color() const
{
	return _ambient_color;
}


inline const ColorRgb &Material::diffuse_color() const
{
	return _diffuse_color;
}


inline const ColorRgb &Material::specular_color() const
{
	return _specular_color;
}


inline const ColorRgb &Material::emissive_color() const
{
	return _emissive_color;
}


inline real_t Material::shininess() const
{
	return _shininess;
}


inline real_t Material::transparency() const
{
	return _transparency;
}


GTB_END_NAMESPACE
