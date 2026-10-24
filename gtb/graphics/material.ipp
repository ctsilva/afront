
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


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
