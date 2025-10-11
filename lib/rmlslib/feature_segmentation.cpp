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
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
#include <queue>
#include <stdio.h>

#include "Camera.h"
#include "Point.h"
#include "Vector.h"
#include "Primitive_Functions.h"
#include "Global_Defs.h"
#include "PointCloud.h"
#include "TriMesh.h"
#include "FeatureEdge.h"

// include functions for each of the stages
#include "Stage1.h"
#include "Stage2.h"
#include "Stage3.h"
#include "Stage4.h"
#include "Stage5.h"
#include "Stage6.h"

// -- GLOBAL VARIABLES -- //
Camera camera_;
int main_window_;
int current_button_ = -1;
int lastX_;
int lastY_;

// assorted clouds and feature polyline stages
PointCloud* pointCloud_;
std::vector< FeatureEdge > featureLines_;
std::vector< PointCloud* > segmentedPoints_;
std::vector< std::vector<Vector> > segmentedNormals_;

// draw flags to allow user to view different items
bool draw_pointCloud_=true;
bool draw_pointWeights_=false;
bool draw_featureLines_=false;
bool draw_featureTangents_=false;
bool draw_segmentedPoints_=false;
bool draw_segmentedNormals_=false;
bool draw_laplacianColors_=false;

// user controls for the point cloud
float noiseTolerance_ = 0.01;
unsigned int thePointSize_=1;
float noiseScale_ = 0.0;
unsigned int numSamples_=200000;

// the laplacian color coding variables
float minLaplacian_ = 0;
float maxLaplacian_ = 1;

/**
 * write out the segmentation file
 */
void writeSegmentationFile( const std::string &filename )
{
  FILE *output = fopen( filename.c_str(),"w" );
  if (!output) 
    {
      fprintf(stderr," [ERROR] writeSegmentationFile() - couldn't open file\n");
      return;
    }

  fprintf(output,"%d # number of segmented groups\n",segmentedPoints_.size());
  for(unsigned int i=0; i<segmentedPoints_.size(); i++)
    {
      fprintf(output,"%d # number of points in group[%d]\n",segmentedPoints_[i]->vertices.size(),i);
      for(unsigned int j=0; j<segmentedPoints_[i]->vertices.size(); j++)
	fprintf(output,"%g %g %g %g %g %g\n",
		segmentedPoints_[i]->vertices[j].x(),
		segmentedPoints_[i]->vertices[j].y(),
		segmentedPoints_[i]->vertices[j].z(),
		segmentedPoints_[i]->verticesNormals[j][0].x(),
		segmentedPoints_[i]->verticesNormals[j][0].y(),
		segmentedPoints_[i]->verticesNormals[j][0].z());
    }
  fclose(output);
}

/**
 * sets the color for the point based on the given float weight.  the 
 * function will range the colors between blue-green-yellow-orange-red
 * based on the min and max laplacian values and the given weight.
 */
void setLaplacianColor( const float &weight )
{
  // compute the percent between the laplacian range
  float value = (weight-minLaplacian_)/(maxLaplacian_-minLaplacian_);
  
  // color between blue and cyan
  if (value <= .25)
    glColor3f( 0, weight/.25, 1 );
  else if (value <= .5)
    glColor3f( 0, 1, 1-(weight-.25)/.25 );
  else if (value <= .75)
    glColor3f( (weight-.5)/.25, 1, 0 );
  else
    glColor3f( 1, 1-(weight-.75)/.25, 0 );
}

/**
 * find the min and maximum laplacian values in the segmented points.
 */
