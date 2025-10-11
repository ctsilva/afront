/**
 *
 * file:	Point.h
 * description:	Stores and computes common operations for a 
 *		point geometry object.  Only contains the location.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _POINT_H_
#define _POINT_H_

// -- INCLUDES -- //
#include "Global_Defs.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>

// -- DEFINES -- //
#ifndef _VECTOR_H_
class Vector;
#endif
#ifndef _ORTHONORMALBASIS_H_
class OrthoNormalBasis;
#endif

// -- CLASS DEFINTION -- //
class Point
{
 public:

  /** 
   * constructors of the point.
   */
		Point();
    Point( const float v[]);
  explicit	Point( const float &x, const float &y, const float &z);
		Point( const Point &other );
  explicit	Point( const Vector &v );

  /**
   * assignment operation.
   */
  Point&	operator=(const Point &other);

  /**
   * point component accessors.
   */
  float&	x() { return x_; };
  float		x() const { return x_; };
  float&	y() { return y_; }; 
  float		y() const { return y_; };
  float&	z() { return z_; };
  float		z() const { return z_; };

  void		get_location( float &x, float &y, float &z ) const { x = x_; y = y_; z = z_; };
  void		set_location( const float &x, const float &y, const float &z ) { x_ = x; y_ = y; z_ = z; };

  /**
   * point-point and point-vector operations.
   */
  Point		operator-() const;

  Point		operator+( const Point &other ) const;
  Vector	operator-( const Point &other ) const;
  Point&	operator+=( const Point &other );
  Point&	operator-=( const Point &other );

  Point		operator*( const float &scalar ) const;
  Point		operator/( const float &scalar ) const;
  Point&	operator*=( const float &scalar );
  Point&	operator/=( const float &scalar );

  Point		operator+( const Vector &other ) const;
  Point 	operator-( const Vector &other ) const;
  Point&	operator+=( const Vector &other );
  Point&	operator-=( const Vector &other );

  float&	operator[]( const unsigned int &i );
  float		operator[]( const unsigned int &i ) const;

  /**
   * other useful operations
   */
  float	distance_squared(const Point &other) const;
  float	distance_to(const Point &other) const;

  bool	        approx_equal( const Point &other,
			      const float &eps = EPSILON ) const;

  Point		changeBasis( const OrthoNormalBasis &from,
			     const OrthoNormalBasis &to ) const;

 protected:
  union {
	struct { float data_[3];};
	struct { float x_,y_,z_;};
  };

}; // end class point

inline Point Min(const Point& v1, const Point& v2)
{
  return Point(MIN(v1.x(),v2.x()),MIN(v1.y(),v2.y()),MIN(v1.z(),v2.z()));
}

inline Point Max(const Point& v1, const Point& v2)
{
  return Point(MAX(v1.x(),v2.x()),MAX(v1.y(),v2.y()),MAX(v1.z(),v2.z()));
}

#ifndef _VECTOR_H_
#include "Vector.h"
#endif
#ifndef _ORTHONORMALBASIS_H_
#include "OrthoNormalBasis.h"
#endif

#endif // _POINT_H_
