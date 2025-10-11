/**
 *
 * file:	Stage5.h
 * description:	Defines the functions used to process the fifth
 *		stage in the feature extraction steps.  Stage 5
 *		receives the feature polylines and the approximating
 *		tangents for their endpoints and tries to clean up
 *		the final feature curves.  This will attach polylines
 *		by filling gaps, will resolve unnecessary corners,
 *		and create new corners where they were previously 
 *		undetected.
 *
 * author:	Joel Daniels
 * date:	May 2006
 *
 */

#ifndef _STAGE5_H_
#define _STAGE5_H_

// -- INCLUDES -- //
#include "Color.h"
#include "Global_Defs.h"
#include "FeatureEdge.h"
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
 * the main function for input into the fourth stage.  This function
 * grows features through the input point cloud.
 */
std::vector< FeatureEdge >	stage5_resolveFeaturePolylines( const std::vector< FeatureEdge > &features,
								PointCloud *cloud );

/**
 * helper function to evaluate the assorted steps of stage 5.
 */
std::vector< Point >		helper5_extractCornerPoints( PointCloud *cloud );
void				helper5_connectToCorners( std::vector<FeatureEdge> &features, const std::vector<Point> &corners );
void				helper5_connectGaps( std::vector<FeatureEdge> &features );
void				helper5_connectNewCorners( std::vector<FeatureEdge> &features, std::vector<Point> &corners );
void				helper5_resolveBadCorners( std::vector<FeatureEdge> &features, std::vector<Point> &corners );
void				helper5_smoothFeatures( std::vector<FeatureEdge> &features, const std::vector< Point > &corners );

void				helper5_findClosestCorner( const Point &point, const Vector &tangent, const std::vector<Point> &corners, unsigned int &closestIndex, float &closestDistance );
void				helper5_findClosestEndPoint( const Point &point, const Vector &tangent, std::vector<FeatureEdge> &features, const unsigned int &startIndex, Point &closestPoint, unsigned int &closestIndex, float &closestDistance );
void				helper5_findClosestFeature( const Point &point, const unsigned int &thisFeature, std::vector< FeatureEdge > &features, unsigned int &featureIndex, unsigned int &pointIndex, float &closestDistance );
void				helper5_findClosestEndPoint2( const Point &point, const Vector &tangent, std::vector< FeatureEdge > &features, unsigned int &closestIndex, float &closestDistance, bool &startFlag );
std::vector< std::pair< unsigned int,bool > >	helper5_countCornerEdges( const Point &point, const std::vector< FeatureEdge > &features );
void				helper5_flipNormals( FeatureEdge &feature );
void				helper5_fixNewCornerNormals( FeatureEdge &feature1, const bool &startFlag, FeatureEdge &split1, FeatureEdge &split2 );
void				helper5_debugCornerPrint( const std::vector<FeatureEdge> &features, const std::vector<Point> &corners );
void				helper5_cleanUpCornerNormals( const Point &corner, std::vector< FeatureEdge > &features );

#endif
