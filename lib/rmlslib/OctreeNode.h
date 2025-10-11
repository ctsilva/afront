/**
 *
 * file:	Octree.h
 * description:	Stores structures and compute for each node of the Octree
 *	        primitives so attributes can be saved for the
 *		different types.
 *
 * author:	Linh Ha
 * date:	March 2006
 *
 */

#ifndef __OCTREENODE_H
#define __OCTREENODE_H

#include "Octree.h"

#include "Point.h"

class Octree;
struct AABB {
	Point min, max;
};

class OctreeNode 
{
public:
  /** 
   * constructors of the octree node.
   */
	OctreeNode(const Point center,float halfWidth,int depth);

   /** 
   * destructor of the octree node.
   */

	~OctreeNode();
   /**
    * octreenode component accessors.
    */
	int getOctant() const;
	int getNumberOfPoint()const;
	int getDepth() const;
	OctreeNode* getChild(int i);
	AABB getBoundBox()const;

	/**
    * octreenode status
    */
	bool isLeaf() const;
    bool hasChild(int i) const;
	
	/**
    * octreenode statistical function
    */
	
	int numObject();
	int numNode();

	int maxNumObject() const;
	int minNumObject() const;
	
	/**
    * octree node oprerational functions
    */
	int octant(const Point& p);
	void createChild(int o, Octree& tree);
	bool insertVertex(int index, const std::vector<Point>& vertices, Octree& tree);
	/**
    * octreenode point iterator
    */
	std::vector<int>::iterator begin();
	std::vector<int>::iterator end();

private:
	OctreeNode* m_pChildren[8];
	std::vector<int> m_pObjList;
	Point m_vCenter;
	float m_fHalfWidth;
	unsigned int   m_iLevel;
	bool m_isLeaf;
	unsigned int m_octant;
};

// Computes the square distance between a point p and an AABB b
inline float sqDist(const Point& p,const OctreeNode& node)
{
	AABB b=node.getBoundBox();
    float sqDist = 0.0f;
    for (int i = 0; i < 3; i++) {
        // For each axis count any excess distance outside box extents
        float v = p[i];
		if (v<b.min[i]) sqDist+= (b.min[i] -v)*(b.min[i] -v);
		if (v>b.max[i])	sqDist+= (v - b.max[i] )*(v-b.max[i]);
    }
    return sqDist;
}
// Computes the square distance between 2 point p and p1
inline float sqDist(const Point p1, const Point p2){
	//return p.distance_squared(p1);
	return ((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y())+(p1.z()-p2.z())*(p1.z()-p2.z()));
}
#endif
