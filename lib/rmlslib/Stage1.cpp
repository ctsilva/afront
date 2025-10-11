/**
 *
 * file:	Stage1.cpp
 * description:	Defines the functions used to process the first
 *		stage in the feature extraction steps.  Stage 1
 *		receives a point cloud and returns the point cloud
 *		that contains all potential feature points.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

// -- INCLUDES -- //
#include "Stage1.h"

/**
 * the main function for input into the first stage.  This
 * function will take the input cloud and controls all the 
 * processes on that cloud to return the potential cloud.
 */
PointCloud*
stage1_createPotentialCloud( PointCloud *inputCloud, const float &noiseAllowance )
{
  // output entry information
  fprintf(stderr," (*) Stage 1: creating the potential feature cloud...");
  clock_t start = clock();

  // 1. compute the weights for the points
  helper1_computePointWeights( inputCloud,POTENTIAL_NEIGHBOR_MAX );

  // 2. extract the points with weights >= cutoff weight
  PointCloud *returnPC=NULL;
  helper1_extractPotentialCloud( inputCloud,noiseAllowance,returnPC );

  // output exit information
  clock_t end = clock();
  float time1 = start / ((float)CLOCKS_PER_SEC);
  float time2 = end / ((float)CLOCKS_PER_SEC);
  fprintf(stderr,"%d potential points...done (%g seconds)\n",returnPC->vertices.size(),time2-time1);

  return returnPC;
}

void
helper1_extractPotentialCloud( PointCloud *inputCloud,
			       const float &noiseAllowance,
			       PointCloud *&results )
{
  std::vector< Point > potentials;
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      // a. add point if above cutoff weight
      if (inputCloud->verticesWeights[i] >= noiseAllowance)
	{
	  potentials.push_back( inputCloud->vertices[i] );
	}
    }
  results = new PointCloud( potentials );
}

void
helper1_dividePotentialCloud( PointCloud *inputCloud,
			      const float &noiseAllowance,
			      PointCloud *&above,
			      PointCloud *&below )
{
  std::vector< Point > potentials,others;
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      // a. add point if above cutoff weight
      if (inputCloud->verticesWeights[i] >= noiseAllowance)
	{
	  potentials.push_back( inputCloud->vertices[i] );
	}
      else
	{
	  others.push_back( inputCloud->vertices[i] );
	}
    }
  above = new PointCloud( potentials );
  below = new PointCloud( others );
}

/**
 * computes the weights of all the points and stores them within the
 * point cloud given.
 */
void
helper1_computePointWeights( PointCloud *inputCloud, const unsigned int &nSize )
{
  // 1. compute the feature weights for all vertices
  inputCloud->verticesWeights.resize( inputCloud->vertices.size() );
  for(unsigned int i=0; i<inputCloud->vertices.size(); i++)
    {
      // a. get the maximum neighborhood sorted by distance
      std::vector< Point > neighbors = inputCloud->kNN( inputCloud->vertices[i],nSize );

      // b. compute the weights for this point based on the neighborhood
      std::vector< float > weights( neighbors.size() );
      Point centroid = inputCloud->vertices[i];
      float weightMax = 1;
      for(unsigned int j=0; j<weights.size(); j++)
	{
	  weights[j] = neighbors[j].distance_squared( inputCloud->vertices[i] ) + 0.01;
	  weights[j] = 1.0/weights[j];
	  centroid += neighbors[j]*weights[j];
	  weightMax += weights[j];
	}
      centroid /= weightMax;

      // c. compute the weighted PCA information for the neighborhood
      std::vector< Eigen_Pair > eigens = weighted_pca_analysis( neighbors,weights );
      if (eigens[0].value_ < 0) eigens[0].value_ *= -1;
      if (eigens[1].value_ < 0) eigens[1].value_ *= -1;
      if (eigens[2].value_ < 0) eigens[2].value_ *= -1;

 // d. fit a quadratic surface above the PCA plane and compute max residual
      HeightSurface quadratic;
      OrthoNormalBasis coordSys( eigens[0].vector_, eigens[1].vector_, eigens[2].vector_ );
      quadratic.fitSurface( centroid, coordSys, neighbors );
      inputCloud->verticesWeights[i] = quadratic.maxResidual( neighbors );
      if(i%10000 == 0) fprintf(stderr,"(%d).",i,inputCloud->vertices.size());
    }
}
