/**
 *
 * file:	FeatureEdge.h
 * description:	Stores and computes common operations for a feature
 *		edge geometry object.  This means that it contains
 *		the endpoints, polyline and tangent information for the
 *		feature.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _FEATURE_EDGE_H_
#define _FEATURE_EDGE_H_

// -- INCLUDES -- //
#include "Global_Defs.h"
#include "Matrix_MxN.h"
#include "Point.h"
#include "Vector.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>

// -- DEFINES -- //

// -- CLASS DEFINTION -- //
class FeatureEdge
{
 public:

  /** 
   * constructors of the feature edge.
   */
			FeatureEdge();
  explicit		FeatureEdge( const std::vector< Point > &pts );
  explicit		FeatureEdge( const std::vector< Feature_Point > &pts );
			FeatureEdge( const std::vector< Point > &pts,
				     const std::vector< Vector > &normals1,
				     const std::vector< Vector > &normals2 );
			FeatureEdge( const std::vector< Point > &pts,
				     const std::vector< std::vector< Vector > > &normals );
			FeatureEdge( const FeatureEdge &other );

  /**
   * assignment operator 
   */
  FeatureEdge&		operator=(const FeatureEdge &other );

  /**
   * accessors for the feature information
   */
  std::vector< Feature_Point >	getFeature() const;
  std::vector< Feature_Point >&	getFeature();

  Vector		getTangentStart();
  Vector		getTangentEnd();

  Point			getStartPoint() const;
  Point			getEndPoint() const;

  Point			getFeaturePoint( const unsigned int &i ) const;
  Point&		getFeaturePoint( const unsigned int &i );

  Vector		getFeatureNormal( const unsigned int &i, const unsigned int &normal ) const;
  Vector&		getFeatureNormal( const unsigned int &i, const unsigned int &normal );

  void			flipNormals1();
  void			flipNormals2();

  unsigned int		getNumPoints() const { return polyline_.size(); }

  /**
   * shortcut modifiers for the feature
   */
  void			appendPoint( const Point &endPoint );
  void			appendPoints( const std::vector<Point> &points );
  void			prependPoint( const Point &endPoint );
  void			prependPoints( const std::vector<Point> &points );

  /** 
   * allows the user to combine to feature edges or split a feature
   */
  FeatureEdge			combineFeatures( const FeatureEdge &other ) const;
  void				attachFeatureToMe( const FeatureEdge &other );
  std::vector<FeatureEdge>	splitFeature( const unsigned int &index, const Point &newPoint, const unsigned int &minSize ) const;
  FeatureEdge			splitFeatureMe( const unsigned int &index, const Point &newPoint, const unsigned int &minSize );
  bool				splitFeatureMe( const unsigned int &index, const Point &newPoint, FeatureEdge &other, const unsigned int &minSize );

  /** 
   * smooths the feature without moving endpoints. returns the max
   * move distance.
   */
  float			smoothFeature();

  /**
   * allows the user to resample the feature with at least the given
   * number of points
   */
  void			resample( const unsigned int &minPoints );

  /**
   * maintains if the end point is connected to a corner point or not,
   * just a flag that the user can set and later access.
   */
  bool			isStartConnected() const { return connected_[0]; };
  bool&			isStartConnected() { return connected_[0]; };
  bool			isEndConnected() const { return connected_[1]; };
  bool&			isEndConnected() { return connected_[1]; };

 protected:

  std::vector< Feature_Point >	polyline_;	// the points of the feature

  bool dirty_;				// the tangents need to be recomputed
  Vector		tangents_[2];	// the tangents for the start and end
  bool			connected_[2];	// the end points are connected to someone else

  /**
   * compute the tangent information
   */
  void			computeTangents();
  static Vector		fitTangent( const Point &p1, const Point &p2, const Point &p3, const Point &p4 );

}; // end class feature edge

#endif // _FEATURE_EDGE_H_
