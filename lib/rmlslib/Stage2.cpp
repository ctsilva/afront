/**
 *
 * file:	Stage2.cpp
 * description:	Defines the functions used to process the second
 *		stage in the feature extraction steps.  Stage 2
 *		receives a point cloud of potential feature points 
 *		as well as the original cloud, and projects all the
 *		points in the potential cloud to the detected features
 *		in the local proximity on the original cloud.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

// -- INCLUDES -- //
#include "Stage2.h"

/**
 * the main function for input into the second stage.  This
 * function will take the potential cloud and project each of 
 * the points to the detected features in the region of the point
 * within the original cloud.
 */
PointCloud*
stage2_projectPotentialCloud( PointCloud *potentialPoints,
			      PointCloud *originalCloud,
			      const float &noiseAllowance )
{
  // output entry information
  fprintf(stderr," (*) Stage 2: projecting points to the features...");
  clock_t start = clock();

  // 1. loop through the potential cloud and project points to the rmls features
  std::vector< Point > features;
  std::vector< float > weights;
  std::vector< std::vector< Vector > > normals;
  for(unsigned int i=0; i<potentialPoints->vertices.size(); i++)
    {
      // a. get the neighborhood of the point
      std::vector< Point > neighbors = originalCloud->kNN( potentialPoints->vertices[i], PROJECT_NEIGHBOR_MAX );

      // b. project point using RMLS to the feature
      Weighted_Point blah = rmls_projection_2_feature( potentialPoints->vertices[i], neighbors, noiseAllowance );

      // c. if blah was near a corner then project to corner at same time!
      if (blah.nearCorner_)
	{
	  // project the point to the corner and save it (with weight)
	  Weighted_Point corner;
	  if( rmls_projection_2_corner(potentialPoints->vertices[i], neighbors, noiseAllowance, corner ))
	    {
	      features.push_back( corner.point_ );
	      weights.push_back( 1.0/(corner.point_.distance_squared(blah.point_)+0.01) );
	      normals.push_back( corner.normals_ );
	    }
	}
      
      // save the point information
      features.push_back( blah.point_ );
      weights.push_back( blah.weight_ );
      normals.push_back( blah.normals_ );
    }

  // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d feature points...done (%g seconds)\n",features.size(),time2-time1);
  return new PointCloud( features,weights,normals );
}
