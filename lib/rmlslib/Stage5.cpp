/**
 *
 * file:	Stage5.cpp
 * description:	Defines the functions used to process the fifth
 *		stage in the feature extraction steps.  Stage 5
 *		receives the feature polylines and the approximating
 *		tangents for their endpoints and tries to clean up
 *		the final feature curves.  This will attach polylines
 *		by filling gaps, will resolve unnecessary corners,
 *		and create new corners where they were previously 
 *		undetected.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

#include "Stage5.h"

/**
 * the main function for input into the fourth stage.  This function
 * grows features through the input point cloud.
 */
std::vector< FeatureEdge >	
stage5_resolveFeaturePolylines( const std::vector< FeatureEdge > &features,
				PointCloud *cloud )
{
  // print out entry information
  fprintf(stderr," (*) Stage 5: resolving feature polylines...");
  clock_t start = clock();

  // 1. find the corner points in the point cloud
  fprintf(stderr,"extracting corners.");
  std::vector<Point> corners = helper5_extractCornerPoints( cloud );

  // 2. connect features to corners if nearby
  std::vector< FeatureEdge > newFeatures = features;
  fprintf(stderr,"connecting to corners.");
  helper5_connectToCorners( newFeatures,corners );

  // 3. resolve corners that aren't really corners
  fprintf(stderr,"resolving bad corners (%d)->",corners.size());
  helper5_resolveBadCorners( newFeatures,corners );
  fprintf(stderr,"->(%d).",corners.size());

  // 4. connect gaps between remaining endpoints
  fprintf(stderr,"connecting gaps.");
  helper5_connectGaps( newFeatures );

  // 5. create new corners connecting remaining endpoints to features
  fprintf(stderr,"creating new corners.");
  helper5_connectNewCorners( newFeatures,corners );

  // 6. smooth the features!
  fprintf(stderr,"smoothing features.");
  helper5_smoothFeatures( newFeatures,corners );

  // print out exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d final features...%d corners...done (%g seconds)\n",newFeatures.size(),corners.size(),time2-time1);
  return newFeatures;
}

/**
 * helper function finds the corner points in the cloud and returns
 * the points in a vector.
 */
std::vector< Point >
helper5_extractCornerPoints( PointCloud *cloud )
{
  std::vector< Point > corners;
  for(unsigned int i=0; i<cloud->vertices.size(); i++)
    {
      if (cloud->verticesNormals[i].size() >= 3)
	corners.push_back( cloud->vertices[i] );
    }
  return corners;
}

/**
 * helper function connects the features that end near a corner and
 * their tangent is pointing in its direction.
 */
void
helper5_connectToCorners( std::vector<FeatureEdge> &features, 
			  const std::vector<Point> &corners )
{
  // loop through each feature
  for(unsigned int i=0; i<features.size(); i++)
    {
      // find the closest corner point to the start
      unsigned int cornerIndex;
      float cornerDistance;
      helper5_findClosestCorner( features[i].getStartPoint(),
				 features[i].getTangentStart(),
				 corners, cornerIndex, cornerDistance );

      // if at a corner point then save as connected!
      if ( EQL(cornerDistance,0.0) )
	{
	  features[i].isStartConnected() = true;
	}
      // if within a given range and in right direction
      else if (cornerDistance < 15*GROWTH_DISTANCE)
	{
	  features[i].prependPoint( corners[cornerIndex] );
	  features[i].isStartConnected() = true;
	}

      // find the closest corner point to the end
      helper5_findClosestCorner( features[i].getEndPoint(),
				 features[i].getTangentEnd(),
				 corners, cornerIndex, cornerDistance );

      // if at a corner point then save as connected!
      if ( EQL(cornerDistance,0.0) )
	{
	  features[i].isEndConnected() = true;
	}
      // if within a given range and in right direction
      else if (cornerDistance < 15*GROWTH_DISTANCE)
	{
	  features[i].appendPoint( corners[cornerIndex] );
	  features[i].isEndConnected() = true;
	}
    }
}

/**
 * finds the closest corner, returns the index and distance to that
 * corner in the given arguments.
 */
