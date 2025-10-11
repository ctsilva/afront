#ifndef GTB_ICOSAHEDRON_INCLUDED
#define GTB_ICOSAHEDRON_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/box3.hpp>
#include <gtb/graphics/ogltools.h>

GTB_BEGIN_NAMESPACE


class icosahedron {
public:
	icosahedron();
	static void render(unsigned depth = 0);
	static Box3 bounding_box();
	static Point3 centroid();

	static void sample_points(std::vector<Point3> &points,
				  unsigned depth = 0);

	enum {
		NUM_VERTICES = 12,
		NUM_FACES = 20
	};

protected:
	static const double m_X;
	static const double m_Z;
	static GLfloat m_vdata[NUM_VERTICES][3];
	static GLuint m_tindices[NUM_FACES][3];
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/icosahedron.ipp>
#endif

#endif // GTB_ICOSAHEDRON_INCLUDED
