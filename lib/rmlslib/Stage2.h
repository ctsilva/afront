/**
 *
 * file:	Stage2.h
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

#ifndef _STAGE2_H_
#define _STAGE2_H_

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
 * the main function for input into the second stage.  This
 * function will take the potential cloud and project each of 
 * the points to the detected features in the region of the point
 * within the original cloud.
 */
PointCloud*
stage2_projectPotentialCloud( PointCloud *potentialPoints,
			      PointCloud *originalCloud,
			      const float &noiseAllowance );

#endif
