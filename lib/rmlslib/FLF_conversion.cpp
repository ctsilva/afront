/**
 *
 * file:	FLFconversion.cpp
 * description:	Feature Loop File converter provides the method
 *		to convert the feature edges into a point list
 *		and set of feature loop index list.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

// -- INCLUDES -- //
#include "FLF_conversion.h"

/**
 * converts our feature list and point clouds into the needed feature
 * loop lists and point lists.
 */
void 
FLF_buildLists( std::vector< InfoPoint > &ptList, 
		std::vector< std::vector< InfoIndex > > &featList, 
		const std::vector< FeatureEdge > &edges )
{
  // output entry information
  fprintf(stderr," (*) Feature Loop Conversion:\n");
  clock_t start = clock();

  // 0. clear out the point list and feature list
  ptList.clear();
  featList.clear();

  // 1. build the point list
  std::vector< std::vector< unsigned int > > edgeIndices;
  FLF_buildPointList( ptList,edgeIndices, edges );

  // 2. build the feature loop lists
  FLF_buildFeatureList( featList, edgeIndices,edges );

  // 3. flip the normals in the feature lists and make it consistance within each loop
  FLF_flipNormals( ptList, featList );
	
   // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"     Finished feature loop construction...(%d points)(%d features)(%g seconds)\n",ptList.size(),featList.size(),time2-time1);
}

/**
 * build the feature loop list from the point list, feature list and the
 * feature index list.
 */
void 
FLF_buildFeatureList( std::vector< std::vector< InfoIndex > > &featList, 
		      const std::vector< std::vector< unsigned int > > &edgesIndices, 
		      const std::vector< FeatureEdge > &edges )
{
  // output entry information
  fprintf(stderr,"     (*) Constructing feature loops...");
  clock_t start = clock();

  // 1. Setup flags to indicate if a feature has been walked down both sides
  std::vector< std::pair< bool,bool > > flags( edges.size() );
  for(unsigned int i=0; i<flags.size(); i++)
    flags[i] = std::pair< bool,bool > (false,false);

  // 2. While there exists a feature not walked in both directions
  unsigned int startFeature,startDirection,startNormal;
  while( FLF_existsUnfinishedFeature( flags,startFeature,startDirection,startNormal ) )
    {
      fprintf(stderr,"-- CREATE LOOP %d --\n",featList.size());

      // a. set the start feature and direction as the current
      unsigned int currentFeature = startFeature;
      unsigned int currentDirection = startDirection;
      unsigned int currentNormal = startNormal;

      // b. walk the features until you complete a loop (back to start)
      std::vector< InfoIndex > featureLoop;
      do 
	{
	  fprintf(stderr," (feature:%d, direction:%d, normal:%d)\n",currentFeature,currentDirection,currentNormal);

	  // 1. append the points of the current feature in the current direction
	  if (currentDirection == 0)
	    {
	      // sanity check!
	      if ( (currentNormal == 0 && flags[currentFeature].first) ||
		   (currentNormal == 1 && flags[currentFeature].second) )
		{
		  fprintf(stderr,"[ERROR] FLF_buildFeatureList() - walking feature[%d] with the same normal twice!\n",currentFeature);
		}

	      // a. append start to end
	      FLF_appendStartToEnd( featureLoop, edgesIndices,edges, currentFeature,currentNormal );

	      // b. flag the feature as finished
	      if (currentNormal == 0) flags[currentFeature].first = true;
	      else flags[currentFeature].second = true;

	      // c. find the next feature
	      FLF_findNextFeature_FromEnd( currentFeature,currentNormal,currentDirection, edgesIndices,edges );
	    }
	  else
	    {
	      // sanity check!
	      if ( (currentNormal == 0 && flags[currentFeature].first) ||
		   (currentNormal == 1 && flags[currentFeature].second) )
		{
		  fprintf(stderr,"[ERROR] FLF_buildFeatureList() - walking feature[%d] with the same normal twice!\n",currentFeature);
		}

	      // a. append end to start
	      FLF_appendEndToStart( featureLoop, edgesIndices,edges, currentFeature,currentNormal );

	      // b. flag the feature as finished
	      if (currentNormal == 0) flags[currentFeature].first = true;
	      else flags[currentFeature].second = true;

	      // c. find the next feature
	      FLF_findNextFeature_FromStart( currentFeature,currentNormal,currentDirection, edgesIndices,edges );
	    }

	  // loop until we get back to the start and are headed in the same direction
	} while ( currentFeature != startFeature || currentDirection != startDirection );

      // save the feature loop!
      featList.push_back( featureLoop );
    }

  // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d feature loops created...done...(%g seconds)\n",featList.size(),time2-time1);
}

