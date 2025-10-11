/**
 *
 * file:	Primitive_Functions.C
 * description:	Defines some common functions used on the
 *		primitive classes.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "Primitive_Functions.h"

/**
 * pca analysis - get back the eigen vectors for a set of points.
 */
std::vector< Eigen_Pair >
pca_analysis( const std::vector< Point > &pts )
{
  // commonly used variables
  unsigned int size = pts.size();

  // find the average values for each of the point dimensions
  Vector average(0,0,0);
  for( unsigned int i=0; i<size; i++ )
    {
      average.x() += pts[i].x();
      average.y() += pts[i].y();
      average.z() += pts[i].z();
    }
  average /= size;

  // subtract the mean from each of the points into a new set
  // and create the covariance matrix from the new points
  Matrix_3x3 covariance(0);
  for( unsigned int i=0; i<size; i++ )
    {
      // subtract the average and store into the new set
      Point new_pt = pts[i] - average;

      // setup the covariance matrix
      covariance(0,0) += new_pt.x()*new_pt.x();
      covariance(0,1) += new_pt.x()*new_pt.y();
      covariance(0,2) += new_pt.x()*new_pt.z();

      covariance(1,0) += new_pt.y()*new_pt.x();
      covariance(1,1) += new_pt.y()*new_pt.y();
      covariance(1,2) += new_pt.y()*new_pt.z();

      covariance(2,0) += new_pt.z()*new_pt.x();
      covariance(2,1) += new_pt.z()*new_pt.y();
      covariance(2,2) += new_pt.z()*new_pt.z();
    }
  covariance /= size-1;

  // compute the eigen pairs of the covariance matrix
  std::vector< Eigen_Pair > eigens(3);
  covariance.eigen_analysis( eigens[0],eigens[1],eigens[2] );

  // return the results
  return eigens;
}

/**
 * pca analysis - get back the eigen vectors for a set of color
 * points.  Creates a vector of points from the given vector and
 * calls the other pca analysis function
 */
std::vector< Eigen_Pair > 
pca_analysis( const std::vector< Color_Point > &pts )
{
  // create the vector of points
  std::vector< Point > new_pts( pts.size() );
  for( unsigned int i=0; i<pts.size(); i++)
    {
      new_pts[i] = pts[i].point_;
    }

  // return the pca_analysis overloaded function
  return pca_analysis( new_pts );
}

/**
 * pca analysis - get back the eigen vectors for a set of vertices.
 * This function builds a vector of points from the vertices and 
 * calls the overloaded pca analysis function.
 */
std::vector< Eigen_Pair > 
pca_analysis( const std::vector< Vertex > &verts )
{
  // create the vector of points
  std::vector< Point > new_pts( verts.size() );
  for( unsigned int i=0; i<verts.size(); i++ )
    {
      new_pts[i] = verts[i].point_;
    }

  // return the pca_analysis overloaded function
  return pca_analysis( new_pts );
}

/**
 * weighted pca analysis - get back the eigen vectors for a set of points.
 */
std::vector< Eigen_Pair >
weighted_pca_analysis( const std::vector< Point > &pts,
		       std::vector< float > &weights )
{
  // make sure that the weights are the same size
  if (weights.size() != pts.size())
    {
      fprintf(stderr," [WARNING] weighted_pca_analysis() -- number of weights does not match number of points\n");

      // if too few add a few more weights (zero in value)
      for (unsigned int i=weights.size(); i<pts.size(); i++)
	{
	  weights.push_back( 0.0 );	// add an extra zero weight
	}
    }

  // commonly used variables
  unsigned int size = pts.size();

  // find the weighted average values for each of the point dimensions
  Vector average(0,0,0);
  float weight_sum = 0;
  for( unsigned int i=0; i<size; i++ )
    {
      average.x() += pts[i].x()*weights[i];
      average.y() += pts[i].y()*weights[i];
      average.z() += pts[i].z()*weights[i];
      weight_sum += weights[i];
    }
  if (!EQL(weight_sum,0.0))
    average /= weight_sum;

  // subtract the mean from each of the points into a new set
  // and create the covariance matrix from the new points
  Matrix_3x3 covariance(0);
  for( unsigned int i=0; i<size; i++ )
    {
      // subtract the average and store into the new set
      Point new_pt = pts[i] - average;

      // setup the covariance matrix
      covariance(0,0) += new_pt.x()*new_pt.x()*weights[i];
      covariance(0,1) += new_pt.x()*new_pt.y()*weights[i];
      covariance(0,2) += new_pt.x()*new_pt.z()*weights[i];

      covariance(1,0) += new_pt.y()*new_pt.x()*weights[i];
      covariance(1,1) += new_pt.y()*new_pt.y()*weights[i];
      covariance(1,2) += new_pt.y()*new_pt.z()*weights[i];

      covariance(2,0) += new_pt.z()*new_pt.x()*weights[i];
      covariance(2,1) += new_pt.z()*new_pt.y()*weights[i];
      covariance(2,2) += new_pt.z()*new_pt.z()*weights[i];
    }
  if (!EQL(weight_sum,0.0))
    covariance /= ((size-1)*weight_sum);

  // compute the eigen pairs of the covariance matrix
  std::vector< Eigen_Pair > eigens(3);
  covariance.eigen_analysis( eigens[0],eigens[1],eigens[2] );

  // return the results
  return eigens;
}

/**
 * pca analysis - get back the eigen vectors for a set of color
 * points.  Creates a vector of points from the given vector and
 * calls the other pca analysis function
 */
std::vector< Eigen_Pair > 
weighted_pca_analysis( const std::vector< Color_Point > &pts,
		       std::vector< float > &weights )
{
  // create the vector of points
  std::vector< Point > new_pts( pts.size() );
  for( unsigned int i=0; i<pts.size(); i++)
    {
      new_pts[i] = pts[i].point_;
    }

  // return the pca_analysis overloaded function
  return weighted_pca_analysis( new_pts,weights );
}

