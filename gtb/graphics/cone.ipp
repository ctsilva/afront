GTB_BEGIN_NAMESPACE


inline bool Cone::operator==(const Cone &c) const
{
	return ((_apex == c._apex)
		&& (_base == c._base));
}


inline bool Cone::operator!=(const Cone &c) const
{
	return !(c == *this);
}


inline const Point3 &Cone::apex() const
{
	return _apex;
}


inline const Circle3 &Cone::base() const
{
	return _base;
}


inline real_t Cone::height() const
{
	return (real_t)((double)Point3::distance(_apex, _base.center()));
}


GTB_END_NAMESPACE
