GTB_BEGIN_NAMESPACE


inline unsigned ModelRtpi::num_strips() const
{
	return _strips.size();
}


inline const RtpiStrip &ModelRtpi::strip(unsigned i) const
{
	assert(i < _strips.size());
	assert(_strips[i] != NULL);
	return *(_strips[i]);
}


inline RtpiStrip &ModelRtpi::strip(unsigned i)
{
	assert(i < _strips.size());
	assert(_strips[i] != NULL);
	return *(_strips[i]);
}


inline unsigned ModelRtpi::num_points() const
{
	unsigned n = 0;
	for (unsigned i = 0; i < _strips.size(); i++) {
		n += strip(i).num_points();
	}
	return n;
}


inline unsigned ModelRtpi::max_points() const
{
	return _max_points;
}


inline real_t ModelRtpi::min_range() const
{
	return _min_r;
}


inline real_t ModelRtpi::max_range() const
{
	return _max_r;
}


inline real_t ModelRtpi::min_theta() const
{
	return _min_t;
}


inline real_t ModelRtpi::max_theta() const
{
	return _max_t;
}


inline real_t ModelRtpi::min_phi() const
{
	return _min_p;
}


inline real_t ModelRtpi::max_phi() const
{
	return _max_p;
}


inline int ModelRtpi::min_intensity() const
{
	return _min_i;
}


inline int ModelRtpi::max_intensity() const
{
	return _max_i;
}


GTB_END_NAMESPACE