/**
 * pca analysis - get back the eigen vectors for a set of vertices.
 * This function builds a vector of points from the vertices and 
 * calls the overloaded pca analysis function.
 */
std::vector< Eigen_Pair > 
weighted_pca_analysis( const std::vector< Vertex > &verts,
		       std::vector< float > &weights )
{
  // create the vector of points
  std::vector< Point > new_pts( verts.size() );
  for( unsigned int i=0; i<verts.size(); i++ )
    {
      new_pts[i] = verts[i].point_;
    }

  // return the pca_analysis overloaded function
  return weighted_pca_analysis( new_pts,weights );
}

/**
 * pca analysis - computes the weights as being the squared distance
 * to the origin point and returns the eigen values.
 */
std::vector< Eigen_Pair > 
weighted_pca_analysis( const Point &origin,
		       const std::vector< Point > &neighbors )
{
  // compute the weights and the weighted average point
  std::vector< float > weights( neighbors.size() );
  for (unsigned int i=0; i<neighbors.size(); i++)
    {
      // the weight is the inverse of the distance to the fourth
      weights[i] = origin.distance_squared( neighbors[i] );
      weights[i] = 1.0 / weights[i];
    }

  // 2. compute the weighted pca analysis
  return weighted_pca_analysis( neighbors,weights );  
}

/**
 * pca unit normal - returns the unit normal approximated via PCA
 * for the collection of 3d point locations.  
 */
Vector 
pca_unit_normal( const std::vector< Point > &pts )
{
  // call the pca_analysis
  std::vector< Eigen_Pair > eigens = pca_analysis( pts );

  // the jacobi iterations rotate the eigen vectors together
  // in practice this has meant that the normal is always the third
  // eigen vector...if we find this not to be true, then we can 
  // use the commented code that performs cross products.
  return eigens[2].vector_.unit_vector();

//   // if all three exist return unit normal of smallest
//   if (eigens[2].value_ > 0 && eigens[2].vector_.length_squared() > 0)
//     {
//       return eigens[2].vector_.unit_vector();
//     }
//   else if (eigens[0].value_ > 0 && eigens[1].vector_.length_squared() > 0 && 
// 	   eigens[1].value_ > 0 && eigens[1].vector_.length_squared() > 0)
//     {
//       // evaluate the cross product of the first two
//       return eigens[1].vector_.unit_cross_product( eigens[0].vector_ );
//     }
//   else
//     {
//       fprintf(stderr,"[ERROR] pca_unit_normal() -- failure for eigen analysis.\n");
//       return Vector(0,0,1);
//     }
}

/**
 * pca unit normal - sets up a list of points from the color points
 * and returns the call to the overloaded version of pca_unit_normal.
 */
Vector 
pca_unit_normal( const std::vector< Color_Point > &pts )
{
  // create the vector of points
  std::vector< Point > new_pts( pts.size() );
  for( unsigned int i=0; i<pts.size(); i++)
    {
      new_pts[i] = pts[i].point_;
    }

  // return the pca_unit_normal overloaded function
  return pca_unit_normal( new_pts );
}

/**
 * pca unit normal - sets up a list of points from the vertices and
 * returns the call to the overloaded version of pca_unit_normal.
 */
Vector 
pca_unit_normal( const std::vector< Vertex > &verts )
{
  // create the vector of points
  std::vector< Point > new_pts( verts.size() );
  for( unsigned int i=0; i<verts.size(); i++ )
    {
      new_pts[i] = verts[i].point_;
    }

  // return the pca_unit_normal overloaded function
  return pca_unit_normal( new_pts );
}

/**
 * mls projection linear -- this function will project a given point to the MLS 
 * approximated surface.  This projected point is returned.  The MLS surface
 * will be approximated with a planar function for this linear solution.
 */
Point
mls_projection_linear( const Point &pt,
		       const std::vector< Point > &pt_cloud )
{
  // 1. compute a subset of the points near to pt
  // std::vector< Point > neighbors = pt_cloud.nearest_k_neighbors( pt,2000 );
  std::vector< Point > neighbors = pt_cloud;

  // 2. compute the weighted pca analysis of the k neighbors
  std::vector< float > weights( neighbors.size() );
  Point weighted_avg(0,0,0);
  float weight_sum = 0;
  for (unsigned int i=0; i<neighbors.size(); i++)
    {
      // the weight is the inverse of the distance to the fourth
      weights[i] = pt.distance_squared( neighbors[i] )+0.001;
      // weights[i] *= weights[i];
      weights[i] = 1.0 / weights[i];

      weighted_avg += neighbors[i]*weights[i];
      weight_sum += weights[i];
    }
  weighted_avg /= weight_sum;

  // 3. compute the weighted pca analysis
  std::vector< Eigen_Pair > eigens = weighted_pca_analysis( neighbors,weights );  
  eigens[2].vector_.normalize();	// normalize the normal vector

  // 4. project the point onto the pca plane through the weighted avg
  Point projection = pt - eigens[2].vector_*((pt-weighted_avg).dot_product(eigens[2].vector_));

  // 5. compute a linear surface through the points near the projection
  weighted_avg = Point(0,0,0);
  weight_sum = 0;
  for( unsigned int i=0; i<neighbors.size(); i++ )
    {
      // the weight is the inverse of the distance to the fourth
      weights[i] = projection.distance_squared( neighbors[i] )+0.001;
      // weights[i] *= weights[i];
      weights[i] = 1.0 / weights[i];

      weighted_avg += neighbors[i]*weights[i];
      weight_sum += weights[i];
    }
  weighted_avg /= weight_sum;
  eigens = weighted_pca_analysis( neighbors,weights );  
  eigens[2].vector_.normalize();	// normalize the normal vector

  // 6. project the point onto this new surface
  return projection - eigens[2].vector_*((projection-weighted_avg).dot_product(eigens[2].vector_));
}