void
helper5_findClosestCorner( const Point &point,
			   const Vector &tangent,
			   const std::vector<Point> &corners,
			   unsigned int &closestIndex,
			   float &closestDistance )
{
  // clear the input arguments
  closestDistance = 100*GROWTH_DISTANCE;		// essentially infinity
  closestIndex = 0;

  // loop through all corners
  for(unsigned int i=0; i<corners.size(); i++)
    {
      // check that corner in valid direction
      if( tangent.dot_product( corners[i]-point ) >= 0 )
	{
	  float tmpDistance = corners[i].distance_squared(point);
	  if (tmpDistance < closestDistance)
	    {
	      closestDistance = tmpDistance;
	      closestIndex = i;
	    }
	}
    }
}

/**
 * connects the gaps by looking at the endpoints of the different
 * features...if they are close enough and have tangents in opposite
 * directions then they get connected!
 */
void
helper5_connectGaps( std::vector<FeatureEdge> &features )
{
  // loop through each of the features and connect to closest
  for(unsigned int i=0; i<features.size(); i++)
    {
      unsigned int closestIndex;
      float closestDistance;
      Point closestPoint;

      // if start not connected!
      if (!features[i].isStartConnected())
	{
	  // find closest end point on another feature to start point
	  helper5_findClosestEndPoint( features[i].getStartPoint(),
				       features[i].getTangentStart(),
				       features, i, closestPoint, 
				       closestIndex, closestDistance );

	  // it is a looping feature...so don't delete it!
	  if (closestDistance < 20*GROWTH_DISTANCE && i == closestIndex)
	    {
	      // attach the end point so that they share the common point
	      features[i].prependPoint( closestPoint );
	    }
	  // if within a given range and in right direction
	  else if (closestDistance < 20*GROWTH_DISTANCE)
	    {
	      // attach the end point so that they share the common point
	      features[i].prependPoint( closestPoint );

	      // combine features into this feature
	      features[i].attachFeatureToMe( features[closestIndex] );
	      helper5_flipNormals( features[i] );

	      // remove other feature from feature list
	      std::vector<FeatureEdge>::iterator theIter = features.begin();
	      for(unsigned int j=0; j<closestIndex && j<features.size(); j++)
		theIter++;
	      features.erase(theIter);
	    }
	}

      // if end not connected!
      if (!features[i].isEndConnected())
	{
	  // find closest end point on another feature to end point
	  helper5_findClosestEndPoint( features[i].getEndPoint(),
				       features[i].getTangentEnd(),
				       features, i, closestPoint, 
				       closestIndex, closestDistance );

	  // if it looped then make sure that we append the end 
	  if (closestDistance < 20*GROWTH_DISTANCE && i == closestIndex)
	    {
	      features[i].appendPoint( closestPoint );
	    }
	  // if within a given range and in right direction
	  else if (closestDistance < 20*GROWTH_DISTANCE)
	    {
	      // attach the end point so that they share the common point
	      features[i].appendPoint( closestPoint );

	      // combine features into this feature
	      features[i].attachFeatureToMe( features[closestIndex] );
	      helper5_flipNormals( features[i] );

	      // remove other feature from feature list
	      std::vector<FeatureEdge>::iterator theIter = features.begin();
	      for(unsigned int j=0; j<closestIndex && j<features.size(); j++)
		theIter++;
	      features.erase(theIter);
	    }
	}
    }
}

/**
 * finds the closest end point of another feature in the list (from
 * the given index to the end of the feature list) by comparing
 * distance and tangent directions.
 */
void 
helper5_findClosestEndPoint( const Point &point,
			     const Vector &tangent,
			     std::vector< FeatureEdge > &features,
			     const unsigned int &startIndex,
			     Point &closestPoint,
			     unsigned int &closestIndex,
			     float &closestDistance )
{
  // clear the input arguments
  closestDistance = 10000*GROWTH_DISTANCE;		// essentially infinity
  closestIndex = 0;

  // loop through all features after the start index
  for(unsigned int i=startIndex; i<features.size(); i++)
    {
      // check that start point is in valid direction
      if( //tangent.dot_product( features[i].getStartPoint()-point ) >= 0 &&
	  tangent.dot_product( features[i].getTangentStart() ) <= 0 )
	{
	  float tmpDistance = features[i].getStartPoint().distance_squared(point);
	  if (tmpDistance < closestDistance)
	    {
	      closestDistance = tmpDistance;
	      closestIndex = i;
	      closestPoint = features[i].getStartPoint();
	    }
	}

      // check that end point is in valid direction
      if( //tangent.dot_product( features[i].getEndPoint()-point ) >= 0 &&
	  tangent.dot_product( features[i].getTangentEnd() ) <= 0 )
	{
	  float tmpDistance = features[i].getEndPoint().distance_squared(point);
	  if (tmpDistance < closestDistance)
	    {
	      closestDistance = tmpDistance;
	      closestIndex = i;
	      closestPoint = features[i].getEndPoint();
	    }
	}
    }

  closestDistance = sqrt(closestDistance);
}