/**
 * this function will search the features with the common corner point as this
 * features end point to find the best match for a feature to next walk down.
 * the given featureOffset, featureNormal, featureDirection are used as input
 * to tell the function the incoming information and will be used as output by the
 * function to tell which is the next feature, the direction to walk, and the 
 * normal to use.
 */
void FLF_findNextFeature_FromEnd( unsigned int &featureOffset, 
				  unsigned int &featureNormal, 
				  unsigned int &featureDirection, 
				  const std::vector< std::vector< unsigned int > > &edgesIndices, 
				  const std::vector< FeatureEdge > &edges )
{
  // 1. get the index of the corner point and the normal
  unsigned int size = edges[featureOffset].getNumPoints();
  unsigned int cornerIndex = edgesIndices[featureOffset][ size-1 ];
  Vector myNormal = edges[featureOffset].getFeatureNormal( size-1,featureNormal );

  // get all feature indices that end at this corner
  std::vector< std::pair< unsigned int, bool > > features;
  for(unsigned int i=0; i<edgesIndices.size(); i++)
    {
      if (i != featureOffset )
	{
	  if (edgesIndices[i][0] == cornerIndex)
	      features.push_back( std::pair<unsigned int,bool> (i,true) );
	  if (edgesIndices[i].back() == cornerIndex)
	      features.push_back( std::pair<unsigned int,bool> (i,false) );
 	}

      // otherwise check if it is a loop!
      else
	{
	  if (edgesIndices[i][0] == cornerIndex && edgesIndices[i].back() == cornerIndex )
	    {
	      if (featureDirection == 0)
		features.push_back( std::pair<unsigned int,bool> (i,true) );
	      else 
		features.push_back( std::pair<unsigned int,bool> (i,false) );
	    }
	}
    }

  // init to itself (but opposite normal and direction)
  float bestNormalDot = 0;
  unsigned int bestIndex = featureOffset;

  unsigned int bestNormal;
  if (featureNormal == 0) bestNormal = 1;
  else bestNormal = 0;

  if (featureDirection == 0) featureDirection = 1;
  else featureDirection = 0;

  // find the edge with the best matching normal
  for(unsigned int i=0; i<features.size(); i++)
    {
      // check if the start is better than the current best
      if (features[i].second)
	{
	  // check normal 0
	  float tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(0,0).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 0;			// save best normal as the first normal
	      featureDirection = 0;		// save the feature direction as start to finish
	    }

	  // check normal 1
	  tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(0,1).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 1;			// save the best normal as the second normal
	      featureDirection = 0;		// save the feature direction as start to finish
	    }
	}
      // otherwise check if the end is better than the current best
      else
	{
	  // check normal 0
	  unsigned int size2 = edges[features[i].first].getNumPoints()-1;
	  float tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(size2,0).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 0;			// save the best normal as the first normal
	      featureDirection = 1;		// save the feature direction as start to finish
	    }

	  // check normal 1
	  tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(size2,1).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 1;			// save the best normal as the second normal
	      featureDirection = 1;		// save the feature direction as start to finish
	    }
	}
    }

  // update the return information
  featureOffset = bestIndex;
  featureNormal = bestNormal;
}

/**
 * this function will search the features with the common corner point as this
 * features end point to find the best match for a feature to next walk down.
 * the given featureOffset, featureNormal, featureDirection are used as input
 * to tell the function the incoming information and will be used as output by the
 * function to tell which is the next feature, the direction to walk, and the 
 * normal to use.
 */
