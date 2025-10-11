/**
 *
 * file:	FeatureEdge.cpp
 * description:	Stores and computes common operations for a feature
 *		edge geometry object.  This means that it contains
 *		the endpoints, polyline and tangent information for the
 *		feature.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "FeatureEdge.h"

/** 
 * constructors of the feature edge.
 */
FeatureEdge::FeatureEdge() 
  : dirty_(false)
{
  tangents_[0] = tangents_[1] = Vector(0,0,0);
  connected_[0] = connected_[1] = false;
}

/**
 * copy in the given polyline. constructor.
 */
FeatureEdge::FeatureEdge( const std::vector< Point > &pts )
{
  polyline_.resize( pts.size() );
  for(unsigned int i=0; i<pts.size(); i++)
    {
      Feature_Point newPoint;
      newPoint.point_ = pts[i];
      newPoint.normal1_ = Vector(1,0,0);
      newPoint.normal2_ = Vector(0,1,0);
      polyline_[i] = newPoint;
    }
  if( pts.size() > 4) computeTangents();
  connected_[0] = connected_[1] = false;
}

/**
 * copy in the given polyline. constructor.
 */
FeatureEdge::FeatureEdge( const std::vector< Feature_Point > &pts )
  : polyline_( pts )
{
  dirty_ = true;
  if( polyline_.size() > 4) computeTangents();
  connected_[0] = connected_[1] = false;
}

/**
 * copy in the given polyline. constructor.
 */
FeatureEdge::FeatureEdge( const std::vector< Point > &pts,
			  const std::vector< Vector > &normals1,
			  const std::vector< Vector > &normals2 )
{
  if (normals1.size() != pts.size() || normals2.size() != pts.size())
    {
      fprintf(stderr,"[ERROR] FeatureEdge() - same number of normals for points!\n");
      return;
    }

  polyline_.resize( pts.size() );
  for(unsigned int i=0; i<pts.size(); i++)
    {
      Feature_Point newPoint;
      newPoint.point_ = pts[i];
      newPoint.normal1_ = normals1[i];
      newPoint.normal2_ = normals2[i];
      polyline_[i] = newPoint;
    }
  if( pts.size() > 4) computeTangents();
  connected_[0] = connected_[1] = false;
}

/**
 * copy in the given polyline. constructor.
 */
FeatureEdge::FeatureEdge( const std::vector< Point > &pts,
			  const std::vector< std::vector< Vector > > &normals )
{
  if (normals.size() != pts.size() )
    {
      fprintf(stderr,"[ERROR] FeatureEdge() - same number of normals for points!\n");
      return;
    }

  polyline_.resize( pts.size() );
  for(unsigned int i=0; i<pts.size(); i++)
    {
      Feature_Point newPoint;
      newPoint.point_ = pts[i];
      if (normals[i].size() >= 2)
	{
	  newPoint.normal1_ = normals[i][0];
	  newPoint.normal2_ = normals[i][1];
	}
      else
	{
	  newPoint.normal1_ = Vector(1,0,0);
	  newPoint.normal2_ = Vector(0,1,0);
	}
      polyline_[i] = newPoint;
    }

  if( pts.size() > 4) computeTangents();
  connected_[0] = connected_[1] = false;
}

/**
 * copy constructor
 */
FeatureEdge::FeatureEdge( const FeatureEdge &other )
  : polyline_(other.polyline_),
    dirty_(other.dirty_)
{
  if (!dirty_)
    {
      tangents_[0] = other.tangents_[0];
      tangents_[1] = other.tangents_[1];
    }
  connected_[0] = other.connected_[0];
  connected_[1] = other.connected_[1];
}

/**
 * assignment operator 
 */
FeatureEdge&	
FeatureEdge::operator=(const FeatureEdge &other )
{
  polyline_ = other.polyline_;
  dirty_ = other.dirty_;
  if (!dirty_)
    {
      tangents_[0] = other.tangents_[0];
      tangents_[1] = other.tangents_[1];
    }
  connected_[0] = other.connected_[0];
  connected_[1] = other.connected_[1];
  return *this;
}

/**
 * the feature polyline accessor
 */
std::vector< Feature_Point >	
FeatureEdge::getFeature() const
{
  return polyline_;
}

/**
 * feature polyline modifier.
 */
