#include <assert.h>


GTB_BEGIN_NAMESPACE


inline int IndexedTriangle::A() const
{
	return _indices[0];
}


inline int IndexedTriangle::B() const
{
	return _indices[1];
}


inline int IndexedTriangle::C() const
{
	return _indices[2];
}


inline bool IndexedTriangle::operator==(const IndexedTriangle &t) const
{
	return ((A() == t.A()) &&
		(B() == t.B()) &&
		(C() == t.C()));
}


inline bool IndexedTriangle::operator!=(const IndexedTriangle &t) const
{
	return !(*this == t);
}


inline int IndexedTriangle::operator[](unsigned i) const
{
	assert(i < 3);
	return _indices[i];
}


inline int &IndexedTriangle::operator[](unsigned i)
{
	assert(i < 3);
	return _indices[i];
}


inline void IndexedTriangle::reset(int a, int b, int c)
{
	_indices[0] = a;
	_indices[1] = b;
	_indices[2] = c;
}

inline const int* IndexedTriangle::get_indices() const
{
    return _indices;
}

inline int* IndexedTriangle::get_indices()
{
    return _indices;
}

GTB_END_NAMESPACE