void FLF_findNextFeature_FromStart( unsigned int &featureOffset, 
				    unsigned int &featureNormal, 
				    unsigned int &featureDirection, 
				    const std::vector< std::vector< unsigned int > > &edgesIndices, 
				    const std::vector< FeatureEdge > &edges )
{
  // 1. get the index of the corner point and the normal
  unsigned int cornerIndex = edgesIndices[featureOffset][ 0 ];
  Vector myNormal = edges[featureOffset].getFeatureNormal( 0,featureNormal );

  // get all feature indices that end at this corner
  std::vector< std::pair< unsigned int, bool > > features;
  for(unsigned int i=0; i<edgesIndices.size(); i++)
    {
      if (i != featureOffset )
	{
	  if (edgesIndices[i][0] == cornerIndex)
	      features.push_back( std::pair<unsigned int,bool> (i,true) );
	  if (edgesIndices[i].back() == cornerIndex)
	      features.push_back( std::pair<unsigned int,bool> (i,false) );
 	}

      // otherwise check if it is a loop!
      else
	{
	  if (edgesIndices[i][0] == cornerIndex && edgesIndices[i].back() == cornerIndex )
	    {
	      if (featureDirection == 0)
		features.push_back( std::pair<unsigned int,bool> (i,true) );
	      else 
		features.push_back( std::pair<unsigned int,bool> (i,false) );
	    }
	}

    }

  // init to itself (but opposite normal and direction)
  float bestNormalDot = 0;
  unsigned int bestIndex = featureOffset;

  unsigned int bestNormal;
  if (featureNormal == 0) bestNormal = 1;
  else bestNormal = 0;

  if (featureDirection == 0) featureDirection = 1;
  else featureDirection = 0;

  // find the edge with the best matching normal
  for(unsigned int i=0; i<features.size(); i++)
    {
      // check if the start is better than the current best
      if (features[i].second)
	{
	  // check normal 0
	  float tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(0,0).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 0;			// save best normal as the first normal
	      featureDirection = 0;		// save the feature direction as start to finish
	    }

	  // check normal 1
	  tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(0,1).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 1;			// save the best normal as the second normal
	      featureDirection = 0;		// save the feature direction as start to finish
	    }
	}
      // otherwise check if the end is better than the current best
      else
	{
	  // check normal 0
	  unsigned int size2 = edges[features[i].first].getNumPoints()-1;
	  float tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(size2,0).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 0;			// save the best normal as the first normal
	      featureDirection = 1;		// save the feature direction as start to finish
	    }

	  // check normal 1
	  tmpNormalDot = fabs(edges[features[i].first].getFeatureNormal(size2,1).dot_product( myNormal ));
	  if (tmpNormalDot > bestNormalDot)
	    {
	      bestNormalDot = tmpNormalDot;	// update the best dot product
	      bestIndex = features[i].first;	// save the new best feature index
	      bestNormal = 1;			// save the best normal as the second normal
	      featureDirection = 1;		// save the feature direction as start to finish
	    }
	}
    }

  // update the return information
  featureOffset = bestIndex;
  featureNormal = bestNormal;
}

/**
 * this function walks the given feature from start to end and appends
 * the index values to the constructing feature loop.
 */
void 
FLF_appendStartToEnd( std::vector< InfoIndex > &featureLoop, 
		      const std::vector< std::vector< unsigned int > > &edgesIndices, 
		      const std::vector< FeatureEdge > &edges, 
		      const unsigned int &featureOffset, 
		      const unsigned int &featureNormal )
{
  // loop through the featureOffset appending new InfoIndex values to the list
  for(unsigned int i=0; i<edgesIndices[featureOffset].size()-1; i++)
    {
      // build the new InfoIndex
      InfoIndex newValue;
      newValue.index_ = edgesIndices[featureOffset][i];
      newValue.x_ = edges[featureOffset].getFeatureNormal(i,featureNormal).x();
      newValue.y_ = edges[featureOffset].getFeatureNormal(i,featureNormal).y();
      newValue.z_ = edges[featureOffset].getFeatureNormal(i,featureNormal).z();

      // append the InfoIndex to the growing feature loop
      featureLoop.push_back( newValue );
    }
}

/**
 * this function walks the given feature from end to start and appends
 * the index values to the constructing feature loop.
 */
