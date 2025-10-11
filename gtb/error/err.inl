GTB_BEGIN_NAMESPACE

inline std::ostream& operator << (std::ostream& s, const CErr& v)
{
	return s << v.m_description;
}

GTB_END_NAMESPACE
