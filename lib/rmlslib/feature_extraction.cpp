/**
 *
 * file:	display_main
 * description:	This is the main display for the class project.  It will
 *		allow the user to load a mesh file and displays the point 
 *		cloud.  The user may modify the noise and number of smaples
 *		using the keyboard commands.  The program will compute
 *		smooth feature lines that describe sharp edges within the 
 *		point cloud.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include <GL/glut.h>
#include "glui.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <queue>

#include "Camera.h"
#include "Point.h"
#include "Vector.h"
#include "Primitive_Functions.h"
#include "Global_Defs.h"
#include "PointCloud.h"
#include "TriMesh.h"
#include "FeatureEdge.h"
#include "PC_io.h"
#include "FLF_io.h"
#include "FLF_conversion.h"

// include functions for each of the stages
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"
#include "Stage4.h"
#include "Stage5.h"

// -- GLOBAL VARIABLES -- //
Camera camera_;
int main_window_,ppm_window_;
int current_button_ = -1;
int lastX_;
int lastY_;

// assorted clouds and feature polyline stages
PointCloud* originalCloud_;
PointCloud* potentialCloud_;
PointCloud* featureCloud_;
PointCloud* smoothCloud_;
std::vector< FeatureEdge > featurePolylines_;
std::vector< FeatureEdge > finalPolylines_;
unsigned int currentFeature_=0;

// testing the the FLF stuff is working properly
std::vector< InfoPoint > ptList_;
std::vector< std::vector< InfoIndex > > featList_;
unsigned int currentLoop_=0;

// draw flags to allow user to view different items
bool draw_pointCloud_=true;
bool draw_potentialCloud_=false;
bool draw_featureCloud_=false;
bool draw_smoothCloud_=false;

bool draw_cornerPoints_=true;
bool draw_edgePoints_=true;
bool draw_flatPoints_=true;

bool draw_featurePolylines_=false;
bool draw_featureTangents_=false;
bool draw_featureNormals_=false;
bool draw_finalPolylines_=false;
bool draw_FLFstuff_=false;

// user controls for the point cloud
float noiseScale_=0.00;
ShapeType shapeType_=Ra;
unsigned int numSamples_=100000;
unsigned int thePointSize_=1;

// user control for computing the noise tolerance
bool draw_noiseToleranceMODE_ = false;
float noiseTolerance_=0.001;
unsigned int noiseTolerancePointOffset_=0;

// forward declaration
unsigned int fileCount_ = 0;
void writePPMfile( const char *filename );

/**
 * read in the feature file and save the feature edges
 */
void readFeatureFile( char *filename )
{
  fprintf(stderr," (*) reading feature file '%s'...",filename);

  // clear the features
  finalPolylines_.clear();

  // open the file
  std::ifstream in;
  in.open(filename);
  if (!in)
    {
      fprintf(stderr,"[ERROR] readFeatureFile('%s') couldn't open file!\n",filename);
      return;
    }

  // process the initial comments
  char buffer[256];
  unsigned int numFeatures;
  in.getline( buffer,256 );		// # number of feature edges (comment)
  in >> numFeatures;			// the number of features
  fprintf(stderr,"feature count (%d)...",numFeatures);

  // read in the features
  for(unsigned int i=0; i<numFeatures; i++)
    {
      // read in the feature comment
      in.getline( buffer,256 );
      in.getline( buffer,256 );

      // read in the size of the feature
      unsigned int featureSize;
      in >> featureSize;
      fprintf(stderr,"%d(%d).",i,featureSize);

      // read in the points
      std::vector< Point > points(featureSize);
      std::vector< Vector > normals1(featureSize);
      std::vector< Vector > normals2(featureSize);
      for(unsigned int j=0; j<featureSize; j++)
	{
	  float x,y,z;
	  in >> x >> y >> z;
	  points[j] = Point(x,y,z);

	  in >> x >> y >> z;
	  normals1[j] = Vector(x,y,z);

	  in >> x >> y >> z;
	  normals2[j] = Vector(x,y,z);
	}

      // save the feature
      finalPolylines_.push_back( FeatureEdge(points,normals1,normals2) );
    }
  fprintf(stderr,"done reading file (%d features) -*\n",finalPolylines_.size());
}

/**
 * write out the features into the given file.
 */
void writeFeatureFile( char *filename )
{
  FILE *filePtr = fopen(filename,"w");
  if (!filePtr)
    {
      fprintf(stderr,"[ERROR] writeFeatureFile() couldn't open file '%s'\n",filename);
      return;
    }

  // write out the feature polyline size
  fprintf(filePtr,"# number of feature polylines\n");
  fprintf(filePtr,"%d\n",finalPolylines_.size());

  // write out each polyline
  for(unsigned int i=0; i<finalPolylines_.size(); i++)
    {
      // write out the size
      fprintf(filePtr,"# -- feature %d --\n",i);
      fprintf(filePtr,"%d\n",finalPolylines_[i].getNumPoints());
      
      // write out the points for the polyline
      for(unsigned int j=0; j<finalPolylines_[i].getNumPoints(); j++)
	{
	  fprintf(filePtr,"%g %g %g %g %g %g %g %g %g\n",
		  finalPolylines_[i].getFeaturePoint(j).x(),
		  finalPolylines_[i].getFeaturePoint(j).y(),
		  finalPolylines_[i].getFeaturePoint(j).z(),
		  finalPolylines_[i].getFeatureNormal(j,0).x(),
		  finalPolylines_[i].getFeatureNormal(j,0).y(),
		  finalPolylines_[i].getFeatureNormal(j,0).z(),
		  finalPolylines_[i].getFeatureNormal(j,1).x(),
		  finalPolylines_[i].getFeatureNormal(j,1).y(),
		  finalPolylines_[i].getFeatureNormal(j,1).z() );
	}
    }

  // close the file
  fclose( filePtr );
}

/**
 * writes out the feature loop file (FLF file) that uses the FLF.h file 
 * in and out operations.
 */
void
writeLoopFile( const char *filename )
{
  // convert the feature edges into the point list and feature lists
  FLF_buildLists( ptList_,featList_, finalPolylines_ );
  fprintf(stderr,"built ptList(%d) featList(%d)...",ptList_.size(),featList_.size());
  FLF_write( filename, ptList_,featList_ );
}

/**
 * writes out the point cloud file.
 */
void
writePointCloudFile( const char *filename )
{
  std::vector< InfoPoint > tmpList(originalCloud_->vertices.size());
  for(unsigned int i=0; i<tmpList.size(); i++)
    {
      tmpList[i].x_ = originalCloud_->vertices[i].x();
      tmpList[i].y_ = originalCloud_->vertices[i].y();
      tmpList[i].z_ = originalCloud_->vertices[i].z();
      tmpList[i].isCorner_ = false;
    }
  PC_write( filename,tmpList );
}

