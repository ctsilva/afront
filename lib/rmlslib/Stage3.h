/**
 *
 * file:	Stage3.h
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

#ifndef _STAGE3_H_
#define _STAGE3_H_

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

#include <stdio.h>
#include <vector>

// -- DEFINES -- //

// -- FUNCTIONS -- //

/**
 * the main function for input into the third stage.  This function
 * smooths the points and resolves corners to give back a better
 * version of the feature projected points.
 */
PointCloud*		stage3_processFeatureCloud( PointCloud *inputCloud,
						    PointCloud *originalCloud,
						    const float &noiseAllowance );

/**
 * helper function - this will resolve the corner points for the 
 * input cloud set by projecting the feature points and then computing
 * the weighted average of clustered points.
 */
std::vector< Weighted_Point >	helper3_resolveCorners( PointCloud *inputCloud, PointCloud *originalCloud, const float &noiseAllowance );
std::vector< Weighted_Point >	helper3_smoothFeatures( PointCloud *inputCloud, const std::vector< Weighted_Point > &corners );
float				helper3_correlationComputation( const std::vector< unsigned int > &indices, const std::vector< Point > &points, Vector &majorAxis );
void				helper3_averageRegularNormals( const unsigned int &current, const std::vector< unsigned int > &neighbors, std::vector< std::vector< Vector > > &smoothedNormals );


#endif