/**
 * mls projection quadratic -- this function will project a given point to the MLS 
 * approximated surface.  This projected point is returned.  The MLS surface
 * will be approximated with a quadratic weighted least squares fit to the 
 * data points close in proximity to the given point's projection onto the weighted
 * pca plane.
 */
Point
mls_projection_quadratic1( const Point &pt,
			  const std::vector< Point > &pt_cloud )
{
  // 1. compute the weighted pca analysis of the k pt_cloud
  std::vector< float > weights( pt_cloud.size() );
  Point weighted_avg(0,0,0);
  float weight_sum = 0;
  for (unsigned int i=0; i<pt_cloud.size(); i++)
    {
      // the weight is the inverse of the distance to the fourth
      weights[i] = pt.distance_squared( pt_cloud[i] )+0.001;
      // weights[i] *= weights[i];
      weights[i] = 1.0 / weights[i];

      weighted_avg += pt_cloud[i]*weights[i];
      weight_sum += weights[i];
    }
  weighted_avg /= weight_sum;

  // 2. compute the weighted pca analysis
  std::vector< Eigen_Pair > eigens = weighted_pca_analysis( pt_cloud,weights );  
  eigens[0].vector_.normalize();	// normalize the normal vector
  eigens[1].vector_.normalize();	// normalize the normal vector
  eigens[2].vector_.normalize();	// normalize the normal vector

  // 3. project the point onto the pca plane through the weighted avg
  Point projection = pt - eigens[2].vector_*((pt-weighted_avg).dot_product(eigens[2].vector_));

  // 4. compute a quadratic surface through the points near the projection
  QuadraticSurface mlsSurface;
  OrthoNormalBasis orientation( eigens[0].vector_, eigens[1].vector_, eigens[2].vector_ );
  mlsSurface.fitSurface( projection, orientation, pt_cloud );

  // 5. compute closest point on surface to the input point --> sigma(0,0)
  return mlsSurface.evaluateSurface(0,0);
}
Point
mls_projection_quadratic2( const Point &pt,
			  const std::vector< Point > &pt_cloud )
{
  // 1. compute the weighted pca analysis of the k pt_cloud
  std::vector< float > weights( pt_cloud.size() );
  Point weighted_avg(0,0,0);
  float weight_sum = 0;
  for (unsigned int i=0; i<pt_cloud.size(); i++)
    {
      // the weight is the inverse of the distance to the fourth
      weights[i] = pt.distance_squared( pt_cloud[i] )+0.001;
      // weights[i] *= weights[i];
      weights[i] = 1.0 / weights[i];

      weighted_avg += pt_cloud[i]*weights[i];
      weight_sum += weights[i];
    }
  weighted_avg /= weight_sum;

  // 2. compute the weighted pca analysis
  std::vector< Eigen_Pair > eigens = weighted_pca_analysis( pt_cloud,weights );  
  eigens[0].vector_.normalize();	// normalize the normal vector
  eigens[1].vector_.normalize();	// normalize the normal vector
  eigens[2].vector_.normalize();	// normalize the normal vector

  // 3. project the point onto the pca plane through the weighted avg
  Point projection = pt - eigens[2].vector_*((pt-weighted_avg).dot_product(eigens[2].vector_));

  // 4. compute a quadratic surface through the points near the projection
  HeightSurface mlsSurface;
  OrthoNormalBasis orientation( eigens[0].vector_, eigens[1].vector_, eigens[2].vector_ );
  mlsSurface.fitSurface( projection, orientation, pt_cloud );

  // 5. compute closest point on surface to the input point --> sigma(0,0)
  return mlsSurface.evaluateSurface(0,0);
}

HeightSurface
mls_fit_surface( const Point &pt,
		 const std::vector< Point > &pt_cloud )
{
  // 1. compute the weighted pca analysis of the k pt_cloud
  std::vector< float > weights( pt_cloud.size() );
  Point weighted_avg(0,0,0);
  float weight_sum = 0;
  for (unsigned int i=0; i<pt_cloud.size(); i++)
    {
      // the weight is the inverse of the distance to the fourth
      weights[i] = pt.distance_squared( pt_cloud[i] )+0.001;
      weights[i] = 1.0 / weights[i];

      weighted_avg += pt_cloud[i]*weights[i];
      weight_sum += weights[i];
    }
  weighted_avg /= weight_sum;

  // 2. compute the weighted pca analysis
  std::vector< Eigen_Pair > eigens = weighted_pca_analysis( pt_cloud,weights );  
  eigens[0].vector_.normalize();	// normalize the normal vector
  eigens[1].vector_.normalize();	// normalize the normal vector
  eigens[2].vector_.normalize();	// normalize the normal vector

  // 3. project the point onto the pca plane through the weighted avg
  Point projection = pt - eigens[2].vector_*((pt-weighted_avg).dot_product(eigens[2].vector_));

  // 4. compute a quadratic surface through the points near the projection
  HeightSurface mlsSurface;
  OrthoNormalBasis orientation( eigens[0].vector_, eigens[1].vector_, eigens[2].vector_ );
  mlsSurface.fitSurface( projection, orientation, pt_cloud );

  // 5. compute closest point on surface to the input point --> sigma(0,0)
  return mlsSurface;
}

/**
 * allow the user to give a desired weighting level and use it to 
 * influence the mls surface fit.
 */
