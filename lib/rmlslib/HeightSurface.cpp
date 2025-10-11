/**
 *
 * file:	HeightSurface.cpp
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
#include "HeightSurface.h"

// -- DEFINES -- //

/**
 * copy constructor - initializes the quadratic surface to have
 * the same properties as the passed surface.
 */
HeightSurface::HeightSurface( const HeightSurface &other )
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
HeightSurface&	
HeightSurface::operator=(const HeightSurface &other)
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
HeightSurface::fitSurface( const std::vector<Point> &cloud )
{
  // A. compute the centroid of the points
  origin_ = Point(0,0,0);
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      origin_+=cloud[i];
    }
  origin_ /= cloud.size();

  // B. compute the pca analysis for the orientation
  std::vector< Eigen_Pair > eigens = pca_analysis( cloud );
  eigens[0].vector_.normalize();	// normalize the normal vector
  eigens[1].vector_.normalize();	// normalize the normal vector
  eigens[2].vector_.normalize();	// normalize the normal vector
  orientation_ = OrthoNormalBasis( eigens[0].vector_,eigens[1].vector_,eigens[2].vector_ );

  // setup the needed matrices
  Matrix_MxN A( cloud.size(),6, 0.0 );
  Matrix_MxN B( cloud.size(),1, 0.0 );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      // project the neighbor point onto the eigen plane to get its (u,v)
      Point neighbor_project = cloud[i] - orientation_.w()*((cloud[i]-origin_).dot_product(orientation_.w()));
      float u = (neighbor_project-origin_).dot_product(orientation_.u());
      float v = (neighbor_project-origin_).dot_product(orientation_.v());

      // 1. setup the A matrix -- the parameters matrix columns: [1.0 u v u*u u*v v*v]
      A(i,0) = 1.0;	// a_00  -> no u or v multiplier, so 1.0
      A(i,1) = u;	// a_10 -> u multiplier
      A(i,2) = v;	// a_01 -> v multiplier
      A(i,3) = u*u;	// a_20 -> u*u multiplier      
      A(i,4) = u*v;	// a_11 -> u*v multiplier
      A(i,5) = v*v;	// a_02 -> v*v multipler

      // 2. setup the B matrix at the same time
      B(i,0) = (cloud[i]-neighbor_project).dot_product(orientation_.w());
    }

  // 3. compute the inverse of the WA matrix
  Matrix_MxN A_inverse = A.inverse();

  // 4. compute the surface coefficients
  Matrix_MxN coefficients = A_inverse*B;

  // 5. save and return the coefficients
  coefficients_[0] = coefficients(0,0);
  coefficients_[1] = coefficients(1,0);
  coefficients_[2] = coefficients(2,0);
  coefficients_[3] = coefficients(3,0);
  coefficients_[4] = coefficients(4,0);
  coefficients_[5] = coefficients(5,0);
}

/**
 * fit surface - least squares fit of the point cloud given the origin
 * and corrdinate system.  This function will project the points to
 * the UV plane of the coord. system to get the parameter values at
 * which the surface should be approximate the points.
 */
void
HeightSurface::fitSurface( const Point &origin, 
			   const OrthoNormalBasis &orientation,
			   const std::vector<Point> &cloud )
{
  // 0. setup the origin and orientation
  origin_ = origin;
  orientation_ = orientation;

  // setup the needed matrices
  Matrix_MxN A( cloud.size(),6, 0.0 );
  Matrix_MxN B( cloud.size(),1, 0.0 );
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

      // 3. setup the B matrix -- the weighted points
      B(i,0) = (cloud[i]-neighbor_project).dot_product(orientation_.w());
    }

  // 4. compute the inverse of the A matrix
  Matrix_MxN A_inverse = A.inverse( );

  // 5. compute the surface coefficients
  Matrix_MxN coefficients = A_inverse*B;

  // 6. save and return the coefficients
  coefficients_[0] = coefficients(0,0);
  coefficients_[1] = coefficients(1,0);
  coefficients_[2] = coefficients(2,0);
  coefficients_[3] = coefficients(3,0);
  coefficients_[4] = coefficients(4,0);
  coefficients_[5] = coefficients(5,0);
}