/**
 * finds the closest end point of another feature in the list (from
 * the given index to the end of the feature list) by comparing
 * distance and tangent directions. returns the closest endpoint heading
 * in the opposite direction!
 */
void 
helper5_findClosestEndPoint2( const Point &point,
			      const Vector &tangent,
			      std::vector< FeatureEdge > &features,
			      unsigned int &closestIndex,
			      float &closestDistance,
			      bool &startFlag )
{
  // clear the input arguments
  closestDistance = 10000*GROWTH_DISTANCE;		// essentially infinity
  closestIndex = 0;

  // loop through all features after the start index
  for(unsigned int i=0; i<features.size(); i++)
    {
      // check that start point is in valid direction
      if(// tangent.dot_product( features[i].getStartPoint()-point ) > 0 &&
	  tangent.dot_product( features[i].getTangentStart() ) >= 0 )
	{
	  float tmpDistance = features[i].getStartPoint().distance_squared(point);
	  if (tmpDistance < closestDistance)
	    {
	      closestDistance = tmpDistance;
	      closestIndex = i;
	      startFlag = true;
	    }
	}

      // check that end point is in valid direction
      if(// tangent.dot_product( features[i].getEndPoint()-point ) > 0 &&
	  tangent.dot_product( features[i].getTangentEnd() ) >= 0 )
	{
	  float tmpDistance = features[i].getEndPoint().distance_squared(point);
	  if (tmpDistance < closestDistance)
	    {
	      closestDistance = tmpDistance;
	      closestIndex = i;
	      startFlag = false;
	    }
	}
    }
}

/**
 * connects endpoints to the middle of features that are nearby thus
 * forming new corner points and adding them to the list.  it will
 * break the feature up into two new features when that happens.
 */
