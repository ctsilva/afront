#ifndef __TSETIO_H
#define __TSETIO_H

//
// Triangle set and Point set input / output routines
//
// Copyright (c) 2001 bla bla
//

#include <gtb/common.hpp>
#include <gtb/graphics/indexed_triangle_set.hpp>
#include <gtb/graphics/surfelset.hpp>

GTB_BEGIN_NAMESPACE

void read_obj(FILE* f, IndexedTriangleSet& triangles);
void read_obj(const char* name, IndexedTriangleSet& triangles);
void write_obj(const char* name, const IndexedTriangleSet& triangles, int addidx=0);

void read_triangleset(const char* name, IndexedTriangleSet& triangles);

GTB_END_NAMESPACE

#endif // __TSETIO_H