Point weighted_mls_projection_quadratic( const Point &pt, 
					 const std::vector< Point > &pt_cloud, 
					 std::vector< float > &weights )
{
  // make sure that the weights are the same size
  if (weights.size() != pt_cloud.size())
    {
      fprintf(stderr," [WARNING] weighted_mls_projection_quadratic() -- number of weights does not match number of points\n");

      // if too few add a few more weights (zero in value)
      for (unsigned int i=weights.size(); i<pt_cloud.size(); i++)
	{
	  weights.push_back( 0.0 );	// add an extra zero weight
	}
    }

  // 1. compute the weighted pca analysis of the k pt_cloud
  Point weighted_avg(0,0,0);
  float weight_sum = 0;
  for (unsigned int i=0; i<pt_cloud.size(); i++)
    {
      weighted_avg += pt_cloud[i]*weights[i];
      weight_sum += weights[i];
    }
  weighted_avg /= weight_sum;

  // 2. compute the weighted pca analysis
  std::vector< Eigen_Pair > eigens = weighted_pca_analysis( pt_cloud,weights );  
  eigens[0].vector_.normalize();	// normalize the normal vector
  eigens[1].vector_.normalize();	// normalize the normal vector
  eigens[2].vector_.normalize();	// normalize the normal vector

  // 3. project the point onto the pca plane through the weighted avg
  Point projection = pt - eigens[2].vector_*((pt-weighted_avg).dot_product(eigens[2].vector_));

  // 4. compute a quadratic surface through the points near the projection
  HeightSurface mlsSurface;
  OrthoNormalBasis orientation( eigens[0].vector_, eigens[1].vector_, eigens[2].vector_ );
  mlsSurface.weightedFitSurface( projection, orientation, pt_cloud, weights );

  // 5. compute closest point on surface to the input point --> sigma(0,0)
  return mlsSurface.evaluateSurface(0,0);
}

/**
 * plane intersect -- computes the intersection of the two planes 
 * described by the normal and point information passed.
 */
Point
planeIntersect( const Point &projectMe,
		const Point &pointA, const Vector &normalA,
		const Point &pointB, const Vector &normalB )
{
  // compute the direction of the intersection line
  Vector lineNormal = normalA.cross_product( normalB );

  // check if planes are parallel
  float denominator = lineNormal.length_squared();
  if ( EQL(denominator,0.0) )
    {
      fprintf(stderr,"[WARNING]: the two planes are parallel!\n");
      return (pointA+pointB)/2.0;
    }

  // compute the point on the intersection line
  float planeA_d = normalA.dot_product( Vector(pointA) );
  float planeB_d = normalB.dot_product( Vector(pointB) );
  Point linePoint = Point( (normalB*planeA_d - normalA*planeB_d).cross_product(lineNormal) / denominator );

  // compute the closest point on the line segment to given point
  float time = (projectMe-linePoint).dot_product( lineNormal ) / denominator;
  return linePoint + lineNormal*time;
}

/**
 * rmls projection_2_feature -- this function ASSUMES that the point
 * is near a feature and will perform RMLS growing of neighborhood to
 * compute the surfaces that approximate the points in the
 * neighborhood.  returns the point to the nearest intersection
 * between multiple surfaces fit to the neighbors with rmls.  the
 * weights given indicate how 'good' a point is, based on the RMLS method
 * for extracting potential feature points (0 being not a feature and 
 * 1 being definitely a feature)
 */
Weighted_Point
rmls_projection_2_feature( const Point &project_pt,
			   const std::vector< Point > &neighbors,
			   const float &noiseThreshold )
{
  // 0. copy neighbors to the remaining neighbors
  std::vector< Point > remainingNeighbors = neighbors;

  // 1. get smoothest point from remainingNeighbors
  Point startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

  // 2. fit the first surface growing from the smoothest point
  HeightSurface surface1 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );

  // if there are enough points remaining 
  HeightSurface surface2;
  if( remainingNeighbors.size() > INIT_RMLS_FIT_SIZE )
    {
      // 3. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

      // 4. fit the second surface growing from the next smoothest point
      surface2 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );
    }
  // no points to describe a second surface!
  else if (remainingNeighbors.size() == 0)
    {
      Weighted_Point returnPt;
      returnPt.point_ = surface1.projectPoint(project_pt);
      returnPt.weight_ = 0;
      returnPt.nearCorner_ = false;
      returnPt.normals_.push_back( surface1.projectNormal(project_pt) );
      return returnPt;
    }
  // otherwise just if the surface to the remaining
  else
    {
      surface2.fitSurface(remainingNeighbors);
      remainingNeighbors.clear();
    }

  // 5. if points remain fit third surface and set flag
  HeightSurface surface3;
  bool surface3_flag = false;
  if (remainingNeighbors.size() > INIT_RMLS_FIT_SIZE )
    {
      // a. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors, noiseThreshold);

      // b. fit the second surface growing from the next smoothest point
      surface3 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );
      surface3_flag = true;

      // c. compute projection onto surface1
      Point project1;
      Vector normal1;
      surface1.projectPoint( project_pt, project1,normal1 );

      // d. compute projection onto surface2
      Point project2;
      Vector normal2;
      surface2.projectPoint( project_pt, project2,normal2 );

      // e. compute projection onto surface3
      Point project3;
      Vector normal3;
      surface3.projectPoint( project_pt, project3,normal3 );

      // f. check that the third surface is useful!
      if ( fabs(normal3.unit_vector().dot_product(normal2.unit_vector())) > 0.9 &&
	   project3.distance_squared(project_pt) < project2.distance_squared(project_pt) )
	{
	  surface2 = surface3;
	} 
      else if ( fabs(normal3.unit_vector().dot_product(normal1.unit_vector())) > 0.9 &&
	   project3.distance_squared(project_pt) < project1.distance_squared(project_pt) )
	{
	  surface1 = surface3;
	}
      else if ( fabs(normal1.unit_vector().dot_product(normal2.unit_vector())) > 0.9 &&
	   project1.distance_squared(project_pt) < project2.distance_squared(project_pt) )
	{
	  surface2 = surface3;
	}
      else if ( fabs(normal1.unit_vector().dot_product(normal2.unit_vector())) > 0.9 &&
	   project2.distance_squared(project_pt) < project1.distance_squared(project_pt) )
	{
	  surface1 = surface3;
	}
    }

  // 6. compute closest projected point until at intersection
  Weighted_Point returnPoint;
  returnPoint.nearCorner_ = surface3_flag;
  returnPoint.point_ = project_pt;

  unsigned int count = 0;
  float movedDistance = 5;
  Point project1,project2;
  Vector normal1(1,0,0),normal2(0,1,0);
  while( count < 50 && movedDistance > MOVE_EPSILON )
    {
      // a. compute projection onto surface1
      surface1.projectPoint( returnPoint.point_, project1,normal1 );

      // b. compute projection onto surface2
      surface2.projectPoint( returnPoint.point_, project2,normal2 );

      // c. move point to intersection of the tangent planes -- loop
      Point movePoint = planeIntersect( returnPoint.point_, project1,normal1, project2,normal2 );
      movedDistance = returnPoint.point_.distance_squared( movePoint );
      returnPoint.point_ = movePoint;

      // sanity counter 
      count++;
    }

  // 7. compute the weight for the point and save the normals
  returnPoint.weight_ = (normal1.unit_vector().cross_product(normal2.unit_vector())).length_squared();
  returnPoint.normals_.push_back( normal1 );
  returnPoint.normals_.push_back( normal2 );

  // 8. return the return point
  return returnPoint;
}