void FLF_appendEndToStart(  std::vector< InfoIndex > &featureLoop,
			    const std::vector< std::vector< unsigned int > > &edgesIndices, 
			    const std::vector< FeatureEdge > &edges, 
			    const unsigned int &featureOffset, 
			    const unsigned int &featureNormal )
{
  // loop through the featureOffset appending new InfoIndex values to the list
  for(unsigned int i=edgesIndices[featureOffset].size()-1; i>0; i--)
    {
      // build the new InfoIndex
      InfoIndex newValue;
      newValue.index_ = edgesIndices[featureOffset][i];
      newValue.x_ = edges[featureOffset].getFeatureNormal(i,featureNormal).x();
      newValue.y_ = edges[featureOffset].getFeatureNormal(i,featureNormal).y();
      newValue.z_ = edges[featureOffset].getFeatureNormal(i,featureNormal).z();

      // append the InfoIndex to the growing feature loop
      featureLoop.push_back( newValue );
    }
}

/**
 * this function will search the feature flags for the first one to
 * contain a zero in its paired entry.  the function will return that
 * offset (indexFeature) that contains the negative entry, it returns
 * the index of the pair that contains the negative value
 * (offsetDirection), and finally the normal that has yet to be walked
 * for that feature (unusedNormal).
 */
bool 
FLF_existsUnfinishedFeature( const std::vector< std::pair< bool,bool > > &featureFlags, 
			     unsigned int &indexFeature, 
			     unsigned int &offsetDirection, 
			     unsigned int &unusedNormal)
{
  // loop through looking for a zero entry!
  for(unsigned int i=0; i<featureFlags.size(); i++)
    {
      // check the first paired entry
      if (!featureFlags[i].first)
	{
	  // save the 'i' value as the indexed feature
	  indexFeature = i;

	  // choose a direction to walk (arbitrary)
	  offsetDirection = 0;

	  // the first is the unused normal
	  unusedNormal = 0;

	  // return true as we found a zero entry!
	  return true;
	}
      // check the second paired entry
      else if (!featureFlags[i].second)
	{
	  // save the 'i' value as the indexed feature
	  indexFeature = i;

	  // choose a direction to walk (arbitrary)
	  offsetDirection = 0;

	  // the second is the unused normal
	  unusedNormal = 1;

	  // return true as we found a zero entry!
	  return true;
	}
    }

  // none found we have finished!
  return false;
}

/**
 * build the point list from the list of features given as edges.
 * simultaneously rebuild the features by storing them as index lists
 * into the point list for future use.
 */
void 
FLF_buildPointList( std::vector< InfoPoint > &ptList, 
		    std::vector< std::vector< unsigned int > > &edgesIndices, 
		    const std::vector< FeatureEdge > &edges )
{
  // output entry information
  fprintf(stderr,"     (*) Constructing point list...");
  clock_t start = clock();

  // 1. build a point list and index list for the features
  for(unsigned int i=0; i<edges.size(); i++)
    {
      // simultaneously build the feature index
      std::vector< unsigned int > feature;

      // conditionally add the end points and return the index!
      feature.push_back( FLF_addCornerPoint( ptList, edges[i].getStartPoint() ) );
      unsigned int endIndex = FLF_addCornerPoint( ptList, edges[i].getEndPoint() );

      // unconditionally add interior points (always unique)
      for(unsigned int j=1; j<edges[i].getNumPoints()-1; j++)
	{
	  // create the InfoPoint as a Non-corner point
	  InfoPoint newPt;
	  newPt.x_ = edges[i].getFeaturePoint(j).x();
	  newPt.y_ = edges[i].getFeaturePoint(j).y();
	  newPt.z_ = edges[i].getFeaturePoint(j).z();
	  newPt.isCorner_ = false;
	  ptList.push_back( newPt );

	  // save the index of the added point to the feature
	  feature.push_back( ptList.size()-1 );
	}

      // save the offset to the end point index (previously added)
      feature.push_back( endIndex );

      // save this feature to the list of feature indices
      edgesIndices.push_back( feature );
    }

  // 2. clean up corner points (versus endpoints and loop points)
  for(unsigned int i=0; i<ptList.size(); i++)
    {
      // check that it is a corner point
      if (ptList[i].isCorner_)
	{
	  // count number of features with this index as an endpoint
	  std::vector< unsigned int > features;
	  for(unsigned int j=0; j<edgesIndices.size(); j++)
	    {
	      if (edgesIndices[j][0] == i) features.push_back(j);
	      if (edgesIndices[j].back() == i) features.push_back(j);
	    }

	  // if only one feature on list then end point NOT corner!
	  if (features.size() == 1) 
	    ptList[i].isCorner_ = false;

	  // if only two features on list then make sure not a loop!
	  else if (features.size() == 2)
	    {
	      if (features[0] == features[1])
		ptList[i].isCorner_ = false;
	    }
	}
    }

  // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d points exist...done...(%g seconds)\n",ptList.size(),time2-time1);
}

