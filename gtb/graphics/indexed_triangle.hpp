#ifndef GTB_INDEXED_TRIANGLE_INCLUDED
#define GTB_INDEXED_TRIANGLE_INCLUDED

#include <gtb/common.hpp>
#include <stdio.h>

GTB_BEGIN_NAMESPACE


class IndexedTriangle {
public:
	IndexedTriangle();
	IndexedTriangle(int A, int B, int C);

	int A() const;
	int B() const;
	int C() const;

	void reset(int A, int B, int C);

	bool operator==(const IndexedTriangle &t) const;
	bool operator!=(const IndexedTriangle &t) const;

	int operator[](unsigned i) const;
	int &operator[](unsigned i);

	void read(FILE *fp);
	void write(FILE *fp) const;

    void flip();

    const int* get_indices() const;
    int* get_indices();
protected:
	int _indices[3];
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/indexed_triangle.ipp>
#endif

#endif // GTB_INDEXED_TRIANGLE_INCLUDED
