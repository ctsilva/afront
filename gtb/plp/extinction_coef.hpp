#ifndef EXTINCTION_COEF_INCLUDED
#define EXTINCTION_COEF_INCLUDED

#include <gtb/graphics/octree_node.hpp>

GTB_BEGIN_NAMESPACE


class extinction_coef {
public:
	explicit extinction_coef(float value);
	explicit extinction_coef(const OctreeNode &node);
	extinction_coef(const OctreeNode &node, const Vector3 &v);
	operator float() const;

protected:
	float m_value;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include "extinction_coef.ipp"
#endif

#endif // EXTINCTION_COEF_INCLUDED
