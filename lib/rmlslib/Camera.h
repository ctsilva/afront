/**
 *
 * file:	Camera.h
 * description:	Stores and computes common operations for a 
 *		camera object.  Includes the location, lookat, up,
 *		height and width of the film.  Allows for operations
 *		including zooms, rotate, and pans.  
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

// -- INCLUDES -- //

// -- DEFINES -- //
#include "Global_Defs.h"
#include "Point.h"
#include "Matrix_MxN.h"
#include "OrthoNormalBasis.h"
#include "Vector.h"

#include <stdio.h>
#include <math.h>

// -- CLASS DEFINTION -- //
class Camera
{
 public:

  // constructors
  Camera() {}

  // initialize the camera to a set of values
  void		initialize( const Point &eye, const Vector &vLook, const Vector &vUp,
			    const float &focal_length, 
			    const float &width, const float &height,
			    const int &window_width, const int &window_height );

  // accessors
  Point		get_eye() const { return eye_; };
  Point		get_lookat() const { return eye_ + Vector(orientation_.w().dot_product(Vector(1,0,0)),
							  orientation_.w().dot_product(Vector(0,1,0)),
							  orientation_.w().dot_product(Vector(0,0,1)))*focal_length_; };
  Vector	get_up() const { return Vector(orientation_.v().dot_product(Vector(1,0,0)),
					       orientation_.v().dot_product(Vector(0,1,0)),
					       orientation_.v().dot_product(Vector(0,0,1))); };
  Vector	get_right() const { return Vector(orientation_.u().dot_product(Vector(1,0,0)),
						  orientation_.u().dot_product(Vector(0,1,0)),
						  orientation_.u().dot_product(Vector(0,0,1))); };

  int		window_width() const { return windowX_; };
  int&		window_width() { return windowX_; };
  int		window_height() const { return windowY_; };
  int&		window_height() { return windowY_; };

  float		film_width() const { return width_; }
  float&	film_width() { return width_; }
  float		film_height() const { return height_; }
  float&	film_height() { return height_; }

  float		focal_length() const { return focal_length_; }
  float&	focal_length() { return focal_length_; }

  // movement operations
  void		zoom( const int &dy );
  void		rotate( const float &theta, const float &phi );
  void		pan( const int &dx, const int &dy );

  // get a world space vector
  Vector	pixel_ray( const int &x, const int &y ) const;

  // print out debug information
  void		print() const;

 protected:

  // camera location and orientation storage
  Point	eye_;
  OrthoNormalBasis orientation_;
  float focal_length_;

  // physical dimensions of the window and image plane
  int windowX_,windowY_;
  float width_,height_;

}; // end class camera

#endif // _CAMERA_H_

