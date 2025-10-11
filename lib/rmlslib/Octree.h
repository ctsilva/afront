/**
 * class comment.
 */

#ifndef __OCTREE_H
#define __OCTREE_H

#include <vector>
#include "Point.h"
#include "OctreeNode.h"
#include <queue>

class OctreeNode;

enum ObjType { NODE_PTR, POINT_INDEX };

/* 
 * Temporary container of both Point and OctreeNode object 
 */

class MixType {
  
 public:	

  /** 
   * constructors of the mixtype
   */
  MixType(int index, const std::vector<Point>& vertices, const Point& ref);
  MixType(OctreeNode* pNode,const Point& ref);
  
  /** 
   * desstructors of the mixtype
   */
  virtual ~MixType();
	
  /**
   * Mixtype component accessors.
   */
  int	  getIndex() const;
  OctreeNode* getNodePtr() const;
  float getDist() const;
  int	  getType() const;

  /**
   * Mixtype data accessors.
   */
  float m_fDist;
  int   m_itype;
  union {
    int index;
    OctreeNode* m_pOctreeNode;
  };
};

/**
 * Mixtype compare operator
 */
inline bool operator <(const MixType& a,const MixType& b)
{
  return (a.getDist()>b.getDist());
}

/* 
 * Octree class
 */

class Octree{
 public:
  /** 
   * constructors of the Octree
   */
  Octree(std::vector<Point>* verticies);
  
  /** 
   * destructor of the Octree
   */
  ~Octree();

  /**
    * octree component accessors.
    */
  unsigned int getMaxNumVerticesPerNode() const;
  unsigned int getMaxDepth() const;
  OctreeNode* getRoot() const;
  
  /**
   * octreenode statistical function
   */
  int numObject() const;
  int numNode() const;
  int maxNumObject() const;

  /**
   * octreenode operation function
   */
  void buildTree(int maxPoint, int maxDepth);
  void boundBox(Point& center, float& halfWidth, const std::vector<Point>& vertexList) const;
  std::vector<int> kNN( const Point &ref, 
			const unsigned int &k,
			const std::vector<Point> &vertices) const;

  std::vector<int> kNNBF(const Point &ref, 
			 const unsigned int &k, 
			 const std::vector<Point> &vertices ) const;

  std::vector<int> kNNM(const Point &ref, 
			const unsigned int &k, 
			const std::vector<Point>& vertices, 
			std::priority_queue<MixType>& q) const;
  int nearestNeighbor(const Point ref, std::vector<Point> vertices, std::priority_queue<MixType>& q) const;
  void kNNMV1(int* ret,const Point ref, int k, const std::vector<Point>& vertices, std::priority_queue<MixType>& q1) const;
  void kNNMV1(int* ret,const Point ref, int k, const std::vector<Point>& vertices) const;

  std::vector< unsigned int > distanceNeighbors( const Point &ref, const float &distance ) const;

  Point m_center;
  float m_halfWidth;

private:
  unsigned int  m_iMaxPointsPerNode;
  unsigned int  m_iMaxSubdivisions; 
  OctreeNode* m_pRootOctree;
  std::vector<Point>* m_pVertices;
};

#endif
