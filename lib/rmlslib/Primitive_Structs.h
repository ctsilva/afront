/**
 *
 * file:	Primitive_Structs.h
 * description:	Stores structures for different combinations of
 *	        primitives so attributes can be saved for the
 *		different types.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _PRIMITIVE_STRUCTS_H_
#define _PRIMITIVE_STRUCTS_H_

#include "Color.h"
#include "Point.h"
#include "Vector.h"

typedef struct {
  Point point_;
  Color color_;
} Color_Point;

typedef struct {
  Vector vector_;
  Color color_;
} Color_Vector;

typedef struct {
  Point point_;
  Vector normal_;
  Color color_;
} Vertex;

typedef struct {
  Vector vector_;
  float value_;
} Eigen_Pair;

typedef struct {
  Point point_;
  float weight_;
  bool nearCorner_;
  std::vector< Vector > normals_;
} Weighted_Point;

typedef struct {
  Point point_;
  Vector normal1_;
  Vector normal2_;
} Feature_Point;

#endif
