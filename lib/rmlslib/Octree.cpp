
#include <vector>
#include <fstream>

#include "Octree.h"
#include "OctreeNode.h"
#include "float.h"

using namespace std;

/** 
 * constructors of the mixtype for Point 
 *	index: index of the point
 *	vertices: container of Point set
 *      ref : referance point 
 */
MixType::MixType(int index, const vector<Point>& vertices, const Point& ref)
:index(index)
{
  m_fDist = sqDist(vertices[index],ref);
  m_itype =1;	
}

/** 
   * constructors of the mixtype for OctreeNode
   *	pNode: pointer to the node
   *    ref : referance point 
   */
MixType::MixType(OctreeNode* pNode,const Point& ref)
:m_pOctreeNode(pNode)
{
  m_fDist = sqDist(ref,*m_pOctreeNode);
  m_itype = 0;
};

/** 
  * destructor of the mixtype for Point 
  */
MixType::~MixType()
{
  m_pOctreeNode = NULL;
}

/** 
  * getType of the node
  */
int MixType::getType() const{
  return m_itype;
}

/** 
  * return the distant to the referance point
  */

float MixType::getDist() const{
  return m_fDist;
}

/** 
  * return index for Point type
  */
int MixType::getIndex() const{
  return index;
}

/** 
  * return Node pointer
  */
OctreeNode* MixType::getNodePtr() const{
  return m_pOctreeNode;
}
/** 
   * constructors of Octree
   *    vertices : Point Set
   */
Octree::Octree(vector<Point>* vertices):m_pVertices(vertices)
{
	m_pRootOctree = NULL;
};

/** 
   * destructor of Octree
   *    vertices : Point Set
   */
Octree::~Octree()
{
	if (m_pRootOctree!=NULL){
		delete m_pRootOctree;
		m_pRootOctree = NULL;
	}
};

 /**
   * octree component accessors.
   */

 /**
   * Maximum number of objects one node can contain, if over it will be subdivided
   * The real number of objects contain in one node refer : maxNumObject()
   */
unsigned int Octree::getMaxNumVerticesPerNode() const
{
	return m_iMaxPointsPerNode;
}
/**
  * Maximum depth of tree one node can contain. Can not go futer
  */
unsigned int Octree::getMaxDepth() const
{
	return m_iMaxSubdivisions;
}

/**
  * Return root node
  */

OctreeNode* Octree::getRoot() const
{
		return m_pRootOctree;
}

/** 
  * Return the bounding box interm of (center, halfWidth)
  *    vertices : input pointset
  */
void Octree::boundBox(Point& center, float& halfWidth, const vector<Point>& vertexList) const
{
  // find the minimum and maximum bounding box points (axis aligned)
  Point maxPoint(FLT_MIN), minPoint(FLT_MAX);

  // iterator through all points 
  vector<Point>::const_iterator i;
  for (i=vertexList.begin();i!=vertexList.end();i++)
    {
      // update min and max information
      minPoint=Min(minPoint,*i);
      maxPoint=Max(maxPoint,*i);
    }

  // center of box is the average point (return center)
  center = (minPoint+maxPoint)/2;

  // compute and return the size of one half the maximum side
  Vector diag=maxPoint - minPoint;
  halfWidth = diag.get_max_axis()/2.0;
}

 /**
   * octreenode statistical function. Use for debug and statistical purpose
   */

 /**
   * number of object in the tree
   */
int Octree::numObject() const{
	return m_pRootOctree->numObject();
}

 /**
   * Real maximum number of object in a leaf
   */
int Octree::maxNumObject()const{
	return m_pRootOctree->maxNumObject();
}
 /**
   * Number of node contain in a tree
   */
int Octree::numNode() const{
	return m_pRootOctree->numNode();
}

/**
  * Tree construction function
  *   maxnPoints : maximum number of of a node before subdiivision
  *   maxDepth : maximum number depth
  * Changing two parameter will affect the memory amount and speed of searchin in the tree
  */