void
helper5_connectNewCorners( std::vector<FeatureEdge> &features, std::vector<Point> &corners )
{
  // find an endpoint that hasn't been connected
  for(unsigned int i=0; i<features.size(); i++)
    {
      // start point not connected to corner yet!
      unsigned int featureIndex,pointIndex;
      float closestDistance;
      bool startFlag;
      FeatureEdge other;
      if (!features[i].isStartConnected())
	{
	  // check if close and in right direction to another feature
	  helper5_findClosestFeature( features[i].getStartPoint(), i, features, 
				      featureIndex, pointIndex, closestDistance );


	  // if within the allowable distance
	  if (closestDistance < 20*GROWTH_DISTANCE && closestDistance >= 0)
	    {
	      // compute and add the new corner point
	      Point newCorner = ( features[featureIndex].getFeaturePoint( pointIndex )*3.0 +
				  features[i].getStartPoint() ) / 4.0;

	      // prepend the point to the feature
	      features[i].prependPoint( newCorner );
              features[i].isStartConnected() = true;

	      // split the feature around the point and attach to the new corner
	      if( features[featureIndex].splitFeatureMe( pointIndex,newCorner,other,MIN_FEATURE_LENGTH ) )
		{
		  helper5_fixNewCornerNormals( features[i],true, features[featureIndex], other );	// fix the normals at the corner
		  features.push_back( other );		// save the other edge
		  corners.push_back( newCorner );	// save the corner
		}
	    }

	  // otherwise check if heading in same as another end point (similar to filling gaps)
	  else
	    {
	      // find closest end point heading in same direction!
	      helper5_findClosestEndPoint2( features[i].getStartPoint(), features[i].getTangentStart(),
					    features, featureIndex, closestDistance, startFlag );

	      // if within the allowable distance
	      if (closestDistance < 15*GROWTH_DISTANCE && closestDistance > 0)
		{
		  if(startFlag)
		    {
		      // prepend the point to the feature
		      features[i].prependPoint( features[featureIndex].getStartPoint() );
		      features[i].isStartConnected() = true;

		      // tell the other feature it is connected
		      features[featureIndex].isStartConnected() = true;

		      // save the new corner
		      corners.push_back( features[featureIndex].getStartPoint() );
		    }
		  else
		    {
		      // prepend the point to the feature
		      features[i].prependPoint( features[featureIndex].getEndPoint() );

		      // tell the other feature it is connected
		      features[featureIndex].isEndConnected() = true;

		      // save the new corner
		      corners.push_back( features[featureIndex].getEndPoint() );
		    }
		}
	    }
	}

      // end point not connected to corner yet!
      if (!features[i].isEndConnected())
	{
	  // check if close and in right direction to another feature
	  helper5_findClosestFeature( features[i].getEndPoint(), i, features, 
				      featureIndex, pointIndex, closestDistance );
	  
	  // if within the allowable distance
	  if (closestDistance < 20*GROWTH_DISTANCE && closestDistance >= 0)
	    {
	      // compute and add the new corner point
	      Point newCorner = ( features[featureIndex].getFeaturePoint( pointIndex )*3.0 +
				  features[i].getEndPoint() ) / 4.0;

	      // prepend the point to the feature
	      features[i].appendPoint( newCorner );
              features[i].isEndConnected() = true;

	      // split the feature around the point and attach to the new corner
	      if( features[featureIndex].splitFeatureMe( pointIndex,newCorner,other,MIN_FEATURE_LENGTH ) )
		{
		  helper5_fixNewCornerNormals( features[i],false, features[featureIndex],other );	// fix the normals at the corner
		  features.push_back( other );		// save the other edge
		  corners.push_back( newCorner );	// save the corner
		}
	    }

	  // otherwise check if heading in same as another end point (similar to filling gaps)
	  else
	    {
	      // find closest end point heading in same direction!
	      helper5_findClosestEndPoint2( features[i].getEndPoint(), features[i].getTangentEnd(),
					    features, featureIndex, closestDistance, startFlag );

	      // if within the allowable distance
	      if (closestDistance < 15*GROWTH_DISTANCE && closestDistance > 0)
		{
		  if(startFlag)
		    {
		      // prepend the point to the feature
		      features[i].appendPoint( features[featureIndex].getStartPoint() );
		      features[i].isStartConnected() = true;

		      // tell the other feature it is connected
		      features[featureIndex].isStartConnected() = true;

		      // save the new corner
		      corners.push_back( features[featureIndex].getStartPoint() );
		    }
		  else
		    {
		      // prepend the point to the feature
		      features[i].appendPoint( features[featureIndex].getEndPoint() );

		      // tell the other feature it is connected
		      features[featureIndex].isEndConnected() = true;

		      // save the new corner
		      corners.push_back( features[featureIndex].getEndPoint() );
		    }
		}
	    }
	}
    }
}

/**
 * helper function finds the closest feature and the feature point
 * along which the end point and tangent are closest to pointing at.
 * this way it will be able to split the feature at the point and
 * create a new corner if it is close enough to the endpoint.
 */
void
helper5_findClosestFeature( const Point &point,
			    const unsigned int &thisFeature,
			    std::vector< FeatureEdge > &features,
			    unsigned int &featureIndex,
			    unsigned int &pointIndex,
			    float &closestDistance )
{
  // clear the inputs
  closestDistance = 10000*GROWTH_DISTANCE;
  featureIndex = pointIndex = 0;

  // search through every feature...
  for(unsigned int i=0; i<features.size(); i++)
    {
      if (i!=thisFeature)
	{
	  // search through every point
	  for(unsigned int j=0; j<features[i].getNumPoints(); j++)
	    {
	      // check if closer than others!
	      float tmpDistance = features[i].getFeaturePoint(j).distance_squared(point);
	      if (tmpDistance < closestDistance)
		{
		  closestDistance = tmpDistance;
		  featureIndex = i;
		  pointIndex = j;
		}
	    }
	}
    }

  closestDistance = sqrt( closestDistance );
}

/**
 * resolves the bad corners.  these are points that define the corner
 * where only two features meet but the tangents for those features
 * are nearly parallel.
 */