/**
 * weighted fit surface - weighted least squares fit of the point
 * cloud given the origin and corrdinate system.  This function will
 * project the points to the UV plane of the coord. system to get the
 * parameter values at which the surface should be approximate the
 * points.
 */
void		
HeightSurface::weightedFitSurface( const Point &origin, 
				   const OrthoNormalBasis &orientation,
				   const std::vector<Point> &cloud )
{
  // 0. setup the origin and orientation
  origin_ = origin;
  orientation_ = orientation;

  // setup the needed matrices
  Matrix_MxN WA( cloud.size(),6, 0.0 );
  Matrix_MxN WB( cloud.size(),1, 0.0 );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      // 1. compute the weight -- weighted distance matrix (never truly constructed)
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
      WB(i,0) = (cloud[i]-neighbor_project).dot_product(orientation_.w())*w;
    }

  // 4. compute the inverse of the WA matrix
  Matrix_MxN WA_inverse = WA.inverse();

  // 5. compute the surface coefficients
  Matrix_MxN coefficients = WA_inverse*WB;

  // 6. save and return the coefficients
  coefficients_[0] = coefficients(0,0);
  coefficients_[1] = coefficients(1,0);
  coefficients_[2] = coefficients(2,0);
  coefficients_[3] = coefficients(3,0);
  coefficients_[4] = coefficients(4,0);
  coefficients_[5] = coefficients(5,0);
}

/**
 * weighted surface fit - allows the user to specify the weights of the
 * points instead of computing it based on the distance from the origin.
 */
void		
HeightSurface::weightedFitSurface( const Point &origin, 
				   const OrthoNormalBasis &orientation,
				   const std::vector<Point> &cloud,
				   const std::vector< float > &weights )
{
  // 0. setup the origin and orientation
  origin_ = origin;
  orientation_ = orientation;

  // setup the needed matrices
  Matrix_MxN WA( cloud.size(),6, 0.0 );
  Matrix_MxN WB( cloud.size(),1, 0.0 );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      // 1. project the neighbor point onto the eigen plane to get its (u,v)
      Point neighbor_project = cloud[i] - orientation_.w()*((cloud[i]-origin_).dot_product(orientation_.w()));
      float u = (neighbor_project-origin_).dot_product(orientation_.u());
      float v = (neighbor_project-origin_).dot_product(orientation_.v());

      // 2. setup the WA matrix -- the parameters matrix columns: [1.0 u v u*u u*v v*v]
      WA(i,0) = weights[i]*1.0;	// a_00  -> no u or v multiplier, so 1.0
      WA(i,1) = weights[i]*u;	// a_10 -> u multiplier
      WA(i,2) = weights[i]*v;	// a_01 -> v multiplier
      WA(i,3) = weights[i]*u*u;	// a_20 -> u*u multiplier      
      WA(i,4) = weights[i]*u*v;	// a_11 -> u*v multiplier
      WA(i,5) = weights[i]*v*v;	// a_02 -> v*v multipler

      // 3. setup the WB matrix -- the weighted points
      WB(i,0) = (cloud[i]-neighbor_project).dot_product(orientation_.w())*weights[i];
    }

  // 4. compute the inverse of the WA matrix
  Matrix_MxN WA_inverse = WA.inverse();

  // 5. compute the surface coefficients
  Matrix_MxN coefficients = WA_inverse*WB;

  // 6. save and return the coefficients
  coefficients_[0] = coefficients(0,0);
  coefficients_[1] = coefficients(1,0);
  coefficients_[2] = coefficients(2,0);
  coefficients_[3] = coefficients(3,0);
  coefficients_[4] = coefficients(4,0);
  coefficients_[5] = coefficients(5,0);
}

/**
 * project point - this first projects the point to the UV plane of the
 * surfaces' coordinate system to compute the parameter values of the 
 * point's projection.  it returns that (u,v) point.
 */
