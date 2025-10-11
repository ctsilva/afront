/**
 *
 * file:	Stage4.cpp
 * description:	Defines the functions used to process the fourth
 *		stage in the feature extraction steps.  Stage 4
 *		receives a set of points that have been projected 
 *		and smoothed to the features of the original point 
 *		cloud.  This stage tries to grow feature polylines 
 *		through the smoothed cloud.  
 *
 * author:	Joel Daniels
 * date:	May 2006
 *
 */

// -- INCLUDES -- //
#include "Stage4.h"

/**
 * the main function for input into the fourth stage.  This function
 * grows features through the input point cloud.
 */
std::vector< FeatureEdge > 
stage4_extractFeaturePolylines( PointCloud *inputCloud )
{
  // print out entry information
  fprintf(stderr," (*) Stage 4: extracting feature polylines...");
  clock_t start = clock();

  // 1. find the indices of all the corner points
  std::vector< unsigned int > cornerIndices;
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      if (inputCloud->verticesNormals[i].size() == 3)
	{
	  cornerIndices.push_back( i );
	}
    }

  // 2. sort the points based on the distance to the nearest corner
  std::vector< std::pair< float,unsigned int > > remainPoints(inputCloud->vertices.size()-cornerIndices.size());
  float maxDistance = -1;
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      // only process non corners!
      if (inputCloud->verticesNormals[i].size() != 3)
	{
	  // compute the distance to the closest corner
	  float cornerDistance = -1;
	  for(unsigned int j=0; j<cornerIndices.size(); j++)
	    {
	      float tmpDistance = inputCloud->vertices[cornerIndices[j]].distance_squared( inputCloud->vertices[i] );
	      if (tmpDistance < cornerDistance || cornerDistance < 0)
		  cornerDistance = tmpDistance;
	    }
	  cornerDistance = sqrt(cornerDistance);

	  // save the information
	  remainPoints.push_back( std::pair< float,unsigned int >(cornerDistance,i) );
	  inputCloud->verticesWeights[i] = cornerDistance;
	  if (maxDistance < cornerDistance || maxDistance < 0)
	    maxDistance = cornerDistance;
	}
    }
  if ( EQL(maxDistance,0.0) ) maxDistance = 1;
  std::sort( remainPoints.begin(), remainPoints.end(), helper4_distanceCompare );

  // 3. setup the visit flags for the vertices
  std::vector< bool > visitedFlags( inputCloud->vertices.size() );
  for(unsigned int i=0; i<visitedFlags.size(); i++)
    {
      visitedFlags[i] = false;
    }

  // 4. grow features while points remain
  std::vector< FeatureEdge > features;
  while( remainPoints.size() > 0 )
    {
      // get the furthest remaining point from all corners
      unsigned int startIndex = remainPoints[0].second;
      remainPoints.erase( remainPoints.begin() );			

      // grow a feature from that point through the remaining points
      std::vector< Point > feature;
      std::vector< std::vector< Vector > > normals;
      if( helper4_growFeature( startIndex, inputCloud,visitedFlags, feature,normals ) )
	{
	  // resolve the normals that are assigned to the feature edge
	  FeatureEdge tmp( feature,normals );
	  helper4_cleanUpNormals( tmp );
	  features.push_back( tmp );
	}
    }

  // 5. loop back through scaling the input cloud weights for coloring purposes
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      inputCloud->verticesWeights[i] /= maxDistance;
    }

  // print out exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d features extracted...done (%g seconds)\n",features.size(),time2-time1);
  return features;
}

/** 
 * helper function used to sort the distances of points to corners
 */
bool helper4_distanceCompare( const std::pair< float,unsigned int > &a,
			      const std::pair< float,unsigned int > &b )
{
  return a.first > b.first;
}

/**
 * helper function will grow a feature from the start point through the
 * input cloud scratching points out of the remaining list that have been
 * approximated by the growing polyline.
 */