void laplacianRangeFinder()
{
  // initialize to some bogus numbers
  minLaplacian_ = 100.0;
  maxLaplacian_ = -100.0;

  // loop through getting the largest and smallest values
  for(unsigned int i=0; i<segmentedPoints_.size(); i++)
    {
      for(unsigned int j=0; j<segmentedPoints_[i]->verticesWeights.size(); j++)
	{
	  if (minLaplacian_ > segmentedPoints_[i]->verticesWeights[j])
	    minLaplacian_ = segmentedPoints_[i]->verticesWeights[j];
	  if (maxLaplacian_ < segmentedPoints_[i]->verticesWeights[j])
	    maxLaplacian_ = segmentedPoints_[i]->verticesWeights[j];
	}
    }

  // make sure that it made sense
  if (minLaplacian_ > maxLaplacian_)
    {
      float tmp = minLaplacian_;
      minLaplacian_ = maxLaplacian_;
      maxLaplacian_ = tmp;
    }

  // print out the range information
  fprintf(stderr,"min(%g)-max(%g)...",minLaplacian_,maxLaplacian_);
}

/**
 * sets the color for the segmented group of points. will try to
 * change colors as much as possible before repeating them to reduce
 * the chance to segmented groups are next to each other with the same
 * color.
 */
void setSegmentedGroupColor( const unsigned int &i )
{
  // switch on i
  switch( i%25 )
    {
    case 0:     glColor3f(1,1,1); break;	// white
    case 1:	glColor3f(.8,.8,.8); break;	// light grey
    case 2:	glColor3f(.5,.5,.5); break;	// dark grey
    case 3:	glColor3f(.1,.1,.1); break;	// blackish
    case 4:	glColor3f(.3,0,0); break;	// dark red
    case 5:	glColor3f(.7,0,0); break;	// dark red
    case 6:	glColor3f(1,0,0); break;	// red
    case 7:	glColor3f(1,.5,.5); break;	// pink
    case 8:	glColor3f(1,.7,0); break;	// orange
    case 9:	glColor3f(.5,.5,0); break;	// dark yellow
    case 10:	glColor3f(1,1,0); break;	// yellow
    case 11:	glColor3f(.7,1,0); break;	// yellow green
    case 12:	glColor3f(0,.3,0); break;	// dark green
    case 13:	glColor3f(0,.7,0); break;	// dark green
    case 14:	glColor3f(0,1,0); break;	// green
    case 15:	glColor3f(.3,1,.3); break;	// light green
    case 16:	glColor3f(.8,1,.8); break;	// light green
    case 17:	glColor3f(0,.5,.5); break;	// cyan
    case 18:	glColor3f(0,1,1); break;	// cyan
    case 19:	glColor3f(0,0,.3); break;	// dark blue
    case 20:	glColor3f(0,0,.7); break;	// dark blue
    case 21:	glColor3f(0,0,1); break;	// blue
    case 22:	glColor3f(.5,.5,1); break;	// light blue
    case 23:	glColor3f(.5,0,1); break;	// purple
    case 24:	glColor3f(1,0,1); break;	// magenta
    default:	glColor3f(1,1,1); break;	// white
    }
}

/**
 * create shape toggles between the different shape types
 */
void createShape(const ShapeType &shapeType_)
{
  fprintf(stderr," (*) creating the shape...");
  pointCloud_ = new PointCloud( shapeType_,numSamples_,noiseScale_ );
  fprintf(stderr,"done\n");
}

/**
 * read in the feature file and save the feature edges
 */
