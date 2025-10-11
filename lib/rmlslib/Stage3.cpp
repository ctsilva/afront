/**
 *
 * file:	Stage3.cpp
 * description:	Defines the functions used to process the third
 *		stage in the feature extraction steps.  Stage 3
 *		receives a set of points that have been projected 
 *		to the features of the original point cloud.  This
 *		stage tries to clean up these points by smoothing 
 *		noisy regions no dominated by PCA, and resolve
 *		the corner points.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

// -- INCLUDES -- //
#include "Stage3.h"

/**
 * the main function for input into the third stage.  This function
 * smooths the points and resolves corners to give back a better
 * version of the feature projected points.
 */
PointCloud*
stage3_processFeatureCloud( PointCloud *inputCloud,
			    PointCloud *originalCloud,
			    const float &noiseAllowance )
{
  // output entry information
  fprintf(stderr," (*) Stage 3: processing and smoothing feature cloud:\n");
  clock_t start = clock();

  // 1. resolve corner points
  std::vector<Weighted_Point> corners = helper3_resolveCorners( inputCloud,originalCloud,noiseAllowance );

  // 2. smooth out noisy regions through pca
  std::vector<Weighted_Point> regulars = helper3_smoothFeatures( inputCloud,corners );

  // 3. combine the two vectors
  std::vector<Point> points(corners.size()+regulars.size());
  std::vector< std::vector<Vector> > normals(corners.size()+regulars.size());
  std::vector<float> weights(corners.size()+regulars.size());
  for(unsigned int i=0; i<points.size(); i++)
    {
      if (i < corners.size())
	{
	  points[i] = corners[i].point_;
	  weights[i] = 0.75;
	  for(unsigned int j=0; j<corners[i].normals_.size(); j++)
	    normals[i].push_back( corners[i].normals_[j] );
	}
      else
	{
	  points[i] = regulars[i-corners.size()].point_;
	  weights[i] = 1.0;
	  for(unsigned int j=0; j<regulars[i-corners.size()].normals_.size(); j++)
	    normals[i].push_back( regulars[i-corners.size()].normals_[j] );
	}
    }

  // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr," (*) Stage 3: %d corners...%d points...done (%g seconds)\n",corners.size(),points.size(),time2-time1);
  return new PointCloud( points,weights,normals );
}

/**
 * helper function - this will resolve the corner points for the 
 * input cloud set by projecting the feature points and then computing
 * the weighted average of clustered points.
 */
std::vector< Weighted_Point >
helper3_resolveCorners( PointCloud *inputCloud,
			PointCloud *originalCloud,
			const float &noiseAllowance )
{
  // output entry information
  fprintf(stderr,"   (*) part A: resolving corners...");
  clock_t start = clock();

  // 1. find all feature points and project them to their corners
  std::vector< Weighted_Point > projectedCorners;
  std::vector< bool > visitedPoint;
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      // if it is a corner point
      if (inputCloud->verticesNormals[i].size() == 3)
	{
	  // a. get the corner point and its associated weight
	  Weighted_Point corner;
	  corner.point_ = inputCloud->vertices[i];
	  corner.weight_ = inputCloud->verticesWeights[i];
	  corner.normals_ = inputCloud->verticesNormals[i];

	  projectedCorners.push_back( corner );
	  visitedPoint.push_back( false );
	}
    }
  fprintf(stderr,"projected corners (%d)...",projectedCorners.size());

  // 2. compute weighted average subset of corner points by clustering
  std::vector< Weighted_Point > finalCorners;
  for(unsigned int i=0; i<projectedCorners.size(); i++)
    {
      // if haven't processed this point
      if (!visitedPoint[i])
	{
	  // get all neighbors within some threshold distance (k-sphere neighbors)
	  std::vector< unsigned int > neighborIndices(1);
	  neighborIndices[0] = i;
	  for(unsigned int j=i+1; j<projectedCorners.size(); j++)
	    {
	      if (!visitedPoint[j])
		{
		  float distance = projectedCorners[j].point_.distance_squared( projectedCorners[i].point_ );
		  if (distance < CORNER_CLUSTER_DISTANCE*CORNER_CLUSTER_DISTANCE)
		    {
		      neighborIndices.push_back(j);
		      visitedPoint[j] = true;
		    }
		}
	    }

	  // get the average corner point
	  Weighted_Point avgCorner;
	  avgCorner.point_ = Point(0,0,0);
	  float totalWeight = 0;
	  for(unsigned int j=0; j<neighborIndices.size(); j++)
	    {
	      avgCorner.point_ += (projectedCorners[neighborIndices[j]].point_ * projectedCorners[neighborIndices[j]].weight_);
	      totalWeight += projectedCorners[neighborIndices[j]].weight_;
	    }
	  avgCorner.point_ /= totalWeight;

	  // make three bad normals (to flag as a corner)
	  avgCorner.normals_.push_back( Vector(1,0,0) );
	  avgCorner.normals_.push_back( Vector(0,1,0) );
	  avgCorner.normals_.push_back( Vector(0,0,1) );

	  // save that avg corner as a final corner
	  finalCorners.push_back(avgCorner);
	}
    }

  // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"final corners (%d)...done (%g seconds)\n",finalCorners.size(),time2-time1);

  // return the final corners
  return finalCorners;
}