std::vector< Feature_Point >&	
FeatureEdge::getFeature()
{
  dirty_ = true;		// possible that it is changed!
  return polyline_;
}

/**
 * get the tangent information for the start of the polyline.
 */
Vector
FeatureEdge::getTangentStart()
{
  // if it is dirty recompute the tangents
  if (dirty_) computeTangents();

  // return the start tangent
  return tangents_[0];
}

/**
 * get the tangent information for the end of the polyline.
 */
Vector
FeatureEdge::getTangentEnd()
{
  // if it is dirty recompute the tangents
  if (dirty_) computeTangents();

  // return the end tangent
  return tangents_[1];
}

/**
 * get the start point for the feature
 */
Point			
FeatureEdge::getStartPoint() const
{
  if (polyline_.size() > 0)
    return polyline_[0].point_;

  fprintf(stderr,"[ERROR] FeatureEdge::getStartPoint() - requested of empty feature!\n");
  return Point(0,0,0);
}

/**
 * get the end point for the feature
 */
Point
FeatureEdge::getEndPoint() const
{
  if (polyline_.size() > 0)
    return polyline_[ polyline_.size()-1 ].point_;

  fprintf(stderr,"[ERROR] FeatureEdge::getEndPoint() - requested of empty feature!\n");
  return Point(0,0,0);
}

/**
 * get the feature point
 */
Point
FeatureEdge::getFeaturePoint( const unsigned int &i ) const
{
  if (polyline_.size() > i )
    {
      return polyline_[i].point_;
    }

  fprintf(stderr,"[ERROR] FeatureEdge::getFeaturePoint(%d) - feature length is %d\n",i,polyline_.size());
  return Point(0,0,0);
}

/**
 * get the feature point
 */
Point&
FeatureEdge::getFeaturePoint( const unsigned int &i )
{
  if (polyline_.size() > i )
    {
      return polyline_[i].point_;
    }

  fprintf(stderr,"[ERROR] FeatureEdge::getFeaturePoint(%d) - feature length is %d\n",i,polyline_.size());
  return polyline_[0].point_;
}

/**
 * get the normal for the feature point
 */
Vector
FeatureEdge::getFeatureNormal( const unsigned int &i, 
			       const unsigned int &normal ) const
{
  if (polyline_.size() > i && normal < 2)
    {
      if (normal == 0) return polyline_[i].normal1_;
      else return polyline_[i].normal2_;
    }

  fprintf(stderr,"[ERROR] FeatureEdge::getFeatureNormal(%d) - feature length is %d\n",i,polyline_.size());
  return Vector(0,0,0);
}

/**
 * get the normal for the feature point
 */
Vector&
FeatureEdge::getFeatureNormal( const unsigned int &i, 
			       const unsigned int &normal )
{
  if (polyline_.size() > i && normal < 2)
    {
      if (normal == 0) return polyline_[i].normal1_;
      else return polyline_[i].normal2_;
    }

  fprintf(stderr,"[ERROR] FeatureEdge::getFeatureNormal(%d) - feature length is %d\n",i,polyline_.size());
  return polyline_[0].normal1_;
}

/**
 * append a point to the polyline
 */
void
FeatureEdge::appendPoint( const Point &endPoint )
{
  Feature_Point newPoint;
  newPoint.point_ = endPoint;
  newPoint.normal1_ = polyline_.back().normal1_;
  newPoint.normal2_ = polyline_.back().normal2_;
  polyline_.push_back( newPoint );
  dirty_ = true;
}

/**
 * append the points to the end of the polyline.
 */
void
FeatureEdge::appendPoints( const std::vector<Point> &points )
{
  for(unsigned int i=0; i<points.size(); i++)
    {
      Feature_Point newPoint;
      newPoint.point_ = points[i];
      newPoint.normal1_ = polyline_.back().normal1_;
      newPoint.normal2_ = polyline_.back().normal2_;
      polyline_.push_back( newPoint );
    }
  dirty_ = true;
}

/**
 * prepend the point on the polyline
 */
void
FeatureEdge::prependPoint( const Point &endPoint )
{
  Feature_Point newPoint;
  newPoint.point_ = endPoint;
  newPoint.normal1_ = polyline_[0].normal1_;
  newPoint.normal2_ = polyline_[0].normal2_;
  polyline_.insert( polyline_.begin(),newPoint );
  dirty_ = true;
}

