/**
 *
 * file:	FLFconversion.h
 * description:	Feature Loop File converter provides the method
 *		to convert the feature edges into a point list
 *		and set of feature loop index list.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

#ifndef _FLF_CONVERSION_H_
#define _FLF_CONVERSION_H_

// -- INCLUDES -- //
#include "Point.h"
#include "Vector.h"
#include "FeatureEdge.h"
#include "PointCloud.h"
#include "FLF_structs.h"

#include <stdio.h>
#include <vector>

/**
 * converts our feature list and point clouds into the needed feature
 * loop lists and point lists.
 */
void FLF_buildLists( std::vector< InfoPoint > &ptList, std::vector< std::vector< InfoIndex > > &featList, const std::vector< FeatureEdge > &edges );

/**
 * helper functions for the building of the lists...for the conversion
 * from features to feature loops and point lists.
 */
void FLF_buildPointList( std::vector< InfoPoint > &ptList, std::vector< std::vector< unsigned int > > &edgesIndices, const std::vector< FeatureEdge > &edges );
unsigned int FLF_addCornerPoint( std::vector< InfoPoint > &ptList, const Point &corner );

void FLF_buildFeatureList( std::vector< std::vector< InfoIndex > > &featList, const std::vector< std::vector< unsigned int > > &edgesIndices, const std::vector< FeatureEdge > &edges );
bool FLF_existsUnfinishedFeature( const std::vector< std::pair< bool,bool > > &featureFlags, unsigned int &indexFeature, unsigned int &offsetDirection, unsigned int &unusedNormal);
void FLF_appendStartToEnd( std::vector< InfoIndex > &featureLoop, const std::vector< std::vector< unsigned int > > &edgesIndices, const std::vector< FeatureEdge > &edges, const unsigned int &featureOffset, const unsigned int &featureNormal );
void FLF_appendEndToStart( std::vector< InfoIndex > &featureLoop, const std::vector< std::vector< unsigned int > > &edgesIndices, const std::vector< FeatureEdge > &edges, const unsigned int &featureOffset, const unsigned int &featureNormal );
void FLF_findNextFeature_FromEnd( unsigned int &featureOffset, unsigned int &featureNormal, unsigned int &featureDirection, const std::vector< std::vector< unsigned int > > &edgesIndices, const std::vector< FeatureEdge > &edges );
void FLF_findNextFeature_FromStart( unsigned int &featureOffset, unsigned int &featureNormal, unsigned int &featureDirection, const std::vector< std::vector< unsigned int > > &edgesIndices, const std::vector< FeatureEdge > &edges );
void FLF_flipNormals( std::vector< InfoPoint > &ptList, std::vector< std::vector< InfoIndex > > &featList );

#endif