/**
 * helper function - this function will smooth the features by doing an
 * mls projection onto the local neighborhood that has a close coorelation 
 * to the PCA vectors.
 */
std::vector< Weighted_Point >
helper3_smoothFeatures( PointCloud *inputCloud, 
			const std::vector< Weighted_Point > &corners )
{
  // output entry information
  fprintf(stderr,"   (*) part B: smoothing projected points...");
  clock_t start = clock();

  // 0. copy over the points that will be smoothed
  std::vector< Point > smoothedPoints;
  std::vector< std::vector< Vector > > smoothedNormals;
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      if (inputCloud->verticesNormals[i].size() != 3)
	{
	  smoothedPoints.push_back( inputCloud->vertices[i] );
	  smoothedNormals.push_back( inputCloud->verticesNormals[i] );
	}
    }

  // 1. loop until the longest move is below a threshold
  float maxMovement = SMOOTH_MOVE_THRESHOLD+1;
  unsigned int count = 0;
  while( maxMovement > SMOOTH_MOVE_THRESHOLD && count < 3)
    {
      // clear the max movement 
      maxMovement = 0;
      PointCloud *smoothedCloud = new PointCloud( smoothedPoints );

      // loop through each point moving to the mls curve
      for(unsigned int i=0; i<smoothedPoints.size(); i++)
	{
	  // a. compute the distance to the nearest corner point
	  float cornerDistance = 100;
	  for(unsigned int j=0; j<corners.size(); j++)
	    {
	      float tmpDistance = corners[j].point_.distance_squared( smoothedPoints[i] );
	      if (tmpDistance < cornerDistance || j == 0)
		cornerDistance = tmpDistance;
	    }

	  // b. grab a large neighborhood around the point
	  std::vector< unsigned int > Nindices = smoothedCloud->kNN_indices( smoothedPoints[i],SMOOTH_NEIGHBOR_MAX );

	  // c. get the minimum neighborhood
	  std::vector< unsigned int > subset(2);
	  subset[0] = i;
	  std::vector< unsigned int >::iterator theIterator = Nindices.begin();
	  subset[1] = Nindices[0];		// need at least 2 so no error on PCA analysis
	  theIterator++;
	  for(unsigned int j=1; j<SMOOTH_NEIGHBOR_MIN && 
		smoothedPoints[Nindices[j]].distance_squared(smoothedPoints[i])<=cornerDistance; j++)
	    {
	      subset.push_back( Nindices[j] );
	      theIterator++;
	    }
	  Nindices.erase( Nindices.begin(),theIterator );

	  // d. grow the neighborhood until PCA correlation near 1
	  Vector majorAxis;
	  while( helper3_correlationComputation( subset,smoothedPoints,majorAxis ) < 0.7 && Nindices.size() > 0)
	    {
	      if (smoothedPoints[Nindices[0]].distance_squared(smoothedPoints[i]) <= cornerDistance)
		{
		  // grow the neighborhood
		  theIterator = Nindices.begin();
		  for(unsigned int j=0; j<Nindices.size() && j<SMOOTH_NEIGHBOR_GROW; j++)
		    {
		      if (smoothedPoints[Nindices[j]].distance_squared(smoothedPoints[i]) <= cornerDistance)
			{
			  subset.push_back(Nindices[j]);
			  theIterator++;
			}
		      else
			{
			  // terminate early
			  j=SMOOTH_NEIGHBOR_GROW;
			}
		    }
		  Nindices.erase(Nindices.begin(),theIterator);
		}
	      else
		{
		  // terminate early!
		  Nindices.clear();
		}
	    }

	  // e. project to the major axis eminating from the centroid
	  Point movedPoint(0,0,0);
	  for(unsigned int k=0; k<subset.size(); k++)
	    {
	      movedPoint += smoothedPoints[subset[k]];
	    }
	  movedPoint /= subset.size();
	  movedPoint = movedPoint + majorAxis*((smoothedPoints[i]-movedPoint).dot_product(majorAxis));
	  
	  // f. save the longest moved point distance
	  float tmpDistance = movedPoint.distance_squared( smoothedPoints[i] );
	  if (maxMovement < tmpDistance)
	    maxMovement = tmpDistance;
	  smoothedPoints[i] = movedPoint;

	  // g. average the normals and flip them to be consistent with the neighbors
	  helper3_averageRegularNormals( i, subset,smoothedNormals );
	}
      count++;
      fprintf(stderr,"loop %d (move %g of %g)...",count,maxMovement,SMOOTH_MOVE_THRESHOLD);
    }

  // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d smoothed points...%d loops...done (%g seconds)\n",smoothedPoints.size(),count,time2-time1);

  // reconstruct the weighted points
  std::vector< Weighted_Point > results(smoothedPoints.size());
  for(unsigned int i=0; i<smoothedPoints.size(); i++)
    {
      Weighted_Point newPt;
      newPt.point_ = smoothedPoints[i];
      newPt.normals_ = smoothedNormals[i];
      results[i] = newPt;
    }
  return results;
}

