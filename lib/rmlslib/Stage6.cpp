/**
 *
 * file:	Stage6.cpp
 * description:	Defines the functions used to process the feature
 *		segmentation steps.  This uses the extracted features
 *		on the original point cloud and grows mls surface 
 *		until they hit a feature.  The points included in the
 *		mls surface are included as part of the segmented 
 *		feature region.
 *
 * author:	Joel Daniels
 * date:	May 2006
 *
 */

// -- INCLUDES -- //
#include "Stage6.h"

/**
 * the main function for the feature segmentation, this takes in the
 * feature polylines and the original point cloud.
 */
std::vector< PointCloud* >
stage6_segmentPointCloud( PointCloud *theCloud,
			  const std::vector<FeatureEdge> &features,
			  const float &noiseAllowance )
{
  // print out entry information
  fprintf(stderr," (*) Stage 6: segmenting the point cloud...");
  clock_t start = clock();

  std::vector< PointCloud* > segmentedPoints;

  // 1. copy the points from the cloud and flag them as unvisited (1)!
  std::vector< Point > points(theCloud->vertices.size());
  std::vector< float > weights(theCloud->vertices.size());
  std::vector< std::vector< Vector > > normals(theCloud->vertices.size());
  for(unsigned int i=0; i<points.size(); i++)
    {
      points[i] = theCloud->vertices[i];  // save the point
      weights[i] = 1;			  // flagged as unvisited!
      normals[i].clear();		  // clear the normals list
    }

  // 2. add the feature points to the points and flag them (-1)!
  for(unsigned int i=0; i<features.size(); i++)
    {
      for(unsigned int j=0; j<features[i].getNumPoints(); j++)
	{
	  points.push_back( features[i].getFeaturePoint(j) );
	  weights.push_back( -1 );
	}
    }

  // 3. create the point cloud with feature points and the weights
  PointCloud *featureCloud = new PointCloud( points );
  featureCloud->verticesWeights = weights;

  // 4. loop through each point and try growing a feature from it
  for(unsigned int current = 0; current<theCloud->vertices.size(); current++)
    {
      // a. if not visited yet then grow the feature (-1=edge, 0=visited, 1=not visited)!
      if (EQL(featureCloud->verticesWeights[current],1))
	{
	  // b. grow region to feature, boundary, or no remaining points
	  PointCloud* region = helper6_growPointRegion( current,featureCloud,noiseAllowance );

	  // c. add points as segmented feature if large enough! (avoid noise...)
	  if (region->vertices.size() > MIN_REGION_SIZE)
	      segmentedPoints.push_back( region );   
	}
    }

  // print out exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d segmented regions...done (%g seconds)\n",segmentedPoints.size(),time2-time1);

  unsigned int count = 0;
  for(unsigned int i=0; i<segmentedPoints.size(); i++)
    {
      count += segmentedPoints[i]->vertices.size();
      fprintf(stderr,"   (*) Segmented region[%d] size = %d\n",i,segmentedPoints[i]->vertices.size());
    }
  fprintf(stderr,"   (*) Final point count = %d\n",count);

  // 5. return segmented point clouds
  return segmentedPoints;
}

/**
 * grows the region starting from the given offset point and will flag
 * all the points added to the return region as visited.  will stop
 * growing in any direction when encountering a feature
 * edge. otherwise continue growing.
 */
PointCloud* 
helper6_growPointRegion( const unsigned int &startOffset,
			 PointCloud* theCloud,
			 const float &noiseAllowance )
{
  std::vector< Point > growRegion;
  std::vector< float > growLaplacian;
  std::vector< std::vector< Vector > > growNormals;

  // 0. compute an inital guess to the normal at the start point
  std::vector< Point > neighbors = theCloud->kNN( theCloud->vertices[startOffset], POTENTIAL_NEIGHBOR_MAX );
  Vector startVector = pca_unit_normal( neighbors );

  // 1. put start offset onto the top of the list
  std::queue< std::pair<unsigned int,Vector> > theQueue;
  theQueue.push( std::pair<unsigned int,Vector> (startOffset,startVector) );

  // 2. while list is not empty grow the region
  while(!theQueue.empty()) 
    {
      // a. get the front entry
      unsigned int current = theQueue.front().first;
      Vector theNormal = theQueue.front().second;
      theQueue.pop();

      // b. if it is not a feature nor visited then grow its neighbors
      if (EQL(theCloud->verticesWeights[current],1))
	{
	  // a. mark this point as visited
	  theCloud->verticesWeights[current] = 0;

	  // b. grab the neighbors of the point
	  std::vector< unsigned int > indices = theCloud->kNN_indices( theCloud->vertices[current], PROJECT_NEIGHBOR_MAX );

	  // c. fit the surface to the neighbors
	  HeightSurface growSurface;
	  if (helper6_fitSurface( growSurface, theNormal,current,indices,theCloud, noiseAllowance ))
	    {
	      // d. add neighbors not yet visited to the list with small residuals
	      for(unsigned int i=0; i<indices.size(); i++)
		{
		  if (growSurface.computeResidual( theCloud->vertices[ indices[i] ] ) <= noiseAllowance &&
		      EQL(theCloud->verticesWeights[ indices[i] ],1) )
		    {
		      Vector pNormal = growSurface.projectNormal( theCloud->vertices[ indices[i] ] );
		      theQueue.push( std::pair<unsigned int,Vector> (indices[i],pNormal) );
		    }
		}
	    }

	  // e. add this point to the growing region point set
	  growRegion.push_back( theCloud->vertices[ current ] );

	  // f. build the normals and the weights too
	  std::vector<Vector> tmpNormals(1);
	  tmpNormals[0] = theNormal;
	  growNormals.push_back( tmpNormals );
	  growLaplacian.push_back( fabs(growSurface.computeLaplacian()) );
	}
    }

  // 3. return the grown region
  return new PointCloud( growRegion, growLaplacian, growNormals );
}