Point		
HeightSurface::projectPoint( const Point &input ) const
{
  // project the point to the UV plane
  Point projection = input - orientation_.w()*((input-origin_).dot_product(orientation_.w()));
  float u = (projection - origin_).dot_product( orientation_.u() );
  float v = (projection - origin_).dot_product( orientation_.v() );

  // compute the height field at that point
  float w = coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;

  // change the basis back into world coordinates
  return projection + orientation_.w()*w;
}

/**
 * project normal - this first projects the point to the UV plane of
 * the surfaces' coordinate system to compute the parameter values of
 * the point's projection.  it returns the normal at the (u,v).
 */
Vector
HeightSurface::projectNormal( const Point &input ) const
{
  // project the point to the UV plane
  Point projection = input - orientation_.w()*((input-origin_).dot_product(orientation_.w()));
  float u = (projection - origin_).dot_product( orientation_.u() );
  float v = (projection - origin_).dot_product( orientation_.v() );

  // compute the normal at the given UV
  float scaleU = coefficients_[1] + 2.0*coefficients_[3]*u + coefficients_[4]*v;
  float scaleV = coefficients_[2] + coefficients_[4]*u + 2.0*coefficients_[5]*v;
  Vector partialU = orientation_.u() + orientation_.w()*scaleU;
  Vector partialV = orientation_.v() + orientation_.w()*scaleV;
  return partialU.unit_cross_product(partialV);
}

/**
 * project point - this first projects the point to the UV plane of the
 * surfaces' coordinate system to compute the parameter values of the 
 * point's projection.  it returns that (u,v) point. it also will evaluate
 * the normal at the (u,v).
 */
void
HeightSurface::projectPoint( const Point &input, Point &returnPoint, Vector &returnNormal ) const
{
  // project the point to the UV plane
  Point projection = input - orientation_.w()*((input-origin_).dot_product(orientation_.w()));
  float u = (projection - origin_).dot_product( orientation_.u() );
  float v = (projection - origin_).dot_product( orientation_.v() );

  // compute the height field at that point
  float w = coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;

  // change the basis back into world coordinates
  returnPoint = projection + orientation_.w()*w;

  // compute the normal at the given UV
  float scaleU = coefficients_[1] + 2.0*coefficients_[3]*u + coefficients_[4]*v;
  float scaleV = coefficients_[2] + coefficients_[4]*u + 2.0*coefficients_[5]*v;
  Vector partialU = orientation_.u() + orientation_.w()*scaleU;
  Vector partialV = orientation_.v() + orientation_.w()*scaleV;
  returnNormal = partialU.unit_cross_product(partialV);
}

/**
 * closest point - this will project the point then try to walk it
 * towards a minimum by sampling neighbors until we get a local
 * minimum closest point.
 */