bool
helper4_growFeature( const unsigned int &startIndex,
		     PointCloud *inputCloud,
		     std::vector< bool > &visitedFlags,
		     std::vector< Point > &featurePoints,
		     std::vector< std::vector< Vector > > &featureNormals )
{
  // check that the start point has been visited already
  if (visitedFlags[startIndex])
    {
      // didn't build a feature...return false!
      return false;
    }

  // 1. grow a neighborhood for the start point until correlation near 1
  Vector majorAxis;
  std::vector<unsigned int> neighbors = helper4_getNeighborhood( startIndex,inputCloud,majorAxis );

  // if no neighbors return false!
  if (neighbors.size() == 0)
    return false;

  // 2. save start point on feature
  std::vector<Point> direction1(1);
  std::vector< std::vector< Vector > > normals1(1);
  direction1[0] = inputCloud->vertices[startIndex];
  normals1[0] = inputCloud->verticesNormals[startIndex];

  // 3. grow feature in both directions of the major axis
  bool loopFlag = false;
  helper4_appendToFeature( startIndex,inputCloud,visitedFlags, majorAxis, direction1,normals1,loopFlag );

  // 4. if didn't loop
  if (!loopFlag)
    {
      // a. grow feature in second direction of major axis
      std::vector<Point> direction2(1);
      std::vector< std::vector<Vector> > normals2(1);
      direction2[0] = inputCloud->vertices[startIndex];
      normals2[0] = inputCloud->verticesNormals[startIndex];
      helper4_appendToFeature( startIndex,inputCloud,visitedFlags, -majorAxis, direction2,normals2,loopFlag );

      // b. append the two results
      featurePoints.resize( direction1.size()+direction2.size()-1 );
      featureNormals.resize( direction1.size()+direction2.size()-1 );
      for(unsigned int i=0; i<featurePoints.size(); i++)
	{
	  if (i<direction1.size())
	    {
	      featurePoints[i] = direction1[ direction1.size()-1-i ];
	      featureNormals[i] = normals1[ direction1.size()-1-i ];
	    }
	  else
	    {
	      featurePoints[i] = direction2[ i-direction1.size()+1 ];
	      featureNormals[i] = normals2[ i-direction1.size()+1 ];
	    }
	}
    }
  // 5. copy direction1 into feature - it defined an entire loop!
  else
    {
      featurePoints = direction1;
      featureNormals = normals1;
    }

  // 6. return the feature if longer than X points
  return featurePoints.size() >= MIN_FEATURE_LENGTH;
}

/**
 * helper function -- grows the neighborhood at the given point in the grow 
 * direction. appends the furthest point within the allowable distance to the
 * feature, marks all points used in the neighborhood as visited.
 */
void
helper4_appendToFeature( const unsigned int &pointIndex,
			 PointCloud *cloud,
			 std::vector< bool > &visitedFlags,
			 const Vector &growDirection,
			 std::vector< Point > &featurePolyline,
			 std::vector< std::vector< Vector > > &featureNormals,
			 bool &loopFlag )
{
  // 1. get the neighborhood of the input point
  Vector majorAxis;
  std::vector< unsigned int > neighbors = helper4_getNeighborhood( pointIndex,cloud,majorAxis );

  // if no neighbors then quit (no append!)
  if(neighbors.size() == 0)
    return;

  // 2. ensure that major axis is in grow direction
  if (majorAxis.dot_product(growDirection) < 0) majorAxis *= -1;

  // 3. compute the next point
  unsigned int nextPoint=pointIndex;
  float pointDistance = -1;
  bool growPoint=false;
  bool cornerPoint=false;
  bool loopPoint=false;
  for(unsigned int i=0; i<neighbors.size(); i++)
    {
      // only process the neighbor if growing in correct direction
      Vector neighborDirection = cloud->vertices[neighbors[i]]-cloud->vertices[pointIndex];
      float neighborDistance = neighborDirection.dot_product(majorAxis);
      if (neighborDistance > 0)
	{
	  // a. save furthest unvisited point in major axis direction
	  if( !loopPoint && !cornerPoint && !visitedFlags[neighbors[i]] && cloud->verticesNormals[neighbors[i]].size() < 3)
	    {
	      if (neighborDistance > pointDistance)
		{
		  pointDistance = neighborDistance;
		  nextPoint = neighbors[i];
		  growPoint = true;
		}
	    }

	  // b. save corner point in major axis direction
	  else if ( cloud->verticesNormals[neighbors[i]].size() == 3 )
	    {
	      // if a new corner or if already have a corner, but this one is closer
	      // then save the corner information
	      if (!cornerPoint || (cornerPoint && neighborDistance < pointDistance) )
		{
		  pointDistance = neighborDistance;
		  nextPoint = neighbors[i];
		  cornerPoint = true;
		  growPoint = loopPoint = false;
		}
	    }

	  // c. check if start of feature in neighborhood (loop!)
	  else if (!cornerPoint && EQL(featurePolyline[0].distance_squared( cloud->vertices[neighbors[i]] ),0.0))
	    {
	      pointDistance = neighborDistance;
	      nextPoint = neighbors[i];
	      growPoint = false;
	      loopPoint = true;
	    }

	  // d. mark neighbor as visited
	  visitedFlags[neighbors[i]] = true;
	}
    }

  // 4. if no next point found then quit early
  if (!growPoint && !cornerPoint && !loopPoint)
    {
      loopFlag = false;
      return;
    }

  // 5. append the next point
  featurePolyline.push_back( cloud->vertices[nextPoint] );  
  featureNormals.push_back( cloud->verticesNormals[nextPoint] );

  // 6. if attached grow point then recurse appendToFeature
  if (growPoint) 
    return helper4_appendToFeature( nextPoint,cloud,visitedFlags,majorAxis,featurePolyline,featureNormals,loopFlag );

  // 7. if attached loop point then flag
  if (loopPoint)
    loopFlag = true;

  // 8. at this point we have attached a loop or corner so just terminate growth
}