void
helper5_resolveBadCorners( std::vector<FeatureEdge> &features, std::vector<Point> &corners )
{
  // loop through each corner
  for(unsigned int i=0; i<corners.size(); i++)
    {
      // count the number of edges coming in to the corner
      std::vector< std::pair<unsigned int,bool> > indices = helper5_countCornerEdges( corners[i],features );
      fprintf(stderr,"[%d]%d.",i,indices.size());

      if (indices.size() == 0)
	{
	  // delete the corner point from the list!
	  std::vector<Point>::iterator Citer = corners.begin();
	  for(unsigned int j=0; j<corners.size() && j<i; j++)
	    Citer++;
	  corners.erase( Citer );
	  
	  // move the i back one!
	  i--;
	}

      // considered noisy corner (a feature attaching to it!)
      else if (indices.size() == 1)
	{
	  // unconnect the start point!
	  if( indices[0].second )
	    features[indices[0].first].isStartConnected() = false;
	  // unconnect the end point!
	  else
	    features[indices[0].first].isEndConnected() = false;

	  // delete the corner point from the list!
	  std::vector<Point>::iterator Citer = corners.begin();
	  for(unsigned int j=0; j<corners.size() && j<i; j++)
	    Citer++;
	  corners.erase( Citer );
	  
	  // move the i back one!
	  i--;
	}

      // if it has only two features -- check if tangents near parallel for combine!
      else if (indices.size() == 2)
	{
	  // check that it isn't a looping corner
	  if (indices[0].first != indices[1].first)
	    {
	      // get the proper tangents
	      Vector tangent1,tangent2;
	      if(indices[0].second) tangent1 = features[indices[0].first].getTangentStart();
	      else tangent1 = features[indices[0].first].getTangentEnd();
	      if(indices[1].second) tangent2 = features[indices[1].first].getTangentStart();
	      else tangent2 = features[indices[1].first].getTangentEnd();

	      // check if opposite then combine and remove old features
	      if (tangent1.dot_product(tangent2) <= 0)
		{
		  // combine into feature 1
		  features[indices[0].first].attachFeatureToMe( features[indices[1].first] );
		  helper5_flipNormals( features[indices[0].first] );

		  // remove the second feature
		  std::vector<FeatureEdge>::iterator Fiter = features.begin();
		  for(unsigned int j=0; j<indices[1].first && j<features.size(); j++)
		    Fiter++;
		  features.erase(Fiter);

		  // delete the corner point from the list!
		  std::vector<Point>::iterator Citer = corners.begin();
		  for(unsigned int j=0; j<corners.size() && j<i; j++)
		    Citer++;
		  corners.erase( Citer );
	  
		  // move the i back one!
		  i--;
		}
	    }
	}
    }
}

/**
 * count the number of features that end or start at the given
 * point. returns the offsets to the features that terminate on this
 * corner.
 */
std::vector< std::pair< unsigned int,bool > >
helper5_countCornerEdges( const Point &corner, const std::vector< FeatureEdge > &features )
{
  // loop through the features
  std::vector< std::pair< unsigned int,bool > > returnFeatures;
  for(unsigned int i=0; i<features.size(); i++)
    {
      if (EQL(0.0, corner.distance_to(features[i].getStartPoint())))
	returnFeatures.push_back( std::pair<unsigned int,bool> (i,true) );
      if (EQL(0.0, corner.distance_to(features[i].getEndPoint())))
	returnFeatures.push_back( std::pair<unsigned int,bool> (i,false) );
    }
  return returnFeatures;
}

/**
 * Smooths out the features keeping the endpoints and corner points in
 * place and relaxing the inbetween points as possible. loops until
 * max movements are minimized.
 */
void
helper5_smoothFeatures( std::vector<FeatureEdge> &features,
			const std::vector< Point > &corners )
{
  // clean up the normals
  for(unsigned int i=0; i<features.size(); i++)
    {
      helper5_flipNormals( features[i] );
    }
  fprintf(stderr,"flipped.");

  // resolve corner normals
  for(unsigned int i=0; i<corners.size(); i++)
    {
      helper5_cleanUpCornerNormals( corners[i],features );
    }
  fprintf(stderr,"resolved corner normals.");

  // loop smoothing features until movement is small
  unsigned int count = 0;
  float maxMovement = SMOOTH_MOVE_THRESHOLD + 1;
  while( maxMovement > SMOOTH_MOVE_THRESHOLD && count < 2 )
    {
      // loop through each feature edge
      maxMovement = 0;
      for(unsigned int i=0; i<features.size(); i++)
	{
	  float tmpDistance = features[i].smoothFeature();
	  if (tmpDistance > maxMovement) maxMovement = tmpDistance;
	}
      fprintf(stderr,"loop %d (move %g)...",count,maxMovement);
      count++;
    }
}

