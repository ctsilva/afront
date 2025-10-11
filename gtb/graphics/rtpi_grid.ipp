GTB_BEGIN_NAMESPACE


inline unsigned RtpiGrid::num_rows() const
{
	return _num_rows;
}


inline unsigned RtpiGrid::num_cols() const
{
	return _num_cols;
}


inline const Rtpi &RtpiGrid::sample(unsigned i, unsigned j) const
{
	assert(_num_rows > 0);
	assert(_num_cols > 0);
	assert(i < _num_rows);
	assert(j < _num_cols);
	return _data[i][j];
}


GTB_END_NAMESPACE