void Octree::buildTree(int maxnPoints,int maxDepth){
	m_iMaxPointsPerNode =maxnPoints;
	m_iMaxSubdivisions= maxDepth;
	Point center;
	float halfWidth;
	//	cout<<"Compute the bounding box " <<endl;
    boundBox(center, halfWidth,*m_pVertices);
	int nPoints= m_pVertices->size();
	//	cout <<"Finish compute the bounding box ";
   // cout<<"Center point "<<'[' << center.x() << ',' << center.y () << ',' << center.z() << ']'<<endl;
	//	cout<<"Half Width " << halfWidth <<endl;
	//	cout << "Building the tree ... " ;
	m_pRootOctree = new OctreeNode(center,halfWidth,0);
	for (int i=0;i!=nPoints; i++){
		m_pRootOctree->insertVertex(i,*m_pVertices,*this);
	}
	//	cout << "finished " <<endl;
};

/**
  * Fiding kNeigbour of a point
  *   ref : referance point
  *   k : number of neighbors
  *   vertices : pointset
  *   q1 : temporal queue to contain sorting result
  * Return list of point in the ascend order of distant to referance point
  */
std::vector<int> Octree::kNNM(const Point &ref, 
			      const unsigned int &k, 
			      const std::vector<Point>& vertices, 
			      priority_queue<MixType>& q1) const
{
  // Priority queue q of mixure node and point
  //cout << "K neigborhood Modified ...";
  std::vector<int> ret;
  q1.push(MixType(m_pRootOctree,ref));
  unsigned int copyK = k;
  while (copyK > 0){
    //Get front
    MixType element=q1.top();
    //Dequeue
    q1.pop();
    if (element.getType() == POINT_INDEX){
      ret.push_back(element.index);
      //cout<<element.index<<" : " <<element.getDist() <<endl;
      copyK--;
      continue;
    }
    //if not leaf node , add children to q1
    OctreeNode* pNode= element.getNodePtr();
    if (!pNode->isLeaf()){
      for (int i=0;i<8;i++)
	{
	  OctreeNode* child =pNode->getChild(i);
	  if (child!=NULL)
	    q1.push(MixType(child,ref)); 
	}
      continue;
    }
    // leaf node
    // if node close enough add all points in node to q2
    vector<int>::iterator i;
    for (i=pNode->begin();i!=pNode->end();i++)
      q1.push(MixType(*i,vertices,ref));
  }
  return ret;
}

/**
  * Finding kNeigbour of a point
  *   ref : referance point
  *   k : number of neighbors
  *   vertices : pointset
  *   q1 : temporal queue to contain sorting result
  * Return list of point in the ascend order of distant to referance point
  */
void Octree::kNNMV1(int* ret,const Point ref, int k, const vector<Point>& vertices, priority_queue<MixType>& q1) const{
	// Priority queue q of mixure node and point
	//cout << "K neigborhood Modified ...";
	while (!q1.empty()) q1.pop();
	q1.push(MixType(m_pRootOctree,ref));
	int index=0;
	while (k > 0){
	//Get front
		MixType element=q1.top();
		//Dequeue
		q1.pop();
		if (element.getType() == POINT_INDEX){
			ret[index++]=element.index;
			//cout << element.index <<endl;
			k--;
			continue;
		}
		//if not leaf node , add children to q1
		OctreeNode* pNode= element.getNodePtr();
		if (!pNode->isLeaf()){
			for (int i=0;i<8;i++)
			{
				OctreeNode* child =pNode->getChild(i);
				if (child!=NULL)
					q1.push(MixType(child,ref)); 
			}
			continue;
		}
		// leaf node
		// if node close enough add all points in node to q2
		vector<int>::iterator i;
		for (i=pNode->begin();i!=pNode->end();i++)
			q1.push(MixType(*i,vertices,ref));
	}
}

void Octree::kNNMV1(int* ret,const Point ref, int k, const vector<Point>& vertices) const
{
	// Priority queue q of mixure node and point
	//cout << "K neigborhood Modified ...";
	priority_queue<MixType> q1;
	while (!q1.empty()) q1.pop();
	q1.push(MixType(m_pRootOctree,ref));
	int index=0;
	while (k > 0){
	//Get front
		MixType element=q1.top();
		//Dequeue
		q1.pop();
		if (element.getType() == POINT_INDEX){
			ret[index++]=element.index;
			//cout << element.index <<endl;
			k--;
			continue;
		}
		//if not leaf node , add children to q1
		OctreeNode* pNode= element.getNodePtr();
		if (!pNode->isLeaf()){
			for (int i=0;i<8;i++)
			{
				OctreeNode* child =pNode->getChild(i);
				if (child!=NULL)
					q1.push(MixType(child,ref)); 
			}
			continue;
		}
		// leaf node
		// if node close enough add all points in node to q2
		vector<int>::iterator i;
		for (i=pNode->begin();i!=pNode->end();i++)
			q1.push(MixType(*i,vertices,ref));
	}
};
/**
  * Fiding kNeigbour of a point
  *   ref : referance point
  *   vertices : pointset
  *   q1 : temporal queue to contain sorting result
  *  Return the index of nearest neigbor hood 
  */