/**
 * this function will clean up the normals assigned to the corners
 * by looking at the six normals and trying to pair them into three
 * groups.
 */
void
helper5_cleanUpCornerNormals( const Point &corner,
			      std::vector< FeatureEdge > &features )
{
  // get the indices of the features coming in to the corner
  std::vector< std::pair<unsigned int,bool> > indices = helper5_countCornerEdges( corner,features );

  // terminate early if there is nothing attached to it (why is it still a corner?!)
  if (indices.size() == 0) return;

  // set all tangents as unvisited
  std::vector< std::pair<bool,bool> > flags(indices.size());
  for(unsigned int i=0; i<flags.size(); i++)
    flags[i] = std::pair<bool,bool> (false,false);

  // greedy algorithm - find best pair and mark as visited
  for(unsigned int i=0; i<flags.size()-1 && flags.size()!=0; i++)
    {
      // if the first is not visited yet find its pair
      if (!flags[i].first)
	{
	  // get the I tangent
	  Vector tangentI;
	  unsigned int thisPoint;
	  if(indices[i].second) thisPoint = 0;
	  else thisPoint = features[ indices[i].first ].getNumPoints()-1;
	  tangentI = features[indices[i].first].getFeatureNormal( thisPoint,0 );

	  unsigned int bestIndex=0,bestNormal=0,bestPoint=0;
	  float bestDot = 0;
	  Vector bestTangent;
	  for(unsigned int j=i+1; j<flags.size(); j++)
	    {
	      // check the first tangent
	      if (!flags[j].first)
		{
		  // get the J tangent -- first one!
		  Vector tangentJ;
		  unsigned int tmpPoint;
		  if(indices[j].second) tmpPoint = 0;
		  else tmpPoint = features[ indices[j].first ].getNumPoints()-1;
		  tangentJ = features[ indices[j].first ].getFeatureNormal(tmpPoint,0);
		  float dotValue = fabs( tangentJ.dot_product( tangentI ) );
		  if (dotValue > bestDot)
		    {
		      bestDot = dotValue;
		      bestIndex = j;
		      bestNormal = 0;
		      bestPoint = tmpPoint;
		      bestTangent = tangentJ;
		    }
		}

	      // check the second tangent
	      if (!flags[j].second)
		{
		  // get the J tangent -- second one!
		  Vector tangentJ;
		  unsigned int tmpPoint;
		  if(indices[j].second) tmpPoint = 0;
		  else tmpPoint = features[ indices[j].first ].getNumPoints()-1;
		  tangentJ = features[ indices[j].first ].getFeatureNormal(tmpPoint,1);
		  float dotValue = fabs( tangentJ.dot_product( tangentI ) );
		  if (dotValue > bestDot)
		    {
		      bestDot = dotValue;
		      bestIndex = j;
		      bestNormal = 1;
		      bestPoint = tmpPoint;
		      bestTangent = tangentJ;
		    }
		}
	    }

	  // average the two and save the information
	  if( tangentI.dot_product( bestTangent ) < 0 )
	    {
	      // compute the average and assign it to the two features
	      features[indices[i].first].getFeatureNormal( thisPoint,0 ) -= bestTangent;
	      features[indices[i].first].getFeatureNormal( thisPoint,0 ).normalize();
	      features[indices[bestIndex].first].getFeatureNormal( bestPoint,bestNormal ) = -features[indices[i].first].getFeatureNormal( thisPoint,0 );
	    }
	  else
	    {
	      // compute the average and assign it to the two features
	      features[indices[i].first].getFeatureNormal( thisPoint,0 ) += bestTangent;
	      features[indices[i].first].getFeatureNormal( thisPoint,0 ).normalize();
	      features[indices[bestIndex].first].getFeatureNormal( bestPoint,bestNormal ) = features[indices[i].first].getFeatureNormal( thisPoint,0 );
	    }

	  // set them as visited
	  flags[i].first = true;
	  if (bestNormal==0)
	    {
	      flags[bestIndex].first = true;
	    }
	  else
	    {
	      flags[bestIndex].second = true;
	    }
	}

      // if the second is not visited yet find its pair
      if (!flags[i].second)
	{
	  // get the I tangent
	  Vector tangentI;
	  unsigned int thisPoint;
	  if(indices[i].second) thisPoint = 0;
	  else thisPoint = features[ indices[i].first ].getNumPoints()-1;
	  tangentI = features[indices[i].first].getFeatureNormal( thisPoint,1 );

	  unsigned int bestIndex=0,bestNormal=0,bestPoint=0;
	  float bestDot = 0;
	  Vector bestTangent;
	  for(unsigned int j=i+1; j<flags.size(); j++)
	    {
	      // check the first tangent
	      if (!flags[j].first)
		{
		  // get the J tangent -- first one!
		  Vector tangentJ;
		  unsigned int tmpPoint;
		  if(indices[j].second) tmpPoint = 0;
		  else tmpPoint = features[ indices[j].first ].getNumPoints()-1;
		  tangentJ = features[ indices[j].first ].getFeatureNormal(tmpPoint,0);
		  float dotValue = fabs( tangentJ.dot_product( tangentI ) );
		  if (dotValue > bestDot)
		    {
		      bestDot = dotValue;
		      bestIndex = j;
		      bestNormal = 0;
		      bestPoint = tmpPoint;
		      bestTangent = tangentJ;
		    }
		}

	      // check the second tangent
	      if (!flags[j].second)
		{
		  // get the J tangent -- second one!
		  Vector tangentJ;
		  unsigned int tmpPoint;
		  if(indices[j].second) tmpPoint = 0;
		  else tmpPoint = features[ indices[j].first ].getNumPoints()-1;
		  tangentJ = features[ indices[j].first ].getFeatureNormal(tmpPoint,1);
		  float dotValue = fabs( tangentJ.dot_product( tangentI ) );
		  if (dotValue > bestDot)
		    {
		      bestDot = dotValue;
		      bestIndex = j;
		      bestNormal = 1;
		      bestPoint = tmpPoint;
		      bestTangent = tangentJ;
		    }
		}
	    }

	  // average the two and save the information
	  if( tangentI.dot_product( bestTangent ) < 0 )
	    {
	      // compute the average and assign it to the two features
	      features[indices[i].first].getFeatureNormal( thisPoint,1 ) -= bestTangent;
	      features[indices[i].first].getFeatureNormal( thisPoint,1 ).normalize();
	      features[indices[bestIndex].first].getFeatureNormal( bestPoint,bestNormal ) = -features[indices[i].first].getFeatureNormal( thisPoint,1 );
	    }
	  else
	    {
	      // compute the average and assign it to the two features
	      features[indices[i].first].getFeatureNormal( thisPoint,1 ) += bestTangent;
	      features[indices[i].first].getFeatureNormal( thisPoint,1 ).normalize();
	      features[indices[bestIndex].first].getFeatureNormal( bestPoint,bestNormal ) = features[indices[i].first].getFeatureNormal( thisPoint,1 );
	    }

	  // set them as visited
	  flags[i].second = true;
	  if (bestNormal==0)
	    {
	      flags[bestIndex].first = true;
	    }
	  else
	    {
	      flags[bestIndex].second = true;
	    }
	}
    }
}

