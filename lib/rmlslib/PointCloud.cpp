#include "PointCloud.h"

#include <iostream>

#include <fstream>

using namespace std;

static void error()

{

  cerr << "Error reading triangle mesh\n";

  exit(1);

}



std::vector<Point> 

readOFFfile(const std::string &filename)

{

  fprintf(stderr," *- Enter file open point cloud reading: '%s' -*\n",filename.c_str());



  // try opening the file for input reading

  ifstream in;

  in.open(filename.c_str());

  if(!in) error();

  if (in.get()!='O') error();

  if (in.get()!='F') error();	

  if (in.get()!='F') error();



  // grab the number of attributes from the file

  unsigned int numverts,numtris,numedge;

  in >> numverts >> numtris >> numedge;

  fprintf(stderr," *- reading number vertices(%d), triangle(%d), edges(%d) -*\n",numverts,numtris,numedge);

  if(!in) error();



  // get the vertices

  // numverts=10000;			// limit the number for debugging

  vector<Point> verts(numverts);

  Point centroid(0,0,0);

  for(unsigned int i=0;i<numverts;i++)

    {

    double x,y,z;

    in >> x >> y >> z;

    if(!in) error();

    verts[i] = Point(z,y,x);

    centroid += verts[i];

  }

  centroid /= numverts;

  fprintf(stderr," *- Finished reading vertices from file '%s' -*\n",filename.c_str());



  // loop back through and compute the radius from the centroid

  float radius = 0;

  for(unsigned int i=0; i<numverts; i++)

    {

      float distance = centroid.distance_to( verts[i] );

      if (distance > radius) 

	radius = distance;

    }



  // scale every point to be within the unit sphere

  for(unsigned int i=0; i<numverts; i++)

    {

      // move the vertices within the unit sphere

      verts[i] = Point(0,0,0) + (verts[i]-centroid)*(1.0/radius);

    }



  // return the number of vertices;

  return verts;

}



PointCloud::PointCloud( const std::string &inFilename,const unsigned int &maxPoints, const unsigned int &maxDepth)

{

  vertices= readOFFfile(inFilename);

  verticesWeights.clear();

  verticesNormals.clear();

  m_pOctree= new Octree(&vertices);

  m_pOctree->buildTree(maxPoints,maxDepth);

//   cout <<"Number of node in the tree " << m_pOctree->numNode() <<endl;

//   cout <<"Number of object in the tree "<<m_pOctree->numObject() <<endl;

//   cout <<"Minimum number of object in a leaf node " << m_pOctree->getRoot()->minNumObject()<<endl;

//   cout <<"Maximum number of object in a leaf node " << m_pOctree->getRoot()->maxNumObject()<<endl;

}



/**

 * point cloud construction from a list of points.

 */

PointCloud::PointCloud( const std::vector< Point > &pts,const unsigned int &maxPoints, const unsigned int &maxDepth)

{

  vertices = pts;

  verticesWeights.clear();

  verticesNormals.clear();

  m_pOctree = new Octree(&vertices);

  m_pOctree->buildTree(maxPoints,maxDepth);

//   cout <<"Number of node in the tree " << m_pOctree->numNode() <<endl;

//   cout <<"Number of object in the tree "<<m_pOctree->numObject() <<endl;

//   cout <<"Minimum number of object in a leaf node " << m_pOctree->getRoot()->minNumObject()<<endl;

//   cout <<"Maximum number of object in a leaf node " << m_pOctree->getRoot()->maxNumObject()<<endl;

}



/**

 * point cloud construction from a list of points.

 */

PointCloud::PointCloud( const std::vector< Point > &pts,

			const std::vector< float > &weights,

			const std::vector< std::vector< Vector > > &normals,

			const unsigned int &maxPoints, 

			const unsigned int &maxDepth)

{

  vertices = pts;

  verticesWeights = weights;

  verticesNormals = normals;

  m_pOctree = new Octree(&vertices);

  m_pOctree->buildTree(maxPoints,maxDepth);

//   cout <<"Number of node in the tree " << m_pOctree->numNode() <<endl;

//   cout <<"Number of object in the tree "<<m_pOctree->numObject() <<endl;

//   cout <<"Minimum number of object in a leaf node " << m_pOctree->getRoot()->minNumObject()<<endl;

//   cout <<"Maximum number of object in a leaf node " << m_pOctree->getRoot()->maxNumObject()<<endl;

}



/**

 * create a point cloud with the given shape type.

 */