/**
 * add a new info point (as a corner point!) iff it is unique
 * to the point list.
 */
unsigned int
FLF_addCornerPoint( std::vector< InfoPoint > &ptList,
		    const Point &corner )
{
  // check if start point is a new point
  for(unsigned int j=0; j<ptList.size(); j++)
    {
      Point tmpPt( ptList[j].x_,ptList[j].y_,ptList[j].z_ );
      if ( EQL( 0.0, tmpPt.distance_to( corner ) ) )
	return j;
    }

  // if not found then add
  InfoPoint pt; 
  pt.x_ = corner.x();
  pt.y_ = corner.y();
  pt.z_ = corner.z();
  pt.isCorner_ = true;
  ptList.push_back( pt );

  // return the offset to the last point
  return ptList.size()-1;
}

Vector TriNormal(const Point& a,const Point& b,const Point& c)
{
	Vector e1 = a-c;
	Vector e2 = b-c;
	return e1.unit_cross_product(e2);
}
/**
 * flips the normals so that they are consistent within each loop.
 * and consistent with the direction of the loop
 */
void
FLF_flipNormals( std::vector< InfoPoint > &ptList, std::vector< std::vector< InfoIndex > > &featList )
{
  for(unsigned int i=0; i<featList.size(); i++)
    {
// 		//1. Find the right normal direction of the loop 
// 		//a. Get a middle point 
// 		unsigned int start_index = 4;
// 		unsigned int cur_index = featList[i][start_index].index_;
// 		Point cur_point(ptList[cur_index].x_, ptList[cur_index].y_, ptList[cur_index].z_);
// 		//b. get the next point in the loop
// 		unsigned int next_index = featList[i][start_index+1].index_;
// 		Point next_point(ptList[next_index].x_, ptList[next_index].y_, ptList[next_index].z_);
// 		//c. get the normal of that point
// 		Vector ref_normal(featList[i][start_index+1].x_, featList[i][start_index+1].y_, featList[i][start_index+1].z_);
// 		//d. get a neibor of the next point
// 		std::vector<Point> neigborPoints = pts->kNN(next_point, NEIGHBOR_MAX);
// 		unsigned int right_direct=0;	
// 		unsigned int wrong_direct=0;
// 		for (unsigned int j=0; j< neigborPoints.size(); j++)
// 		{
// 			Vector normal = TriNormal(cur_point, next_point, neigborPoints[j]);
// 			float length = normal.dot_product(ref_normal);
// 			if (length > 0.7) right_direct++;
// 			if (length < -0.7) wrong_direct++;
// 		}
// 		//flip normal if there are more point in the oposite direction
// 		if (right_direct < wrong_direct) ref_normal *=-1;
// 		fprintf(stderr, "Number of right direction normal %d wrong direction normal %d", right_direct, wrong_direct);
// 		Vector 	previous = ref_normal;

      Vector previous(featList[i][0].x_,featList[i][0].y_,featList[i][0].z_);
      for(unsigned int j=1; j<featList[i].size(); j++)
	{
	  Vector current(featList[i][j].x_,featList[i][j].y_,featList[i][j].z_);
	  if (previous.dot_product(current) < 0)
	    {
	      current *= -1;
	      featList[i][j].x_ = current.x();
	      featList[i][j].y_ = current.y();
	      featList[i][j].z_ = current.z();
	    }
	  previous = current;
	}
    }
}

