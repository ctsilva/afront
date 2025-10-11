/**
 *
 * file:	Vector.C
 * description:	Stores and computes common operations for a 
 *		vector geometry object.  Only contains the location.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "Vector.h"

/**
 * No argument constructor - init to z-axis
 */
Vector::Vector()
  : x_(0),
    y_(0),
    z_(1) {}

/**
 * One argument constructor - init to the given xyz 
 */
Vector::Vector(const float t)
: x_(t),y_(t),z_(t)
{

}
/**
 * Three argument constructor - init to the given xyz 
 */
Vector::Vector( const float &x, const float &y, const float &z )
  : x_(x),
    y_(y),
    z_(z) {}

/**
 * copy constructor - copy the given vector
 */
Vector::Vector( const Vector &other )
  : x_(other.x_),
    y_(other.y_),
    z_(other.z_) {}

/**
 * point constructor - turn a point into a vector
 */
Vector::Vector( const Point &p )
  : x_(p.x()),
    y_(p.y()),
    z_(p.z()) {}

/**
 * assignment
 */
Vector&	
Vector::operator=(const Vector &other)
{
  x_ = other.x_;
  y_ = other.y_;
  z_ = other.z_;
  return *this;
}

/**
 * negation operator - flips the vector about the origin
 */
Vector	
Vector::operator-() const
{
  return Vector( -x_, -y_, -z_ );
}

/**
 * multiplication operator - multiplies the vector by the scalar
 * returning the new vector.
 */
Vector	
Vector::operator*( const float &scalar ) const
{
  return Vector( x_*scalar, y_*scalar, z_*scalar );
}

/**
 * division operator - divides the vector by the scalar and returns
 * the new vector.
 */
Vector	
Vector::operator/( const float &scalar ) const
{
  if (scalar < EPSILON && scalar > -EPSILON)
    {
      fprintf(stderr," [ERROR] Vector::operator/() -- division by zero!\n");
      return Vector(0,0,0);
    }

  return Vector( x_/scalar, y_/scalar, z_/scalar );
}

/**
 * multiplication assignment operator - multiplies and assigns the results
 * to this vector.
 */
Vector&	
Vector::operator*=( const float &scalar )
{
  x_ *= scalar;
  y_ *= scalar;
  z_ *= scalar;
  return *this;
}

/**
 * division assignment operator - divides and assigns the results to
 * this vector.
 */
Vector&	
Vector::operator/=( const float &scalar )
{
  if (scalar < EPSILON && scalar > -EPSILON)
    {
      fprintf(stderr," [ERROR] Vector::operator/=() -- division by zero!\n");
      x_ = y_ = z_ = 0;
      return *this;
    }

  x_ /= scalar; 
  y_ /= scalar;
  z_ /= scalar;
  return *this;
}

/**
 * addition operator - adds the two vectors and returns the result
 */
Vector	
Vector::operator+( const Vector &other ) const
{
  return Vector( x_+other.x_, y_+other.y_, z_+other.z_ );
}

/**
 * subtraction operator - subtracts the two vectors and returns the 
 * result.
 */
Vector 	
Vector::operator-( const Vector &other ) const
{
  return Vector( x_-other.x_, y_-other.y_, z_-other.z_ );
}

/**
 * addition assignmnet operator - adds the vector into this vector and
 * returns the reference.
 */
Vector&	
Vector::operator+=( const Vector &other )
{
  x_ += other.x_;
  y_ += other.y_;
  z_ += other.z_;
  return *this;
}

/**
 * subtraction assignment operator - subtracts the vector from this 
 * vector and returns the reference.
 */
Vector&
Vector::operator-=( const Vector &other )
{
  x_ -= other.x_;
  y_ -= other.y_;
  z_ -= other.z_;
  return *this;
}

/**
 * radians between - measures the angle of the smaller between the
 * two vectors and returns the radians between them.
 */
float	
Vector::radians_between ( const Vector &other ) const
{
  return acos(this->unit_vector().dot_product( other.unit_vector() ));
}
  
/**
 * dot product - returns the dot of the two vectors.
 */
