/**

 * class comments.

 */



#ifndef __POINTCLOUD__

#define __POINTCLOUD__



#include <queue>

#include <string>

#include "Octree.h"

#include "Primitive_Structs.h"

#include "TriMesh.h"



enum ShapeType {Cube, Trapezoid, Icosahedron, SemiSphere, Cylinder, TwoSphere, FanDisk, Piston, Knuckle, Ra, Scheme, Dog, Fireman, Frog, ScallopCube};



class PointCloud

{



 public:



  // constructors

  PointCloud(const std::string &inFilename, 

	     const unsigned int &maxPoints=10, 

	     const unsigned int &maxDepth=10);

  PointCloud(const std::vector< Point > &pts, 

	     const unsigned int &maxPoints=10, 

	     const unsigned int &maxDepth=10);

  PointCloud(const std::vector< Point > &pts, 

	     const std::vector< float > &weights,

	     const std::vector< std::vector< Vector > > &normals,

	     const unsigned int &maxPoints=10, 

	     const unsigned int &maxDepth=10);

  PointCloud(const ShapeType &type,

	     const unsigned int &numSamples,

	     const float &noise=0,

	     const unsigned int &maxPoints=10,

	     const unsigned int &maxDepth=10 );



  // destructor

  ~PointCloud();



  // load file to create a new point cloud

  bool loadOFFfile( const char *name,

		    const unsigned int &numSamples,

		    const float &noise=0,

		    const unsigned int &maxPoints=10,

		    const unsigned int &maxDepth=10 );

  bool loadTXTfile( const char *name,

		    const unsigned int &numSamples,

		    const float &noise=0,

		    const unsigned int &maxPoints=10,

		    const unsigned int &maxDepth=10 );



  // useful functions

  void get_bound_sphere( Point &centroid, float &radius ) const;

  void scale( const float &value );

	

  // nearest neighbor computations

  std::vector<Point> kNN( const Point &ref, const unsigned int &k ) const;

  std::vector<unsigned int> kNN_indices( const Point &ref, const unsigned int &k ) const;



  std::vector<int> kNNV1( const Point &ref, const unsigned int &k ) const;

  std::vector<int> kNNBF( const Point &ref, const unsigned int &k ) const;

  std::vector<int> kNNM( const Point &ref, const unsigned int &k, std::priority_queue<MixType> &q ) const;

  void kNNMV1( int* ret, const Point &ref, const unsigned int &k, std::priority_queue<MixType>& q) const;

  int nearestNeighbor(const Point &ref, std::priority_queue<MixType>& q ) const;



  std::vector<unsigned int> distanceNeighbors( const Point &ref, const float &distance ) const;



  std::vector<Point> vertices;

  std::vector<float> verticesWeights;

  std::vector< std::vector<Vector> > verticesNormals;



  Octree* m_pOctree;



 private:

  // helper functions for the create shape stuff

  static Point sampleCylinder( const float &radius, const Point &top, const Point &bottom, const float &n );
  static Point sampleQtrSphere( const float &radius, const Point &center, const float &n );
  static std::vector<Point> createScallopCube( const unsigned &numSamples,
					       const float &noise );
  static std::vector<Point> createCube( const unsigned int &numSamples,
					const float &noise);
  static std::vector<Point> createTrapezoid( const unsigned int &numSamples,
					     const float &noise);
  static std::vector<Point> createIcosahedron( const unsigned int &numSamples,
					       const float &noise);
  static std::vector<Point> createSemiSphere( const unsigned int &numSamples,
					      const float &noise);
  static std::vector<Point> createCylinder( const unsigned int &numSamples,
					    const float &noise);
  static std::vector<Point> createTwoSphere( const unsigned int &numSamples,
					     const float &noise);
  static std::vector<Point> createFanDisk( const unsigned int &numSamples,
					   const float &noise);
  static std::vector<Point> createPiston( const unsigned int &numSamples,
					  const float &noise);
  static std::vector<Point> createKnuckle( const unsigned int &numSamples,
					   const float &noise);
  static std::vector<Point> createRa( const unsigned int &numSamples,
				      const float &noise);
  static std::vector<Point> createScheme( const unsigned int &numSamples,
					  const float &noise);
  static std::vector<Point> createDog( const unsigned int &numSamples,
				       const float &noise);

  static Point sampleTriangle( const Point &p, const Point &p1, const Point &p2, const float &noise );

};



std::vector<Point> readOFFfile(char* filename);

#endif

