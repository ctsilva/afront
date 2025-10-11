#include <fstream>
#include "TriMesh.h"


void error()
{
  printf(" Error while reading file ");
  exit(-1);
}


TriMesh::TriMesh()
{
  
};

TriMesh::~TriMesh()
{
  
};

bool TriMesh::loadOFF(const char* filename)
{
  std::ifstream in;
  in.open(filename);
  if(!in) error();
  if (in.get()!='O') error();
  if (in.get()!='F') error();	
  if (in.get()!='F') error();
  int nEdges_;
  in >> nVertices_ >> nTriangles_ >> nEdges_;
  if(!in) error();
  std::cerr << "reading " << nVertices_ << " vertices...";
  vertices_.resize(nVertices_);
  faces_.resize(nTriangles_);

  Point centroid(0.0,0.0,0.0);
  
  for(unsigned int i=0;i<nVertices_;i++){
     float x,y,z;
     in >> x >> y >> z;
     if(!in) error();
     vertices_[i]=Point(x,y,z);
     centroid += vertices_[i];
  }
  centroid /= nVertices_;
// loop back through and compute the radius from the centroid
  float radius = 0;
  for(unsigned int i=0; i<nVertices_; i++)
   {
     float distance = centroid.distance_to( vertices_[i] );
     if (distance > radius) 
	radius = distance;
   }

  // scale every point to be within the unit sphere
 for(unsigned int i=0; i<nVertices_; i++)
   {
      // move the vertices within the unit sphere
     vertices_[i] = Point(0,0,0) + (vertices_[i]-centroid)*(10.0/radius);
   }

  std::cerr << " reading " << nTriangles_ << " triangles...";
  for(unsigned int i=0; i < nTriangles_; i++){
      unsigned int temp, a, b, c;
      in >> temp >> a >> b >> c;
      if(!in) error();
      Tri_Index triObj_;
      triObj_.a_ = a;
      triObj_.b_ = b;
      triObj_.c_ = c;
      faces_[i]=triObj_;
  }
  return true;
};

int TriMesh::load3DS(const char* filename)
{
  return 1;
};

/**
 * read TXT file -- these are the cubit files that we received from
 * jason.  They have a bunch of unneeded information that we throw out.
 */
bool
TriMesh::loadTXT(const char *filename)
{
  fprintf(stderr,"reading '%s'...",filename);

  // try opening the file for input reading
  std::ifstream in;
  in.open(filename);
  if(!in)
    {
      fprintf(stderr,"[ERROR]: TriMesh::loadTXT() - error reading file\n");
      return false;
    }

  // process the garbage at the front of the file
  char buffer[256];
  in.getline( buffer,256 );		// ! Database Title
  in.getline( buffer,256 );		// cubit(file.g)
  in.getline( buffer,256 );		// ! Database initial variables
  in.getline( buffer,256 );		// x x ! dimensions, version number
  in >> nVertices_ >> nTriangles_;	// get the number of vertices and faces
  in.getline( buffer,256 );		// x ! nodes, elements, element blocks
  in.getline( buffer,256 );		// x x ! #node sets, #side sets
  in.getline( buffer,256 );		// x x ! len: node set list, dist fact length
  in.getline( buffer,256 );		// x x x ! side sets len: element, node, dist fact
  in.getline( buffer,256 );		// ! coordinate names
  in.getline( buffer,256 );		// x y z
  in.getline( buffer,256 );		// ! coordinates

  // get the vertices
  vertices_.resize( nVertices_ );
  Point centroid(0,0,0);
  for(unsigned int i=0; i<nVertices_; i++)
    {
      in >> vertices_[i].x() >> vertices_[i].y() >> vertices_[i].z();
      centroid += vertices_[i];
    }
  centroid /= nVertices_;
  fprintf(stderr,"%d vertices...",vertices_.size());

  // read and throw out the explicit map
  char *token1,*token2;
  bool foundConnectivity = false;
  while (!foundConnectivity && !in.eof())
    {
      // grab the line of text
      in.getline( buffer,256 );

      // get the first token
      if ( (token1 = strtok(buffer," ")) != NULL )
	{
	  if ( strcmp(token1,"!") ==0 )
	    {
	      // get the second token
	      token2 = strtok( NULL," " );
	      foundConnectivity = (strcmp(token2,"Connectivity")==0);
	    }
	}
    }

  // if didn't find the triangle list then print error and quit
  if (!foundConnectivity)
    {
      fprintf(stderr,"[ERROR] TriMesh::loadTXT() - no triangles found!\n");
      return false;
    }

  // read in triangles
  faces_.resize( nTriangles_ );
  for(unsigned int i=0; i<nTriangles_; i++)
    {
      in >> faces_[i].a_ >> faces_[i].b_ >> faces_[i].c_;
      faces_[i].a_--;
      faces_[i].b_--;
      faces_[i].c_--;
    }
  fprintf(stderr,"%d triangles...",faces_.size());

  // loop back through and compute the radius from the centroid
  float radius = 0;
  for(unsigned int i=0; i<nVertices_; i++)
    {
      float distance = centroid.distance_to( vertices_[i] );
      if (distance > radius) 
	radius = distance;
    }

  // scale every point to be within the unit sphere
  for(unsigned int i=0; i<nVertices_; i++)
    {
      // move the vertices within the unit sphere
      vertices_[i] = Point(0,0,0) + (vertices_[i]-centroid)*(10.0/radius);
    }

  // close the file
  fprintf(stderr,"closing '%s'...",filename);
  in.close();

  // return true as everything worked out
  return true;
}