/** 
 * helper function used for the priority queue.
 */
bool residualCompare( const std::pair< float,unsigned int > &a,
		      const std::pair< float,unsigned int > &b )
{
  return a.first > b.first;
}

/**
 * rmls fit surface grows a neighborhood from the given start point
 * and returns the surface that it fit to that neighborhood. also, it 
 * will remove the points used to fit the surface from the passed remaining
 * neighbor point vector.
 */
HeightSurface
rmls_fit_surface( const Point &startPoint,
		  std::vector< Point > &remainingNeighbors,
		  const float &noiseThreshold )
{
  if(remainingNeighbors.size() == 0)
    fprintf(stderr,"[WARNING] HeightSurface::rmls_fit_surface() -- empty neighborhood!\n");

  // 0. create a point cloud
  PointCloud tmpCloud( remainingNeighbors );

  // 1. sort the neighborhood by distance to the start point
  remainingNeighbors = tmpCloud.kNN( startPoint, tmpCloud.vertices.size() );
  //  fprintf(stderr,"sorted.");
 
  // 2. grab a subset of the neighbors that lie on a smooth region near the start pt
  std::vector< Point > fitNeighbors( INIT_RMLS_FIT_SIZE );
  fitNeighbors.clear();
  std::vector< Point >::iterator N_iter = remainingNeighbors.begin();
  for(unsigned int i=0; i<INIT_RMLS_FIT_SIZE && N_iter!=remainingNeighbors.end(); i++)
    {
      fitNeighbors.push_back(remainingNeighbors[i]);
      N_iter++;
    }


  remainingNeighbors.erase( remainingNeighbors.begin(), N_iter );
  //  fprintf(stderr,"initNeighbors.");

  // 3. fit a surface to these points growing the 1st neighborhood
  HeightSurface surfaceFit;
  surfaceFit.fitSurface( fitNeighbors );
  //  fprintf(stderr,"fit.");

  // partial sort based on residual
  bool doneFlag = false;
  while( remainingNeighbors.size() > 0 && !doneFlag)
    {
      // sort the neighbors by residual
      unsigned int offset = surfaceFit.sortByResiduals(noiseThreshold,remainingNeighbors);

      // check if we can grow the neighborhood
      if( offset > 0 )
	{
	  //	  fprintf(stderr,"growOffset(%d).",offset);

	  // add up to 20% new fit neighbors and remove from remaining
	  N_iter = remainingNeighbors.begin();
	  unsigned int count=0;
	  for(unsigned int i=0; i<.2*fitNeighbors.size() && i<offset; i++)
	    {
	      fitNeighbors.push_back( remainingNeighbors[i] );
	      N_iter++;
	      count++;
	    }
	  //	  fprintf(stderr,"added(%d).",count);

	  // remove the added fit neighbors from the remaining neighbor set
	  remainingNeighbors.erase( remainingNeighbors.begin(), N_iter );

	  // refit the new surface to these neighbors
	  surfaceFit.fitSurface( fitNeighbors );
	  //	  fprintf(stderr,"refit.");

	  // remove the neighbors with too high of a residual
	  std::vector< std::pair<float,unsigned int> > residuals( fitNeighbors.size() );
	  for(unsigned int i=0; i<fitNeighbors.size(); i++)
	    {
	      residuals[i] = std::pair<float,unsigned int> (surfaceFit.computeResidual(fitNeighbors[i]),i);
	    }

	  // sort the points be residuals
	  std::sort( residuals.begin(), residuals.end(), residualCompare );

	  // release the points above some threshold up to 20%
	  std::vector<Point> tmpNeighbors;
	  unsigned int count2=0;
	  for(unsigned int i=0; i<fitNeighbors.size(); i++)
	    {
	      if (residuals[i].first >= noiseThreshold && i+1<count)
		{
		  remainingNeighbors.push_back( fitNeighbors[i] );
		  count2++;
		}
	      else
		tmpNeighbors.push_back( fitNeighbors[i] );
	    }
	  fitNeighbors.resize( tmpNeighbors.size() );
	  for(unsigned int i=0; i<tmpNeighbors.size(); i++)
	    {
	      fitNeighbors[i] = tmpNeighbors[i];
	    }
	  //	  fprintf(stderr,"remove(%d).",count2);

	  // refit the new surface to these neighbors
	  //	  fprintf(stderr,"size(%d).",fitNeighbors.size());
	  surfaceFit.fitSurface( fitNeighbors );
	  //	  fprintf(stderr,"refit.");
	}
      // otherwise didn't add any so quit
      else
	{
	  doneFlag = true;
	}
    }

  // 4. return the surface that was fit to the neighbors
  return surfaceFit;
}

/**
 * computes the "smoothest" point from the neighborhood, based on the
 * potential features in a small neighborhood of 12.
 */