/**
 * create shape toggles between the different shape types
 */
void createShape()
{
  fprintf(stderr," (*) creating the shape...");
  originalCloud_ = new PointCloud( shapeType_,numSamples_,noiseScale_ );
  fprintf(stderr,"done\n");
}

/**
 * parse the command line -- allows the user to specify the type of 
 * point cloud that they would like to read in from the data directory.
 * by default we use the dragon.
 */
void parse_command_line( int &arg_count,
			 char** &arg_array )
{
  // scroll through each of the arguments and perform the desired flag
  int current = 1;
  while(current<arg_count)
    {
      // get the current tag
      std::string command = arg_array[current];

      // -- TAG: Help
      // print out the help menu so that the user better understands the 
      if (command == "-help")
	{
	  fprintf(stderr," -- HELP MENU --\n");
	  fprintf(stderr," -help        print out this help information\n");
	  fprintf(stderr,"              (note) there are no tag options\n");
	  fprintf(stderr," \n");
	  fprintf(stderr," -- DONE --\n");
	  current++;
	}
      // -- TAG: unknown
      else
	{
	  fprintf(stderr," -- ERROR: unknown tag '%s'\n",command.c_str());
	  current++;
	}
    }
}

/**
 * initialize the environment: point cloud and camera.
 */
void init_environment()
{
  // create some point cloud for the start information
  createShape();		

  // initialize the camera
  float init_focus = 10;
  float init_width = 80;
  float init_height = 60;
  int init_window_width = 800;
  int init_window_height = 800; 

  // compute the initial lookat and eye
  Point center;
  float radius;
  originalCloud_->get_bound_sphere( center, radius );
  if ( EQL(radius,0) ) radius = 1.0;
  Vector look_dir(0,-radius/2.0,-1.25*radius);
  camera_.initialize( center - look_dir, look_dir, Vector(0,1,0), init_focus,
		      init_width, init_height, init_window_width, init_window_height );
}

/** 
 * glut idle -- an idle function called when nothing is happening.
 */
void myGlutIdle(void)
{
  // make sure the main window is active
  if (glutGetWindow() != main_window_)
    glutSetWindow( main_window_ );

  // redraw the scene over and over
  glutPostRedisplay();
}

/**
 * glut mouse -- called when there is a mouse action click or release.
 */
void myGlutMouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN)
    {
      current_button_ = button;
    }
  else if (button == current_button_)
    {
      current_button_ = -1;
    }

  lastX_ = x;
  lastY_ = y;
}

/**
 * glut motion -- called when there is mouse motion.
 */ 
void myGlutMotion(int x, int y)
{
  // the change in mouse positions
  int dx = x - lastX_;
  int dy = y - lastY_;
  double theta,phi;

  // switch on the mouse button and move accordingly
  switch( current_button_ )
    {
    case GLUT_LEFT_BUTTON:
      camera_.pan( -dx,dy );
      break;
    case GLUT_MIDDLE_BUTTON:
      camera_.zoom( -dy );
      break;
    case GLUT_RIGHT_BUTTON:
      theta = -dx*0.007;
      phi = -dy*0.007;
      camera_.rotate( theta,phi );
      break;
    }

  // update the last mouse spots
  lastX_ = x;
  lastY_ = y;

  // redisplay the image
  glutPostRedisplay();
}

/**
 * glut keyboard -- called when there is a keyboard button typed.
 */