PointCloud::PointCloud( const ShapeType &type,

			const unsigned int &numSamples,

		        const float &noise,

		        const unsigned int &maxPoints,

		        const unsigned int &maxDepth )

{

  // create the shape switch on the type

  switch(type)

    {

    case ScallopCube:
      vertices = createScallopCube( numSamples,noise );
      break;

    case Cube:
      vertices = createCube(numSamples,noise);
      break;

    case Trapezoid:
      vertices = createTrapezoid(numSamples,noise);
      break;

    case Icosahedron:
      vertices = createIcosahedron(numSamples,noise);
      break;

    case SemiSphere:
      vertices = createSemiSphere(numSamples,noise);
      break;

    case Cylinder:
      vertices = createCylinder(numSamples,noise);
      break;

    case TwoSphere:
      vertices = createTwoSphere(numSamples,noise);
      break;

    case FanDisk:
      vertices = createFanDisk(numSamples,noise);
      break;

    case Piston:
      vertices = createPiston( numSamples,noise );
      break;

    case Knuckle:
      vertices = createKnuckle( numSamples,noise );
      break;

    case Ra:
      vertices = createRa( numSamples,noise );
      break;

    case Scheme:
      vertices = createScheme( numSamples,noise );
      break;

  case Dog:
      vertices = createDog(numSamples,noise );
      break;

    default:
      fprintf(stderr,"[WARNING]: unknown shape type...building default (cube)\n");
      vertices = createCube(numSamples,noise);
      break;
    }

  verticesWeights.clear();
  verticesNormals.clear();

  m_pOctree = new Octree(&vertices);
  m_pOctree->buildTree(maxPoints,maxDepth);
}



/** 

 * destructor 

 */

PointCloud::~PointCloud()

{

  if (m_pOctree!= NULL)

    {

      delete m_pOctree;

    }

}



/**

 * scale -- this funnction loops through all the points and scales

 * their distance from the centroid (origin) by the given value.

 */

void

PointCloud::scale( const float &value )

{

  // scale all points

  Point centroid(0,0,0);

  for(unsigned int i=0; i<vertices.size(); i++)

    {

      vertices[i] = centroid + (vertices[i]-centroid)*value;

    }



  // delete and rebuild octree

  int maxPoints = m_pOctree->getMaxNumVerticesPerNode();

  int maxDepth = m_pOctree->getMaxDepth();

  delete( m_pOctree );

  m_pOctree= new Octree(&vertices);

  m_pOctree->buildTree(maxPoints,maxDepth);

}



/**

 * bound sphere -- this funnction loops through all the points and

 * returns the averaged centroid and the longest distance to that

 * point.

 */

void

PointCloud::get_bound_sphere( Point &centroid,

			      float &radius ) const

{

  // compute the centroid

  centroid = Point(0,0,0);

  for(unsigned int i=0; i<vertices.size(); i++)

    {

      centroid += vertices[i];

    }

  centroid /= vertices.size();



  // compute the radius to the centroid

  radius = 0;

  for(unsigned int i=0; i<vertices.size(); i++)

    {

      float distance = vertices[i].distance_to( centroid );

      if (distance > radius)

	radius = distance;

    }

}



/**

 * distance neighbors -- Finds the neighbors within the point cloud

 * that are within the given distance to the reference point.

 */

std::vector< unsigned int > 

PointCloud::distanceNeighbors(const Point &ref, 

			      const float &distance ) const

{

  return m_pOctree->distanceNeighbors( ref,distance );

}



/**

 * k nearest neighbor -- Finding the K nearest neighbors in the cloud

 * of ref point using one mixture type priority queue.

 */

std::vector<Point> 

PointCloud::kNN(const Point &ref, 

		const unsigned int &k) const

{

  int* nn= new int [k];

  std::vector<Point> knn(k);

  m_pOctree->kNNMV1(nn,ref,k,vertices);

  for (unsigned int i=0; i<k; i++)

    {

      knn[i]=vertices[nn[i]];

    } 

  delete[] nn;

  return knn;

}



/**

 * k nearest neighbor -- Finding the K nearest neighbors in the cloud

 * of ref point using one mixture type priority queue.

 */

std::vector< unsigned int > 

PointCloud::kNN_indices(const Point &ref, 

			const unsigned int &k) const

{

  int* nn= new int [k];

  std::vector<unsigned int> knn(k);

  m_pOctree->kNNMV1(nn,ref,k,vertices);

  for(unsigned int i=0; i<k; i++)

    knn[i] = nn[i];

  delete[] nn;

  return knn;

}



