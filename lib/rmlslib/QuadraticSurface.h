/**
 *
 * file:	QuadraticSurface.h
 * description:	Stores and computes common operations for a 
 *		quadratic surface.  This contains the coefficients for 
 *		the surface and will be able to perform a set of computatioins
 *		on the surface, such as: point projection, residual 
 *		statistics computations to a vector of points, and weighted
 *		surface fitting.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _QUADRATIC_SURFACE_H_
#define _QUADRATIC_SURFACE_H_

// -- INCLUDES -- //
#include "Global_Defs.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "Matrix_MxN.h"
#include "Primitive_Structs.h"
#include "Point.h"
#include "OrthoNormalBasis.h"
#include "Vector.h"

// -- DEFINES -- //

// -- CLASS DEFINTION -- //
class QuadraticSurface
{
 public:

  /** 
   * constructors of the point.
   */
  QuadraticSurface() {};
  QuadraticSurface( const QuadraticSurface &other );

  /**
   * assignment operation.
   */
  QuadraticSurface&	operator=(const QuadraticSurface &other);

  /**
   * weighted least squares fit.
   */
  void		fitSurface( const std::vector<Point> &cloud );
  void		fitSurface( const Point &origin, const OrthoNormalBasis &orientation,
			    const std::vector<Point> &cloud );

  void		weightedFitSurface( const Point &origin, const OrthoNormalBasis &orientation,
				    const std::vector<Point> &cloud );
  /**
   * useful point operations.
   */
  Point		projectPoint( const Point &input ) const;
  float		computeResidual( const Point &input ) const;
  Point		evaluateSurface( const float &u, const float &v ) const;

  /**
   * statistics for the surface
   */
  float		meanResidual( const std::vector< Point > &cloud ) const;
  float		maxResidual( const std::vector< Point > &cloud ) const;
  float		medianResidual( const std::vector< Point > &cloud ) const;
  float		kthOrderResidual( const float &percent, const std::vector< Point > &cloud ) const;

 protected:
  OrthoNormalBasis	orientation_;
  Point			origin_;
  Point			coefficients_[6];

}; // end quadratic surface

#endif // _QUADRATIC_SURFACE_H_
