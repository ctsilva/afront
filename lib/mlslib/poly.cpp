#include "stdafx.h"
#include "mlslibdefs.h"
#include "poly.h"
#include "poly1.h"
#include "poly2.h"

using namespace gtb;

MLSLIB_BEGIN_NAMESPACE

template <class REAL>
void write_poly(FILE* f, const Poly<REAL>* c)
{
	write_int(c->degree(), f);
	c->write(f);
}

template <class REAL>
Poly<REAL>* read_poly(FILE* f)
{
	int degree;
	read_int(&degree, f);
	Poly<REAL>* c;
	switch (degree)
	{
	case 1:
		c = new Poly1<REAL>;
		break;
	case 2:
		c = new Poly2<REAL>;
		break;
	default:
		assert(0);
	}
	c->read(f);
	return c;
}

template void write_poly(FILE* f, const Poly<float>* c);
template void write_poly(FILE* f, const Poly<double>* c);
template Poly<float>* read_poly(FILE* f);
template Poly<double>* read_poly(FILE* f);

MLSLIB_END_NAMESPACE