void myGlutKeyboard(unsigned char key, int x, int y)
{
  Point eye,look;
  Vector up;
  char filename[256];
  std::vector< InfoIndex > tmpList;
  unsigned int i;

  switch(key)
    {
      // help menu
    case 'h':
    case 'H':
      fprintf(stderr," ---- HELP MENU ----\n");
      fprintf(stderr," '1'    load an .off file into the window\n");
      fprintf(stderr," '2'    load a .txt file into the window\n");
      fprintf(stderr," '3'    load a .fef file into the window (feature file!)\n");
      fprintf(stderr," '4'    change shape to a basic cube\n");
      fprintf(stderr," '5'    change shape to a basic trapezoid\n");
      fprintf(stderr," '6'    change shape to a basic icosahedron\n");
      fprintf(stderr," '7'    change shape to a basic semisphere\n");
      fprintf(stderr," '8'    change shape to a basic cylinder\n");
      fprintf(stderr," '9'    change shape to a basic two intersecting spheres\n");
      fprintf(stderr," '0'    recreate the basic shapes\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'N'    increases the noise in the shape\n");
      fprintf(stderr," 'n'    decreases the noise in the shape\n");
      fprintf(stderr," 'S'    increases the number of vertices in the shape\n");
      fprintf(stderr," 's'    decreases the number of vertices in the shape\n");
      fprintf(stderr," 'V'    increases the size of the vertex drawn\n");
      fprintf(stderr," 'v'    decreases the size of the vertex drawn\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'x'    recreate the features in the point cloud\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'c'    toggle drawing the point 'C'loud\n");
      fprintf(stderr," 'p'    toggle drawing the 'P'otential feature points\n");
      fprintf(stderr," 'f'    toggle drawing the 'F'eature points\n");
      fprintf(stderr," 'g'    toggle drawing the Smoothed feature points\n");
      fprintf(stderr," 't'    toggle drawing the Corner ('T'-junction) feature points\n");
      fprintf(stderr," 'e'    toggle drawing the 'E'dge feature points\n");
      fprintf(stderr," 'r'    toggle drawing the flat feature points\n");
      fprintf(stderr," 'j'    toggle drawing the normals associated with each point\n");
      fprintf(stderr," 'y'    toggle drawing the feature polylines\n");
      fprintf(stderr," 'u'    toggle drawing the feature tangents\n");
      fprintf(stderr," 'i'    toggle drawing the final feature polylines\n");
      fprintf(stderr," 'k'    toggle drawing the FLF information\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'o'    'o'utput the feature edge information into a file\n");
      fprintf(stderr," 'O'    'O'utput the feature loop file information\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'l'    flip to the next feature loop\n");
      fprintf(stderr," 'L'    flip to the previous feature loop\n");
      fprintf(stderr," 'm'    flip the first tangents of the current loop\n");
      fprintf(stderr," 'M'    flip the second tangents of the current loop\n");
      fprintf(stderr,"\n");
      fprintf(stderr," ','    flip the loop direction of the current feature\n");
      fprintf(stderr," '<'    recommit the .flf file\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'a'    compute the noise 'A'llowance - (change to noise mode)\n");
      fprintf(stderr," 'Z'    next point in noise allowance computation\n");
      fprintf(stderr," 'z'    previous point in noise allowance computation\n");
      fprintf(stderr," 'W'    fine tune the noise tolerance up\n");
      fprintf(stderr," 'w'    fine tune the noise tolerance down\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'Q'    screen capture\n");
      fprintf(stderr,"\n");
      break;

      // -------------------------------------------------- //
      // change shape 
    case 48:
      fprintf(stderr," *- reforming the shape -*\n");
      createShape();
      break;
    case 49:
      fprintf(stderr," *- load .off file: ");
      std::cin >> filename;
      originalCloud_->loadOFFfile( filename,numSamples_,noiseScale_ );
      fprintf(stderr,"done\n");
      break;
    case 50:
      fprintf(stderr," *- load .txt file: ");
      std::cin >> filename;
      originalCloud_->loadTXTfile( filename,numSamples_,noiseScale_ );
      fprintf(stderr,"done\n");
      break;
    case 51:
      fprintf(stderr," *- load .fef file: ");
      std::cin >> filename;
      readFeatureFile( filename );
      break;
    case 52:
      fprintf(stderr," *- load .flf file: ");
      std::cin >> filename;
      FLF_read( filename, ptList_,featList_ );
      fprintf(stderr," *- done reading (%d points and %d features) -*\n",ptList_.size(),featList_.size());
      break;
    case 53:
      fprintf(stderr," *- switching model to a CUBE -*\n");
      shapeType_ = Cube;
      createShape();
      break;
//     case 54:
//       fprintf(stderr," *- switching model to a TRAPEZOID -*\n");
//       shapeType_ = Trapezoid;
//       createShape();
//       break;
    case 55:
      fprintf(stderr," *- switching model to a Icosahedron -*\n");
      shapeType_ = Icosahedron;
      createShape();
      break;
    case 56:
      fprintf(stderr," *- switching model to the Semi Sphere -*\n");
      shapeType_ = SemiSphere;
      createShape();
      break;
    case 57:
      fprintf(stderr," *- switching model to the Cylinder -*\n");
      shapeType_ = Dog;
      createShape();
      break;
//     case 58:
//       fprintf(stderr," *- switching model to the Two Intersecting Spheres -*\n");
//       shapeType_ = TwoSphere;
//       createShape();
//       break;
     case 54:
       fprintf(stderr," *- switching model to the Scalloped Cube -*\n");
       shapeType_ = ScallopCube;
       createShape();
       break;

      // -------------------------------------------------- //
      // computation of the noise tolerance for the system
    case 'a':
    case 'A':
      if ( draw_noiseToleranceMODE_ = !draw_noiseToleranceMODE_ )
	{
	  fprintf(stderr," *- Enter noise tolerance computation mode -*\n");
	  fprintf(stderr," use the 's' key to skip to the next point and the\n");
	  fprintf(stderr," 'z' key to skip to the previous key.  The current\n");
	  fprintf(stderr," point is highlighted in blue while its neighbors are\n");
	  fprintf(stderr," red.  Also shown is the fit surface to the neighbor\n");
	  fprintf(stderr," cloud.  Choose the point with the desired fit, in a\n");
	  fprintf(stderr," smooth region and hit 'a' to commit.\n");
	  noiseTolerancePointOffset_ = 0;
	}
      else
	{
	  fprintf(stderr," *- noise tolerance set to %g -*\n",noiseTolerance_);
	}
      break;
    case 'Z':
      if (noiseTolerancePointOffset_ >= originalCloud_->vertices.size()-1)
	noiseTolerancePointOffset_ = 0;
      else
	noiseTolerancePointOffset_++;
      break;
    case 'z':
      if (noiseTolerancePointOffset_ == 0)
	noiseTolerancePointOffset_ = originalCloud_->vertices.size()-1;
      else
	noiseTolerancePointOffset_--;
      break;

      // -------------------------------------------------- //
      // decrease the noise tolerance
    case 'w':
      noiseTolerance_-=0.0005;
      if (noiseTolerance_<=0)
	{
	  noiseTolerance_ = 0;
	  break;
	}
      fprintf(stderr," *- noise tolerance = %g -*\n",noiseTolerance_);
      if (originalCloud_->vertices.size() != originalCloud_->verticesWeights.size())
	fprintf(stderr," *- type 'x' to see potential cloud results -*\n");
      else
	{
	  potentialCloud_ = NULL;
	  helper1_extractPotentialCloud( originalCloud_,noiseTolerance_,potentialCloud_ );
	  fprintf(stderr,"    potential points size = %d\n",potentialCloud_->vertices.size());
	}
      break;
      // increase the noise tolerance
    case 'W':
      noiseTolerance_+=0.0005;
      if (noiseTolerance_>0.25)
	{ 
	  noiseTolerance_ = 0.25;
	  break;
	}
      fprintf(stderr," *- noise tolerance = %g -*\n",noiseTolerance_);
      if (originalCloud_->vertices.size() != originalCloud_->verticesWeights.size())
	fprintf(stderr," *- type 'x' to see potential cloud results -*\n");
      else
	{
	  potentialCloud_ = NULL;
	  helper1_extractPotentialCloud( originalCloud_,noiseTolerance_,potentialCloud_ );
	  fprintf(stderr,"    potential points size = %d\n",potentialCloud_->vertices.size());
	}
      break;

      // -------------------------------------------------------- //
      // flip to the next feature loop
    case 'l':
      currentLoop_++;
      if (currentLoop_ > featList_.size()) currentLoop_ = 0;

      currentFeature_++;
      if (currentFeature_ > finalPolylines_.size()) currentFeature_ = 0;
      break;
    case 'L':
      if (currentLoop_ == 0) currentLoop_ = featList_.size();
      else currentLoop_--;

      if (currentFeature_ == 0) currentFeature_ = finalPolylines_.size();
      else currentFeature_--;
      break;

      // flip the first normals of the current feature edge
    case 'm':
      if (currentFeature_ < finalPolylines_.size())
	{
	  fprintf(stderr," *- flipping normal 1 group on current feature -*\n");
	  finalPolylines_[currentFeature_].flipNormals1();
	}
      break;
    case 'M':
      if (currentFeature_ < finalPolylines_.size())
	{
	  fprintf(stderr," *- flipping normal 2 group on current feature -*\n");
	  finalPolylines_[currentFeature_].flipNormals2();
	}
      break;

    case 44:
      if (currentLoop_ < featList_.size())
	{
	  fprintf(stderr," *- flipping current loop direction -*\n");
	  tmpList.resize( featList_[currentLoop_].size() );
	  for(i=0; i<featList_[currentLoop_].size(); i++)
	    {
	      tmpList[ tmpList.size()-1-i ].x_ = featList_[currentLoop_][i].x_;
	      tmpList[ tmpList.size()-1-i ].y_ = featList_[currentLoop_][i].y_;
	      tmpList[ tmpList.size()-1-i ].z_ = featList_[currentLoop_][i].z_;
	      tmpList[ tmpList.size()-1-i ].index_ = featList_[currentLoop_][i].index_;
	    }
	  featList_[currentLoop_] = tmpList;
	}
      break;
    case 60:
      if (featList_.size() > 0)
	{
	  fprintf(stderr," *- write feature loops file (.flf): ");
	  std::cin >> filename;
	  FLF_write( filename, ptList_,featList_ );
	  fprintf(stderr," *- complete file output -*\n");
	}
      break;

      // -------------------------------------------------- //
      // decrease the noise of the cube
    case 'n':
      noiseScale_-=0.01;
      if (noiseScale_<0)
	{
	  noiseScale_ = 0;
	  break;
	}
      fprintf(stderr," *- noise = %g (type 'x' to recompute features) -*\n",noiseScale_);
      createShape();
      break;
      // increase the noise of the cube
    case 'N':
      noiseScale_+=0.01;
      if (noiseScale_>1)
	{ 
	  noiseScale_ = 1;
	  break;
	}
      fprintf(stderr," *- noise = %g (type 'x' to recompute features) -*\n",noiseScale_);
      createShape();
      break;

      // -------------------------------------------------- //
      // decrease the size of the vertices
    case 'v':
      if (thePointSize_-1 < thePointSize_)
	thePointSize_--;
      fprintf(stderr," *- vertex drawing size = %d\n",thePointSize_);
      break;
      // increase the size of the vertices
    case 'V':
      if (thePointSize_+1 > thePointSize_)
	thePointSize_++;
      fprintf(stderr," *- vertex drawing size = %d\n",thePointSize_);
      break;

      // -------------------------------------------------- //
      // decrease sample sizes and update shape
    case 's':
      if( numSamples_-1000 < numSamples_ )
	numSamples_ -= 1000;
      fprintf(stderr," *- number of sample points = %d (type '0' to recreate the shape) -*\n",numSamples_);
      break;
      // increase sample sizes and update shape
    case 'S':
      if( numSamples_+1000 > numSamples_ )
	numSamples_ += 1000;
      fprintf(stderr," *- number of sample points = %d (type '0' to recreate the shape) -*\n",numSamples_);
      break;

      // -------------------------------------------------- //
      // toggle the drawing of feature points only
    case 'c':
    case 'C':
      if ( draw_pointCloud_ = !draw_pointCloud_ )
	fprintf(stderr," *- drawing the point cloud: enable -*\n");
      else
	fprintf(stderr," *- drawing the point cloud: disable -*\n");
      break;
      // toggle drawing potential feature points
    case 'p':
    case 'P':
      if( draw_potentialCloud_ = !draw_potentialCloud_ )
	fprintf(stderr," *- drawing the potential feature points: enable -*\n");
      else
	fprintf(stderr," *- drawing the potential feature points: disable -*\n");
      break;
      // toggle drawing feature points
    case 'f':
    case 'F':
      if( draw_featureCloud_ = !draw_featureCloud_ )
	fprintf(stderr," *- drawing the feature points: enable -*\n");
      else
	fprintf(stderr," *- drawing the feature points: disable -*\n");
      break;
      // toggle normals on features
    case 'g':
    case 'G':
      if ( draw_smoothCloud_ = !draw_smoothCloud_ )
	fprintf(stderr," *- drawing smooth feature points: enable -*\n");
      else
	fprintf(stderr," *- drawing smooth feature points: disable -*\n");
      break;
      // toggle corner feature draw
    case 't':
    case 'T':
      if( draw_cornerPoints_ = !draw_cornerPoints_ )
	fprintf(stderr," *- drawing corner points: enable -*\n");
      else
	fprintf(stderr," *- drawing corner points: disable -*\n");
      break;
      // toggle edge points draw
    case 'e':
    case 'E':
      if ( draw_edgePoints_ = !draw_edgePoints_ )
	fprintf(stderr," *- drawing edge points: enable -*\n");
      else
	fprintf(stderr," *- drawing edge points: disable -*\n");
      break;
      // toggle the drawing of the feature polylines
    case 'r':
    case 'R':
      if ( draw_flatPoints_ = !draw_flatPoints_ )
	fprintf(stderr," *- drawing the non feature points: enable -*\n");
      else
	fprintf(stderr," *- drawing the non feature points: disable -*\n");
      break;
      // toggle the drawing of the normals of the points
    case 'j':
    case 'J':
      if ( draw_featureNormals_ = !draw_featureNormals_ )
	fprintf(stderr," *- drawing the normals for the points: enable -*\n");
      else
	fprintf(stderr," *- drawing the normals for the points: disable -*\n");
      break;
      // toggle the drawing of the feature polylines
    case 'y':
    case 'Y':
      if ( draw_featurePolylines_ = !draw_featurePolylines_ )
	fprintf(stderr," *- drawing the feature polylines: enable -*\n");
      else
	fprintf(stderr," *- drawing the feature polylines: disable -*\n");
      break;
      // toggle the drawing of the smooth feature tangents
    case 'u':
    case 'U':
      if ( draw_featureTangents_ = !draw_featureTangents_ )
	fprintf(stderr," *- drawing the feature tangents: enable -*\n");
      else
	fprintf(stderr," *- drawing the feature tangents: disable -*\n");
      break;
      // toggle the drawing of the final feature polylines
    case 'i':
    case 'I':
      if ( draw_finalPolylines_ = !draw_finalPolylines_ )
	fprintf(stderr," *- drawing the final polylines: enable -*\n");
      else
	fprintf(stderr," *- drawing the final polylines: disable -*\n");
      break;
      // toggle the drawing of the flf information
    case 'k':
    case 'K':
      if ( draw_FLFstuff_ = !draw_FLFstuff_ )
	fprintf(stderr," *- drawing the FLF information: enable -*\n");
      else
	fprintf(stderr," *- drawing the FLF information: disable -*\n");
      break;

      // -------------------------------------------------- //
      // recreate the feature lines
    case 'x':
      if (originalCloud_ && !draw_noiseToleranceMODE_)
	{
	  fprintf(stderr," *- extracting features edges from point cloud (%d points) -*\n",originalCloud_->vertices.size());
	  if (originalCloud_->verticesWeights.size() != originalCloud_->vertices.size()) potentialCloud_ = stage1_createPotentialCloud(originalCloud_, noiseTolerance_ );
	  else helper1_extractPotentialCloud( originalCloud_,noiseTolerance_,potentialCloud_ );
	  fprintf(stderr," *- done extracting potential points...type 'X' to complete feature extraction -*\n");
	}
      else
	{
	  fprintf(stderr," *- no cloud available for feature extraction or in noise tolerance mode -*\n");
	}
      break;
    case 'X':
      if (potentialCloud_ && !draw_noiseToleranceMODE_)
	{
	  fprintf(stderr," *- extracting feature edges from point cloud (%d potential points) -*\n",potentialCloud_->vertices.size());
	  featureCloud_ = stage2_projectPotentialCloud(potentialCloud_,originalCloud_, noiseTolerance_ );
	  smoothCloud_ = stage3_processFeatureCloud(featureCloud_,originalCloud_, noiseTolerance_ );
	  featurePolylines_ = stage4_extractFeaturePolylines( smoothCloud_ );
	  finalPolylines_ = stage5_resolveFeaturePolylines( featurePolylines_,smoothCloud_ );
	  fprintf(stderr," *- done feature edge extractions -*\n");
	}
      else
	{
	  fprintf(stderr," *- no potential points available for feature extraction or in noise tolerance mode -*\n");
	}
      break;

      // ----------------------------------------------------- //
      // output the feature lines to a file given on the line
    case 'o':
      fprintf(stderr," *- write feature edge file (.fef): ");
      std::cin >> filename;
      writeFeatureFile( filename );
      fprintf(stderr," *- complete file output -*\n");
      break;
    case 'O':
      fprintf(stderr," *- write out the point cloud file (.pc): ");
      std::cin >> filename;
      writePointCloudFile( filename );
      fprintf(stderr," *- complete file output -*\n");
      fprintf(stderr," *- write feature loops file (.flf): ");
      std::cin >> filename;
      writeLoopFile( filename );
      fprintf(stderr," *- complete file output -*\n");
      break;

      // ----------------------------------------------- //
      // screen shot
    case 'Q':
      sprintf(filename,"output_%d.ppm",fileCount_++);
      fprintf(stderr," *- writing image to file: '%s'...",filename);
      writePPMfile( filename );
      fprintf(stderr,"done -*\n");
      break;

    default:
      fprintf(stderr,"[%d/%c]: unknown action requested\n",key,key);
      break;
    }
}

