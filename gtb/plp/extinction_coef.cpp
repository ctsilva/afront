#include <gtb/gtb.hpp>
#include "extinction_coef.hpp"

#ifdef OUTLINE
#define inline
#include "extinction_coef.ipp"
#undef inline
#endif

GTB_BEGIN_NAMESPACE


// Francois X. Sillion.  A Unified Hierarchical Algorithm for Global
// Illumination with Scattering Volumes and Object Clusters.  IEEE
// TVCG 1(3), 1995.  Equation (15).
extinction_coef::extinction_coef(const OctreeNode &node)
{
	assert(node.has_model());
	const IndexedTriangleSet &its = node.model();
	float V = node.volume();
	float A = 0.0;
	for (unsigned i = 0; i < its.num_triangles(); i++) {
		A += its.triangle(i).area();
	}
	m_value = A / (4.0 * V);
}


// Francois X. Sillion, George Drettakis, and Cyril Soler.  A
// Clustering Algorithm for Radiance Calculation in General
// Environments.  Eurographics Rendering Workshop 1995.  Equation (6).
extinction_coef::extinction_coef(const OctreeNode &node, const Vector3 &v)
{
	assert(node.has_model());
	const IndexedTriangleSet &its = node.model();
	float V = node.volume();
	float A = 0.0;
	for (unsigned i = 0; i < its.num_triangles(); i++) {
		Triangle3 t = its.triangle(i);
		Vector3 n = t.normal();
		if (!isfinite(n.x()) || !isfinite(n.y()) || !isfinite(n.z())) {
			// ignore degenerate triangles
			continue;
		}
		float f = n.dot(v);
		assert(fabs(f) <= 1.0);
		if (f > 0.0) {
			A += its.triangle(i).area() * f;
		}
	}
	// Note that the factor of 4 from the isotripic formula is no
	// longer present.
	float k = A / V;
	assert(k >= 0.0);
	// Hack: In theory, k <= 1 always.  But because of overlapping
	// triangles, k may end up being > 1, and we then clamp it.
	// We really should get rid of overlapping triangles...
	if (k > 1.0) {
		k = 1.0;
	}
	m_value = k;
}


GTB_END_NAMESPACE
