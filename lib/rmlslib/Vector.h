/**
 *
 * file:	Vector.h
 * description:	Stores and computes common operations for a 
 *		vector geometry object.  Only contains the location.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#include "Point.h"

#ifndef _VECTOR_H_
#define _VECTOR_H_

// -- INCLUDES -- //
#include "Global_Defs.h"
#include <stdio.h>
#include <math.h>

// -- DEFINES -- //
#ifndef _ORTHONORMALBASIS_H_
class OrthoNormalBasis;
#endif

// -- CLASS DEFINTION -- //
class Vector
{
 public:

  /** 
   * constructors of the vector.
   */
		Vector();
		Vector(const float t);
  explicit	Vector( const float &x, const float &y, const float &z );
		Vector( const Vector &other );
  explicit	Vector( const Point &p );

  /**
   * assignment operation.
   */
  Vector&	operator=(const Vector &other);

  /**
   * vector component accessors.
   */
  float&	x() { return x_; };
  float	x() const { return x_; };
  float&	y() { return y_; }; 
  float	y() const { return y_; };
  float&	z() { return z_; };
  float	z() const { return z_; };

  void		get_direction( float &x, float &y, float &z ) const { x = x_; y = y_; z = z_; };
  void		set_direction( const float &x, const float &y, const float &z ) { x_ = x; y_ = y; z_ = z; };

  /**
   * point-point and point-vector operations.
   */
  Vector	operator-() const;

  Vector	operator*( const float &scalar ) const;
  Vector	operator/( const float &scalar ) const;
  Vector&	operator*=( const float &scalar );
  Vector&	operator/=( const float &scalar );

  Vector	operator+( const Vector &other ) const;
  Vector 	operator-( const Vector &other ) const;
  Vector&	operator+=( const Vector &other );
  Vector&	operator-=( const Vector &other );

  float		get_max_axis() const;
  float		get_min_axis() const;

  /**
   * other useful operations
   */
  float		radians_between ( const Vector &other ) const;
  
  float		dot_product( const Vector &other ) const;
  Vector	cross_product( const Vector &other ) const;
  Vector	unit_cross_product( const Vector &other ) const;

  void		normalize();
  Vector	unit_vector() const;

  float		length_squared() const;
  float		length() const;

  Vector	project_onto_me( const Vector &other ) const;
  Vector	perpendicular_to_me( const Vector &other ) const;

  bool	        approx_equal( const Vector &other,
			      const float &eps = EPSILON ) const;
  bool	        approx_parallel( const Vector &other,
				 const float &eps = EPSILON ) const;

  Vector	changeBasis( const OrthoNormalBasis &from,
			     const OrthoNormalBasis &to ) const;
protected:
  float		x_,y_,z_;
      
}; // end class vector

#ifndef _ORTHONORMALBASIS_H_
#include "OrthoNormalBasis.h"
#endif

#endif // _VECTOR_H_