Point
rmls_smoothest_point( const std::vector< Point > &neighbors,
		      const float &noiseThreshold )
{
  if (neighbors.size() == 0)
    {
      fprintf(stderr,"[WARNING] rmls_smoothest_point() -- zero neighbors exist!\n");
      return Point(0,0,0);
    }
  if(neighbors.size() <= INIT_RMLS_FIT_SIZE )
    {
      return neighbors[0];
    }

  PointCloud cloud( neighbors );

  // loop through all points and save the minimal 
  unsigned int minOffset=0;
  float minWeight=-1;
  float allowance = noiseThreshold/2.0;
  for(unsigned int i=0; i<neighbors.size(); i++)
    {
      // fit a surface to the local neighborhood
      HeightSurface theFit;
      std::vector< Point > localNeighbors = cloud.kNN( neighbors[i],INIT_RMLS_FIT_SIZE );
      theFit.fitSurface( localNeighbors );

      // compute the weight for the i'th point
      float theWeight = theFit.maxResidual( localNeighbors );
      if (theWeight < minWeight || minWeight < 0)
	{
	  if (theWeight <= allowance) return neighbors[i];

	  minWeight = theWeight;
	  minOffset = i;
	}
    }

  // return the min weight
  return neighbors[minOffset];
}

/**
 * rmls projection_2_surface -- this function ASSUMES that the point is near a feature
 * and will perform RMLS growing of neighborhood to compute the surfaces that approximate
 * the points in the neighborhood. returns the projection of the point to the nearest 
 * surface fit using rmls.
 */
Vertex
rmls_projection_2_surface( const Point &projectPt,
			   const std::vector< Point > &neighbors,
			   const float &noiseThreshold )
{
  // 0. copy neighbors to the remaining neighbors
  std::vector< Point > remainingNeighbors = neighbors;

  // 1. get smoothest point from remainingNeighbors
  Point startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

  // 2. fit the first surface growing from the smoothest point
  HeightSurface surface1 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );

  // if there are enough points remaining 
  HeightSurface surface2;
  if( remainingNeighbors.size() > INIT_RMLS_FIT_SIZE )
    {
      // 3. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

      // 4. fit the second surface growing from the next smoothest point
      surface2 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );
    }
  // otherwise just if the surface to the remaining
  else
    {
      surface2.fitSurface(remainingNeighbors);
      remainingNeighbors.clear();
    }

  // 5. if points remain fit third surface and set flag
  HeightSurface surface3;
  bool surface3_flag = false;
  if (remainingNeighbors.size() > INIT_RMLS_FIT_SIZE)
    {
      // a. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

      // b. fit the second surface growing from the next smoothest point
      surface3 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );
      surface3_flag = true;

      // c. compute projection onto surface1
      Point project1;
      Vector normal1;
      surface1.projectPoint( projectPt, project1,normal1 );

      // d. compute projection onto surface2
      Point project2;
      Vector normal2;
      surface2.projectPoint( projectPt, project2,normal2 );

      // e. compute projection onto surface3
      Point project3;
      Vector normal3;
      surface3.projectPoint( projectPt, project3,normal3 );

      // f. check that the third surface is useful!
      if ( fabs(normal3.unit_vector().dot_product(normal2.unit_vector())) > 0.9 &&
	   project3.distance_squared(projectPt) < project2.distance_squared(projectPt) )
	{
	  surface2 = surface3;
	  surface3_flag = false;
	} 
      else if ( fabs(normal3.unit_vector().dot_product(normal1.unit_vector())) > 0.9 &&
	   project3.distance_squared(projectPt) < project1.distance_squared(projectPt) )
	{
	  surface1 = surface3;
	  surface3_flag = false;
	}
      else if ( fabs(normal1.unit_vector().dot_product(normal2.unit_vector())) > 0.9 &&
	   project1.distance_squared(projectPt) < project2.distance_squared(projectPt) )
	{
	  surface2 = surface3;
	  surface3_flag = false;
	}
      else if ( fabs(normal1.unit_vector().dot_product(normal2.unit_vector())) > 0.9 &&
	   project2.distance_squared(projectPt) < project1.distance_squared(projectPt) )
	{
	  surface1 = surface3;
	  surface3_flag = false;
	}
    }

  // 6. compute projection onto surface1
  Point project1 = surface1.projectPoint( projectPt );
  float distance1 = project1.distance_squared( projectPt );

  // 7. compute projection onto surface2
  Point project2 = surface2.projectPoint( projectPt );
  float distance2 = project2.distance_squared( projectPt );

  // 8. save the closer of the two
  if (distance2 < distance1)
    {
      project1 = project2;
      distance1 = distance2;
    }
	  
  // 9. if surface3 exists, then need to consider it!
  if (surface3_flag)
    {
      // c. compute projection onto surface3
      Point project3 = surface3.projectPoint( projectPt );
      float distance3 = project3.distance_squared( projectPt );

      // d. save the closest
      if (distance3 < distance1)
	{
	  project1 = project3;
	  distance1 = distance3;
	}
    }

  // 10. return the return point
  Vertex returnVertex;
  returnVertex.point_ = project1;
  returnVertex.normal_ = projectPt-project1;
  return returnVertex;
}

/**
 * rmls projection_2_corner -- this function ASSUMES that the point
 * is near a feature and will perform RMLS growing of neighborhood to
 * compute the surfaces that approximates the points in the
 * neighborhood.  returns the point to the nearest intersection
 * between multiple surfaces fit to the neighbors with rmls.  the
 * weights given indicate how 'good' a point is, based on the RMLS method
 * for extracting potential feature points (0 being not a feature and 
 * 1 being definitely a feature)
 */
Weighted_Point
rmls_projection_2_corner( const Point &project_pt,
			  const std::vector< Point > &neighbors,
			  const float &noiseThreshold )
{
  Weighted_Point returnPoint;
  if( rmls_projection_2_corner( project_pt,neighbors,noiseThreshold, returnPoint ))
    return returnPoint;
  else
    return rmls_projection_2_feature( project_pt,neighbors, noiseThreshold);
}

