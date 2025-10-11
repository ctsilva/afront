#ifndef FRONT_INCLUDED
#define FRONT_INCLUDED

#include <gtb/graphics/octree_node.hpp>
#include <set>

GTB_BEGIN_NAMESPACE


typedef std::set<OctreeNode *, OctreeNode::CompareSolidity> plp_front;


GTB_END_NAMESPACE

#ifndef OUTLINE
#include "front.ipp"
#endif

#endif // FRONT_INCLUDED
