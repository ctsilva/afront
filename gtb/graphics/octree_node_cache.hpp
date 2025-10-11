#ifndef OCTREE_NODE_CACHE_INCLUDED
#define OCTREE_NODE_CACHE_INCLUDED

#include <gtb/graphics/octree_node.hpp>
#include <list>
#include <map>
#include <string>

GTB_BEGIN_NAMESPACE


class OctreeNodeCache {
public:
	explicit OctreeNodeCache(const char *file_name_prefix);
	~OctreeNodeCache();

	// bring to memory, and assign high priority
	void fetch(OctreeNode *node);

	// bring to memory, and assign low priority
	void prefetch(OctreeNode *node, bool &node_was_in_cache);

	void reset_counters();
	unsigned num_hits() const;
	unsigned num_misses() const;
	unsigned num_prefetches() const;

	enum { DEFAULT_SIZE = 50 };
	void set_size(unsigned size);
	void double_size();
	void halve_size();
	unsigned size() const;

	const std::list<OctreeNode *> &nodes() const;
    bool contains(const std::vector<OctreeNode *> &nodes) const;

protected:
	void read_node_data(OctreeNode *node) const;

	typedef std::list<OctreeNode *> NodeList;
	typedef std::map<OctreeNode *, NodeList::iterator> NodeMap;

	std::string _file_name_prefix;
	NodeList _nodes;
	NodeMap _node_map;
	unsigned _size;
	unsigned _num_hits;
	unsigned _num_misses;
	unsigned _num_prefetches;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include "octree_node_cache.ipp"
#endif

#endif // OCTREE_NODE_CACHE_INCLUDED
