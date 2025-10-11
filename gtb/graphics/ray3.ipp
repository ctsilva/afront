#include <gtb/graphics/line3.hpp>


GTB_BEGIN_NAMESPACE


template<class T>
inline tRay3<T> &tRay3<T>::operator=(const tRay3 &r)
{
	if (&r != this) {
		_source = r._source;
		_direction = r._direction;
	}
	return *this;
}


template<class T>
inline bool tRay3<T>::operator==(const tRay3 &r) const
{
	return (_source == r._source) && (_direction == r._direction);
}


template<class T>
inline bool tRay3<T>::operator!=(const tRay3 &r) const
{
	return !(*this == r);
}


template<class T>
inline tRay3<T> tRay3<T>::operator-() const
{
	return tRay3(_source, -_direction);
}


template<class T>
inline const tPoint3<T> &tRay3<T>::source() const
{
	return _source;
}


template<class T>
inline const tVector3<T> &tRay3<T>::direction() const
{
	return _direction;
}


template<class T>
inline bool tRay3<T>::is_degenerate() const
{
	return !real::is_equal(_direction.length(), 1.0);
}


template<class T>
inline const tPoint3<T> tRay3<T>::point(value_type a) const
{
	return _source + (a * _direction);
}


template<class T>
inline typename tRay3<T>::value_type tRay3<T>::t(const tPoint3<T> &p) const
{
	return (p - _source).dot(_direction);
}


template<class T>
inline tLine3<T> tRay3<T>::supporting_line() const
{
//	return tLine3<T>(*this);
    return tLine3<T>(source(), direction());
}


template<class T>
inline bool tRay3<T>::contains(const tPoint3<T> &p) const
{
	return (p - _source).cross(_direction).is_zero() && (t(p) >= 0.0);
}


template<class T>
inline tPoint3<T> tRay3<T>::projection(const tPoint3<T> &p) const
{
	return _source + t(p) * _direction;
}


GTB_END_NAMESPACE
