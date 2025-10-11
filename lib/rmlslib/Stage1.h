/**
 *
 * file:	Stage1.h
 * description:	Defines the functions used to process the first
 *		stage in the feature extraction steps.  Stage 1
 *		receives a point cloud and returns the point cloud
 *		that contains all potential feature points.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

#ifndef _STAGE1_H_
#define _STAGE1_H_

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
 * the main function for input into the first stage.  This
 * function will take the input cloud and controls all the 
 * processes on that cloud to return the potential cloud.
 */
PointCloud*
stage1_createPotentialCloud( PointCloud *inputCloud,
			     const float &noiseAllowance);

/**
 * helper functions...
 */
void helper1_extractPotentialCloud( PointCloud *inputCloud, const float &noiseAllowance, PointCloud *&results );
void helper1_dividePotentialCloud( PointCloud *inputCloud, const float &noiseAllowance, PointCloud *&aboveThreshold, PointCloud *&belowThreshold );
void helper1_computePointWeights( PointCloud *inputCloud, const unsigned int &nSize );


#endif