/**
 * fits a surface to the indexed neighborhood if there are a
 * significant number of edge vertices in the neighborhood, then the
 * rmls like statistical fit surface function is called to perform the
 * fit.
 */
bool
helper6_fitSurface( HeightSurface &surface, 
		    const Vector &startNormal,
		    const unsigned int &startIndex,
		    const std::vector< unsigned int > &indices, 
		    PointCloud* theCloud, 
		    const float &noiseAllowance )
{
  // 1. ensure enough indices present
  if (indices.size() < 6) return false;

  // 2. extract feature edge points in the neighborhood
  std::vector< unsigned int > edges;
  for(unsigned int i=0; i<indices.size(); i++)
    {
      // check if the i'th neighbor is an edge!
      if ( EQL(theCloud->verticesWeights[ indices[i] ],-1) )
	{
	  edges.push_back( indices[i] );
	}
    }

  std::vector< Point > fitCloud;

  // the fit cloud contains only the edge vertices and current point
  if (edges.size() > 0)
    {
      return helper6_statisticalFit( surface,startNormal,startIndex,indices,theCloud,noiseAllowance );
    }

  // otherwise fit cloud contains all neighbors
  fitCloud.push_back( theCloud->vertices[ startIndex ] );
  for(unsigned int i=0; i<indices.size(); i++)
    {
      fitCloud.push_back( theCloud->vertices[ indices[i] ] );
    }

  // if the fit cloud is large enough then try adding neighbors
  if (fitCloud.size() >= 12)
    {
      // d. fit the surface to the grown cloud
      surface.fitSurface( fitCloud );
      return true;
    }
  else
    {
      return false;
    }
}
      
/**
 * fits a surface to the indexed neighborhood using RMLS like
 * statistics because there should be a significant number of edge
 * vertices in the neighborhood.
 */
bool
helper6_statisticalFit( HeightSurface &surface, 
			const Vector &startNormal,
			const unsigned int &startIndex,
			const std::vector< unsigned int > &indices, 
			PointCloud* theCloud, 
			const float &noiseAllowance )
{
  // 1. rmls fit surfaces to the indexed point clouds
  std::vector< Point > region( indices.size() );
  for(unsigned int i=0; i<indices.size(); i++)
    {
      region[i] = theCloud->vertices[ indices[i] ];
    }
  std::vector< HeightSurface > surfaces = rmls_fit_surfaces( region,noiseAllowance );

  // 2. find normals for the projections onto each surface
  std::vector< Point > projections( surfaces.size() );
  std::vector< Vector > normals( surfaces.size() );

  for(unsigned int i=0; i<surfaces.size(); i++)
    {
      surfaces[i].projectPoint( theCloud->vertices[startIndex], projections[i],normals[i] );
      if(!EQL(normals[i].length_squared(),0.0)) normals[i].normalize();
    }

  // 3. find the best normal and the closest point
  float bestNormalDot = 0;
  unsigned int bestOffset = 0;
  for(unsigned int i=0; i<surfaces.size(); i++)
    {
      // check if this is closer to the given start normal
      float tmpNormalDot = fabs( startNormal.dot_product( normals[i] ) );
      if ( tmpNormalDot > bestNormalDot )
	{
	  bestNormalDot = tmpNormalDot;
	  bestOffset = i;
	}
    }

  // 4. return that best surface fit if it is a good one!
  if (bestNormalDot > 0.9999)
    {
      surface = surfaces[ bestOffset ];  
      return true;
    }
  else
    {
      return false;
    }
}
