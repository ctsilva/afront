GTB_BEGIN_NAMESPACE


inline timer::timer()
{
}


inline void timer::start()
{
#ifndef WIN32
	gettimeofday(&m_t1, NULL);
#endif
}


inline void timer::stop()
{
#ifndef WIN32
	gettimeofday(&m_t2, NULL);
#endif
}


inline double timer::elapsed_seconds() const
{
	return ((m_t2.tv_usec - m_t1.tv_usec) * 0.000001 +
		(m_t2.tv_sec - m_t1.tv_sec));
}


inline double timer::elapsed_milliseconds() const
{
	return ((m_t2.tv_usec - m_t1.tv_usec) * 0.001 +
		(m_t2.tv_sec - m_t1.tv_sec) * 1000.0);
}


GTB_END_NAMESPACE