/**

 * k nearest neighbor -- Finding the K nearest neighbors in the cloud

 * of ref point using two saperate priority queue.

 */

std::vector<int> 

PointCloud::kNNV1(const Point &ref, 

		const unsigned int &k) const

{

  return m_pOctree->kNN(ref,k,vertices);

}

/**

 * k nearest neighbor BF -- Finding the K nearest neighbors in the

 * cloud of ref point using brute force by computing all the distance

 * and use sorting method to sort throough all distances and pick k

 * nearest points.

 */

std::vector<int> 

PointCloud::kNNBF(const Point &ref, 

		  const unsigned int &k) const

{

  return m_pOctree->kNNBF(ref,k,vertices);

}



/** 

 * k nearest neighbor M -- function comment.Finding the K nearest

 * neighbors in the cloud of ref point using only one mix priority

 * queue. This function require one priority queue ititialize outside

 * the scope of function

 */

std::vector<int> 

PointCloud::kNNM(const Point &ref, 

		 const unsigned int &k, 

		 std::priority_queue<MixType>& q ) const

{

  return m_pOctree->kNNM(ref,k,vertices,q);

}



/**

 * k nearest neighbor MV1 --Finding the K nearest neighbors in the

 * cloud of ref point using only one mix priority queue. This function

 * require one priority queue ititialize outside the scope of

 * function. The value return is an array of k neigbour index.

 */

void 

PointCloud::kNNMV1( int* ret, 

		    const Point &ref, 

		    const unsigned int &k, 

		    std::priority_queue<MixType>& q) const

{

    m_pOctree->kNNMV1(ret,ref,k,vertices,q);

}



/**

 * nearest neighbor -- Find the nearest neigbouhood of a ref point in

 * the cloud

 */

int PointCloud::nearestNeighbor(const Point &ref, 

				std::priority_queue<MixType>& q) const

{

  return m_pOctree->nearestNeighbor(ref,vertices,q);

}



/**

 * create a cylinder

 */

std::vector<Point> 

PointCloud::createCylinder( const unsigned int &numSamples,

			    const float &noise )

{

  // create a sphere of data points

  std::vector< Point > pts( numSamples );

  for(unsigned int i=0; i<pts.size(); i++)

    {

      float random1 = ((float)(rand()%1000))/1000.0;

      float random2 = ((float)(rand()%1000))/1000.0;

      float random3 = ((float)(rand()%1000))/1000.0;

      float phi = 2.0*PIE*random1;

      float sinTheta = sqrt(random2);

      float radius = 2 - (((float)(rand()%10000))/10000)*noise - noise/2.0;



      // most pf the time it must be a sphere

      if (i%4 == 0 || i%4 == 1)

	{

	  radius = 2;

	  pts[i] = Point (radius*sin(phi), 

			  random3*5,			// height = 0 - 5

			  radius*cos(phi));

	}



      // the bottom end cap

      else if (i%4 == 2)

	{

	  pts[i] = Point( radius*sinTheta*sin(phi),

			  random3*noise + 5,	// height = 5+noise

			  radius*sinTheta*cos(phi) );

	}

      // the top end cap

      else

	{

	  pts[i] = Point( radius*sinTheta*sin(phi),

			  random3*noise,		// height = noise

			  radius*sinTheta*cos(phi) );

	}

    }



  // use samples to create a new point cloud

  return pts;

}



/**

 * create a semi-sphere point cloud

 */

std::vector<Point>

PointCloud::createSemiSphere( const unsigned int &numSamples,

			      const float &noise)

{

  // create a sphere of data points
  std::vector< Point > pts( numSamples );
  for(unsigned int i=0; i<pts.size(); i++)
    {
      float random1 = ((float)(rand()%1000))/1000;
      float random2 = ((float)(rand()%1000))/1000;
      float phi = 2.0*PIE*random1;
      float sinTheta = sqrt(random2);
      float radius = 10 - (((float)(rand()%10000))/10000)*noise - noise/2.0;

      // most pf the time it must be a sphere
      if (i%3 == 0 || i%3 == 1)
	{
	  float cosTheta = sqrt(1-random2);
	  pts[i] = Point (radius*sinTheta*sin(phi), 
			  radius*cosTheta, 
			  radius*sinTheta*cos(phi));
	}

      // the other time it is on the disc
      else
	{
	  pts[i] = Point( radius*sinTheta*sin(phi),
			  radius-10.0,
			  radius*sinTheta*cos(phi) );
	}
    }

  // use samples to create a new point cloud
  return pts;
}