Point		
HeightSurface::closestPoint( const Point &input ) const
{
  // project the point to the UV plane
  Point projection = input - orientation_.w()*((input-origin_).dot_product(orientation_.w()));
  float u = (projection - origin_).dot_product( orientation_.u() );
  float v = (projection - origin_).dot_product( orientation_.v() );

  // compute the height field at that point
  float w = coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;

  // change the basis back into world coordinates
  Point current_point = projection + orientation_.w()*w;
  float current_distance = current_point.distance_squared( input );

  // while sample neighbors are closer update point
  Point minimum_neighbor;
  float minimum_distance;
  bool minimum_flag = true;
  for(unsigned int i=0; i<10 && minimum_flag; i++)
    {
      // compute four neighbors 
      float u_offset = 1;
      float v_offset = 1;
      float u2 = u+u_offset;
      float v2 = v+v_offset;
      float w2 = coefficients_[0] + coefficients_[1]*u2 + coefficients_[2]*v2 + 
	coefficients_[3]*u2*u2 + coefficients_[4]*u2*v2 + coefficients_[5]*v2*v2;

      minimum_neighbor = origin_+ orientation_.u()*u2 + orientation_.v()*v2 + orientation_.w()*w2;
      minimum_distance = minimum_neighbor.distance_squared( input );

      // compute another random neighbor
      u2 = u-u_offset;
      v2 = v+v_offset;
      w2 = coefficients_[0] + coefficients_[1]*u2 + coefficients_[2]*v2 + 
	coefficients_[3]*u2*u2 + coefficients_[4]*u2*v2 + coefficients_[5]*v2*v2;

      Point tmp_neighbor = origin_+ orientation_.u()*u2 + orientation_.v()*v2 + orientation_.w()*w2;
      float tmp_distance = tmp_neighbor.distance_squared( input );
      if (minimum_distance > tmp_distance)
	{
	  minimum_neighbor = tmp_neighbor;
	  minimum_distance = tmp_distance;
	}

      // compute another random neighbor
      u2 = u+u_offset;
      v2 = v-v_offset;
      w2 = coefficients_[0] + coefficients_[1]*u2 + coefficients_[2]*v2 + 
	coefficients_[3]*u2*u2 + coefficients_[4]*u2*v2 + coefficients_[5]*v2*v2;

      tmp_neighbor = origin_+ orientation_.u()*u2 + orientation_.v()*v2 + orientation_.w()*w2;
      tmp_distance = tmp_neighbor.distance_squared( input );
      if (minimum_distance > tmp_distance)
	{
	  minimum_neighbor = tmp_neighbor;
	  minimum_distance = tmp_distance;
	}

      // compute another random neighbor
      u2 = u-u_offset;
      v2 = v-v_offset;
      w2 = coefficients_[0] + coefficients_[1]*u2 + coefficients_[2]*v2 + 
	coefficients_[3]*u2*u2 + coefficients_[4]*u2*v2 + coefficients_[5]*v2*v2;

      tmp_neighbor = origin_+ orientation_.u()*u2 + orientation_.v()*v2 + orientation_.w()*w2;
      tmp_distance = tmp_neighbor.distance_squared( input );
      if (minimum_distance > tmp_distance)
	{
	  minimum_neighbor = tmp_neighbor;
	  minimum_distance = tmp_distance;
	}

      // if minimum_distance less than current then update
      if (minimum_distance < current_distance)
	{
	  current_distance = minimum_distance;
	  current_point = minimum_neighbor;
	  minimum_flag = true;
	}
      else
	{
	  minimum_flag = false;
	}
    }
  return current_point;
}

/**
 * compute residual - this function will compute the residual of the
 * point basically returning the distance to the projection. but based
 * on our implementation we are able to short cut this computation by
 * merely comparing heights over the UV plane of the orientation.
 */
float 
HeightSurface::computeResidual( const Point &input ) const
{
  // project the point to the UV plane
  Point projection = input - orientation_.w()*((input-origin_).dot_product(orientation_.w()));
  float u = (projection - origin_).dot_product( orientation_.u() );
  float v = (projection - origin_).dot_product( orientation_.v() );

  // compute the height field at that point
  float w = coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;

  // compute the points' height above UV
  float pt_height = (input - projection).dot_product( orientation_.w() );

  // return the difference in heights
  return fabs( pt_height-w );
}

/**
 * evaluate surface - this will return the point for the surface at the
 * given (u,v) parameteric location.
 */
Point
HeightSurface::evaluateSurface( const float &u, const float &v ) const
{
  // compute the height field at that point
  float w = coefficients_[0] + coefficients_[1]*u + coefficients_[2]*v + 
    coefficients_[3]*u*u + coefficients_[4]*u*v + coefficients_[5]*v*v;

  // change the basis back into world coordinates
  return origin_ + orientation_.u()*u + orientation_.v()*v + orientation_.w()*w;
//   OrthoNormalBasis world( Vector(1,0,0), Vector(0,1,0), Vector(0,0,1) );
//   return Point(u,v,w).changeBasis( orientation_,world )+origin_;
}
Vector
HeightSurface::evaluateNormal( const float &u, const float &v ) const
{
  // compute the normal at the given UV
  float scaleU = coefficients_[1] + 2.0*coefficients_[3]*u + coefficients_[4]*v;
  float scaleV = coefficients_[2] + coefficients_[4]*u + 2.0*coefficients_[5]*v;
  Vector partialU = orientation_.u() + orientation_.w()*scaleU;
  Vector partialV = orientation_.v() + orientation_.w()*scaleV;
  return partialU.unit_cross_product(partialV);
}

