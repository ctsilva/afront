/**
 *
 * file:	Color.h
 * description:	Stores and computes common operations for a 
 *		color attribute.  Only contains the RGB.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _COLOR_H_
#define _COLOR_H_

// -- INCLUDES -- //
#include "Global_Defs.h"
#include <stdio.h>
#include <math.h>

// -- DEFINES -- //

// -- CLASS DEFINTION -- //
class Color
{
 public:

  /** 
   * constructors of the point.
   */
  Color();
  Color( const float &r, const float &g, const float &b );
  Color( const Color &other );

  /**
   * assignment operation.
   */
  Color&	operator=(const Color &other);

  /**
   * point component accessors.
   */
  float&	r() { return r_; };
  float	r() const { return r_; };
  float&	g() { return g_; }; 
  float	g() const { return g_; };
  float&	b() { return b_; };
  float	b() const { return b_; };

  void		get_color( float &r, float &g, float &b ) const { r = r_; g = g_; b = b_; };
  void		set_color( const float &r, const float &g, const float &b ) { r_ = r; g_ = g; b_ = b; };

  /**
   * color-color operations.
   */
  Color		operator-() const;

  Color		operator+( const Color &other ) const;
  Color 	operator-( const Color &other ) const;
  Color&	operator+=( const Color &other );
  Color&	operator-=( const Color &other );

  Color		operator*( const float &scalar ) const;
  Color		operator/( const float &scalar ) const;
  Color&	operator*=( const float &scalar );
  Color&	operator/=( const float &scalar );

  /**
   * other operations
   */
  void          scale();
  Color		scaled_color() const;

  void          truncate();
  Color		capped_color() const;

  void		negate();
  Color		negative_color() const;

  bool	        approx_equal( const Color &other,
			      const float &eps = EPSILON ) const;

 protected:
  float		r_,g_,b_;
      
}; // end class color

#endif // _COLOR_H_