/**
 * this function will make sure that each point has a normal
 * associated with it and that the normals are pointing in the proper
 * directions!  It means that the end points will have to be fixed
 * since they were most likely given garbage normal information.
 */
void
helper5_flipNormals( FeatureEdge &feature )
{
  // get the size of the feature for future use
  unsigned int size = feature.getNumPoints();

  // loop from the start to the end flipping and switching normals
  Vector previous1 = feature.getFeatureNormal( 0,0 );
  Vector previous2 = feature.getFeatureNormal( 0,1 );
  for(unsigned int i=1; i<size; i++)
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

  // erase a few near the ends and propogate the middle outwards
  unsigned int offset = 4;
  if ( size/2 < 4 ) offset = size/2;
  for(unsigned int i=0; i<offset; i++)
    {
      feature.getFeatureNormal(i,0) = feature.getFeatureNormal(offset,0);
      feature.getFeatureNormal(i,1) = feature.getFeatureNormal(offset,1);
      feature.getFeatureNormal(size-1-i,0) = feature.getFeatureNormal(size-1-offset,0);
      feature.getFeatureNormal(size-1-i,1) = feature.getFeatureNormal(size-1-offset,1);
    }
}

/**
 * fix the normals at the corner that has been newly created by 
 * splitting a feature.  the arguments are as followed:
 * feature1  - the feature that was attached (insighted the split)
 * startFlag - true if attached at the start false if end
 * split1    - the first half of the split feature (corner is at end)
 * split2    - the second half of the split feature (corner is at start)
 */
