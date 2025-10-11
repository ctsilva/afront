/**
 *
 * file:	Color.C
 * description:	Stores and computes common operations for a 
 *		color attribute.  Only contains the RGB.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "Color.h"

// -- DEFINES -- //

/**
 * No argument constructor - init to black
 */
Color::Color()
  : r_(0),
    g_(0),
    b_(0) {}

/**
 * three argument constructor - init to the rgb given
 */
Color::Color( const float &r, const float &g, const float &b )
  : r_(r),
    g_(g),
    b_(b) {}

/**
 * copy constructor - init to color given
 */
Color::Color( const Color &other )
  : r_(other.r_),
    g_(other.g_),
    b_(other.b_) {}

/**
 * assignment operator - return reference to new color
 */
Color&	
Color::operator=(const Color &other)
{
  r_ = other.r_;
  g_ = other.g_;
  b_ = other.b_;
  return *this;
}

/**
 * negative operator - returns a negative version of the
 * color, but what does that mean? Will just flipping the
 * color signs be enough or should we flip then subtract 
 * all values from one?
 */
Color
Color::operator-() const
{
  return Color( -r_, -g_, -b_ );
}

/**
 * addition operator - adds two colors together and returns the
 * results.
 */
Color
Color::operator+( const Color &other ) const
{
  return Color( r_+other.r_, g_+other.g_, b_+other.b_ );
}

/**
 * subtraction operator - subtracts two colors and returns the results.
 */
Color 	
Color::operator-( const Color &other ) const
{
  return Color( r_-other.r_, g_-other.g_, b_-other.b_ );
}

/**
 * addition assignment operator - adds the other color and returns a
 * reference to this color, the result.
 */
Color&	
Color::operator+=( const Color &other )
{
  r_ += other.r_;
  g_ += other.g_;
  b_ += other.b_;
  return *this;
}

/**
 * subtraction assignment operator - subtracts the other color and returns
 * a reference to this color, the result.
 */
Color&	
Color::operator-=( const Color &other )
{
  r_-=other.r_;
  g_-=other.g_;
  b_-=other.b_;
  return *this;
}

/**
 * multiplication operator - multiplies the scalar to this color's values
 * and returns the new color
 */
Color
Color::operator*( const float &scalar ) const
{
  return Color( r_*scalar, g_*scalar, b_*scalar );
}

/**
 * division operator - divides the scalar and returns the new color.
 */
Color
Color::operator/( const float &scalar ) const
{
  if ( scalar < EPSILON && scalar > -EPSILON )
    {
      fprintf(stderr," [ERROR] Color::operator/() -- division by zero!\n");
      return Color(0,0,0);
    }

  return Color( r_/scalar, g_/scalar, b_/scalar );
}

/**
 * multiply assignment operator - multiplies the color and saves them in
 * this color, returns a reference to the results.
 */
Color&	
Color::operator*=( const float &scalar )
{
  r_ *= scalar;
  g_ *= scalar;
  b_ *= scalar;
  return *this;
}

/**
 * divide assignment operator - divides the color and saves them into 
 * this color, returns a reference to the results.
 */
Color&	
Color::operator/=( const float &scalar )
{
  if ( scalar < EPSILON && scalar > -EPSILON )
    {
      fprintf(stderr," [ERROR] Color::operator/=() -- division by zero!\n");
      r_ = g_ = b_ = 0;
    }
  else
    {
      r_ /= scalar;
      g_ /= scalar;
      b_ /= scalar;
    }
  return *this;
}

/**
 * scale - will scale the color components such that the
 * largest component is equal to 1.
 */
void          
Color::scale()
{
  float max = r_;
  if (max < g_) max = g_;
  if (max < b_) max = b_;

  r_ /= max;
  g_ /= max;
  b_ /= max;
}

/**
 * scale color - will return a scaled version of this color.
 */
Color		
Color::scaled_color() const
{
  Color theCopy( *this );
  theCopy.scale();
  return theCopy;
}

/**
 * truncate - truncates the color components between 0-1.
 */
void          
Color::truncate()
{
  if (r_ > 1.0) r_ = 1.0;
  else if (r_ < 0.0) r_ = 0.0;

  if (g_ > 1.0) g_ = 1.0;
  else if (g_ < 0.0) g_ = 0.0;

  if (b_ > 1.0) b_ = 1.0;
  else if (b_ < 0.0) b_ = 0.0;
}

/**
 * capped color - returns a truncated version of the current color.
 */
Color		
Color::capped_color() const
{
  Color theCopy(*this);
  theCopy.truncate();
  return theCopy;
}

/**
 * negate - flips the color component to the opposite side of the 
 * color wheel.
 */
void
Color::negate() 
{
  this->scale();
  r_ = 1.0 - r_;
  g_ = 1.0 - g_; 
  b_ = 1.0 - b_;
}

/**
 * negative - returns the negative version of this color on the color
 * wheel.
 */
Color
Color::negative_color() const
{
  Color theCopy(*this);
  theCopy.negate();
  return theCopy;
}

/**
 * approx_equal - returns the boolean of whether the two colors
 * are nearly equal.
 */
bool
Color::approx_equal( const Color &other,
		     const float &eps ) const
{
  return (fabs( r_-other.r_ ) < eps &&
	  fabs( g_-other.g_ ) < eps &&
	  fabs( b_-other.b_ ) < eps );
}