/**
 * mean residual - computes the average residual of the point cloud.
 */
float		
HeightSurface::meanResidual( const std::vector< Point > &cloud ) const
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
HeightSurface::maxResidual( const std::vector< Point > &cloud ) const
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
HeightSurface::medianResidual( const std::vector< Point > &cloud ) const
{
  return kthOrderResidual( 0.5,cloud );
}

/**
 * kth order residual - computes the kth order value of the residuals.
 */
float		
HeightSurface::kthOrderResidual( const float &percent, 
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

/**
 * comparison helper function for the sorting of the nth element in
 * the kthOrderResidual with partial sort function.
 */
bool
ResidualCompare( const std::pair< float,Point > &a,
		 const std::pair< float,Point > &b )
{
  return a.first < b.first;
}

/**
 * kth order residual - computes the kth order value of the residuals.
 */
float		
HeightSurface::kthOrderResidual_PartialSort( const float &percent, 
					     std::vector< Point > &cloud ) const
{
  // if empty...
  if (cloud.size() == 0)
    return 0;

  // compute the list of residuals
  std::vector< std::pair<float,Point> > residuals( cloud.size() );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      residuals[i] = std::pair<float,Point>(computeResidual( cloud[i] ),cloud[i]);
    }

  // compute the offset desired - truncate!
  unsigned int offset = (unsigned int)(percent*cloud.size());

  // return the kth order statistics 
  nth_element( residuals.begin(), residuals.begin()+offset, residuals.end(), &ResidualCompare );

  // copy back the information into the cloud
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      cloud[i] = residuals[i].second;
    }

  return residuals[offset].first;
}

/**
 * sort the point cloud based on the residuals and return the offset of 
 * the point that is the first point ABOVE the given value
 */
unsigned int
HeightSurface::sortByResiduals( const float &threshold,
				std::vector< Point > &cloud ) const
{
  // if empty...
  if (cloud.size() == 0)
    return 0;

  // compute the list of residuals
  std::vector< std::pair<float,Point> > residuals( cloud.size() );
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      residuals[i] = std::pair<float,Point>(computeResidual( cloud[i] ),cloud[i]);
    }

  // sort the residual pairs
  sort( residuals.begin(), residuals.end(), &ResidualCompare );

  // copy back to the cloud
  unsigned int returnOffset = cloud.size()-1;
  bool foundFlag = false;
  for(unsigned int i=0; i<cloud.size(); i++)
    {
      cloud[i] = residuals[i].second;
      if (!foundFlag && residuals[i].first > threshold)
	{
	  returnOffset = i;
	  foundFlag = true;
	}
    }

  // return the offset
  return returnOffset;
}

/**
 * print out the surface information
 */
void
HeightSurface::print() const
{
  fprintf(stderr," (*) origin_ = (%g,%g,%g)\n",origin_.x(),origin_.y(),origin_.z());
  fprintf(stderr," (*) orientation_.u() = (%g,%g,%g)\n",orientation_.u().x(),orientation_.u().y(),orientation_.u().z());
  fprintf(stderr," (*) orientation_.w() = (%g,%g,%g)\n",orientation_.w().x(),orientation_.w().y(),orientation_.w().z());
  fprintf(stderr," (*) orientation_.v() = (%g,%g,%g)\n",orientation_.v().x(),orientation_.v().y(),orientation_.v().z());
  fprintf(stderr," (*) coefficients -> (%g) + (%g)*u + (%g)*v + (%g)*u*u + (%g)*u*v + (%g)*v*v\n",coefficients_[0],coefficients_[1],coefficients_[2],coefficients_[3],coefficients_[4],coefficients_[5]);
}

/**
 * compute the laplacian for the height surface
 */
float
HeightSurface::computeLaplacian() const
{
  return 2*coefficients_[3] + 2*coefficients_[5];
}
