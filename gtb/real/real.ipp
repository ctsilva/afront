#include <assert.h>


GTB_BEGIN_NAMESPACE


inline bool real::is_zero(real_t x, real_t eps)
{
	assert(eps >= EPS);
	return (x > -eps) && (x < eps);
}


inline bool real::is_positive(real_t x, real_t eps)
{
	assert(eps >= EPS);
	return x >= eps;
}


inline bool real::is_positive_or_zero(real_t x, real_t eps)
{
	assert(eps >= EPS);
	return x > -eps;
}


inline bool real::is_negative(real_t x, real_t eps)
{
	assert(eps >= EPS);
	return (x <= -eps);
}


inline bool real::is_negative_or_zero(real_t x, real_t eps)
{
	assert(eps >= EPS);
	return x < eps;
}


inline bool real::is_equal(real_t x, real_t y, real_t eps)
{
	assert(eps >= EPS);
	return is_zero(x - y, eps);
}


inline bool real::is_greater(real_t x, real_t y, real_t eps)
{
	assert(eps >= EPS);
	return is_positive(x - y, eps);
}


inline bool real::is_greater_or_equal(real_t x, real_t y, real_t eps)
{
	assert(eps >= EPS);
	return is_positive_or_zero(x - y, eps);
}


inline bool real::is_less(real_t x, real_t y, real_t eps)
{
	assert(eps >= EPS);
	return is_negative(x - y, eps);
}


inline bool real::is_less_or_equal(real_t x, real_t y, real_t eps)
{
	assert(eps >= EPS);
	return is_negative_or_zero(x - y, eps);
}


inline void real::set_eps(real_t eps)
{
	assert(eps > 0.0);
	EPS = eps;
}


template<class T>
inline bool treal<T>::is_zero(value_type x, value_type eps)
{
	assert(eps >= EPS);
	return (x > -eps) && (x < eps);
}

template<class T>
inline bool treal<T>::is_equal(value_type x, value_type y, value_type eps)
{
	return is_zero(x - y, eps);
}

template<class T>
inline bool treal<T>::is_positive(value_type x)
{
	return x >= 0;
}

GTB_END_NAMESPACE
