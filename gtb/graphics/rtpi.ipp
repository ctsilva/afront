GTB_BEGIN_NAMESPACE


inline real_t Rtpi::r() const
{
	return _r;
}


inline real_t Rtpi::t() const
{
	return _t;
}


inline real_t Rtpi::p() const
{
	return _p;
}


inline int Rtpi::i() const
{
	return _i;
}


inline void Rtpi::set_r(real_t a_r)
{
	_r = a_r;
}


inline void Rtpi::set_t(real_t a_t)
{
	_t = a_t;
}


inline void Rtpi::set_p(real_t a_p)
{
	_p = a_p;
}


inline void Rtpi::set_i(int a_i)
{
	_i = a_i;
}


inline void Rtpi::reset(real_t a_r, real_t a_t, real_t a_p, int a_i)
{
	_r = a_r;
	_t = a_t;
	_p = a_p;
	_i = a_i;
}


inline void Rtpi::read(FILE *fp)
{
#ifdef REAL_IS_FLOAT
	read_float(&_r, fp);
	read_float(&_t, fp);
	read_float(&_p, fp);
	read_int(&_i, fp);
#else
	float pr, pt, pp;
	int pi;
	read_float(&pr, fp);
	read_float(&pt, fp);
	read_float(&pp, fp);
	read_int(&pi, fp);
	reset(pr, pt, pp, pi);
#endif
}


inline void Rtpi::write(FILE *fp) const
{
	write_float(_r, fp);
	write_float(_t, fp);
	write_float(_p, fp);
	write_int(_i, fp);
}


GTB_END_NAMESPACE
