/**
 *
 * file:	Octree.cpp
 * description:	
 *	    Contruction and opreation function on the Octree node class
 *
 * author:	Linh Ha
 * date:	March 2006
 *
 */
#include "OctreeNode.h"
#include <iostream>

using namespace std;

#define NUM_CHILDREN 8

/** 
 * Constructors of the octree node 
 *	center    : Center of Bounding Box
 *  halfWidth : half size of bonding box 
 *  depth : current depthh of this node
 */
OctreeNode::OctreeNode(const Point center,float halfWidth, int depth)
		:m_vCenter(center),m_fHalfWidth(halfWidth),m_iLevel(depth)
{
  for (int i=0;i<NUM_CHILDREN;i++)
		m_pChildren[i]=NULL;
	m_isLeaf=true;
}

 /** 
   * destructor of the octree node.
   */
OctreeNode::~OctreeNode(){
	for (unsigned i = 0; i < NUM_CHILDREN; i++) {
		if (m_pChildren[i] != NULL)
			delete m_pChildren[i];
	}
}
 /**
  * return the relative position of octreenode in sapce of parent node
  */
int OctreeNode::getOctant() const
{
	return m_octant;
};

/**
 * return number of point contain in the leaf
 */
int OctreeNode::getNumberOfPoint()const
{
	return m_pObjList.size();
}

/**
 * return current depth
 */

int OctreeNode::getDepth() const
{
	return m_iLevel;
}

/**
 * return pointer to the i(th) child or (child in octant i)
 */

OctreeNode* OctreeNode::getChild(int i){
	return m_pChildren[i];
}

/**
 * return the bounding box of this node in term (Pmax,Pmin)
 */

AABB OctreeNode::getBoundBox() const{
	AABB b;
	b.min= m_vCenter - Vector(m_fHalfWidth);
	b.max= m_vCenter + Vector(m_fHalfWidth);
	return b;
}

//----------Octreenode status----------------------------------------

/**
  * Is leaf node
  */
bool OctreeNode::isLeaf() const{
	return m_isLeaf;
}

/**
  * Has child i 
  */
bool OctreeNode::hasChild(int i) const 
{
	return (m_pChildren[i] != NULL);
}

//--------- octreenode statistical function -----------------

/**
  * Total of object contain in this the tree under this node
  */
int OctreeNode::numObject(){
	if (isLeaf()) 
		return m_pObjList.size();
	else {
		int sum = 0;
		for (int i=0;i<8;i++)
			if (m_pChildren[i] != NULL)
			{
				sum+=m_pChildren[i]->numObject();
			}
		return sum;
	}
}

/**
  * Total number of node contain in this the tree under this node
  */
int OctreeNode::numNode(){
	if (isLeaf()) {
		if (m_pObjList.size()>0) return 1;
		return 0;
	}
	else {
		int sum = 0;
		for (int i=0;i<8;i++)
			if (m_pChildren[i] != NULL)
			{
				sum+=m_pChildren[i]->numNode();
			}
		return sum;
	}
}

/* Function maxNumObject 
	Statitical properties of Octree
	Get minimun number of object contain in the leaf node 
*/
int OctreeNode::maxNumObject() const{
	if (isLeaf()) 
		return m_pObjList.size();
	else {
		int maxObject = 0;
		for (int i=0;i<8;i++)
			if (m_pChildren[i] != NULL)
			{
				int nObject = m_pChildren[i]->maxNumObject();
				if (nObject > maxObject) maxObject = nObject;
			}
		return maxObject;
	}
}
/* Function minNumObject 
	Statitical properties of Octree
	Get minimun number of object contain in the leaf node 
*/
int OctreeNode::minNumObject() const{
	if (isLeaf()) 
		return m_pObjList.size();
	else {
		int minObject = INT_MAX;
		for (int i=0;i<8;i++)
			if (m_pChildren[i] != NULL)
			{
				int nObject = m_pChildren[i]->minNumObject();
				if (nObject < minObject) minObject = nObject;
			}
		return minObject;
	}
}

//-------------------octreenode oprerational functions--------------------------

/**
  * Return the octant of the node the point belong
  */
int OctreeNode::octant(const Point& p){
	int index=0;
	for (int i=0;i<3;i++){
		if (p[i]-m_vCenter[i]>0.0) index |= (1<<i);
	}
	return index;
}

/**
  * Insert new vertex in to the octree node
  *   index : index of the new node
  *   vertices : container of Point set
  *   tree : current octree
  */
bool OctreeNode::insertVertex(int index, const vector<Point>& vertices, Octree& tree){
	const Point& v=vertices[index];
	//find leaf that contain it
	if (!isLeaf())
	{
		int o=octant(v);
		if (m_pChildren[o]==NULL)
			createChild(o,tree);	
		return m_pChildren[o]->insertVertex(index, vertices, tree);
	}
	// add point to this leaf node unstil node is not full
	m_pObjList.push_back(index);
	if (m_pObjList.size() <= tree.getMaxNumVerticesPerNode()) 
		return true;
	// If full but reach the max depth stop subdivide
	if (m_iLevel > tree.getMaxDepth()) {
		//cout << "maximum octree depth reached" <<endl;
		return true;
	}

	// If full but don't reach the max depth subdivide
	m_isLeaf=false;
	for (unsigned i=0;i<m_pObjList.size();i++){
		int pointIndex= m_pObjList[i];
		Point v2 = vertices[pointIndex];
		int o=octant(v2);
		if (m_pChildren[o]==NULL)
			createChild(o,tree);
		if (!m_pChildren[o]->insertVertex(pointIndex,vertices,tree)){
			cout<< "Error create tree "; 
			return false;
		}
	}
	m_pObjList.clear();
	return true;
}

/**
  * Create the memory allocation for a new child
  */

void OctreeNode::createChild(int i, Octree& tree){
	float step= m_fHalfWidth * 0.5;
	float x = ((i & 1)? step : -step); 
	float y = ((i & 2)? step : -step);
	float z = ((i & 4)? step : -step);
	Vector offset(x,y,z);
	Point newCenter = m_vCenter + offset;
	m_pChildren[i] = new OctreeNode(newCenter, step, m_iLevel +1);
}
/**
  * octreenode point iterator
  */
vector<int>::iterator OctreeNode::begin() 
{
	return m_pObjList.begin();
}

vector<int>::iterator OctreeNode::end()
{
	return m_pObjList.end();
}
