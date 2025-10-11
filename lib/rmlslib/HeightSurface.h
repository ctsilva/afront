/**
 *
 * file:	HeightSurface.h
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

#include "Primitive_Functions.h"

#ifndef _HEIGHT_SURFACE_H_
#define _HEIGHT_SURFACE_H_

// -- INCLUDES -- //
#include "Global_Defs.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "Matrix_MxN.h"
#include "Point.h"
#include "Vector.h"
#include "Primitive_Structs.h"
#include "OrthoNormalBasis.h"

// -- DEFINES -- //

// -- CLASS DEFINTION -- //
class HeightSurface
{
 public:

  /** 
   * constructors of the point.
   */
  HeightSurface() {};
  HeightSurface( const HeightSurface &other );

  /**
   * assignment operation.
   */
  HeightSurface&	operator=(const HeightSurface &other);

  /**
   * weighted least squares fit.
   */
  void		fitSurface( const std::vector<Point> &cloud );
  void		fitSurface( const Point &origin, const OrthoNormalBasis &orientation,
			    const std::vector<Point> &cloud );
  void		weightedFitSurface( const Point &origin, const OrthoNormalBasis &orientation,
				    const std::vector<Point> &cloud );
  void		weightedFitSurface( const Point &origin, const OrthoNormalBasis &orientation,
				    const std::vector<Point> &cloud, const std::vector< float > &weights );

  /**
   * useful point operations.
   */
  Point		projectPoint( const Point &input ) const;
  Vector	projectNormal( const Point &input ) const;
  void		projectPoint( const Point &input, Point &returnPoint, Vector &returnNormal ) const;
  Point		closestPoint( const Point &input ) const;
  float		computeResidual( const Point &input ) const;
  Point		evaluateSurface( const float &u, const float &v ) const;
  Vector        evaluateNormal( const float &u, const float &v ) const;
  float         computeLaplacian() const;

  /**
   * statistics for the surface
   */
  float		meanResidual( const std::vector< Point > &cloud ) const;
  float		maxResidual( const std::vector< Point > &cloud ) const;
  float		medianResidual( const std::vector< Point > &cloud ) const;
  float		kthOrderResidual( const float &percent, const std::vector< Point > &cloud ) const;
  float		kthOrderResidual_PartialSort( const float &percent, std::vector< Point > &cloud ) const;

  unsigned int	sortByResiduals( const float &threshold, std::vector< Point > &cloud ) const;
  void		print() const;

 protected:
  OrthoNormalBasis	orientation_;
  Point			origin_;
  float			coefficients_[6];

}; // end quadratic surface

#endif // _QUADRATIC_SURFACE_H_