/**
 * helper function -- get neighborhood will grow the neighborhood for a point
 * until the correlation is near 1.  it will also return the vector for the
 * major axis.
 */
std::vector< unsigned int >
helper4_getNeighborhood( const unsigned int &pointIndex,
			 PointCloud *cloud,
			 Vector &majorAxis )
{
  // 1. grab the neighbors within the given distance ball
  std::vector< unsigned int > neighbors = cloud->distanceNeighbors( cloud->vertices[pointIndex],GROWTH_DISTANCE );
  if(neighbors.size() < 3)
    {
      // not a large enough neighborhood! quit!
      neighbors.clear();
      return neighbors;
    }

  // 2. compute the pca analysis of the neighborhood
  std::vector< Point > points( neighbors.size() );
  for(unsigned int i=0; i<neighbors.size(); i++)
    {
      points[i] = cloud->vertices[neighbors[i]];
    }
  majorAxis = (pca_analysis(points))[0].vector_;

  // 3. return the indices to the neighbors
  return neighbors;
}

/**
 * this function will make sure that each point has a normal
 * associated with it and that the normals are pointing in the proper
 * directions!  It means that the end points will have to be fixed
 * since they were most likely given garbage normal information.
 */
void
helper4_cleanUpNormals( FeatureEdge &feature )
{
  // get the size of the feature for future use
  unsigned int size = feature.getNumPoints();

  // fix the end points by assigning identical normals as one in
  feature.getFeatureNormal( 0,0 ) = feature.getFeatureNormal( 1,0 );
  feature.getFeatureNormal( 0,1 ) = feature.getFeatureNormal( 1,1 );

  // loop from the start to the end flipping and switching normals
  Vector previous1 = feature.getFeatureNormal( 0,0 );
  Vector previous2 = feature.getFeatureNormal( 0,1 );
  for(unsigned int i=1; i<size-1; i++)
    {
      // find vector more similar to previous1
      float dotValue1 = fabs( previous1.dot_product(feature.getFeatureNormal(i,0)) );
      float dotValue2 = fabs( previous1.dot_product(feature.getFeatureNormal(i,1)) );

      // the second is more similar to previous 1, perform switch!
      if (dotValue2 > dotValue1)
	{
	  Vector tmp = feature.getFeatureNormal(i,0);
	  feature.getFeatureNormal(i,0) = feature.getFeatureNormal(i,1);
	  feature.getFeatureNormal(i,1) = tmp;
	}

      // determine if we need to flip the normals
      if ( previous1.dot_product(feature.getFeatureNormal(i,0)) < 0 )
	feature.getFeatureNormal(i,0) *= -1;
      if ( previous2.dot_product(feature.getFeatureNormal(i,1)) < 0 )
	feature.getFeatureNormal(i,1) *= -1;

      // update the previous information
      previous1 = feature.getFeatureNormal(i,0);
      previous2 = feature.getFeatureNormal(i,1);
    }

  // copy the second to last information into the last point
  feature.getFeatureNormal( size-1,0 ) = feature.getFeatureNormal( size-2,0 );
  feature.getFeatureNormal( size-1,1 ) = feature.getFeatureNormal( size-2,1 );
}