/**
 * prepend a bunch of points
 */
void 
FeatureEdge::prependPoints( const std::vector<Point> &points )
{
  for(unsigned int i=points.size(); i>0; i--)
    {
      Feature_Point newPoint;
      newPoint.point_ = points[i-1];
      newPoint.normal1_ = polyline_[0].normal1_;
      newPoint.normal2_ = polyline_[0].normal2_;
      polyline_.insert( polyline_.begin(),newPoint );
    }
  dirty_ = true;
}

/** 
 * allows the user to combine to feature edges 
 */
FeatureEdge		
FeatureEdge::combineFeatures( const FeatureEdge &other ) const
{

  // figure out how they meet up - case 1: start with other.start
  if (EQL( 0.0, getStartPoint().distance_to( other.getStartPoint() ) ))
    {
      // setup the points for the new feature
      std::vector<Feature_Point> points( polyline_.size()+other.polyline_.size()-1 );

      // add other's points from back to front
      for(unsigned int i=0; i<other.polyline_.size(); i++)
	{
	  points[i].point_ = other.polyline_[ other.polyline_.size()-1-i ].point_;
	  points[i].normal1_ = other.polyline_[ other.polyline_.size()-1-i ].normal1_;
	  points[i].normal2_ = other.polyline_[ other.polyline_.size()-1-i ].normal2_;
	}

      // add this feature from front to back (skip first)
      for(unsigned int i=1; i<polyline_.size(); i++)
	{
	  points[other.polyline_.size()-1+i].point_ = polyline_[i].point_;
	  points[other.polyline_.size()-1+i].normal1_ = polyline_[i].normal1_;
	  points[other.polyline_.size()-1+i].normal2_ = polyline_[i].normal2_;
	}

      // create and return the feature
      FeatureEdge returnEdge(points);
      returnEdge.connected_[0] = other.connected_[1];
      returnEdge.connected_[1] = connected_[1];
      return returnEdge;
    }

  // figure out how they meet up - case 2: start with other.end
  else if (EQL( 0.0, getStartPoint().distance_to( other.getEndPoint() ) ))
    {
      // setup the points for the new feature
      std::vector<Feature_Point> points( polyline_.size()+other.polyline_.size()-1 );

      // add other's points from front to back
      for(unsigned int i=0; i<other.polyline_.size(); i++)
	{
	  points[i].point_ = other.polyline_[i].point_;
	  points[i].normal1_ = other.polyline_[i].normal1_;
	  points[i].normal2_ = other.polyline_[i].normal2_;
	}

      // add this feature from front to back (skip first)
      for(unsigned int i=1; i<polyline_.size(); i++)
	{
	  points[other.polyline_.size()-1+i].point_ = polyline_[i].point_;
	  points[other.polyline_.size()-1+i].normal1_ = polyline_[i].normal1_;
	  points[other.polyline_.size()-1+i].normal2_ = polyline_[i].normal2_;
	}

      // create and return the feature
      FeatureEdge returnEdge(points);
      returnEdge.connected_[0] = other.connected_[0];
      returnEdge.connected_[1] = connected_[1];
      return returnEdge;
    }

  // figure out how they meet up - case 3: end with other.start 
  else if (EQL( 0.0, getEndPoint().distance_to( other.getStartPoint() ) ))
    {
      // setup the points for the new feature
      std::vector<Feature_Point> points( polyline_.size()+other.polyline_.size()-1 );

      // add this feature from front to back
      for(unsigned int i=0; i<polyline_.size(); i++)
	{
	  points[i].point_ = polyline_[i].point_;
	  points[i].normal1_ = polyline_[i].normal1_;
	  points[i].normal2_ = polyline_[i].normal2_;
	}

      // add other's points from front to back
      for(unsigned int i=1; i<other.polyline_.size(); i++)
	{
	  points[i+polyline_.size()-1].point_ = other.polyline_[i].point_;
	  points[i+polyline_.size()-1].normal1_ = other.polyline_[i].normal1_;
	  points[i+polyline_.size()-1].normal2_ = other.polyline_[i].normal2_;
	}

      // create and return the feature
      FeatureEdge returnEdge(points);
      returnEdge.connected_[0] = connected_[0];
      returnEdge.connected_[1] = other.connected_[1];
      return returnEdge;
    }

  // figure out how they meet up - case 4: end with other.end
  else if (EQL( 0.0, getEndPoint().distance_to( other.getEndPoint() ) ))
    {
      // setup the points for the new feature
      std::vector<Feature_Point> points( polyline_.size()+other.polyline_.size()-1 );

      // add this feature from front to back
      for(unsigned int i=0; i<polyline_.size(); i++)
	{
	  points[i].point_ = polyline_[i].point_;
	  points[i].normal1_ = polyline_[i].normal1_;
	  points[i].normal2_ = polyline_[i].normal2_;
	}

      // add other's points from back to front
      for(unsigned int i=1; i<other.polyline_.size(); i++)
	{
	  points[i+polyline_.size()-1].point_ = other.polyline_[ other.polyline_.size()-1-i ].point_;
	  points[i+polyline_.size()-1].normal1_ = other.polyline_[ other.polyline_.size()-1-i ].normal1_;
	  points[i+polyline_.size()-1].normal2_ = other.polyline_[ other.polyline_.size()-1-i ].normal2_;
	}

      // create and return the feature
      FeatureEdge returnEdge(points);
      returnEdge.connected_[0] = connected_[0];
      returnEdge.connected_[1] = other.connected_[0];
      return returnEdge;
    }
  // couldn't figure out how they meet up! - trouble
  else
    {
      fprintf(stderr,"[ERROR] FeatureEdge::combineFeatures() - two unconnected features!\n");
      return FeatureEdge();
    }
}