/**

 * create a fandisk cloud from the off model

 */

std::vector<Point> 

PointCloud::createFanDisk( const unsigned int &numSamples,

			   const float &noise )

{

  // create a sphere of data points

  TriMesh fandiskMesh;

  fandiskMesh.loadOFF("data/fandisk.off");

  return fandiskMesh.pointCloud(numSamples,noise);

}



/**

 * create a piston cloud from the txt model

 */

std::vector<Point> 

PointCloud::createPiston( const unsigned int &numSamples,

			  const float &noise )

{

  // create a sphere of data points

  TriMesh pistonMesh;

  pistonMesh.loadTXT("data/qtr_piston.txt");

  return pistonMesh.pointCloud(numSamples,noise);

}



/**

 * create a knuckle cloud from the txt model

 */

std::vector<Point> 

PointCloud::createKnuckle( const unsigned int &numSamples,

			   const float &noise )

{

  // create a sphere of data points

  TriMesh mesh;

  mesh.loadTXT("data/knuckle.txt");

  return mesh.pointCloud(numSamples,noise);

}



/**

 * create a ra cloud from the txt model

 */

std::vector<Point> 

PointCloud::createRa( const unsigned int &numSamples,

		      const float &noise )

{

  // create a sphere of data points

  TriMesh mesh;

  mesh.loadTXT("data/ra.txt");

  return mesh.pointCloud(numSamples,noise);

}



/**

 * create a scheme from the txt model

 */

std::vector<Point> 

PointCloud::createScheme( const unsigned int &numSamples,

			  const float &noise )

{

  // create a sphere of data points

  TriMesh mesh;

  mesh.loadTXT("data/scheme.txt");

  return mesh.pointCloud(numSamples,noise);

}


/**

 * create a scheme from the txt model

 */

std::vector<Point> 

PointCloud::createDog( const unsigned int &numSamples,

			  const float &noise )

{

  // create a sphere of data points

  TriMesh mesh;

  mesh.loadIV("data/dog.iv");
  return mesh.pointCloud(numSamples,noise);

}


/**

 * create a two sphere point clouds that intersect

 */

std::vector<Point>

PointCloud::createTwoSphere( const unsigned int &numSamples,

			     const float &noise )

{

  // create a sphere of data points

  std::vector< Point > pts( numSamples );

  for(unsigned int i=0; i<pts.size(); i++)

    {

      float random1 = ((float)(rand()%1000))/1000;

      float random2 = ((float)(rand()%1000))/1000;

      float phi = 2.0*PIE*random1;

      float sinTheta = sqrt(random2);

      float radius = 10 - (((float)(rand()%10000))/10000)*noise - noise/2.0;

      float cosTheta = sqrt(1-random2);



      // 1/4 goes to top semi-sphere of first sphere

      if (i%4 == 0)

	{



	  pts[i] = Point (radius*sinTheta*sin(phi), 

			  radius*cosTheta, 

			  radius*sinTheta*cos(phi));

	}

      // 1/4 goes to the bottom of the same sphere

      else if (i%4 == 1)

	{



	  pts[i] = Point (radius*sinTheta*sin(phi), 

			  -radius*cosTheta, 

			  radius*sinTheta*cos(phi));

	}



      // 1/4 goes to top semi-sphere of second sphere

      else if (i%4 == 3)

	{



	  pts[i] = Point (radius*sinTheta*sin(phi) + 8, 

			  radius*cosTheta, 

			  radius*sinTheta*cos(phi));

	}

      // 1/4 goes to the bottom of the same sphere

      else

	{



	  pts[i] = Point (radius*sinTheta*sin(phi) + 8, 

			  -radius*cosTheta, 

			  radius*sinTheta*cos(phi));

	}

    }



  // use samples to create a new point cloud

  return pts;

}



/**

 * create cube sample point cloud

 */

std::vector<Point>

PointCloud::createCube( const unsigned int &numSamples,

			const float &noise )

