#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/sphere.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/sphere.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


template <class T>
tSphere<T>::tSphere()
{
}


template <class T>
tSphere<T>::tSphere(const Point3 &c, value_type r)
	: _center(c),
	  _radius(r)
{
}


template <class T>
tSphere<T>::tSphere(const tSphere &s)
	: _center(s._center),
	  _radius(s._radius)
{
}


template <class T>
void tSphere<T>::render(int slices,
		    int stacks,
		    GLenum draw_style,
		    GLenum normal_type) const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	const Point3 &so = center();
	glTranslatef(so.x(), so.y(), so.z());
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, draw_style);
	gluQuadricNormals(qobj, normal_type);
	gluSphere(qobj, radius(), slices, stacks);
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

template class tSphere<float>;
template class tSphere<double>;

GTB_END_NAMESPACE
