GTB_BEGIN_NAMESPACE


inline void Light::enable()
{
	glEnable(_light);
}


inline void Light::disable()
{
	glDisable(_light);
}


inline bool Light::is_enabled()
{
	GLboolean enabled;
	glGetBooleanv(_light, &enabled);
	return enabled != 0;
}


inline void Light::set_ambient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	_ambient[0] = r;
	_ambient[1] = g;
	_ambient[2] = b;
	_ambient[3] = a;
}


inline void Light::set_diffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	_diffuse[0] = r;
	_diffuse[1] = g;
	_diffuse[2] = b;
	_diffuse[3] = a;
}


inline void Light::set_specular(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	_specular[0] = r;
	_specular[1] = g;
	_specular[2] = b;
	_specular[3] = a;
}


inline void Light::set_position(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
	_position[3] = w;
}


inline void Light::load() const
{
	glLightfv(_light, GL_AMBIENT, _ambient);
	glLightfv(_light, GL_DIFFUSE, _diffuse);
	glLightfv(_light, GL_SPECULAR, _specular);
	glLightfv(_light, GL_POSITION, _position);
	glEnable(_light);
}


GTB_END_NAMESPACE