/**
 * rmls projection_2_corner -- this function ASSUMES that the point
 * is near a feature and will perform RMLS growing of neighborhood to
 * compute the surfaces that approximates the points in the
 * neighborhood.  returns the point to the nearest intersection
 * between multiple surfaces fit to the neighbors with rmls.  the
 * weights given indicate how 'good' a point is, based on the RMLS method
 * for extracting potential feature points (0 being not a feature and 
 * 1 being definitely a feature)
 */
bool
rmls_projection_2_corner( const Point &project_pt,
			  const std::vector< Point > &neighbors,
			  const float &noiseThreshold,
			  Weighted_Point &returnPoint )
{
  // 0. copy neighbors to the remaining neighbors
  std::vector< Point > remainingNeighbors = neighbors;

  // 1. get smoothest point from remainingNeighbors
  Point startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

  // 2. fit the first surface growing from the smoothest point
  HeightSurface surface1 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );

  // if there are enough points remaining 
  HeightSurface surface2;
  if( remainingNeighbors.size() > INIT_RMLS_FIT_SIZE )
    {
      // 3. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

      // 4. fit the second surface growing from the next smoothest point
      surface2 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );
    }
  // otherwise just if the surface to the remaining
  else
    {
      fprintf(stderr,"[WARNING]: rmls_projection_2_corner(A) with point not near corner!\n");
      fprintf(stderr," -- %d points remaining to fit the third surface!\n",remainingNeighbors.size());
      return false;
    }

  // 5. if points remain fit third surface and set flag
  HeightSurface surface3;
  if (remainingNeighbors.size() > INIT_RMLS_FIT_SIZE )
    {
      // a. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

      // b. fit the second surface growing from the next smoothest point
      surface3 = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );
    }
  // opt out because not near a corner!
  else
    {
      fprintf(stderr,"[WARNING]: rmls_projection_2_corner(B) with point not near corner!\n");
      fprintf(stderr," -- %d points remaining to fit the third surface!\n",remainingNeighbors.size());
      return false;
    }

  // 6. compute closest projected point until at intersection
  returnPoint.point_ = project_pt;
  unsigned int count = 0;
  float movedDistance = 5;
  Point project1,project2,project3;
  Vector normal1(1,0,0),normal2(0,1,0),normal3(0,0,1);
  while( count < 50 && movedDistance > MOVE_EPSILON )
    {
      // a. compute projection onto surface1
      surface1.projectPoint( returnPoint.point_, project1,normal1 );

      // b. compute projection onto surface2
      surface2.projectPoint( returnPoint.point_, project2,normal2 );

      // c. compute projection onto surface3
      surface3.projectPoint( returnPoint.point_, project3,normal3 );

      // d. compute distances to the projections
      float distance1 = project1.distance_squared( returnPoint.point_ );
      float distance2 = project2.distance_squared( returnPoint.point_ );
      float distance3 = project3.distance_squared( returnPoint.point_ );

      // e. compute the closer two planes
      Point closer1P,closer2P;
      Vector closer1N,closer2N;
      unsigned int furthestFlag;
      if (distance1 < distance3 && distance2 < distance3 )
	{
	  closer1P = project1; closer1N = normal1; 
	  closer2P = project2; closer2N = normal2;
	  furthestFlag = 3;
	}
      else if (distance2 < distance1 && distance3 < distance1 )
	{
	  closer1P = project2; closer1N = normal2; 
	  closer2P = project3; closer2N = normal3;
	  furthestFlag = 1;
	}
      else
	{
	  closer1P = project1; closer1N = normal1; 
	  closer2P = project3; closer2N = normal3;
	  furthestFlag = 2;
	}

      // f. move point to intersection of the tangent planes -- loop
      Point movePoint = planeIntersect( returnPoint.point_, closer1P,closer1N, closer2P,closer2N );
     
      // g. project move point onto the other plane
      if (furthestFlag == 1)
	movePoint = surface1.projectPoint( movePoint );
      else if (furthestFlag == 2)
	movePoint = surface2.projectPoint( movePoint );
      else  
	movePoint = surface3.projectPoint( movePoint );

      // h. move the return point and save the distance moved
      movedDistance = returnPoint.point_.distance_squared( movePoint );
      returnPoint.point_ = movePoint;

      // sanity counter 
      count++;
    }

  // 7. compute the weight and save the normals computed for the three faces
  returnPoint.weight_ = 1.0;
  returnPoint.nearCorner_ = true;
  returnPoint.normals_.push_back( normal1 );
  returnPoint.normals_.push_back( normal2 );
  returnPoint.normals_.push_back( normal3 );

  // 7. return the return point
  return true;
}

/**
 * rmls fit surfaces -- this function fits all the possible surfaces
 * to the set of points and will return those surfaces in a vector.
 * The user may do as they please, but it should be noted that some of
 * the surfaces may be overlaying and actual noise, so the user should
 * check the projection normals against the surfaces to make sure that
 * they are each unique.
 */
std::vector< HeightSurface >
rmls_fit_surfaces( const std::vector< Point > &neighbors,
		   const float &noiseThreshold )
{
  // 0. copy neighbors to the remaining neighbors
  std::vector< Point > remainingNeighbors = neighbors;

  // 1. get smoothest point from remainingNeighbors
  Point startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

  // 2. fit the first surface growing from the smoothest point
  std::vector< HeightSurface > returnSurfaces(1);
  returnSurfaces[0] = rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold );

  // 3. if there are enough points remaining grow another surface
  if( remainingNeighbors.size() > INIT_RMLS_FIT_SIZE )
    {
      // a. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

      // b. fit the second surface growing from the next smoothest point
      returnSurfaces.push_back( rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold ) );
    }

  // 4. if points remain fit third surface
  if (remainingNeighbors.size() > INIT_RMLS_FIT_SIZE )
    {
      // a. get smoothest point from the remaining neighbors again
      startPoint = rmls_smoothest_point(remainingNeighbors,noiseThreshold);

      // b. fit the second surface growing from the next smoothest point
      returnSurfaces.push_back( rmls_fit_surface( startPoint,remainingNeighbors, noiseThreshold ) );
    }

  // 5. return the fit surfaces
  return returnSurfaces;
}