int Octree::nearestNeighbor(const Point ref, vector<Point> vertices, priority_queue<MixType>& q1) const{
// Priority queue q of mixure node and point
	//cout << "K neigborhood Modified ...";
	q1.push(MixType(m_pRootOctree,ref));
	while (true){
	//Get front
		MixType element=q1.top();
		//Dequeue
		q1.pop();
		if (element.getType() == POINT_INDEX)
			return element.index;
		//if not leaf node , add children to q1
		OctreeNode* pNode= element.getNodePtr();
		if (!pNode->isLeaf()){
			for (int i=0;i<8;i++)
			{
				OctreeNode* child =pNode->getChild(i);
				if (child!=NULL)
					q1.push(MixType(child,ref)); 
			}
			continue;
		}
		// leaf node
		// if node close enough add all points in node to q2
		vector<int>::iterator i;
		for (i=pNode->begin();i!=pNode->end();i++)
			q1.push(MixType(*i,vertices,ref));
	}
}



class NodePtr{

public:	
	NodePtr(OctreeNode* pNode,const Point& ref)
	{
		m_pOctreeNode=pNode;	
		m_fDist = sqDist(ref,*m_pOctreeNode);
	};
	float getDist() const{
		return m_fDist;
	}
	OctreeNode* getNodePtr(){
		return m_pOctreeNode;
	}
	
private:
	float m_fDist;
	OctreeNode* m_pOctreeNode;
};

bool operator <(const NodePtr& a,const NodePtr& b){
		return (a.getDist()>b.getDist());
}

class PointIndex{
public:	
	PointIndex(int index, const vector<Point>& vertices, const Point& ref):
	index(index)
	{
		dist = sqDist(vertices[index],ref);
	}
	PointIndex():index(INT_MAX), dist(FLT_MAX) {};
	float getDist() const{
		return dist;
	}
	int getIndex() const{
		return index;
	}
private:
	int index;
	float dist;
};

bool operator <(const PointIndex& a,const PointIndex& b){
		return (a.getDist()<b.getDist());
}

/**
  * Fiding kNeigbour of a point base of the brute force sorting method
  *   ref : referance point
  *   vertices : pointset
  *   q1 : temporal queue to contain sorting result
  *  Return the index of nearest neigbor hood 
  * This function is usefull for testing purpose
  */
std::vector<int> 
Octree::kNNBF(const Point &ref, 
	      const unsigned int &k, 
	      const std::vector<Point> &vertices) const
{
  // looping through all points computing distances
  std::vector<PointIndex> q2;
  for (unsigned int i=0;i<vertices.size();i++)
    {
      q2.push_back(PointIndex(i,vertices,ref));
    }

  // sort list by distance
  std::sort(q2.begin(),q2.end());

  // print out the list for debugging purposes
  std::vector<int> ret;
  ofstream out;
  out.open("out.txt",ios::binary);
  for (unsigned int i=0; i<k; i++)
    {
      out << q2[i].getIndex() << ":" << q2[i].getDist()<<endl;
      ret.push_back(q2[i].getIndex());
    }
  cout <<"done" <<endl;
  out.close();

  // return the list of indices to the points
  return ret;
};

/**
  * Fiding kNeigbour of a point base on two priority queue 
  *   ref : referance point
  *   vertices : pointset
  *   q1 : temporal queue to contain sorting result
  *  Return the index of nearest neigbor hood 
  * This function is usefull for testing purpose
  */