/**
 * attach feature to me will combine the features and replace this
 * feature with the result.
 */
void
FeatureEdge::attachFeatureToMe( const FeatureEdge &other )
{
  *this = combineFeatures( other );
}

/** 
 * smooths the feature without moving endpoints. returns the max
 * move distance.
 */
float
FeatureEdge::smoothFeature()
{
  // Laplacian smoothing
  float maxDistance = 0;
  for(unsigned int i=1; i<polyline_.size()-2; i++)
    {
      // get the before and after points in the polyline and move centroid
      Point moved;
      moved = (polyline_[i+1].point_ + polyline_[i-1].point_)/2.0;
      polyline_[i].normal1_ = (polyline_[i+1].normal1_ + polyline_[i-1].normal1_)/2.0;
      polyline_[i].normal2_ = (polyline_[i+1].normal2_ + polyline_[i-1].normal2_)/2.0;

      float tmpDistance = moved.distance_squared( polyline_[i].point_ );
      polyline_[i].point_ = moved;

      // save longest move distance
      if (tmpDistance > maxDistance) maxDistance = tmpDistance;
    }
  return maxDistance;
}

/**
 * given the points it will fit quadratic curve to them and return the
 * tangent at the end point.
 */
Vector
FeatureEdge::fitTangent( const Point &p1, 
			 const Point &p2, 
			 const Point &p3, 
			 const Point &p4 )
{
  // 1. compute parameters for the points based on length
  float u[4];
  u[0] = 0;				// parameter for point p4
  u[1] = p4.distance_to(p3);		// parameter for point p3
  u[2] = u[1] + p3.distance_to(p2);	// parameter for point p2
  u[3] = u[2] + p2.distance_to(p1);	// parameter for point p1

  // 2. fill out the parameter matrix
  Matrix_MxN theParameters(4,3);
  for(unsigned int i=0; i<4; i++)
    {
      theParameters(i,0) = 1.0;
      theParameters(i,1) = u[i];
      theParameters(i,2) = u[i]*u[i];
    }

  // 3. fill out the point matrix
  Matrix_MxN thePointsX(4,1);
  Matrix_MxN thePointsY(4,1);
  Matrix_MxN thePointsZ(4,1);
  thePointsX(0,0) = p4.x();
  thePointsX(1,0) = p3.x();
  thePointsX(2,0) = p2.x();
  thePointsX(3,0) = p1.x();

  thePointsY(0,0) = p4.y();
  thePointsY(1,0) = p3.y();
  thePointsY(2,0) = p2.y();
  thePointsY(3,0) = p1.y();

  thePointsZ(0,0) = p4.z();
  thePointsZ(1,0) = p3.z();
  thePointsZ(2,0) = p2.z();
  thePointsZ(3,0) = p1.z();

  // 4. compute the coefficients for the curve
  Matrix_MxN inverse = theParameters.inverse();
  Matrix_MxN coefficientsX = inverse*thePointsX;
  Matrix_MxN coefficientsY = inverse*thePointsY;
  Matrix_MxN coefficientsZ = inverse*thePointsZ;
  return Vector( coefficientsX(1,0), coefficientsY(1,0), coefficientsZ(1,0) );
}

