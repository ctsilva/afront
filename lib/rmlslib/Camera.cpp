/**
 *
 * file:	Camera.cpp
 * description:	Stores and computes common operations for a 
 *		camera object.  Includes the location, lookat, up,
 *		height and width of the film.  Allows for operations
 *		including zooms, rotate, and pans.  
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "Camera.h"

/**
 * print out the camera information
 */
void
Camera::print() const
{
  fprintf(stderr," -- camera information --\n");
  fprintf(stderr," eye = (%g,%g,%g)\n",eye_.x(),eye_.y(),eye_.z());
  fprintf(stderr," orient.u = (%g,%g,%g)\n",orientation_.u().x(),orientation_.u().y(),orientation_.u().z());
  fprintf(stderr," orient.v = (%g,%g,%g)\n",orientation_.v().x(),orientation_.v().y(),orientation_.v().z());
  fprintf(stderr," orient.w = (%g,%g,%g)\n",orientation_.w().x(),orientation_.w().y(),orientation_.w().z());
  fprintf(stderr,"\n");
  fprintf(stderr," right vector = (%g,%g,%g)\n",get_right().x(),get_right().y(),get_right().z());
  fprintf(stderr," up vector = (%g,%g,%g)\n",get_up().x(),get_up().y(),get_up().z());
  fprintf(stderr," lookat pt = (%g,%g,%g)\n",get_lookat().x(),get_lookat().y(),get_lookat().z());
  fprintf(stderr," -- done information dump --\n");
}

/**
 * initialize -- sets up the camera information with the given attributes.
 */
void
Camera::initialize(const Point &eye, const Vector &vLook, const Vector &vUp,
		   const float &focal_length, const float &width, const float &height,
		   const int &window_width, const int &window_height )
{
  // straight copies into variables
  focal_length_ = focal_length;
  width_ = width;
  height_ = height;
  windowX_ = window_width;
  windowY_ = window_height;

  // setup the orientation information and the eye location
  orientation_.initFromWV( vLook, vUp );
  OrthoNormalBasis world( Vector(1,0,0), Vector(0,1,0), Vector(0,0,1) );
  eye_ = eye;//.changeBasis( world,orientation_ );
}

/**
 * zoom -- moves the eye towards the lookat point as close as it
 * can get it, then it will start to move the eye and the lookat 
 * point together if they are too close.
 */
void		
Camera::zoom( const int &dy )
{
  // get the right, up and look vectors
  Vector vLook = Vector( orientation_.w().dot_product( Vector(1,0,0) ),
			 orientation_.w().dot_product( Vector(0,1,0) ),
			 orientation_.w().dot_product( Vector(0,0,1) ) );

  // scale the change by how far away we are
  float scale = sqrt(focal_length_)*dy*0.03;

  // move the eye
  eye_ += vLook*scale;
}

/**
 * rotate -- moves the camera about the lookat point maintaining the
 * distance and the up and right vectors.
 */
void		
Camera::rotate( const float &theta, const float &phi )
{
    Matrix_MxN R1(4,4),R2(4,4),R(4,4),M1(4,4),M2(4,4),M(4,4);
    R1.makeIdentity();
    R2.makeIdentity();
    M1.makeIdentity();
    M2.makeIdentity();

    R1.rotate( phi, this->get_right() );
    R2.rotate( -theta, this->get_up() );
    R = R1 * R2;

    M1.translate( this->get_lookat()-eye_ );
    M2.translate( -(this->get_lookat()-eye_) );
    M = M1 * R * M2;
    
    eye_ = (M * Matrix_MxN(eye_)).makePoint();
    Vector up = (R * Matrix_MxN(this->get_up())).makeVector();
    Vector right = (R * Matrix_MxN(this->get_right())).makeVector();

    orientation_.initFromUV( right,up );

//   // get the look at point
//   Point lookat = this->get_lookat();
//   Vector neye = eye_ - lookat;

//   // rotate in the xz plane first
//   Vector neye2( (float)cos(theta)*neye.x() + (float)sin(theta)*neye.z(),
// 		neye.y(),
// 		-(float)sin(theta)*neye.x() + (float)cos(theta)*neye.z() );

//   // get the perpendicular up vector from this new direction
//   Vector f = -neye2;
//   Vector vUp(0,1,0);
//   if (f.unit_vector().dot_product(vUp) > .9999)
//     vUp = Vector(1,0,0);
//   Vector vRight = f.cross_product( vUp );
//   vUp = vRight.cross_product( f );

//   float len = f.length();
//   f.normalize();
//   vUp.normalize();

//   // now rotate vertically (maintaining the distance to the lookat point)
//   neye = (f*(float)cos(phi) + vUp*(float)sin(phi))*len;

//   // set the eye
//   eye_ = lookat - neye;

//   // recompute the orientation
//   Vector vLook = lookat - eye_;

//   // recompute the up vector
//   vUp = Vector(0,1,0);
//   vRight = f.cross_product( vUp );
//   vUp = vRight.cross_product( f );

//   orientation_.initFromWV( vLook, vUp );
}

/**
 * pan - this translates the eye and the lookat perpendicular to the
 * image plane, on the plane defined by the up_ and right_ vectors.
 */
void		
Camera::pan( const int &dx, const int &dy )
{
  // get the right, up and look vectors
  Vector vRight = Vector( orientation_.u().dot_product( Vector(1,0,0) ),
			  orientation_.u().dot_product( Vector(0,1,0) ),
			  orientation_.u().dot_product( Vector(0,0,1) ) );
  Vector vUp = Vector( orientation_.v().dot_product( Vector(1,0,0) ),
		       orientation_.v().dot_product( Vector(0,1,0) ),
		       orientation_.v().dot_product( Vector(0,0,1) ) );

  // scale the change by how far away we are
  float scale = sqrt(focal_length_)*0.007;

  // move the eye
  eye_ += -vRight*dx*scale + vUp*dy*scale;
}

/**
 * pixel_ray -- this will translate the pixel coordinates given (0,0) is
 * located in the upper left hand corner of the window, into a vector 
 * within the world.  To make it into a ray, obtain the origin by querying
 * the camera for the eye location.
 */
Vector	
Camera::pixel_ray( const int &x, const int &y ) const
{
  // compute the halfway pixel (center of the image plane)
  float halfX = (float)(windowX_) / 2.0;
  float halfY = (float)(windowY_) / 2.0;

  // compute the pixel (x,y) percent up and across image plane
  float percentX = ((float)(x)-halfX)/((float)(windowX_)-halfX);
  float percentY = (halfY-(float)(y))/((float)(windowY_)-halfY);

  // compute the distance for the percentage
  float distanceX = percentX * width_;
  float distanceY = percentY * height_;

  // create the vector
  Vector ray( distanceX, distanceY, focal_length_ );
  OrthoNormalBasis world( Vector(1,0,0), Vector(0,1,0), Vector(0,0,1) );
  return ray.changeBasis( world,orientation_ );
}