{

  // randomly create a cube with 6 sides

  std::vector< Point > samples( numSamples );

  for(unsigned int i=0; i<numSamples; i++)

    {

      unsigned int side = i%6;

      float n = ((float)(rand()%10000))/10000 * noise;

      float random1 = ((float)(rand()%10001))/1000.0;

      float random2 = ((float)(rand()%10000))/1000.0;

      switch(side)

	{

	case 0:

	case 6:

	  samples[i] = Point( 0+n, random1, random2 );

	  break;

	case 1:

	  samples[i] = Point( 10+n, random1, random2 );

	  break;

	case 2:

	  samples[i] = Point( random1, 0+n, random2 );

	  break;

	case 3:

	  samples[i] = Point( random1, 10+n, random2 );

	  break;

	case 4:

	  samples[i] = Point( random1, random2, 0+n );

	  break;

	case 5:

	  samples[i] = Point( random1, random2, 10+n );

	  break;

	default:

	  fprintf(stderr,"initializing the points -- unknown side!\n");

	  break;

	}

    }



  // use samples to create a new point cloud

  return samples;

}

/**
 * sample the cylinder walls
 */
Point
PointCloud::sampleCylinder( const float &radius,
			    const Point &top,
			    const Point &bottom,
			    const float &n )
{
  // obtain two random values [0,1]
  float random1 = ((float)(rand()%1000))/1000.0 * 2 * PIE;
  float random2 = ((float)(rand()%1000))/1000.0;

  Point scaled = bottom + (top-bottom)*(1-random2);
  
  // return a point with random sample on cylinder wall
  return Point( scaled.x() + radius*sin(random1)+n, 
		scaled.y(),
		scaled.z() + radius*cos(random1)+n );
}

/**
 * sample the quarter sphere
 */
Point
PointCloud::sampleQtrSphere( const float &radius,
			     const Point &center,
			     const float &n )
{

  // create a triangle to represent the qtr sphere
  Point pt1( 1,0,0 );
  Point pt2( 0,-1,0 );
  Point pt3( 0,0,1 );
  Point sample = sampleTriangle( pt1,pt2,pt3, 0 );
  
  // return the normalized point to the qtr sphere
  return Point( Vector( sample ).unit_vector() )*(radius+n) + center;
}

/**
 * create the scalloped cube sample point cloud
 */
std::vector<Point>
PointCloud::createScallopCube( const unsigned int &numSamples,
			       const float &n )
{
  // cylinder center top and bottom
  Point cylinderTop( .6, 1, .6 );
  Point cylinderBottom( .6, 0, .6 );
  Point sphereCenter( 0, 1, 0 );

  // randomly create a cube with 6 sides
  std::vector< Point > samples( numSamples );
  for(unsigned int i=0; i<numSamples; i++)
    {
      unsigned int side = i%8;
      float n = ((float)(rand()%100000))/100000 * n;
      float random1 = ((float)(rand()%10000))/10000.0;
      float random2 = ((float)(rand()%10000))/10000.0;
      switch(side)
	{
	  // front of cube
	case 0:
	  samples[i] = Point( 0+n, random1, random2 );
	  break;
	  // back of cube
	case 1:
	  samples[i] = Point( 1+n, random1, random2 );
	  break;
	  // bottom of cube
	case 2:
	  samples[i] = Point( random1, 0+n, random2 );
	  break;
	  // top of cube
	case 3:
	  samples[i] = Point( random1, 1+n, random2 );
	  break;
	  // left of cube
	case 4:
	  samples[i] = Point( random1, random2, 0+n );
	  break;
	  // right of cube
	case 5:
	  samples[i] = Point( random1, random2, 1+n );
	  break;
	  // the scallop
	case 6:
	  samples[i] = sampleCylinder( .20,cylinderTop,cylinderBottom,n );
	  break;
	  // the cylinder
	case 7:
	  samples[i] = sampleQtrSphere( .50,sphereCenter,n );
	  break;
	default:
	  fprintf(stderr,"initializing the points -- unknown side!\n");
	  break;
	}

      // check if the sample needs to be redone for cylinder
      if ( side != 7 && (samples[i].distance_to( cylinderTop ) < .20 ||
			 samples[i].distance_to( cylinderBottom ) < .20 ) )
	{
	  samples[i] = sampleCylinder( .20,cylinderTop,cylinderBottom,n );
	}
      // check if the sample needs to be redone for sphere
      else if (side != 6 && samples[i].distance_to( sphereCenter ) < .50)
	{
	  samples[i] = sampleQtrSphere( .50,sphereCenter,n );
	}
    }

  // use samples to create a new point cloud
  return samples;
}

/** 
 * helper function used to sample the triangle that is defined by the 
 * given vertices.
 */