/**
 * compute the tangent information
 */
void
FeatureEdge::computeTangents()
{
  // only able to compute tangents if the size > 4
  unsigned int size = polyline_.size();
  if (size < 4)
    {
      fprintf(stderr,"[WARNING] FeatureEdge::computeTangents() - requested tangent computation with feature of size %d (must be 4)!\n",polyline_.size());
      tangents_[0] = tangents_[1] = Vector(0,0,0);
    }

  // get the first four points and smooth them
  Point p1 = polyline_[0].point_; 
  Point p2 = polyline_[1].point_;
  Point p3 = polyline_[2].point_;
  Point p4 = polyline_[3].point_;
  for(unsigned int i=0; i<3; i++)
    {
      p1 = (p1+p1+p2)/3.0;
      p2 = (p1+p2+p3)/3.0;
      p3 = (p2+p3+p4)/3.0;
      p4 = (p3+p4+p4)/3.0;
    }
  tangents_[0] = fitTangent( p1,p2,p3,p4 );//polyline_[0],polyline_[1],polyline_[2],polyline_[3] );

  // get the last four points and smooth them
  p1 = polyline_[size-1].point_;
  p2 = polyline_[size-2].point_;
  p3 = polyline_[size-3].point_;
  p4 = polyline_[size-4].point_;
  for(unsigned int i=0; i<3; i++)
    {
      p1 = (p1+p1+p2)/3.0;
      p2 = (p1+p2+p3)/3.0;
      p3 = (p2+p3+p4)/3.0;
      p4 = (p3+p4+p4)/3.0;
    }
  tangents_[1] = fitTangent( p1,p2,p3,p4 );//polyline_[size-1],polyline_[size-2],polyline_[size-3],polyline_[size-4] );
}

/**
 * splits the feature into two features and returns the two features
 * in a standard vector.  the point is replaced by the new point and
 * the two vectors are flagged as connected at that point.
 */
std::vector<FeatureEdge>	
FeatureEdge::splitFeature( const unsigned int &index, const Point &newPoint, const unsigned int &minSize ) const
{
  // copy the polyline into two features
  std::vector<Point> poly1(index+1);
  std::vector<Point> poly2(polyline_.size()-index);
  std::vector< Vector > norms1[2];
  std::vector< Vector > norms2[2];
  for(unsigned int i=0; i<polyline_.size(); i++)
    {
      // copy point into poly1
      if (i<index)
	{
	  poly1[i] = polyline_[i].point_;
	  norms1[0].push_back( polyline_[i].normal1_ );
	  norms1[1].push_back( polyline_[i].normal2_ );
	}
      // copy newpoint into both polys
      else if (i==index)
	{
	  poly1[i] = poly2[0] = newPoint;
	  norms1[0].push_back( polyline_[i].normal1_ );
	  norms1[1].push_back( polyline_[i].normal2_ );
	  norms2[0].push_back( polyline_[i].normal1_ );
	  norms2[1].push_back( polyline_[i].normal2_ );
	}
      // copy point into poly2
      else
	{
	  poly2[ i-index ] = polyline_[i].point_;
	  norms2[0].push_back( polyline_[i].normal1_ );
	  norms2[1].push_back( polyline_[i].normal2_ );
	}
    }
  if (poly1.size() != norms1[0].size()) fprintf(stderr,"[ERROR] WTF sizes are fuct.\n");
  if (poly2.size() != norms2[0].size()) fprintf(stderr,"[ERROR] WTF sizes are fuct.\n");

  // create the two features
  std::vector<FeatureEdge> newFeatures(2);
  newFeatures[0] = FeatureEdge( poly1, norms1[0],norms1[1] );
  newFeatures[1] = FeatureEdge( poly2, norms2[0],norms2[1] );

  // resample the features if too short!
  if (newFeatures[0].getNumPoints() < minSize)
      newFeatures[0].resample( minSize );
  if (newFeatures[1].getNumPoints() < minSize)
    newFeatures[1].resample( minSize );

  // save the connect information
  newFeatures[0].connected_[0] = connected_[0];
  newFeatures[0].connected_[1] = true;
  newFeatures[1].connected_[0] = true;
  newFeatures[1].connected_[1] = connected_[1];

  // return the created features
  return newFeatures;
}