/**
 * read IV file -- these are the cubit files that we received from
 * jason.  They have a bunch of unneeded information that we throw out.
 */
bool TriMesh::loadIV(const char *filename)
{
  fprintf(stderr,"reading '%s'...",filename);
  // try opening the file for input reading
  std::ifstream in;
  in.open(filename);
  if(!in)
    {
      fprintf(stderr,"[ERROR]: TriMesh::loadIV() - error reading file\n");
      return false;
    }
  // process the garbage at the front of the file
  char buffer[256];
  in.getline( buffer,256 );		// #Inventor V2.0 ascii
  // read and throw out the explicit map
  char *token1;
  bool foundCoordinate = false;
  while (!foundCoordinate && !in.eof())
    {
      // grab the line of text
      in.getline( buffer,256 );
      if ( (token1 = strtok(buffer," ")) != NULL )
			if ( strcmp(token1,"Coordinate3") ==0 )
				foundCoordinate = true;
    }
  // if didn't find the coordinate then print error and quit
  if (!foundCoordinate)
    {
      fprintf(stderr,"[ERROR] TriMesh::loadTXT() - no cordinate found!\n");
      return false;
    }
  in.getline( buffer,256 );		// point [
  bool foundEndToken = false;
  Point centroid(0,0,0);
  token1 = NULL;
  while (!foundEndToken && ! in.eof())
  {
	  Point p;
	  in.getline(buffer, 255);
	  if ( (token1 = strstr(buffer,"]")) != NULL )
		  foundEndToken = true;
	  else
	  {
		sscanf(buffer,"%g %g %g", &p.x(), &p.y(), &p.z());
		centroid +=p;
	    vertices_.push_back(p);
	  }
  }
  nVertices_ = vertices_.size();
  centroid /=nVertices_;
    
  // read in triangles
  bool foundIndexedFaceSet = false;
  while (!foundIndexedFaceSet && !in.eof())
    {
      // grab the line of text
      in.getline( buffer,256 );
      if ( (token1 = strtok(buffer," ")) != NULL )
			if ( strcmp(token1,"IndexedFaceSet") ==0 )
				foundIndexedFaceSet = true;
    }
  // if didn't find the triangle list then print error and quit
  if (!foundIndexedFaceSet)
    {
      fprintf(stderr,"[ERROR] TriMesh::loadTXT() - no triangle found!\n");
      return false;
    }
   in.getline( buffer,256 );		// coordIndex [
   foundEndToken = false;
   token1 = NULL;
   faces_.clear();
   while (!foundEndToken && ! in.eof())
   {
	  Tri_Index tri;
	  //unsigned int a, b, c;
	 
	  in.getline(buffer, 255);
	  if ( (token1 = strstr(buffer,"]")) != NULL )
		  foundEndToken = true;
	  else
	  {
		  unsigned int a, b, c;
		  unsigned int i=0;
		  while (buffer[i]!=0)
		  {
			  if (buffer[i] == ',') buffer[i]=' ';
			  i++;
		  }
		  sscanf(buffer,"%d %d %d",&a,&b,&c);
		  tri.a_ = a;
		  tri.b_ = b;
		  tri.c_ = c;
		  faces_.push_back(tri);
	  }
   }
   // loop back through and compute the radius from the centroid
   nTriangles_ = faces_.size();

   fprintf(stderr, "Reading '%d' point '%d' triangle : ", nVertices_, nTriangles_);

   float radius = 0;
   for(unsigned int i=0; i<nVertices_; i++)
   {
      float distance = centroid.distance_to( vertices_[i] );
	  if (distance > radius) 
		  radius = distance;
   }
  // scale every point to be within the unit sphere
   for(unsigned int i=0; i<nVertices_; i++)
   {
	   // move the vertices within the unit sphere
	   vertices_[i] = Point(0,0,0) + (vertices_[i]-centroid)*(10.0/radius);
   }
   // close the file
   fprintf(stderr,"closing '%s'...",filename);
   in.close();
   // return true as everything worked out
   return true;
}