void readFeatureFile( char *filename )
{
  fprintf(stderr," (*) reading feature file '%s'...",filename);

  // clear the features
  featureLines_.clear();

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
      featureLines_.push_back( FeatureEdge(points,normals1,normals2) );
    }
  in.close();
  fprintf(stderr,"done reading file (%d features) -*\n",featureLines_.size());
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
  // load some initial shape and fef file
  createShape( Ra );
  readFeatureFile( "data/ra.fef" );

  // initialize the camera
  float init_focus = 10;
  float init_width = 80;
  float init_height = 60;
  int init_window_width = 800;
  int init_window_height = 600; 

  // compute the initial lookat and eye
  Point center;
  float radius;
  pointCloud_->get_bound_sphere( center, radius );
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

  switch(key)
    {
      // help menu
    case 'h':
    case 'H':
      fprintf(stderr," ---- HELP MENU ----\n");
      fprintf(stderr," '1'    load an .off file into the window\n");
      fprintf(stderr," '2'    load a .txt file into the window\n");
      fprintf(stderr," '3'    load an .obj file into the window\n");
      fprintf(stderr," '4'    change shape to a basic cube\n");
      fprintf(stderr," '5'    change shape to a basic trapezoid\n");
      fprintf(stderr," '6'    change shape to a basic icosahedron\n");
      fprintf(stderr," '7'    change shape to a basic semisphere\n");
      fprintf(stderr," '8'    change shape to a basic cylinder\n");
      fprintf(stderr," '9'    change shape to a basic two intersecting spheres\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'W'    fine tune the noise tolerance up\n");
      fprintf(stderr," 'w'    fine tune the noise tolerance down\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'V'    increases the size of the vertex drawn\n");
      fprintf(stderr," 'v'    decreases the size of the vertex drawn\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'x'    computes the potential cloud to give an idea of the noise tolerance\n");
      fprintf(stderr," 'X'    segment the points of the cloud with given features\n");
      fprintf(stderr,"\n");
      fprintf(stderr," 'c'    toggle drawing the point 'C'loud\n");
      fprintf(stderr," 'p'    toggle drawing the point weights for the cloud\n");
      fprintf(stderr," 'i'    toggle drawing the feature polylines\n");
      fprintf(stderr," 'u'    toggle drawing the feature tangents\n");
      fprintf(stderr," 'o'    toggle drawing the segmented point regions\n");
      fprintf(stderr," 't'    toggle drawing the segmented normals\n");
      fprintf(stderr," 'l'    toggle drawing the 'L'aplacian color information\n");
      fprintf(stderr,"\n"); 
      fprintf(stderr," 'd'    dump out a file of the segmented points (.seg)\n");
      break;

      // -------------------------------- //
    case 'd':
      fprintf(stderr," *- write .seg file: ");
      std::cin>> filename;
      writeSegmentationFile( filename );
      break;

      // -------------------------------------------------- //
      // change shape 
    case 49:
      fprintf(stderr," *- load .off file: ");
      std::cin >> filename;
      fprintf(stderr," (*) ");
      pointCloud_->loadOFFfile( filename,numSamples_,noiseScale_ );
      fprintf(stderr,"\n *- load .fef file: ");
      std::cin >> filename;
      readFeatureFile( filename );
      break;
    case 50:
      fprintf(stderr," *- load .txt file: ");
      std::cin >> filename;
      fprintf(stderr," (*) ");
      pointCloud_->loadTXTfile( filename,numSamples_,noiseScale_ );
      fprintf(stderr,"\n *- load .fef file: ");
      std::cin >> filename;
      readFeatureFile( filename );
      break;
    case 51:
      fprintf(stderr," *- load .obj file: NOT SUPPORTED...");
      fprintf(stderr,"done\n");
      break;
    case 52:
      fprintf(stderr," *- switching model to a CUBE -*\n");
      createShape(Cube);
      fprintf(stderr," *- load data/cube.fef file -*\n");
      readFeatureFile( "data/cube.fef" );
      break;
    case 53:
      fprintf(stderr," *- switching model to a TRAPEZOID -*\n");
      createShape(Trapezoid);
      fprintf(stderr," *- load data/trapezoid.fef file -*\n");
      readFeatureFile( "data/trapezoid.fef" );
      break;
    case 54:
      fprintf(stderr," *- switching model to a Icosahedron -*\n");
      createShape(Icosahedron);
      fprintf(stderr," *- load data/icosahedron.fef file -*\n");
      readFeatureFile( "data/icosahedron.fef" );
      break;
    case 55:
      fprintf(stderr," *- switching model to the Semi Sphere -*\n");
      createShape(SemiSphere);
      fprintf(stderr," *- load data/semiSphere.fef file -*\n");
      readFeatureFile( "data/semiSphere.fef" );
      break;
    case 56:
      fprintf(stderr," *- switching model to the Cylinder -*\n");
      createShape(Cylinder);
      fprintf(stderr," *- load data/cylinder.fef file -*\n");
      readFeatureFile( "data/cylinder.fef" );
      break;
    case 57:
      fprintf(stderr," *- switching model to the Two Intersecting Spheres -*\n");
      createShape(TwoSphere);
      fprintf(stderr," *- load data/intersectSpheres.fef file -*\n");
      readFeatureFile( "data/intersectSpheres.fef" );
      break;

      // -------------------------------------------------- //
      // decrease the noise tolerance
    case 'w':
      noiseTolerance_-=0.001;
      if (noiseTolerance_<0)
	{
	  noiseTolerance_ = 0;
	  break;
	}
      fprintf(stderr," *- noise tolerance = %g -*\n",noiseTolerance_);
      if (pointCloud_->vertices.size() != pointCloud_->verticesWeights.size())
	fprintf(stderr," *- type 'x' to see noise weights for points -*\n");
      break;
      // increase the noise tolerance
    case 'W':
      noiseTolerance_+=0.001;
      if (noiseTolerance_>0.25)
	{ 
	  noiseTolerance_ = 0.25;
	  break;
	}
      fprintf(stderr," *- noise tolerance = %g -*\n",noiseTolerance_);
      if (pointCloud_->vertices.size() != pointCloud_->verticesWeights.size())
	fprintf(stderr," *- type 'x' to see noise weights for points -*\n");
      break;

      // -------------------------------------------------- //
      // decrease the noise of the cube
    case 'n':
      noiseScale_-=0.01;
      if (noiseScale_<=0)
	{
	  noiseScale_ = 0.01;
	  break;
	}
      fprintf(stderr," *- noise = %g (type 'x' to recompute features) -*\n",noiseScale_);
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
      // toggle the drawing of feature points only
    case 'p':
    case 'P':
      if ( draw_pointWeights_ = !draw_pointWeights_ )
	fprintf(stderr," *- drawing the point cloud weights: enable -*\n");
      else
	fprintf(stderr," *- drawing the point cloud weights: disable -*\n");
      break;
      // toggle the drawing of the feature polylines
    case 'i':
    case 'I':
      if ( draw_featureLines_ = !draw_featureLines_ )
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
      // toggle the drawing of the segmented points
    case 'o':
    case 'O':
      if( draw_segmentedPoints_ = !draw_segmentedPoints_ )
	fprintf(stderr," *- drawing the segmented point groups: enable -*\n");
      else
	fprintf(stderr," *- drawing the segmented point groups: disable -*\n");
      break;
      // toggle the drawing of the segmented normals
    case 't':
    case 'T':
     if ( draw_segmentedNormals_ = !draw_segmentedNormals_ )
	fprintf(stderr," *- drawing the segmented normals: enable -*\n");
      else
	fprintf(stderr," *- drawing the segmented normals: disable -*\n");
      break;
      // toggle the drawing of the segmented points with laplacian information
    case 'l':
    case 'L':
      if ( draw_laplacianColors_ = !draw_laplacianColors_ )
	fprintf(stderr," *- drawing the laplacian colored information: enable -*\n");
      else
	fprintf(stderr," *- drawing the laplacian colored information: disable -*\n");
      break;

      // -------------------------------------------------- //
      // recreate the feature lines
    case 'x':
      if (pointCloud_)
	{
	  if (pointCloud_->verticesWeights.size() != pointCloud_->vertices.size()) 
	    {
	      fprintf(stderr," *- computing weights for noise tolerance tuning (%d points) -*\n",pointCloud_->vertices.size());
	      helper1_computePointWeights( pointCloud_,POTENTIAL_NEIGHBOR_MAX );
	      fprintf(stderr," *- done computing noise tolerance weights...type 'X' to complete feature segmentation -*\n");
	    }
	}
      else
	{
	  fprintf(stderr," *- no cloud available for feature segmentation -*\n");
	}
      break;
    case 'X':
      if (pointCloud_)
	{
	  fprintf(stderr," *- segmenting the original point cloud -*\n");
	  segmentedPoints_ = stage6_segmentPointCloud( pointCloud_,featureLines_, noiseTolerance_);
	  fprintf(stderr," *- done segmentation of the point cloud -*\n");
	  fprintf(stderr," *- evaluating min and max laplacian values...");
	  laplacianRangeFinder();
	  fprintf(stderr,"done -*\n");
	}
      else
	{
	  fprintf(stderr," *- no cloud available for feature segmentation -*\n");
	}
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
 * glut display -- draw a scene within the glut window.
 */
void myGlutDisplay(void )
{
  glClearColor(0,0,0, 0);
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

  // add a light just above and right of the viewer
//   GLfloat position[] = {5.0,5.0,0.0,1.0};
//   GLfloat ambient[] = {0.0,0.0,0.0,1.0};
//   GLfloat diffuse[] = {1.0,1.0,1.0,1.0};
//   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
//   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//   glLightfv(GL_LIGHT0, GL_POSITION, position);
//   glEnable( GL_LIGHTING );
//   glEnable( GL_LIGHT0 );

  // setup the window information
  gluLookAt( eye.x(), eye.y(), eye.z(),
	     look.x(), look.y(), look.z(),
 	     up.x(), up.y(), up.z() );

  // disable lighting computations
  glDisable(GL_LIGHTING);

  // draw the point cloud weights
  if (draw_pointWeights_ && pointCloud_)
    {
      for (unsigned int j=0;j<pointCloud_->vertices.size() && j<pointCloud_->verticesWeights.size();j++)
	{
	  if (pointCloud_->verticesWeights[j] >= noiseTolerance_ )
	    {
	      glPointSize( thePointSize_ );
	      glBegin( GL_POINTS );
	      glColor3f(1,0,0);
	      glVertex3f(pointCloud_->vertices[j].x(), pointCloud_->vertices[j].y(),pointCloud_->vertices[j].z());
	      glEnd();
	    }
	}
    }

  // draw the point cloud
  if (draw_pointCloud_ && pointCloud_)
    {
      for (unsigned int j=0;j<pointCloud_->vertices.size();j++)
	{
	  glPointSize( thePointSize_ );
	  glBegin( GL_POINTS );
	  glColor3f(1,1,1);
	  glVertex3f(pointCloud_->vertices[j].x(), pointCloud_->vertices[j].y(),pointCloud_->vertices[j].z());
	  glEnd();
	}
    }

  // draw the segmented point groups
  if (draw_segmentedPoints_)
    {
      // draw the segmented points in the assorted colors
      glPointSize( thePointSize_ );
      glBegin( GL_POINTS );
      // loop through each group
      for(unsigned int i=0; i<segmentedPoints_.size(); i++)
	{
	  // set the color for the group
	  setSegmentedGroupColor(i);
	  for(unsigned int j=0; j<segmentedPoints_[i]->vertices.size(); j++)
	    {
	      glVertex3f(segmentedPoints_[i]->vertices[j].x(), segmentedPoints_[i]->vertices[j].y(),segmentedPoints_[i]->vertices[j].z());
	    }
	}
      glEnd();
    }

  // draw the affiliated normals in the same color
  if (draw_segmentedNormals_)
    {
      glLineWidth( 1.0 );
      glBegin( GL_LINES );
      for(unsigned int i=0; i<segmentedPoints_.size(); i++)
	{
	  // set the color for the group
	  setSegmentedGroupColor(i);
	  for(unsigned int j=0; j<segmentedPoints_[i]->vertices.size(); j++)
	    {
	      if (segmentedPoints_[i]->verticesNormals[j].size()!=0)
		{
		  glVertex3f(segmentedPoints_[i]->vertices[j].x(), 
			     segmentedPoints_[i]->vertices[j].y(),
			     segmentedPoints_[i]->vertices[j].z());
		  glVertex3f(segmentedPoints_[i]->vertices[j].x() + segmentedPoints_[i]->verticesNormals[j][0].x()*0.2, 
			     segmentedPoints_[i]->vertices[j].y() + segmentedPoints_[i]->verticesNormals[j][0].y()*0.2,
			     segmentedPoints_[i]->vertices[j].z() + segmentedPoints_[i]->verticesNormals[j][0].z()*0.2);
		}
	    }
	}
      glEnd();
    }

  // draw the laplacian colored information
  if (draw_laplacianColors_)
    {
      // draw the segmented points in the assorted colors
      glPointSize( thePointSize_ );
      glBegin( GL_POINTS );
      // loop through each group
      for(unsigned int i=0; i<segmentedPoints_.size(); i++)
	{
	  for(unsigned int j=0; j<segmentedPoints_[i]->vertices.size(); j++)
	    {
	      // set the laplacian color
	      setLaplacianColor(segmentedPoints_[i]->verticesWeights[j]);
	      glVertex3f(segmentedPoints_[i]->vertices[j].x(), segmentedPoints_[i]->vertices[j].y(),segmentedPoints_[i]->vertices[j].z());
	    }
	}
      glEnd();
    }

  // draw the feature polylines
  if (draw_featureLines_)
    {
      // draw the features
      for(unsigned int i=0; i<featureLines_.size(); i++)
	{
	  // draw the polyline as red lines
	  glLineWidth( 2.0 );
	  glBegin(GL_LINE_STRIP);
	  glColor3f(0,0,1);
	  for(unsigned int j=0; j<featureLines_[i].getNumPoints(); j++)
	    {
	      glVertex3f( featureLines_[i].getFeaturePoint(j).x(), featureLines_[i].getFeaturePoint(j).y(), featureLines_[i].getFeaturePoint(j).z() );
	    }
	  glEnd();

	  // draw the intermediate points as yellow
	  glPointSize( thePointSize_ );
	  glBegin(GL_POINTS);
	  glColor3f(0,1,1);
	  for(unsigned int j=1; j<featureLines_[i].getNumPoints()-1; j++)
	    {
	      glVertex3f( featureLines_[i].getFeaturePoint(j).x(), featureLines_[i].getFeaturePoint(j).y(), featureLines_[i].getFeaturePoint(j).z() );
	    }
	  glEnd();

	  // draw the endpoints larger and orange
	  glPointSize( thePointSize_+1 );
	  glBegin(GL_POINTS);
	  glColor3f(0,1,0);
	  glVertex3f( featureLines_[i].getStartPoint().x(), featureLines_[i].getStartPoint().y(), featureLines_[i].getStartPoint().z() );
	  glVertex3f( featureLines_[i].getEndPoint().x(), featureLines_[i].getEndPoint().y(), featureLines_[i].getEndPoint().z() );
	  glEnd();

	  // if draw the tangents 
	  if (draw_featureTangents_)
	    {
	      glLineWidth( 1.0 );
	      glBegin(GL_LINES);
	      glColor3f(0,1,0);

	      Point endPoint = featureLines_[i].getStartPoint();
	      Vector theTangent = featureLines_[i].getTangentStart();
	      glVertex3f( endPoint.x(), endPoint.y(), endPoint.z() );
	      glVertex3f( endPoint.x()+theTangent.x(),
			  endPoint.y()+theTangent.y(),
			  endPoint.z()+theTangent.z() );

	      endPoint = featureLines_[i].getEndPoint();
	      theTangent = featureLines_[i].getTangentEnd();
	      glVertex3f( endPoint.x(), endPoint.y(), endPoint.z() );
	      glVertex3f( endPoint.x()+theTangent.x(),
			  endPoint.y()+theTangent.y(),
			  endPoint.z()+theTangent.z() );

	      glEnd();
	    }
	}
    }

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

  // create the glut window
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(camera_.window_width(), camera_.window_height());
  glutInitWindowPosition(100,100);
  main_window_ = glutCreateWindow("Features Edges");

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