/**
 * splits the feature into two but saves the first split leg as
 * itself, the second is sent on to the user as the returned feature
 * edge.
 */
FeatureEdge			
FeatureEdge::splitFeatureMe( const unsigned int &index, const Point &newPoint, const unsigned int &minSize )
{
  // don't split if index is an endpoint!
  if (index == 0 || index == polyline_.size()-1)
    fprintf(stderr,"[WARNING]: asking to split feature at an end point\n");

  // split the feature into two pieces
  std::vector< FeatureEdge > splits = splitFeature(index,newPoint,minSize);
  *this = splits[0];
  return splits[1];
}

/**
 * splits the feature into two but saves the first split leg as
 * itself, the second is sent on to the user as the returned feature
 * edge. returns true if successful.
 */
bool
FeatureEdge::splitFeatureMe( const unsigned int &index, 
			     const Point &newPoint, 
			     FeatureEdge &other, 
			     const unsigned int &minSize )
{
  // don't split if index is an endpoint!
  if (index == 0 || index == polyline_.size()-1)
    return false;

  // split the feature into two pieces
  std::vector< FeatureEdge > splits = splitFeature(index,newPoint,minSize);
  *this = splits[0];
  other = splits[1];
  return true;
}

/**
 * resample the feature such that the feature will have at least the
 * number of points.
 */
void
FeatureEdge::resample( const unsigned int &minPoints )
{
  // make sure has fewer than points desired
  if (polyline_.size() >= minPoints)
    return;

  // how many line segments to spread the points over
  unsigned int segments = polyline_.size()-1;
  if (segments == 0)
    {
      // no segments so repeat point?!
      fprintf(stderr,"[WARNING] FeatureEdge::resample() - resampling a single point feature!\n");
      std::vector<Feature_Point> tmp(minPoints);
      for(unsigned int i=0; i<minPoints; i++)
	{
	  tmp[i].point_ = polyline_[0].point_;
	  tmp[i].normal1_ = polyline_[0].normal1_;
	  tmp[i].normal2_ = polyline_[0].normal2_;
	}
      polyline_ = tmp;
      return;
    }

  // figure out how many points to put on each segment
  unsigned int need = minPoints-polyline_.size();
  unsigned int betweeners = (unsigned int)(ceil( ((float)need) / ((float)segments) ));

  // storing points into a temporary vector
  std::vector< Feature_Point > tmp(segments*betweeners + segments + 1);
  tmp[0] = polyline_[0];		// save the first point
  unsigned int count = 1;
  for(unsigned int i=1; i<polyline_.size(); i++)
    {
      // start adding betweener points!
      for(unsigned int j=0; j<betweeners; j++)
	{
	  tmp[count].point_ = polyline_[i-1].point_ + (polyline_[i].point_-polyline_[i-1].point_)*((j+1)/(betweeners+1));
	  tmp[count].normal1_ = polyline_[i-1].normal1_ + (polyline_[i].normal1_-polyline_[i-1].normal1_)*((j+1)/(betweeners+1));
	  tmp[count++].normal2_ = polyline_[i-1].normal2_ + (polyline_[i].normal2_-polyline_[i-1].normal2_)*((j+1)/(betweeners+1));
	}

      tmp[count].point_ = polyline_[i].point_;
      tmp[count].normal1_ = polyline_[i].normal1_;
      tmp[count++].normal2_ = polyline_[i].normal2_;
    }
  polyline_ = tmp;
}

/**
 * allow the user to flip the entire first group of normals in the 
 * opposite direction
 */
void 
FeatureEdge::flipNormals1()
{
  for(unsigned int i=0; i<polyline_.size(); i++)
    {
      polyline_[i].normal1_ *= -1;
    }
}
void 
FeatureEdge::flipNormals2()
{
  for(unsigned int i=0; i<polyline_.size(); i++)
    {
      polyline_[i].normal2_ *= -1;
    }
}