/**
 * Sample the triangle with noise height 
 */
 
Point makeSample(const Point& p, const Point& p1, const Point& p2, float noiseHeight_)
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
  float noise = (((float)(rand()))/RAND_MAX-0.5) * noiseHeight_;
  return (p*b+p1*b1+p2*b2)+normal*noise;
}

/**
 * Produce the noise meshed to test for the algorithms by maing random pointset 
 * from the input model and take random point as noise
 * pts: number of point in the new point set , error_percent : percents of point is error.
 * noiseHeight for ech noise sample
 */
std::vector<Point> TriMesh::pointCloud(unsigned int pts, float error_percent, float  noise_height) const 
{
  std::vector<Point> mesh_;
  mesh_.resize(pts);

  unsigned int nFrequency_   = int (1.0/ error_percent); 
  unsigned current_face_ =0;
  unsigned int seed = (unsigned int) (((float)(rand()))/RAND_MAX*nFrequency_);
  for (unsigned int i=0; i< pts; i++)
    {
     
      unsigned int a = faces_[current_face_].a_;
      unsigned int b = faces_[current_face_].b_;
      unsigned int c = faces_[current_face_].c_;
      
      const Point& A = vertices_[a];
      const Point& B = vertices_[b];
      const Point& C = vertices_[c];
      
      if (i % nFrequency_ == seed) 
	mesh_[i] = makeSample(A,B,C,noise_height);
      else
	mesh_[i] = makeSample(A,B,C,0.0);
      if (i % nFrequency_ == (nFrequency_ -1))
      	seed =(unsigned int) (((float)(rand()))/RAND_MAX*nFrequency_);
      current_face_++;
      if (current_face_ == nTriangles_)
	current_face_ = 0;
    }

  return mesh_;
}

/**
 * Produce the noise meshed to test for the algorithms by maing random pointset 
 * from the input model and take random point as noise
 * pts: number of point in the new point set , noise_height: is the height of the
 * given noise range.
 */
std::vector<Point> 
TriMesh::pointCloud( const unsigned int &pts, 
		     const float &noise_height) const 
{

  unsigned int real_pts = pts;
  if (pts < nTriangles_)
	  fprintf(stderr, "[WARNING]: the number of sampler bellow the number of triangles of the input model \n");
  while (real_pts< nTriangles_*2)
	  real_pts *=2;
 
  // compute a set of samples by sampling all faces in the mesh
  std::vector<Point> points( real_pts );

  // loop through all the desired points to be made
  unsigned int currentFace = 0;
  for (unsigned int i=0; i<real_pts; i++)
    {
      // get the vertices of the current face
      unsigned int a = faces_[currentFace].a_;
      unsigned int b = faces_[currentFace].b_;
      unsigned int c = faces_[currentFace].c_;
     
      const Point& A = vertices_[a];
      const Point& B = vertices_[b];
      const Point& C = vertices_[c];

      // sample the face randomly
      points[i] = makeSample(A,B,C,noise_height);

      // increment the current face
      currentFace++;
      if (currentFace >= nTriangles_)
	currentFace = 0;
    }

  // return the point set
  return points;
}

