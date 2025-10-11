/**
 *
 * file:	Point.C
 * description:	Stores and computes common operations for a 
 *		point geometry object.  Includes attributes such
 *		as color, normal, and 3D location.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "Point.h"

/** 
 * No argument constructor - uses default values for the point.
 */
Point::Point() 
  : x_(0),
    y_(0),
    z_(0) {}

/**
 * Initialized point constructor - sets the location to the given
 * arguments and uses default values for color and normal.
 */
Point::Point( const float &x, const float &y, const float &z )
  : x_(x),
    y_(y),
    z_(z) {}

/**
 * Initialized point constructor - sets the location to the given
 * arguments and uses default values for color and normal.
 */
Point::Point( const float v[])
{
	data_[0]= v[0];
	data_[1]= v[1];
  data_[2]= v[2];
}


/**
 * copy constructor - duplicates the components of the given point to
 * initialize this point.
 */
Point::Point( const Point &other )
  : x_( other.x_ ),
    y_( other.y_ ),
    z_( other.z_ ) {}

/**
 * Vector constructor - initializes the point location to the vector
 * and gives default values to the point's color and normal.
 */
Point::Point( const Vector &v )
  : x_( v.x() ),
    y_( v.y() ),
    z_( v.z() ) {}

/**
 * assignment operator
 */
Point&	
Point::operator=(const Point &other)
{
  x_ = other.x_;
  y_ = other.y_;
  z_ = other.z_;
  return *this;
}

/**
 * negative operator - returns a point with signs flipped for each
 * location value and normal value, the negative version of the point.
 */
Point
Point::operator-() const
{
  return Point( -x_, -y_, -z_ );
}

/**
 * addition operator - adds a point to the current point and returns
 * the resulting point at the new location. 
 */
Point
Point::operator+( const Point &other ) const
{
  return Point( x_+other.x_, y_+other.y_, z_+other.z_ );
}

/**
 * subtraction operator - returns the vector from the given point to
 * this point. 
 */
Vector	
Point::operator-( const Point &other ) const
{
  return Vector( x_-other.x_, y_-other.y_, z_-other.z_ );
}

/**
 * addition assignment operator - adds the point to this point and 
 * returns a reference to the result.
 */
Point&	
Point::operator+=( const Point &other )
{
  x_ += other.x_;
  y_ += other.y_;
  z_ += other.z_;
  return *this;
}

/**
 * subtraction assignment operator - subtracts the point from this
 * point and returns a reference to the result.
 */
Point&	
Point::operator-=( const Point &other )
{
  x_ -= other.x_;
  y_ -= other.y_;
  z_ -= other.z_;
  return *this;
}

/**
 * multiplication operator - scales the point by the value and
 * returns the resulting point.
 */
Point		
Point::operator*( const float &scalar ) const
{
  return Point( x_*scalar, y_*scalar, z_*scalar );
}

/**
 * divide operator - scales the point by dividing the value and
 * returns the resulting point.
 */
Point		
Point::operator/( const float &scalar ) const
{
  // error output!
  if ( scalar < EPSILON && scalar > -EPSILON )
    {
      fprintf(stderr," [ERROR] Point::operator/() -- division by zero\n");
      return Point(0,0,0);
    }

  return Point( x_/scalar, y_/scalar, z_/scalar );
}

/**
 * multiply assignment operator - multiplies then saves the new location
 * and returns a reference to this point.
 */
Point&
Point::operator*=( const float &scalar )
{
  x_ *= scalar;
  y_ *= scalar;
  z_ *= scalar;
  return *this;
}

/**
 * divide assignment operator - divides and saves the new location then
 * returns a reference to this point.
 */
Point&
Point::operator/=( const float &scalar ) 
{
  // error output!
  if ( scalar < EPSILON && scalar > -EPSILON )
    {
      fprintf(stderr," [ERROR] Point::operator/=() -- division by zero\n");
      x_ = 0;
      y_ = 0;
      z_ = 0;
    }
  else 
  {
    x_ /= scalar;
    y_ /= scalar;
    z_ /= scalar;
  }
  return *this;
}

/**
 * vector addition operator - adds the vector to the point and returns the
 * resulting point location.
 */
Point
Point::operator+( const Vector &other ) const
{
  return Point( x_+other.x(), y_+other.y(), z_+other.z() );
}

/**
 * vector subtraction operator - subtracts the vector from the point and
 * returns the resulting point location.
 */
Point
Point::operator-( const Vector &other ) const
{
  return Point( x_-other.x(), y_-other.y(), z_-other.z() );
}

/**
 * vector add assignment operator - adds the vector to this point and 
 * returns a reference to the results.
 */
Point&
Point::operator+=( const Vector &other ) 
{
  x_ += other.x();
  y_ += other.y();
  z_ += other.z();
  return *this;
}

/**
 * vector subtract assignment - subtracts the vector from this point and
 * returns a reference to the results.
 */
Point&
Point::operator-=( const Vector &other )
{
  x_ -= other.x();
  y_ -= other.y();
  z_ -= other.z();
  return *this;
}

/**
 * distance squared - returns the square of the distance between
 * the two points. It is useful in that it will not perform the sqrt
 * function call as with the distance function, useful for speed up
 * purposes when exact distance doesn't matter.
 */
float	
Point::distance_squared(const Point &other) const
{
  return (x_-other.x_)*(x_-other.x_) + (y_-other.y_)*(y_-other.y_) + (z_-other.z_)*(z_-other.z_);
}

/**
 * distance to - returns the distance between the two points.
 */
float
Point::distance_to(const Point &other) const
{
  return sqrt( this->distance_squared( other ) );
}

/**
 * approximately equal - returns the boolean of whether two points 
 * describe approximately the same location in space.
 */
bool
Point::approx_equal( const Point &other,
		     const float &eps ) const
{
  return this->distance_to( other ) < eps;
}

/**
 * change basis -- returns this point as if it were defined in another
 * ortho normal basis.
 */
Point
Point::changeBasis( const OrthoNormalBasis &from,
		    const OrthoNormalBasis &to ) const
{
  return Point( x_*(from.u().dot_product(to.u())) + y_*(from.v().dot_product(to.u())) + z_*(from.w().dot_product(to.u())),
		x_*(from.u().dot_product(to.v())) + y_*(from.v().dot_product(to.v())) + z_*(from.w().dot_product(to.v())),
		x_*(from.u().dot_product(to.w())) + y_*(from.v().dot_product(to.w())) + z_*(from.w().dot_product(to.w())) );
}

/**
 * operator [] - Access operator allows user to access point like an
 * array.
 */
float 
Point::operator[]( const unsigned int &i ) const
{
  if ( i>2 )
    {
      fprintf(stderr," [WARNING] Point::operator[] -- out of bounds index (%d)\n",i);
      return data_[0];
    }
  else
    {
      return data_[i];
    }
}

/**
 * operator [] - Access operator allows user to access point like an
 * array.
 */
float&
Point::operator[]( const unsigned int &i )
{
  if ( i>2 )
    {
      fprintf(stderr," [EXTREME DANGER] Point::operator[] -- out of bounds index (%d)\n",i);
      return data_[0];
    }
  else
    {
      return data_[i];
    }
}