std::vector<int> 
Octree::kNN(const Point &ref, 
	    const unsigned int &k, 
	    const std::vector<Point> &vertices ) const
{
  std::priority_queue<NodePtr> q1;	// Priority queue q1 of node
  std::vector<PointIndex> q2;		// Priority queue q2 of points

  // setup the lists q1 and q2
  q1.push(NodePtr(m_pRootOctree,ref));
  while( q2.size()<k )
    {
      // get the top element and remove it from the queue
      NodePtr element=q1.top();
      q1.pop();
	
      //if not leaf node, add children to q1
      OctreeNode* pNode= element.getNodePtr();
      if (!pNode->isLeaf())
	{
	  for (int i=0;i<8;i++)
	    {
	      OctreeNode* pChild = pNode->getChild(i);
	      if (pChild != NULL) q1.push(NodePtr(pChild,ref)); 
	    }
	}
      // otherwise add all points in node to q2
      else
	{
	  vector<int>::iterator i;
	  for (i=pNode->begin();i!=pNode->end();i++)
	    {
	      q2.push_back(PointIndex(*i,vertices,ref));
	    }
	}
    }

  // sort the points q2 based on distances
  std::sort(q2.begin(),q2.end());
  while (!q1.empty())
    {
      // get the top element and remove from q1
      NodePtr element=q1.top();
      q1.pop();

      // if distance is less than the kth distance then compute some closer points
      OctreeNode* pNode= element.getNodePtr();
      if (element.getDist() < q2[k-1].getDist())
	{
	  // if not leaf node, add children to q1
	  if (!pNode->isLeaf())
	    {
	      for (int i=0;i<8;i++)
		{
		  OctreeNode* pChild = pNode->getChild(i);
		  if (pChild != NULL) q1.push(NodePtr(pChild,ref)); 
		}
	    }
	  // otherwise it is a leaf node (compare point distances)
	  else 
	    {
	      // if node close enough add all points in node to q2
	      std::vector<int>::iterator i;
	      for (i=pNode->begin();i!=pNode->end();i++)
		{
		  q2.push_back(PointIndex(*i,vertices,ref));
		}

	      // sort all points based on their distances		
	      std::sort(q2.begin(),q2.end());
	      //nth_element(q2.begin(), q2.begin()+(k-1), q2.end());
	      //stable_sort(q2.begin(),q2.end());
	      //sort_heap(q2.begin(),q2.end());
	    }
	}
    }

  // setup the return vector
  std::vector<int> ret( q2.size() );
  for (unsigned int i=0; i<k; i++)
    {
      ret[i] = q2[i].getIndex();
    }
  return ret;
}

/**
 * finds all neighbors to the reference point that are within the given 
 * distance to that point.  This function will return the indices to this
 * set of neighbor points.
 */
std::vector< unsigned int >
Octree::distanceNeighbors ( const Point &ref, const float &distance ) const
{
  // setup the priority queue of mixure node and point
  priority_queue<MixType> q1;	// static so destructor called only once!
  while (!q1.empty()) q1.pop();		// clear out the queue information

  // start at the root of the octree
  q1.push(MixType(m_pRootOctree,ref));

  // continue adding points that are within the distance allowed
  std::vector< unsigned int > returnIndices;
  float lastDistance = 0;
  while(lastDistance <= distance && !q1.empty()) 
    {
      // get the node from the top of the queue
      MixType element = q1.top();
      q1.pop();

      // check if popped node within allowable distance
      lastDistance = element.getDist();
      if ( lastDistance <= distance )
	{
	  // if the node is a LEAF node (point) 
	  if (element.getType() == POINT_INDEX)
	    {
	      // save the element
	      returnIndices.push_back( element.index );
	    }

	  // otherwise add children of node to the queue
	  else
	    {
	      // get the current node pointer
	      OctreeNode* pNode= element.getNodePtr();

	      // if it not a leaf add the children nodes to the queue
 	      if (!pNode->isLeaf())
 		{
		  // there will be at most 8 children
		  for (int i=0;i<8;i++)
		    {
		      // get child and make sure that they exist
		      OctreeNode* child = pNode->getChild(i);
		      if (child!=NULL)
			q1.push(MixType(child,ref)); 
		    }
		}

	      // otherwise add the node's points to the queue
	      else
		{
		  // if node close enough add all points in node to q2
		  vector<int>::iterator i;
		  for (i=pNode->begin();i!=pNode->end();i++)
		    q1.push(MixType(*i,*m_pVertices,ref));
		}
	    }
	}
    }

  // return the computed indices
  return returnIndices;
}