/**
 * glut reshape -- the window is reshaped by the user.
 */
void myGlutReshape(int	x, int y)
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
  glViewport(tx, ty, tw, th);

  camera_.window_width() = tw;
  camera_.window_height() = th;

  glutPostRedisplay();
}

/**
 * draws the scene in a normal mode so that we can differentiate
 * when the user is trying to select a noise tolerance so that
 * they know things are still waiting for their input.
 */
void drawNormalMode()
{
  glClearColor(1,1,1, 0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // projection transform
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 1, 1000);

  // camera transform
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // get the information from the camera
  Point eye = camera_.get_eye();
  Point look = camera_.get_lookat();
  Vector up = camera_.get_up();

  // setup the window information
  gluLookAt( eye.x(), eye.y(), eye.z(),
	     look.x(), look.y(), look.z(),
 	     up.x(), up.y(), up.z() );

  // disable lighting computations
  glDisable(GL_LIGHTING);

  // draw the potential points
  if (draw_potentialCloud_ && potentialCloud_)
    {
      for (unsigned int j=0;j<potentialCloud_->vertices.size();j++)
	{
	  glPointSize( thePointSize_ );
	  glBegin( GL_POINTS );
	  glColor3f(1,0,0);
	  glVertex3f(potentialCloud_->vertices[j].x(), potentialCloud_->vertices[j].y(),potentialCloud_->vertices[j].z());
	  glEnd();
	}
    }

  // draw the point cloud
  if (draw_pointCloud_ && originalCloud_)
    {
      for (unsigned int j=0;j<originalCloud_->vertices.size();j++)
	{
	  glPointSize( thePointSize_ );
	  glBegin( GL_POINTS );
	  glColor3f(0,0,0);
	  glVertex3f(originalCloud_->vertices[j].x(), originalCloud_->vertices[j].y(),originalCloud_->vertices[j].z());
	  glEnd();
	}
    }

  // draw the feature points
  if (draw_featureCloud_ && featureCloud_)
    {
      for (unsigned int j=0;j<featureCloud_->vertices.size();j++)
	{
	  // draw corner points
	  if (featureCloud_->verticesNormals[j].size() >= 3 && draw_cornerPoints_)
	    {
	      glPointSize( thePointSize_+1 );
	      glBegin( GL_POINTS );
	      float value = (featureCloud_->verticesWeights[j])/0.75;
	      if (value > 1.0) value = 1.0;
	      glColor3f(0,1,value);
	      glVertex3f(featureCloud_->vertices[j].x(), featureCloud_->vertices[j].y(), featureCloud_->vertices[j].z() );
	      glEnd();
	    }

	  // draw edge points
	  else if (featureCloud_->verticesNormals[j].size() == 2 && draw_edgePoints_)
	    {
 	      glPointSize( thePointSize_+1 );
	      glBegin( GL_POINTS );
	      float value = featureCloud_->verticesWeights[j];
	      glColor3f(1,value,0);
	      glVertex3f(featureCloud_->vertices[j].x(), featureCloud_->vertices[j].y(), featureCloud_->vertices[j].z() );
	      glEnd();
	    }
	  else if (featureCloud_->verticesNormals[j].size() == 1 && draw_flatPoints_)
	    {
 	      glPointSize( thePointSize_+1 );
	      glBegin( GL_POINTS );
	      glColor3f(1,0,1);
	      glVertex3f(featureCloud_->vertices[j].x(), featureCloud_->vertices[j].y(), featureCloud_->vertices[j].z() );
	      glEnd();
	    }

	  // draw the normals associated with the points
	  if( draw_featureNormals_ )
	    {
	      glLineWidth( 2.0 );
	      glBegin( GL_LINES );
	      float value = featureCloud_->verticesWeights[j];
	      glColor3f(1,value,0);
	      for(unsigned int k=0; k<featureCloud_->verticesNormals[j].size(); k++)
		{
		  glVertex3f(featureCloud_->vertices[j].x(), featureCloud_->vertices[j].y(), featureCloud_->vertices[j].z() );
		  glVertex3f(featureCloud_->vertices[j].x()+featureCloud_->verticesNormals[j][k].x()*0.5,
			     featureCloud_->vertices[j].y()+featureCloud_->verticesNormals[j][k].y()*0.5,
			     featureCloud_->vertices[j].z()+featureCloud_->verticesNormals[j][k].z()*0.5);
		}
	      glEnd();
	    }
	}
    }

  // draw the smooth cloud
  if (draw_smoothCloud_ && smoothCloud_)
    {
      for(unsigned int i=0; i<smoothCloud_->vertices.size(); i++)
	{
	  // draw corner points
	  if (smoothCloud_->verticesNormals[i].size() == 3 && draw_cornerPoints_)
	    {
	      glPointSize( thePointSize_+1 );
	      glBegin( GL_POINTS );
	      float value = (smoothCloud_->verticesWeights[i])/0.75;
	      if (value > 1.0) value = 1.0;
	      glColor3f(0,1,value);
	      glVertex3f(smoothCloud_->vertices[i].x(), smoothCloud_->vertices[i].y(), smoothCloud_->vertices[i].z() );
	      glEnd();
	    }

	  // draw edge points
	  else if (smoothCloud_->verticesNormals[i].size() == 2 && draw_edgePoints_)
	    {
 	      glPointSize( thePointSize_+1 );
	      glBegin( GL_POINTS );
	      float value = smoothCloud_->verticesWeights[i];
	      glColor3f(1,0,value);
	      glVertex3f(smoothCloud_->vertices[i].x(), smoothCloud_->vertices[i].y(), smoothCloud_->vertices[i].z() );
	      glEnd();
	    }
	  else if (smoothCloud_->verticesNormals[i].size() == 1 && draw_flatPoints_)
	    {
 	      glPointSize( thePointSize_+1 );
	      glBegin( GL_POINTS );
	      glColor3f(1,0,1);
	      glVertex3f(smoothCloud_->vertices[i].x(), smoothCloud_->vertices[i].y(), smoothCloud_->vertices[i].z() );
	      glEnd();
	    }

	  // draw the normals associated with the points
	  if( draw_featureNormals_ )
	    {
	      glLineWidth( 1.0 );
	      glBegin( GL_LINES );
	      float value = smoothCloud_->verticesWeights[i];
	      glColor3f(1,0,value);
	      for(unsigned int k=0; k<featureCloud_->verticesNormals[i].size(); k++)
		{
		  glVertex3f(smoothCloud_->vertices[i].x(), smoothCloud_->vertices[i].y(), smoothCloud_->vertices[i].z() );
		  glVertex3f(smoothCloud_->vertices[i].x()+smoothCloud_->verticesNormals[i][k].x()*0.5,
			     smoothCloud_->vertices[i].y()+smoothCloud_->verticesNormals[i][k].y()*0.5, 
			     smoothCloud_->vertices[i].z()+smoothCloud_->verticesNormals[i][k].z()*0.5 );
		}
	      glEnd();
	    }
	}
    }

  // draw the feature polylines
  if (draw_featurePolylines_)
    {
      // draw the features
      for(unsigned int i=0; i<featurePolylines_.size(); i++)
	{
	  // draw the polyline as red lines
	  glLineWidth( 3.0 );
	  glBegin(GL_LINE_STRIP);
	  glColor3f(1,0,0);
	  for(unsigned int j=0; j<featurePolylines_[i].getNumPoints(); j++)
	    {
	      glVertex3f( featurePolylines_[i].getFeaturePoint(j).x(), featurePolylines_[i].getFeaturePoint(j).y(), featurePolylines_[i].getFeaturePoint(j).z() );
	    }
	  glEnd();

	  // draw the intermediate points as yellow
	  glPointSize( thePointSize_ );
	  glBegin(GL_POINTS);
	  glColor3f(1,1,0);
	  for(unsigned int j=1; j<featurePolylines_[i].getNumPoints()-1; j++)
	    {
	      glVertex3f( featurePolylines_[i].getFeaturePoint(j).x(), featurePolylines_[i].getFeaturePoint(j).y(), featurePolylines_[i].getFeaturePoint(j).z() );
	    }
	  glEnd();

	  // draw the endpoints larger and orange
	  glPointSize( thePointSize_+1 );
	  glBegin(GL_POINTS);
	  glColor3f(1,0.7,0);
	  glVertex3f( featurePolylines_[i].getStartPoint().x(), featurePolylines_[i].getStartPoint().y(), featurePolylines_[i].getStartPoint().z() );
	  glColor3f(1,0,1);
	  glVertex3f( featurePolylines_[i].getEndPoint().x(), featurePolylines_[i].getEndPoint().y(), featurePolylines_[i].getEndPoint().z() );
	  glEnd();

	  // if draw the tangents 
	  if (draw_featureTangents_)
	    {
	      glLineWidth( 2.0 );
	      glBegin(GL_LINES);
	      glColor3f(1,0.7,0);

	      Point endPoint = featurePolylines_[i].getStartPoint();
	      Vector theTangent = featurePolylines_[i].getTangentStart();
	      glVertex3f( endPoint.x(), endPoint.y(), endPoint.z() );
	      glVertex3f( endPoint.x()+theTangent.x(),
			  endPoint.y()+theTangent.y(),
			  endPoint.z()+theTangent.z() );

	      glColor3f(1,0,1);
	      endPoint = featurePolylines_[i].getEndPoint();
	      theTangent = featurePolylines_[i].getTangentEnd();
	      glVertex3f( endPoint.x(), endPoint.y(), endPoint.z() );
	      glVertex3f( endPoint.x()+theTangent.x(),
			  endPoint.y()+theTangent.y(),
			  endPoint.z()+theTangent.z() );

	      glEnd();
	    }

	  // draw the normals associated with the points
	  if( draw_featureNormals_ )
	    {
	      glLineWidth( 2.0 );
	      glBegin( GL_LINES );
	      for(unsigned int j=1; j<featurePolylines_[i].getNumPoints()-1; j++)
		{
		  glColor3f(1,1,0);
		  glVertex3f( featurePolylines_[i].getFeaturePoint(j).x(), featurePolylines_[i].getFeaturePoint(j).y(), featurePolylines_[i].getFeaturePoint(j).z() );
		  glVertex3f(featurePolylines_[i].getFeaturePoint(j).x()+featurePolylines_[i].getFeatureNormal(j,0).x()*0.5,
			     featurePolylines_[i].getFeaturePoint(j).y()+featurePolylines_[i].getFeatureNormal(j,0).y()*0.5,
			     featurePolylines_[i].getFeaturePoint(j).z()+featurePolylines_[i].getFeatureNormal(j,0).z()*0.5);

		  glColor3f(1,0.7,0);
		  glVertex3f( featurePolylines_[i].getFeaturePoint(j).x(), featurePolylines_[i].getFeaturePoint(j).y(), featurePolylines_[i].getFeaturePoint(j).z() );
		  glVertex3f(featurePolylines_[i].getFeaturePoint(j).x()+featurePolylines_[i].getFeatureNormal(j,1).x()*0.5,
			     featurePolylines_[i].getFeaturePoint(j).y()+featurePolylines_[i].getFeatureNormal(j,1).y()*0.5,
			     featurePolylines_[i].getFeaturePoint(j).z()+featurePolylines_[i].getFeatureNormal(j,1).z()*0.5);

		}
	      glEnd();
	    }
	}
    }

  // draw the feature polylines
  if (draw_finalPolylines_)
    {
      // draw the features
      for(unsigned int i=0; i<finalPolylines_.size(); i++)
	{
	  // draw the polyline as red lines
	  glLineWidth( 3.0 );
	  glBegin(GL_LINE_STRIP);
	  glColor3f(0,0,1);
	  for(unsigned int j=0; j<finalPolylines_[i].getNumPoints(); j++)
	    {
	      glVertex3f( finalPolylines_[i].getFeaturePoint(j).x(), finalPolylines_[i].getFeaturePoint(j).y(), finalPolylines_[i].getFeaturePoint(j).z() );
	    }
	  glEnd();

	  if (currentFeature_ == finalPolylines_.size() || currentFeature_ == i)
	    {

	      // draw the intermediate points as yellow
	      glPointSize( thePointSize_ );
	      glBegin(GL_POINTS);
	      glColor3f(0,1,1);
	      for(unsigned int j=1; j<finalPolylines_[i].getNumPoints()-1; j++)
		{
		  glVertex3f( finalPolylines_[i].getFeaturePoint(j).x(), finalPolylines_[i].getFeaturePoint(j).y(), finalPolylines_[i].getFeaturePoint(j).z() );
		}
	      glEnd();

	      // draw the endpoints larger and orange
	      glPointSize( thePointSize_+1 );
	      glBegin(GL_POINTS);
	      glColor3f(0,1,0);
	      glVertex3f( finalPolylines_[i].getStartPoint().x(), finalPolylines_[i].getStartPoint().y(), finalPolylines_[i].getStartPoint().z() );
	      glVertex3f( finalPolylines_[i].getEndPoint().x(), finalPolylines_[i].getEndPoint().y(), finalPolylines_[i].getEndPoint().z() );
	      glEnd();

	      // if draw the tangents 
	      if (draw_featureTangents_)
		{
		  glLineWidth( 2.0 );
		  glBegin(GL_LINES);
		  glColor3f(0,1,0);
		  
		  Point endPoint = finalPolylines_[i].getStartPoint();
		  Vector theTangent = finalPolylines_[i].getTangentStart();
		  glVertex3f( endPoint.x(), endPoint.y(), endPoint.z() );
		  glVertex3f( endPoint.x()+theTangent.x(),
			      endPoint.y()+theTangent.y(),
			      endPoint.z()+theTangent.z() );

		  endPoint = finalPolylines_[i].getEndPoint();
		  theTangent = finalPolylines_[i].getTangentEnd();
		  glVertex3f( endPoint.x(), endPoint.y(), endPoint.z() );
		  glVertex3f( endPoint.x()+theTangent.x(),
			      endPoint.y()+theTangent.y(),
			      endPoint.z()+theTangent.z() );

		  glEnd();
		}

	      // draw the normals associated with the points
	      if( draw_featureNormals_ )
		{
		  glLineWidth( 1.0 );
		  glBegin( GL_LINES );
		  for(unsigned int j=0; j<finalPolylines_[i].getNumPoints(); j++)
		    {
		      glColor3f(0,1,0);
		      glVertex3f( finalPolylines_[i].getFeaturePoint(j).x(), finalPolylines_[i].getFeaturePoint(j).y(), finalPolylines_[i].getFeaturePoint(j).z() );
		      glVertex3f(finalPolylines_[i].getFeaturePoint(j).x()+finalPolylines_[i].getFeatureNormal(j,0).x()*0.5,
				 finalPolylines_[i].getFeaturePoint(j).y()+finalPolylines_[i].getFeatureNormal(j,0).y()*0.5,
				 finalPolylines_[i].getFeaturePoint(j).z()+finalPolylines_[i].getFeatureNormal(j,0).z()*0.5);
		      
		      glColor3f(0,1,1);
		      glVertex3f( finalPolylines_[i].getFeaturePoint(j).x(), finalPolylines_[i].getFeaturePoint(j).y(), finalPolylines_[i].getFeaturePoint(j).z() );
		      glVertex3f(finalPolylines_[i].getFeaturePoint(j).x()+finalPolylines_[i].getFeatureNormal(j,1).x()*0.5,
				 finalPolylines_[i].getFeaturePoint(j).y()+finalPolylines_[i].getFeatureNormal(j,1).y()*0.5,
				 finalPolylines_[i].getFeaturePoint(j).z()+finalPolylines_[i].getFeatureNormal(j,1).z()*0.5);
		    }
		  glEnd();
		}
	    }
	}
    }

  // draw the FLF stuff
  if (draw_FLFstuff_)
    {
      // draw the feature loops
      for(unsigned int i=0; i<featList_.size(); i++)
	{
	  // draw all loops or just the i'th loop
	  if (currentLoop_ == featList_.size() || currentLoop_ == i)
	    {
	      // loop through drawing the points -- yellow
	      glPointSize( thePointSize_ );
	      glBegin( GL_POINTS );
	      for(unsigned int j=0; j<featList_[i].size(); j++)
		{
		  if (ptList_[ featList_[i][j].index_ ].isCorner_) glColor3f(0,1,1);
		  else glColor3f(1,1,0);
		  glVertex3f( ptList_[ featList_[i][j].index_ ].x_,
			      ptList_[ featList_[i][j].index_ ].y_,
			      ptList_[ featList_[i][j].index_ ].z_ );
		}
	      glEnd();

	      // loop through again drawing normals and line segments
	      glLineWidth( 1.0 );
	      glBegin( GL_LINES );
	      for(unsigned int j=0; j<featList_[i].size(); j++)
		{
		  // if not first complete line segment
		  if (j!=0)
		    glVertex3f( ptList_[ featList_[i][j].index_ ].x_,
				ptList_[ featList_[i][j].index_ ].y_,
				ptList_[ featList_[i][j].index_ ].z_ );

		  // draw the normal -- yellow -> red
		  glColor3f(1, ((float)(featList_[i].size()-j))/(float)(featList_[i].size()), 0);
		  glVertex3f( ptList_[ featList_[i][j].index_ ].x_,
			      ptList_[ featList_[i][j].index_ ].y_,
			      ptList_[ featList_[i][j].index_ ].z_ );
		  glVertex3f( ptList_[ featList_[i][j].index_ ].x_ + featList_[i][j].x_,
			      ptList_[ featList_[i][j].index_ ].y_ + featList_[i][j].y_,
			      ptList_[ featList_[i][j].index_ ].z_ + featList_[i][j].z_ );

		  // if not last start next line segment -- red
		  if (j!=featList_[i].size()-1)
		    {
		      glColor3f(1,0,0);
		      glVertex3f( ptList_[ featList_[i][j].index_ ].x_,
				  ptList_[ featList_[i][j].index_ ].y_,
				  ptList_[ featList_[i][j].index_ ].z_ );
		    }
		}
	      glEnd();
	    }
	}
    }
}