/**
 * project_2_curve - this function will project the given point
 * to the quadratic function that estimates the given point cloud.
 */
Point
project_2_curve( const Point &projectPt,
		 const std::vector< Point > &ptCloud )
{
  // 0. compute the centroid of the cloud
  std::vector< float > weights( ptCloud.size() );
  Point weightedCenter(0,0,0);
  float weightSum=0;
  for(unsigned int i=0; i<ptCloud.size(); i++)
    {
      weights[i] = 1.0/(ptCloud[i].distance_squared( projectPt )+0.001);
      weightedCenter += ptCloud[i]*weights[i];
      weightSum += weights[i];
    }
  weightedCenter /= weightSum;

  // 1. compute the weighted PCA and get the principal eigen
  Vector majorAxis = weighted_pca_analysis( ptCloud, weights )[0].vector_;
  majorAxis.normalize();
  //  return weightedCenter + majorAxis*(projectPt-weightedCenter).dot_product(majorAxis);

  // 2. compute the parameters for the points in the cloud
  Matrix_MxN Parameters( ptCloud.size(),3 );
  for(unsigned int i=0; i<ptCloud.size(); i++)
    {
      float u = (ptCloud[i]-weightedCenter).dot_product( majorAxis );
      Parameters(i,0) = weights[i];
      Parameters(i,1) = u*weights[i];
      Parameters(i,2) = u*u*weights[i];
    }

  // 3. fill out the point matrix
  Matrix_MxN PtsX( ptCloud.size(),1 );
  Matrix_MxN PtsY( ptCloud.size(),1 );
  Matrix_MxN PtsZ( ptCloud.size(),1 );
  for(unsigned int i=0; i<ptCloud.size(); i++)
    {
      PtsX(i,0) = ptCloud[i].x()*weights[i];
      PtsY(i,0) = ptCloud[i].y()*weights[i];
      PtsZ(i,0) = ptCloud[i].z()*weights[i];
    }

  // 4. compute the coefficients for the curve
  Matrix_MxN inverse = Parameters.inverse();
  Matrix_MxN c_X = inverse*PtsX;
  Matrix_MxN c_Y = inverse*PtsY;
  Matrix_MxN c_Z = inverse*PtsZ;

  // 5. return the projection of the point
  float U = (projectPt-weightedCenter).dot_product( majorAxis );
  return Point( c_X(0,0) + c_X(1,0)*U + c_X(2,0)*U*U,
		c_Y(0,0) + c_Y(1,0)*U + c_Y(2,0)*U*U,
		c_Z(0,0) + c_Z(1,0)*U + c_Z(2,0)*U*U );
}

/**
 * rmls_projection
 *
 * a quick function to project a point to the nearest rmls 
 * surface.
 */
Vertex 
rmls_projection( const Point &pt,
		 const std::vector< Point > &neighbors,
		 const float &noiseTolerance )
{
  // get the surfaces
  std::vector< HeightSurface > surfs = rmls_fit_surfaces( neighbors, noiseTolerance );

  // project to each and save the closest
  double closestDistance = -1;
  Vertex closestPoint;
  for (unsigned int i=0; i<surfs.size(); i++)
    {
      Point projectPt;
      Vector projectNorm;
      surfs[i].projectPoint( pt, projectPt, projectNorm );
      if (pt.distance_to(projectPt) < closestDistance || closestDistance < 0)
	{
	  closestDistance = pt.distance_to(projectPt);
	  closestPoint.point_ = projectPt;
	  closestPoint.normal_ = projectNorm;
	}
    }

  // return the results
  return closestPoint;
}

/**
 * mls_projection
 * 
 * projects the point to the mls surface and returns the normal
 */
Vertex
mls_projection( const Point &pt,
		const std::vector< Point > &pt_cloud )
{
  // 1. compute the weighted pca analysis of the k pt_cloud
  std::vector< float > weights( pt_cloud.size() );
  Point weighted_avg(0,0,0);
  float weight_sum = 0;
  for (unsigned int i=0; i<pt_cloud.size(); i++)
    {
      // the weight is the inverse of the distance to the fourth
      weights[i] = pt.distance_squared( pt_cloud[i] )+0.001;
      weights[i] = 1.0 / weights[i];

      weighted_avg += pt_cloud[i]*weights[i];
      weight_sum += weights[i];
    }
  weighted_avg /= weight_sum;

  // 2. compute the weighted pca analysis
  std::vector< Eigen_Pair > eigens = weighted_pca_analysis( pt_cloud,weights );  
  eigens[0].vector_.normalize();	// normalize the normal vector
  eigens[1].vector_.normalize();	// normalize the normal vector
  eigens[2].vector_.normalize();	// normalize the normal vector

  // 3. project the point onto the pca plane through the weighted avg
  Point projection = pt - eigens[2].vector_*((pt-weighted_avg).dot_product(eigens[2].vector_));

  // 4. compute a quadratic surface through the points near the projection
  HeightSurface mlsSurface;
  OrthoNormalBasis orientation( eigens[0].vector_, eigens[1].vector_, eigens[2].vector_ );
  mlsSurface.fitSurface( projection, orientation, pt_cloud );

  // 5. compute closest point on surface to the input point --> sigma(0,0)
  Vertex returnValue;
  returnValue.point_ = mlsSurface.evaluateSurface(0,0);
  returnValue.normal_ = mlsSurface.evaluateNormal( 0,0 );
  return returnValue;
}
