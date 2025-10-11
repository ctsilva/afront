/**
 *
 * file:	QuadraticSurface.cpp
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

// -- INCLUDES -- //
#include "QuadraticSurface.h"

// -- DEFINES -- //

/**
 * copy constructor - initializes the quadratic surface to have
 * the same properties as the passed surface.
 */
QuadraticSurface::QuadraticSurface( const QuadraticSurface &other )
  : orientation_( other.orientation_ ),
    origin_( other.origin_ )
{
  for(unsigned int i=0; i<6; i++)
    coefficients_[i] = other.coefficients_[i];
}

/**
 * assignment operator - assigns the given surface attributes into 
 * this surface.
 */
QuadraticSurface&	
QuadraticSurface::operator=(const QuadraticSurface &other)
{
  orientation_ = other.orientation_;
  origin_ = other.origin_;
  for(unsigned int i=0; i<6; i++)
    coefficients_[i] = other.coefficients_[i];
  return *this;
}

/**
 * fit surface - computes a least squares fit of the point cloud to 
 * the surface using the world coordinates for the orientation and the
 * point (0,0,0) as the origin.
 */
void		
QuadraticSurface::fitSurface( const std::vector<Point> &cloud )
{
  // 0. save the world origin and orientation
  orientation_ = OrthoNormalBasis( Vector(1,0,0),Vector(0,1,0),Vector(0,0,1) );
  origin_ = Point(0,0,0);

  // setup the needed matrices  
  Matrix_MxN A( cloud.size(),6, 0.0 );
  Matrix_MxN Bx( cloud.size(),1, 0.0 );
  Matrix_MxN By( cloud.size(),1, 0.0 );
  Matrix_MxN Bz( cloud.size(),1, 0.0 );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      // project the neighbor point onto the eigen plane to get its (u,v)
      Point neighbor_project = cloud[i] - orientation_.w()*((cloud[i]-origin_).dot_product(orientation_.w()));
      float u = (neighbor_project-origin_).dot_product(orientation_.u());
      float v = (neighbor_project-origin_).dot_product(orientation_.v());

      // 1. setup the WA matrix -- the parameters matrix columns: [1.0 u v u*u u*v v*v]
      A(i,0) = 1.0;	// a_00  -> no u or v multiplier, so 1.0
      A(i,1) = u;	// a_10 -> u multiplier
      A(i,2) = v;	// a_01 -> v multiplier
      A(i,3) = u*u;	// a_20 -> u*u multiplier      
      A(i,4) = u*v;	// a_11 -> u*v multiplier
      A(i,5) = v*v;	// a_02 -> v*v multipler

      // 3. setup the WB matrix -- the weighted points
      Bx(i,0) = cloud[i].x();
      By(i,0) = cloud[i].y();
      Bz(i,0) = cloud[i].z();
    }

  // 4. compute the inverse of the WA matrix
  Matrix_MxN A_inverse = A.inverse();

  // 5. compute the surface coefficients
  Matrix_MxN coefficients_X = A_inverse*Bx;
  Matrix_MxN coefficients_Y = A_inverse*By;
  Matrix_MxN coefficients_Z = A_inverse*Bz;

  // 6. save and return the coefficients
  coefficients_[0].x() = coefficients_X(0,0); coefficients_[0].y() = coefficients_Y(0,0); coefficients_[0].z() = coefficients_Z(0,0);
  coefficients_[1].x() = coefficients_X(1,0); coefficients_[1].y() = coefficients_Y(1,0); coefficients_[1].z() = coefficients_Z(1,0);
  coefficients_[2].x() = coefficients_X(2,0); coefficients_[2].y() = coefficients_Y(2,0); coefficients_[2].z() = coefficients_Z(2,0);
  coefficients_[3].x() = coefficients_X(3,0); coefficients_[3].y() = coefficients_Y(3,0); coefficients_[3].z() = coefficients_Z(3,0);
  coefficients_[4].x() = coefficients_X(4,0); coefficients_[4].y() = coefficients_Y(4,0); coefficients_[4].z() = coefficients_Z(4,0);
  coefficients_[5].x() = coefficients_X(5,0); coefficients_[5].y() = coefficients_Y(5,0); coefficients_[5].z() = coefficients_Z(5,0);
}

/**
 * fit surface - least squares fit of the point cloud given the origin
 * and corrdinate system.  This function will project the points to
 * the UV plane of the coord. system to get the parameter values at
 * which the surface should be approximate the points.
 */