/**
 * draws the scene in a noise tolerance mode so that we can
 * differentiate when the user is trying to select a noise tolerance
 * so that they know things are still waiting for their input.
 */
void drawNoiseToleranceMode()
{
  glClearColor(0.5,0.5,0.5, 0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // projection transform
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 1, 1000);

  // camera transform
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // get the information from the camera
  Point eye = camera_.get_eye();
  Point look = camera_.get_lookat();
  Vector up = camera_.get_up();

  // setup the window information
  gluLookAt( eye.x(), eye.y(), eye.z(),
	     look.x(), look.y(), look.z(),
 	     up.x(), up.y(), up.z() );

  // disable lighting computations
  glDisable(GL_LIGHTING);

  // draw the current point 
  glPointSize( thePointSize_+1 );
  glBegin( GL_POINTS );
  glColor3f( 0,0,1 );
  glVertex3f( originalCloud_->vertices[ noiseTolerancePointOffset_ ].x(),
	      originalCloud_->vertices[ noiseTolerancePointOffset_ ].y(),
	      originalCloud_->vertices[ noiseTolerancePointOffset_ ].z() );

  // draw the neighbor points
  glColor3f( 1,0,0 );
  std::vector< Point > neighbors = originalCloud_->kNN( originalCloud_->vertices[ noiseTolerancePointOffset_ ], POTENTIAL_NEIGHBOR_MAX );
  for(unsigned int i=0; i<neighbors.size(); i++)
    {
      glVertex3f( neighbors[i].x(), neighbors[i].y(), neighbors[i].z() );
    }
  glEnd();

  // draw the fit surface
  glPointSize( 1 );
  glBegin( GL_POINTS );
  HeightSurface surface = mls_fit_surface( originalCloud_->vertices[ noiseTolerancePointOffset_ ],neighbors );
  for(unsigned int i=0; i<100; i++)
    {
      float u = ((float)(i)-50)/50.0 * 2.5;
      for(unsigned int j=0; j<100; j++)
	{
	  float v = ((float)(j)-50)/50.0 * 2.5;
	  glColor3f( u/2.5, 1, v/2.5);
	  Point eval = surface.evaluateSurface(u,v);
	  glVertex3f( eval.x(),eval.y(),eval.z() );
	}
    }
  glEnd();

  // draw the point cloud
  if (draw_pointCloud_ && originalCloud_)
    {
      for (unsigned int j=0;j<originalCloud_->vertices.size();j++)
	{
	  glPointSize( thePointSize_ );
	  glBegin( GL_POINTS );
	  glColor3f(1,1,1);
	  glVertex3f(originalCloud_->vertices[j].x(), originalCloud_->vertices[j].y(),originalCloud_->vertices[j].z());
	  glEnd();
	}
    }

  // update the noise tolerance
  float tmpTolerance = surface.maxResidual( neighbors );
  if ( !EQL(tmpTolerance,noiseTolerance_) ) 
    {
      noiseTolerance_ = tmpTolerance;
      if ( EQL(noiseTolerance_,0.0) ) noiseTolerance_ += 0.001;
      fprintf(stderr," *- updating noise tolerance to %g -*\n",noiseTolerance_);
    }
}

