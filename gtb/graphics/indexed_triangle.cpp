#include <gtb/gtb.hpp>
#ifndef WIN32
#include <algorithm>
#include <gtb/graphics/indexed_triangle.hpp>
#include <gtb/io/io.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/indexed_triangle.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


IndexedTriangle::IndexedTriangle()
{
	_indices[0] = -1;
	_indices[1] = -1;
	_indices[2] = -1;
}


IndexedTriangle::IndexedTriangle(int arg_A,
				 int arg_B,
				 int arg_C)
{
	_indices[0] = arg_A;
	_indices[1] = arg_B;
	_indices[2] = arg_C;
}


void IndexedTriangle::read(FILE *fp)
{
	read_int(&_indices[0], fp);
	read_int(&_indices[1], fp);
	read_int(&_indices[2], fp);
}


void IndexedTriangle::write(FILE *fp) const
{
	write_int(_indices[0], fp);
	write_int(_indices[1], fp);
	write_int(_indices[2], fp);
}


void IndexedTriangle::flip()
{
    std::swap(_indices[0], _indices[1]);
}

GTB_END_NAMESPACE