void		
QuadraticSurface::fitSurface( const Point &origin, 
			      const OrthoNormalBasis &orientation,
			      const std::vector<Point> &cloud )
{
  // 0. save the used orientation and world coordinates
  origin_ = origin;
  orientation_ = orientation;

  // setup the needed matrices
  Matrix_MxN A( cloud.size(),6, 0.0 );
  Matrix_MxN Bx( cloud.size(),1, 0.0 );
  Matrix_MxN By( cloud.size(),1, 0.0 );
  Matrix_MxN Bz( cloud.size(),1, 0.0 );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      // 1. project the neighbor point onto the eigen plane to get its (u,v)
      Point neighbor_project = cloud[i] - orientation_.w()*((cloud[i]-origin_).dot_product(orientation_.w()));
      float u = (neighbor_project-origin_).dot_product(orientation_.u());
      float v = (neighbor_project-origin_).dot_product(orientation_.v());

      // 2. setup the A matrix -- the parameters matrix columns: [1.0 u v u*u u*v v*v]
      A(i,0) = 1.0;	// a_00  -> no u or v multiplier, so 1.0
      A(i,1) = u;	// a_10 -> u multiplier
      A(i,2) = v;	// a_01 -> v multiplier
      A(i,3) = u*u;	// a_20 -> u*u multiplier      
      A(i,4) = u*v;	// a_11 -> u*v multiplier
      A(i,5) = v*v;	// a_02 -> v*v multipler

      // 3. setup the WB matrix -- the weighted points
      Bx(i,0) = cloud[i].x();
      By(i,0) = cloud[i].y();
      Bz(i,0) = cloud[i].z();
    }

  // 4. compute the inverse of the A matrix
  Matrix_MxN A_inverse = A.inverse();

  // 5. compute the surface coefficients
  Matrix_MxN coefficients_X = A_inverse*Bx;
  Matrix_MxN coefficients_Y = A_inverse*By;
  Matrix_MxN coefficients_Z = A_inverse*Bz;

  // 6. save and return the coefficients
  coefficients_[0].x() = coefficients_X(0,0); coefficients_[0].y() = coefficients_Y(0,0); coefficients_[0].z() = coefficients_Z(0,0);
  coefficients_[1].x() = coefficients_X(1,0); coefficients_[1].y() = coefficients_Y(1,0); coefficients_[1].z() = coefficients_Z(1,0);
  coefficients_[2].x() = coefficients_X(2,0); coefficients_[2].y() = coefficients_Y(2,0); coefficients_[2].z() = coefficients_Z(2,0);
  coefficients_[3].x() = coefficients_X(3,0); coefficients_[3].y() = coefficients_Y(3,0); coefficients_[3].z() = coefficients_Z(3,0);
  coefficients_[4].x() = coefficients_X(4,0); coefficients_[4].y() = coefficients_Y(4,0); coefficients_[4].z() = coefficients_Z(4,0);
  coefficients_[5].x() = coefficients_X(5,0); coefficients_[5].y() = coefficients_Y(5,0); coefficients_[5].z() = coefficients_Z(5,0);
}

/**
 * weighted fit surface - weighted least squares fit of the point
 * cloud given the origin and corrdinate system.  This function will
 * project the points to the UV plane of the coord. system to get the
 * parameter values at which the surface should be approximate the
 * points.
 */