/**
 * helper function - computes the correlation of the point cloud and the pca axis
 * information.
 */
float
helper3_correlationComputation( const std::vector< unsigned int > &indices, 
				const std::vector< Point > &points, 
				Vector &majorAxis )
{
  // 0. build the cloud
  std::vector< Point > cloud( indices.size() );
  for(unsigned int i=0; i<cloud.size(); i++)
    cloud[i] = points[ indices[i] ];

  // 1. compute the weighted PCA analysis
  std::vector< Eigen_Pair > eigens = pca_analysis( cloud );
  if(eigens[0].value_ < 0) eigens[0].value_ *= -1;
  if(eigens[1].value_ < 0) eigens[1].value_ *= -1;
  if(eigens[2].value_ < 0) eigens[2].value_ *= -1;

  // 2. return the comparison of major axis to total axis lengths
  majorAxis = eigens[0].vector_.unit_vector();
  return eigens[0].value_ / (eigens[0].value_+eigens[1].value_+eigens[2].value_);
}

/**
 * helper function - computes the average of the normals for a regular point
 * based on the tangents of the neighbors...it will flip the normals associated
 * with the point to be similar to those of the neighbors.
 */
void
helper3_averageRegularNormals( const unsigned int &current, 
			       const std::vector< unsigned int > &neighbors,
			       std::vector< std::vector< Vector > > &smoothedNormals )
{
  // initialize the average normals to current normals
  Vector avgNormal1(1,0,0),avgNormal2(0,1,0);
  if (smoothedNormals[current].size() >= 1) avgNormal1 = smoothedNormals[current][0];
  if (smoothedNormals[current].size() >= 2) avgNormal2 = smoothedNormals[current][1];

  // break the normals into two sets while averaging them
  unsigned int group1=0,group2=0;
  unsigned int flipped1=0,flipped2=0;
  for(unsigned int i=0; i<neighbors.size(); i++)
    {
      // for each of the normals compute the dot products for each avgNormal
      std::vector< float > dotsGrp1( smoothedNormals[neighbors[i]].size() );
      std::vector< float > dotsGrp2( smoothedNormals[neighbors[i]].size() );
      for(unsigned int j=0; j<smoothedNormals[neighbors[i]].size(); j++)
	{
	  // compute the dot product with the two initialized normals
	  dotsGrp1[j] = fabs( avgNormal1.dot_product( smoothedNormals[neighbors[i]][j] ) );
	  dotsGrp2[j] = fabs( avgNormal2.dot_product( smoothedNormals[neighbors[i]][j] ) );
	}

      // get the best normal for the first group
      unsigned int bestOffset = 0;
      float bestDot = -1;
      for(unsigned int j=0; j<dotsGrp1.size(); j++)
	{
	  // better normal for group 1 than group 2 and previous best!
	  if (dotsGrp1[j] > bestDot && dotsGrp1[j] > dotsGrp2[j]) 
	    {
	      bestDot = dotsGrp1[j];
	      bestOffset = j;
	    }
	}

      // update the group 1 with the new information if valid
      if (bestDot >= 0)
	{
	  group1++;
	  if (avgNormal1.dot_product( smoothedNormals[neighbors[i]][bestOffset] ) < 0)
	    {
	      avgNormal1 -= smoothedNormals[neighbors[i]][bestOffset];
	      flipped1++;
	    }
	  else
	    avgNormal1 += smoothedNormals[neighbors[i]][bestOffset];
	  if (avgNormal1.length() > 0) avgNormal1.normalize();
	}

      // get the best normal for the second group
      bestDot = -1;
      for(unsigned int j=0; j<dotsGrp2.size(); j++)
	{
	  // better normal for group 2 than group 1 and previous best!
	  if (dotsGrp2[j] > bestDot && dotsGrp2[j] > dotsGrp1[j]) 
	    {
	      bestDot = dotsGrp2[j];
	      bestOffset = j;
	    }
	}

      // update the group 1 with the new information if valid
      if (bestDot >= 0)
	{
	  group2++;
	  if (avgNormal2.dot_product( smoothedNormals[neighbors[i]][bestOffset] ) < 0)
	    {
	      avgNormal2 -= smoothedNormals[neighbors[i]][bestOffset];
	      flipped2++;
	    }
	  else
	    avgNormal2 += smoothedNormals[neighbors[i]][bestOffset];
	  if (avgNormal2.length() > 0) avgNormal2.normalize();
	}
    }

  // check if we had to flip more than we kept as is (then flip the avgs)
  if ( group1/2 < flipped1 ) avgNormal1 *= -1;
  if ( group2/2 < flipped2 ) avgNormal2 *= -1;

  // save the averaged normals into the smoothed normal list
  smoothedNormals[current].resize(2);
  smoothedNormals[current][0] = avgNormal1;
  smoothedNormals[current][1] = avgNormal2;
}
