GTB_BEGIN_NAMESPACE


inline void RtpiStrip::add_point(const Rtpi &p)
{
	_data.push_back(p);
}


inline unsigned RtpiStrip::num_points() const
{
	return _data.size();
}


inline const Rtpi &RtpiStrip::operator[](unsigned i) const
{
	assert(i < _data.size());
	return _data[i];
}


inline Rtpi &RtpiStrip::operator[](unsigned i)
{
	assert(i < _data.size());
	return _data[i];
}


GTB_END_NAMESPACE
