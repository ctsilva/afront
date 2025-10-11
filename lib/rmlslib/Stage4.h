/**
 *
 * file:	Stage4.h
 * description:	Defines the functions used to process the fourth
 *		stage in the feature extraction steps.  Stage 4
 *		receives a set of points that have been projected 
 *		and smoothed to the features of the original point 
 *		cloud.  This stage tries to grow feature polylines 
 *		through the smoothed cloud.  
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

#ifndef _STAGE4_H_
#define _STAGE4_H_

// -- INCLUDES -- //
#include "Color.h"
#include "Global_Defs.h"
#include "Matrix_3x3.h"
#include "Matrix_MxN.h"
#include "Point.h"
#include "PointCloud.h"
#include "Primitive_Structs.h"
#include "Primitive_Functions.h"
#include "QuadraticSurface.h"
#include "Vector.h"
#include "FeatureEdge.h"

#include <stdio.h>
#include <vector>

// -- DEFINES -- //

// -- FUNCTIONS -- //

/**
 * the main function for input into the fourth stage.  This function
 * grows features through the input point cloud.
 */
std::vector< FeatureEdge >	stage4_extractFeaturePolylines( PointCloud *inputCloud );

/**
 * helper functions for stage 4 - manage the growth of feature polylines
 * through the given point cloud.
 */
bool				helper4_distanceCompare( const std::pair< float,unsigned int > &a,
							 const std::pair< float,unsigned int > &b );
bool				helper4_growFeature( const unsigned int &startIndex,
						     PointCloud *inputCloud,
						     std::vector< bool > &visitedFlags,
						     std::vector< Point > &featurePoints,
						     std::vector< std::vector< Vector > > &featureNormals );
void				helper4_appendToFeature( const unsigned int &pointIndex,
							 PointCloud *cloud,
							 std::vector< bool > &visitedFlags,
							 const Vector &growDirection,
							 std::vector< Point > &featurePolyline,
							 std::vector< std::vector< Vector > > &featureNormals,
							 bool &loopFlag );
std::vector< unsigned int >	helper4_getNeighborhood( const unsigned int &pointIndex,
							 PointCloud *cloud,
							 Vector &majorAxis );
void				helper4_cleanUpNormals( FeatureEdge &feature );


#endif
