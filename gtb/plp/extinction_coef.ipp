GTB_BEGIN_NAMESPACE


inline extinction_coef::extinction_coef(float value)
	: m_value(value)
{
}


inline extinction_coef::operator float() const
{
	return m_value;
}


GTB_END_NAMESPACE