void
helper5_fixNewCornerNormals( FeatureEdge &feature1,
			     const bool &startFlag,
			     FeatureEdge &split1,
			     FeatureEdge &split2 )
{
  // get the tangents to assign to the corner
  Vector tangent1,tangent2;
  if (startFlag)
    {
      tangent1 = feature1.getFeatureNormal(0,0);
      tangent2 = feature1.getFeatureNormal(0,1);
    }
  else
    {
      unsigned int size = feature1.getNumPoints();
      tangent1 = feature1.getFeatureNormal(size-1,0);
      tangent2 = feature1.getFeatureNormal(size-1,1);
    }

  // compute the cross product of the tangents
  Vector crossed = tangent1.unit_cross_product( tangent2 );

  // the direction further from the cross product is the one we change
  unsigned int split1Size = split1.getNumPoints();
  Vector compare1_1 = split1.getFeatureNormal( split1Size-1,0 );
  Vector compare1_2 = split1.getFeatureNormal( split1Size-1,1 );
  Vector compare2_1 = split2.getFeatureNormal( 0,0 );
  Vector compare2_2 = split2.getFeatureNormal( 0,1 );

  // change the second vectors!
  if ( fabs(crossed.dot_product(compare1_1)) > fabs(crossed.dot_product(compare1_2)) )
    {
      // of the second normals find which is closer to tangent1
      float dot1 = fabs(tangent1.dot_product( compare1_2 ));
      float dot2 = fabs(tangent1.dot_product( compare2_2 ));
      if (dot1 > dot2)
	{
	  split1.getFeatureNormal( split1Size-1,1 ) = tangent1;
	  split2.getFeatureNormal( 0,1 ) = tangent2;
	}
      else
	{
	  split1.getFeatureNormal( split1Size-1,1 ) = tangent2;
	  split2.getFeatureNormal( 0,1 ) = tangent1;
	}
    }
  // change the first vectors!
  else
    {
      // of the second normals find which is closer to tangent1
      float dot1 = fabs(tangent1.dot_product( compare1_1 ));
      float dot2 = fabs(tangent1.dot_product( compare2_1 ));
      if (dot1 > dot2)
	{
	  split1.getFeatureNormal( split1Size-1,0 ) = tangent1;
	  split2.getFeatureNormal( 0,0 ) = tangent2;
	}
      else
	{
	  split1.getFeatureNormal( split1Size-1,0 ) = tangent2;
	  split2.getFeatureNormal( 0,0 ) = tangent1;
	}
    }
}

void
helper5_debugCornerPrint( const std::vector<FeatureEdge> &features, 
			  const std::vector<Point> &corners )
{
  for(unsigned int i=0; i<features.size(); i++)
    {
      fprintf(stderr,"feature[%d]: start connected -> ",i);
      if (features[i].isStartConnected()) fprintf(stderr,"true  ");
      else fprintf(stderr,"false  ");
      fprintf(stderr,"end connected -> ",i);
      if (features[i].isEndConnected()) fprintf(stderr,"true\n");
      else fprintf(stderr,"false\n");
    }

  for(unsigned int i=0; i<corners.size(); i++)
    {
      std::vector< std::pair<unsigned int,bool> > indices = helper5_countCornerEdges( corners[i],features );
      fprintf(stderr,"corner[%d]: connected to %d features {",indices.size());
      for(unsigned int j=0; j<indices.size(); j++)
	{
	  if (indices[j].second)
	    fprintf(stderr,"%d(s),",indices[j].first);
	  else
	    fprintf(stderr,"%d(e),",indices[j].first);
	}
      fprintf(stderr,"}\n");
    }
}