void		
QuadraticSurface::weightedFitSurface( const Point &origin, 
				      const OrthoNormalBasis &orientation,
				      const std::vector<Point> &cloud )
{
  // 0. save the used orientation and world coordinates
  origin_ = origin;
  orientation_ = orientation;

  // setup the needed matrices
  Matrix_MxN WA( cloud.size(),6, 0.0 );
  Matrix_MxN WBx( cloud.size(),1, 0.0 );
  Matrix_MxN WBy( cloud.size(),1, 0.0 );
  Matrix_MxN WBz( cloud.size(),1, 0.0 );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      // 1. setup the W matrix -- weighted distance matrix
      double w = 1.0 / origin.distance_squared( cloud[i] );

      // project the neighbor point onto the eigen plane to get its (u,v)
      Point neighbor_project = cloud[i] - orientation_.w()*((cloud[i]-origin_).dot_product(orientation_.w()));
      float u = (neighbor_project-origin_).dot_product(orientation_.u());
      float v = (neighbor_project-origin_).dot_product(orientation_.v());

      // 2. setup the WA matrix -- the parameters matrix columns: [1.0 u v u*u u*v v*v]
      WA(i,0) = w*1.0;	// a_00  -> no u or v multiplier, so 1.0
      WA(i,1) = w*u;	// a_10 -> u multiplier
      WA(i,2) = w*v;	// a_01 -> v multiplier
      WA(i,3) = w*u*u;	// a_20 -> u*u multiplier      
      WA(i,4) = w*u*v;	// a_11 -> u*v multiplier
      WA(i,5) = w*v*v;	// a_02 -> v*v multipler

      // 3. setup the WB matrix -- the weighted points
      WBx(i,0) = cloud[i].x()*w;
      WBy(i,0) = cloud[i].y()*w;
      WBz(i,0) = cloud[i].z()*w;
    }

  // 4. compute the inverse of the WA matrix
  Matrix_MxN WA_inverse = WA.inverse();

  // 5. compute the surface coefficients
  Matrix_MxN coefficients_X = WA_inverse*WBx;
  Matrix_MxN coefficients_Y = WA_inverse*WBy;
  Matrix_MxN coefficients_Z = WA_inverse*WBz;

  // 6. save and return the coefficients
  coefficients_[0].x() = coefficients_X(0,0); coefficients_[0].y() = coefficients_Y(0,0); coefficients_[0].z() = coefficients_Z(0,0);
  coefficients_[1].x() = coefficients_X(1,0); coefficients_[1].y() = coefficients_Y(1,0); coefficients_[1].z() = coefficients_Z(1,0);
  coefficients_[2].x() = coefficients_X(2,0); coefficients_[2].y() = coefficients_Y(2,0); coefficients_[2].z() = coefficients_Z(2,0);
  coefficients_[3].x() = coefficients_X(3,0); coefficients_[3].y() = coefficients_Y(3,0); coefficients_[3].z() = coefficients_Z(3,0);
  coefficients_[4].x() = coefficients_X(4,0); coefficients_[4].y() = coefficients_Y(4,0); coefficients_[4].z() = coefficients_Z(4,0);
  coefficients_[5].x() = coefficients_X(5,0); coefficients_[5].y() = coefficients_Y(5,0); coefficients_[5].z() = coefficients_Z(5,0);
}

/**
 * project point - this first projects the point to the UV plane of the
 * surfaces' coordinate system to compute the parameter values of the 
 * point's projection.  it returns that (u,v) point.
 */
Point		
QuadraticSurface::projectPoint( const Point &input ) const
{
  // project the point to the UV plane
  Point projection = input - orientation_.w()*((input-origin_).dot_product(orientation_.w()));
  float u = (projection - origin_).dot_product( orientation_.u() );
  float v = (projection - origin_).dot_product( orientation_.v() );

  // compute the height field at that point
  return coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;
}

/**
 * compute residual - this function will compute the residual of the
 * point basically returning the distance to the projection. but based
 * on our implementation we are able to short cut this computation by
 * merely comparing heights over the UV plane of the orientation.
 */
float 
QuadraticSurface::computeResidual( const Point &input ) const
{
  // project the point to the UV plane
  Point projection = input - orientation_.w()*((input-origin_).dot_product(orientation_.w()));
  float u = (projection - origin_).dot_product( orientation_.u() );
  float v = (projection - origin_).dot_product( orientation_.v() );

  // compute the height field at that point
  Point surface_pt = coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;

  // return the distance to the point
  return surface_pt.distance_to( input );
}

/**
 * evaluate surface - this will return the point for the surface at the
 * given (u,v) parameteric location.
 */
Point
QuadraticSurface::evaluateSurface( const float &u, const float &v ) const
{
  // compute the height field at that point
  return coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;
}

/**
 * mean residual - computes the average residual of the point cloud.
 */
float		
QuadraticSurface::meanResidual( const std::vector< Point > &cloud ) const
{
  float avgValue = 0;
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      avgValue += computeResidual( cloud[i] );
    }
  return avgValue / cloud.size();
}

/**
 * max residual - computes the maximum residual of the point cloud to the
 * surface and returns the value.
 */
float		
QuadraticSurface::maxResidual( const std::vector< Point > &cloud ) const
{
  float maxValue = 0;
  for( unsigned int i=0; i<cloud.size(); i++)
    {
      float tmpValue = computeResidual( cloud[i] );
      if (tmpValue > maxValue)
	maxValue = tmpValue;
    }
  return maxValue;
}

/**
 * median residual - computes the median residual value by calling the 
 * kth order residual with 50%.
 */
float		
QuadraticSurface::medianResidual( const std::vector< Point > &cloud ) const
{
  return kthOrderResidual( 0.5,cloud );
}

/**
 * kth order residual - computes the kth order value of the residuals.
 */
float		
QuadraticSurface::kthOrderResidual( const float &percent, 
				    const std::vector< Point > &cloud ) const
{
  // compute the list of residuals
  std::vector< float > residuals( cloud.size() );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      residuals[i] = computeResidual( cloud[i] );
    }

  // compute the offset desired - truncate!
  unsigned int offset = (unsigned int)(percent*cloud.size());

  // return the kth order statistics 
  nth_element( residuals.begin(), residuals.begin()+offset, residuals.end() );
  return residuals[offset];
}