/**
 * writes out the ppm file
 */
void writePPMfile(const char* filename)
{
  // make sure we open the file for writing
  std::ofstream out;
  out.open(filename,std::ios::binary);
  if (!out)
    {
      fprintf(stderr,"[ERROR] writePPMfile() - unable to open file: %s\n",filename);
      return;
    }

  // draw the image to the ppm window
  glutSetWindow( ppm_window_ );  
  if (draw_noiseToleranceMODE_) drawNoiseToleranceMode();
  else drawNormalMode();
  glutSwapBuffers();

  // read that HUGE image to the memory
  GLubyte* pixels = new GLubyte [ 3*PIXELS_WIDE*PIXELS_HIGH ];
  glReadBuffer( GL_FRONT );
  glReadPixels( 0,0, PIXELS_WIDE,PIXELS_HIGH, GL_RGB,GL_UNSIGNED_BYTE, pixels);

  // output the image in the ppm file format
  out << "P6 " << PIXELS_WIDE << " " << PIXELS_HIGH << " 255\n";
  out.write(reinterpret_cast<char*>(pixels), 3*PIXELS_WIDE*PIXELS_HIGH);	

  // free the memory and reset the viewport to the original size
  delete []pixels;
  glutSetWindow( main_window_ );

  // close the file
  out.close();
}

