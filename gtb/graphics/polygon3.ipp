#include <gtb/graphics/plane.hpp>


GTB_BEGIN_NAMESPACE


template<class T>
inline const tVector3<T> &tPolygon3<T>::normal() const
{
	return _normal;
}


template<class T>
inline typename tPolygon3<T>::value_type tPolygon3<T>::D() const
{
	assert(_points.size() >= 3);
	return -(_normal * _points[0]);
}


template<class T>
inline unsigned tPolygon3<T>::num_points() const
{
	return _points.size();
}


template<class T>
inline const tPoint3<T> &tPolygon3<T>::point(unsigned i) const
{
	assert(i < _points.size());
	return _points[i];
}


template<class T>
inline tBox3<T> tPolygon3<T>::bounding_box() const
{
	return tBox3<T>::bounding_box(_points);
}


template<class T>
inline tPoint3<T> tPolygon3<T>::centroid() const
{
	return tPoint3<T>::centroid(_points);
}


template<class T>
inline tPlane<T> tPolygon3<T>::plane() const
{
	assert(_points.size() >= 3);
	return tPlane<T>(normal(), D());
}


template<class T>
inline bool tPolygon3<T>::intersects(const tSegment3<T> &segment,
			  value_type &hit_time,
			  tPoint3<T> &hit_point) const
{
	assert(_points.size() >= 3);
	if (plane().intersects(segment, hit_point, hit_time)) {
		return contains(hit_point);
	} else {
		return false;
	}
}


template<class T>
inline bool tPolygon3<T>::intersects(const tSegment3<T> &segment) const
{
	value_type hit_time;
	tPoint3<T> hit_point;
	return intersects(segment, hit_time, hit_point);
}


template<class T>
inline bool tPolygon3<T>::asymmetrically_intersects(const tPolygon3 &p) const
{
	for (unsigned i = 0; i < _points.size(); i++) {
		unsigned j = (i + 1) % _points.size();
		tSegment3<T> s(_points[i], _points[j]);
		if (p.intersects(s)) {
			return true;
		}
	}
	return false;		
}


template<class T>
inline bool tPolygon3<T>::intersects(const tPolygon3 &p) const
{
	return (asymmetrically_intersects(p)
		|| p.asymmetrically_intersects(*this));
}


GTB_END_NAMESPACE
