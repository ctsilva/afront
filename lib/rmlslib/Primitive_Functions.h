/**
 *
 * file:	Primitive_Functions.h
 * description:	Defines some common functions used on the
 *		primitive classes.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _HEIGHT_SURFACE_H_
class HeightSurface;
#endif

#ifndef _PRIMITIVE_FUNCTIONS_H_
#define _PRIMITIVE_FUNCTIONS_H_

// -- INCLUDES -- //
#include "Color.h"
#include "Global_Defs.h"
#include "Matrix_3x3.h"
#include "Matrix_MxN.h"
#include "Point.h"
#include "PointCloud.h"
#include "Primitive_Structs.h"
#include "QuadraticSurface.h"
#include "Vector.h"

#include <stdio.h>
#include <vector>

// -- DEFINES -- //
#define MOVE_EPSILON	0.001
#define SMOOTH_EPSILON  0.0001
#define INIT_RMLS_FIT_SIZE	20 // 12 (before but some not good things...)

// -- FUNCTIONS -- //

/**
 * pca analysis - get back the eigen vectors for a set of 
 * points, color points or vertices.
 */
std::vector< Eigen_Pair > pca_analysis( const std::vector< Point > &pts);
std::vector< Eigen_Pair > pca_analysis( const std::vector< Color_Point > &pts);
std::vector< Eigen_Pair > pca_analysis( const std::vector< Vertex > &verts);

/**
 * weighted pca analysis - get back the eigen vectors for a set of 
 * points, color points or vertices that has been weighted by the
 * given vector values.
 */
std::vector< Eigen_Pair > weighted_pca_analysis( const Point &origin, const std::vector< Point > &pts );
std::vector< Eigen_Pair > weighted_pca_analysis( const std::vector< Point > &pts, std::vector< float > &weights );
std::vector< Eigen_Pair > weighted_pca_analysis( const std::vector< Color_Point > &pts, std::vector< float > &weights );
std::vector< Eigen_Pair > weighted_pca_analysis( const std::vector< Vertex > &verts, std::vector< float > &weights );

/**
 * pca unit normal - returns the unit normal approximated via PCA
 * for the collection of 3d point locations.
 */
Vector pca_unit_normal( const std::vector< Point > &pts );
Vector pca_unit_normal( const std::vector< Color_Point > &pts );
Vector pca_unit_normal( const std::vector< Vertex > &verts );

/**
 * mls surface - this function will project the given point to the MLS 
 * approximated surface.  The projection of the point is returned.
 */
Point mls_projection_linear( const Point &pt, const std::vector< Point > &pt_cloud );
Point mls_projection_quadratic1( const Point &pt, const std::vector< Point > &pt_cloud );
Point mls_projection_quadratic2( const Point &pt, const std::vector< Point > &pt_cloud );

Vertex mls_projection( const Point &pt, const std::vector< Point > &pt_cloud );
Point weighted_mls_projection_quadratic( const Point &pt, const std::vector< Point > &pt_cloud, std::vector< float > &weights );
HeightSurface mls_fit_surface( const Point &origin, const std::vector< Point > &points );

/**
 * project a point to the quadratic curve that goes through the 
 * neighborhood of points.
 */
Point project_2_curve( const Point &projectPt, const std::vector< Point > &pt_cloud );

/**
 * rmls surface - this function will project the given point to the RMLS
 * approximated surface.  The projection of the point is returned.
 */
Vertex rmls_projection( const Point &pt, const std::vector< Point > &neighbors, const float &noiseThreshold_ );
Weighted_Point rmls_projection_2_feature( const Point &project_pt, const std::vector< Point > &neighbors, const float &noiseThreshold );
Weighted_Point rmls_projection_2_corner( const Point &project_pt, const std::vector< Point > &neighbors, const float &noiseThreshold );
bool rmls_projection_2_corner( const Point &project_pt, const std::vector< Point > &neighbors, const float &noiseThreshold, Weighted_Point &corner );
Vertex rmls_projection_2_surface( const Point &project_pt, const std::vector< Point > &neighbors, const float &noiseThreshold );

HeightSurface rmls_fit_surface( const Point &startPoint, std::vector< Point > &remainingNeighbors, const float &noiseThreshold );
std::vector< HeightSurface > rmls_fit_surfaces( const std::vector< Point > &neighbors, const float &noiseThreshold );
Point rmls_smoothest_point( const std::vector< Point > &neighbors, const float &noiseThreshold );

/**
 * planar surface intersection line and point computations.
 */
Point
planeIntersect( const Point &projectMe,
		const Point &pointA, const Vector &normalA,
		const Point &pointB, const Vector &normalB );

#ifndef _HEIGHT_SURFACE_H_
#include "HeightSurface.h"
#endif

#endif