Point
PointCloud::sampleTriangle( const Point& p, 
			    const Point& p1, 
			    const Point& p2, 
			    const float &noiseScale)
{
  float b, b1;
  do{
    b = (float)rand()/RAND_MAX;
    b1= (float)rand()/RAND_MAX;
  } while (b+b1>1);
  float b2= 1-b1-b;
  Vector e1= p1-p;
  Vector e2= p2-p;
  Vector normal = e1.unit_cross_product(e2);
  float noise = (((float)(rand()))/RAND_MAX-0.5) * noiseScale;

  return (p*b+p1*b1+p2*b2)+normal*noise;
}



/**

 * create Trapeziod sample point cloud

 */

std::vector<Point>

PointCloud::createTrapezoid( const unsigned int &numSamples,

			     const float &noise )

{

  // setup the triangles for a trapezoid

  Point vertices [8] = { Point(-4.0, -4.0, 4.0), 

			 Point(-4.0, 4.0,  4.0), 

			 Point(4.0,  4.0,  4.0), 

			 Point(4.0,  -4.0, 4.0),

			 Point(-8.0, -8.0, -4.0), 

			 Point(-8.0, 8.0,  -4.0),

			 Point(8.0,  8.0,  -4.0), 

			 Point(8.0,  -8.0, -4.0) };

  int faces[12][3] = {

    {0,2,1}, {0,3,2}, {4,5,6}, {4,6,7},

    {0,1,5}, {0,5,4}, {3,7,6}, {3,6,2},

    {1,2,6}, {1,6,5}, {4,3,0}, {4,7,3}

  };



  // randomly create a trapezoid

  std::vector< Point > samples( numSamples );

  for(unsigned int i=0; i<numSamples; i++)

  {

    int face = i%12;

    samples[i]=sampleTriangle( vertices[ faces[face][0] ],

			       vertices[ faces[face][1] ],

			       vertices[ faces[face][2] ],

			       noise );

  }

  // use samples to create a new point cloud

  return samples;

}



/**

 * create icosahedron sample point cloud

 */

std::vector<Point>

PointCloud::createIcosahedron( const unsigned int &numSamples,

			       const float &noise )

{

  // setup the vertex and face lists

  float X = 5.25731112119133606;

  float Z = 8.50650808352039932;

  Point vertices [12]= { Point(-X, 0.0,   Z), Point(X,  0.0,   Z), 

			 Point(-X, 0.0,  -Z), Point(X,  0.0,  -Z),

			 Point(0.0,  Z,   X), Point(0.0,  Z,  -X), 

			 Point(0.0, -Z,   X), Point(0.0, -Z,  -X),

			 Point(Z,    X, 0.0), Point(-Z,   X, 0.0), 

			 Point(Z,   -X, 0.0), Point(-Z,  -X, 0.0) };

  int faces[20][3] = {

    {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},

    {8,10,1}, {8,3,10},{5,3,8}, {5,2,3}, {2,7,3},

    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},

    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}

  };



  // randomly create a trapezoid

  std::vector< Point > samples( numSamples );

  for(unsigned int i=0; i<numSamples; i++)

  {

    int face = i%20;

    samples[i] = sampleTriangle( vertices[ faces[face][0] ],

				 vertices[ faces[face][1] ],

				 vertices[ faces[face][2] ],

				 noise );

  }



  // use samples to create a new point cloud

  return samples;

}



/**

 * load the file and create the new point cloud

 */

bool

PointCloud::loadOFFfile( const char *name,

			 const unsigned int &numSamples,

			 const float &noise,

			 const unsigned int &maxPoints,

			 const unsigned int &maxDepth )

{

  // load the triangle mesh

  TriMesh mesh;

  if ( !mesh.loadOFF(name) ) return false;

  vertices = mesh.pointCloud(numSamples,noise);



  // create the point cloud information

  verticesWeights.clear();

  verticesNormals.clear();

  m_pOctree = new Octree(&vertices);

  m_pOctree->buildTree(maxPoints,maxDepth);

  return true;

}



/**

 * load the file and create the new point cloud

 */

bool

PointCloud::loadTXTfile( const char *name,

			 const unsigned int &numSamples,

			 const float &noise,

			 const unsigned int &maxPoints,

			 const unsigned int &maxDepth )

{

  // load the triangle mesh

  TriMesh mesh;

  if ( !mesh.loadTXT(name) ) return false;

  vertices = mesh.pointCloud(numSamples,noise);



  // create the point cloud information

  verticesWeights.clear();

  verticesNormals.clear();

  m_pOctree = new Octree(&vertices);

  m_pOctree->buildTree(maxPoints,maxDepth);

  return true;

}
