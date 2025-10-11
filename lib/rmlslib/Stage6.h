/**
 *
 * file:	Stage6.h
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

#ifndef _STAGE6_H_
#define _STAGE6_H_

// -- INCLUDES -- //
#include "Color.h"
#include "FeatureEdge.h"
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
 * the main function for the feature segmentation, this takes in the
 * feature polylines and the original point cloud.
 */
std::vector< PointCloud* >	stage6_segmentPointCloud( PointCloud *theCloud, const std::vector<FeatureEdge> &features, const float &noiseAllowance );

/**
 * helper functions for growing the segmented regions within the point
 * cloud, as defined by the given feature polylines.
 */
PointCloud*	helper6_growPointRegion( const unsigned int &startOffset, PointCloud* theCloud, const float &noiseAllowance );
bool		helper6_fitSurface( HeightSurface &surface, const Vector &startNormal, const unsigned int &startIndex, const std::vector< unsigned int > &indices, PointCloud* theCloud, const float &noiseAllowance );
bool		helper6_statisticalFit( HeightSurface &surface, const Vector &startNormal, const unsigned int &startIndex, const std::vector< unsigned int > &indices, PointCloud* theCloud, const float &noiseAllowance );

#endif
