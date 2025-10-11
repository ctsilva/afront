/**
 *
 * file:	Global_Defs.h
 * description:	Repository for all the globally defined variables.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _GLOBAL_DEFS_H_
#define _GLOBAL_DEFS_H_

#define	EPSILON		0.00000001
#define PIE		3.14159265

#define NEIGHBOR_MIN	12
#define NEIGHBOR_MAX	100

#define POTENTIAL_NEIGHBOR_MAX 36	// the max neighborhood for potential feature detection (suggested = 1/2*NEIGHBOR_MAX)
#define PROJECT_NEIGHBOR_MAX 72		// the neighborhood for projecting potential features pts (suggested = 2*POTENTIAL_NEIGHBOR_MAX)
#define CORNER_NEIGHBOR_MAX 90		// the neighborhood for projecting to the corner (suggested = 20+PROJECT_NEIGHBOR_MAX)

#define SMOOTH_NEIGHBOR_MIN	10
#define SMOOTH_NEIGHBOR_MAX     30
#define SMOOTH_NEIGHBOR_GROW	5

#define GROW_WEIGHT_THRESHOLD	2
#define SMOOTH_MOVE_THRESHOLD	0.00005

#define GROWTH_ANGLE		30      // allowable growing angle of the features (degrees)
#define GROWTH_DISTANCE		0.01	// allowable growing distance between points

#define CORNER_CLUSTER_DISTANCE	0.25	// the distance of the k-sphere used for resolving corner clusters
#define MIN_FEATURE_LENGTH	4	// defines the minimum length allowable for features (must be > 4 because tangent compute)

#define MIN_REGION_SIZE		5	// the minimum allowed region size for segmentation of the point cloud

#define PIXELS_WIDE		1200	// for screen shots
#define PIXELS_HIGH		1200	// for screen shots

// approximately equal macro
#ifndef EQL
#define EQL(a,b) (fabs(a-b) < EPSILON && fabs(a-b) > -EPSILON)
#endif

#ifndef MIN
#define MIN(a,b) ( ((a)<(b)) ? (a) : (b) )
#endif
#ifndef MAX
#define MAX(a,b) ( ((a)>(b)) ? (a) : (b) )
#endif 

#endif