/**
 * glut display -- draw a scene within the glut window.
 */
void myGlutDisplay(void )
{
  if (draw_noiseToleranceMODE_) drawNoiseToleranceMode();
  else drawNormalMode();
  glutSwapBuffers(); 
}

/**
 * main -- the programs entry point to allow the programmer to
 * setup the gl window and callback functions, then control the
 * course of the program.
 */
int main( int argc, char* argv[])
{
  // print out an introduction
  fprintf(stderr," -- point cloud feature extraction --\n");
  fprintf(stderr," this program extracts the feature points from a point cloud\n");
  fprintf(stderr," and displays smooth curves that describe these edges.\n");
  fprintf(stderr," -- may 2006 --\n");

  // parse the command line
  parse_command_line( argc,argv );

  // initialize the environment
  init_environment();

  // create the ppm file window
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(PIXELS_WIDE,PIXELS_HIGH);
  glutInitWindowPosition(100,100);
  ppm_window_ = glutCreateWindow("PPM output window");

  // create the glut window
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(camera_.window_width(), camera_.window_height());
  glutInitWindowPosition(100,100);
  main_window_ = glutCreateWindow("Features Edges");
  glutSetWindow(main_window_);

  // set callbacks
  glutDisplayFunc(myGlutDisplay);
  GLUI_Master.set_glutReshapeFunc(myGlutReshape);
  GLUI_Master.set_glutIdleFunc(myGlutIdle);
  GLUI_Master.set_glutKeyboardFunc(myGlutKeyboard);
  GLUI_Master.set_glutMouseFunc(myGlutMouse);
  glutMotionFunc(myGlutMotion);

  // initialize gl
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);

  // give control over to glut
  glutMainLoop();

  return 0;
}