float	
Vector::dot_product( const Vector &other ) const
{
  return x_*other.x_ + y_*other.y_ + z_*other.z_;
}

/**
 * cross product - returns the cross of the two vectors
 */
Vector	
Vector::cross_product( const Vector &other ) const
{
  return Vector( y_*other.z_ - z_*other.y_,
		 z_*other.x_ - x_*other.z_,
		 x_*other.y_ - y_*other.x_ );
}

/**
 * unit cross product - returns the unit cross of the two vectors
 */
Vector	
Vector::unit_cross_product( const Vector &other ) const
{
  Vector theCross( y_*other.z_ - z_*other.y_,
		   z_*other.x_ - x_*other.z_,
		   x_*other.y_ - y_*other.x_ );
  return theCross.unit_vector();
}

/**
 * normalize - shrink the length of the vector to 1 but maintain
 * the components' proportionality.
 */
void
Vector::normalize()
{
  float L = this->length();
  if (L < EPSILON)
    {
      fprintf(stderr," [WARNING] Vector::normalize() -- vector length is zero!\n");
      return;
    }
  x_ /= L;
  y_ /= L;
  z_ /= L;
}

/**
 * unit vector -- returns a unit vector version of this vector.
 */
Vector	
Vector::unit_vector() const
{
  float L = this->length();
  if (L < EPSILON)
    {
      fprintf(stderr," [WARNING] Vector::unit_vector() -- vector length is zero!\n");
      return Vector(0,0,0);
    }
  return Vector( x_/L, y_/L, z_/L );
}

/**
 * length squared -- returns the square of the length of the vector.
 */
float	
Vector::length_squared() const
{
  return this->dot_product( *this );
}

/**
 * length - returns the sqrt of the length squared function.
 */
float	
Vector::length() const
{
  return sqrt( this->dot_product( *this ) );
}

/**
 * project onto me -- projects the vector onto this vector
 * and returns the component in this vector direction.
 */
Vector 
Vector::project_onto_me( const Vector &other ) const
{
  return other.unit_vector() * (this->dot_product( other ));
}

/**
 * perpendicular to me -- returns the component of the vector
 * that is perpendicular to this vector.
 */
Vector 
Vector::perpendicular_to_me( const Vector &other ) const
{
  Vector thePerpendicular = (this->cross_product( other )).unit_cross_product( *this );
  return thePerpendicular * (thePerpendicular.dot_product( other ));
}

/**
 * approx equal -- returns the boolean indicating whether the two 
 * vectors are basically the same.
 */
bool
Vector::approx_equal( const Vector &other,
		      const float &eps ) const
{
  return ( fabs(x_-other.x_) < eps &&
	   fabs(y_-other.y_) < eps &&
	   fabs(z_-other.z_) < eps );
}

/**
 * parallel -- returns the boolean indicating if two vectors are 
 * parallel (in same or opposite directions)
 */
bool 
Vector::approx_parallel( const Vector &other,
			 const float &eps ) const
{
  return 1.0 - fabs(this->unit_vector().dot_product( other.unit_vector() )) < eps; 
}

/**
 * change basis -- returns this point as if it were defined in another
 * ortho normal basis.
 */
Vector
Vector::changeBasis( const OrthoNormalBasis &from,
		     const OrthoNormalBasis &to ) const
{
  return Vector( x_*(from.u().dot_product(to.u())) + y_*(from.v().dot_product(to.u())) + z_*(from.w().dot_product(to.u())),
		 x_*(from.u().dot_product(to.v())) + y_*(from.v().dot_product(to.v())) + z_*(from.w().dot_product(to.v())),
		 x_*(from.u().dot_product(to.w())) + y_*(from.v().dot_product(to.w())) + z_*(from.w().dot_product(to.w())) );
}

/**
 * get_max_axis -- finds the component with the largest value.
 */
float 
Vector::get_max_axis() const
{
  return MAX( MAX(x_,y_), z_ );
}

/**
 * get_min_axis -- returns the minimum component of the vector.
 */
float 
Vector::get_min_axis() const
{
  return MIN( MIN(x_,y_), z_ );
}
