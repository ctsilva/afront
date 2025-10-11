#ifndef __SURFELSETIO_HPP
#define __SURFELSETIO_HPP

//
// Triangle set and Point set input / output routines
//
// Copyright (c) 2001 bla bla
//

#include <gtb/common.hpp>
#include <gtb/graphics/indexed_triangle_set.hpp>
#include <gtb/graphics/surfelset.hpp>
#include <gtb/graphics/surfelsetview.hpp>
#include <gtb/graphics/surfelhierarchy.h>

GTB_BEGIN_NAMESPACE

template<class T>
void read_points(const char* name, tsurfel_set<T>& surfels);
template<class T>
void write_points(const char* name, const tsurfel_set<T>& surfels);
template<class T>
void append_points(const char* name, const tsurfel_set<T>& surfels);

template<class T>
void read_obj(const char* name, tsurfel_set<T>& surfels);
template<class T>
void write_obj(const char* name, const tsurfel_set<T>& surfels);

template <class T>
void write_obj(const char* name, const tsurfelset_view<T>& surfels);
void write_obj(const char* name, const surfel_hierarchy_view& surfels);
template<class T>
void write_obj(FILE* f, const tsurfel_set<T>& surfels);

/*
 * binary points file
 */
#define BP_FORMAT_DOUBLE 1
#define BP_FORMAT_FLOAT 2

struct binpoints_header
{
    // in-memory header of the binary points file
    int version;     // V1 Currently X
    unsigned N;      // V1 Total number of points
    bool hn,hc,hr;   // V1 have normals, colors or radiuses
    unsigned chunks; // V2 # of chunks in the file 
    int format;       // V3 float/double
};

struct binpoints_chunkheader
{
    unsigned K;
};

template<class T>
void read_bin_points(const char* name, tsurfel_set<T>& surfels);
template<class T>
void read_bin_points(FILE* f, tsurfel_set<T>& surfels);
template<class T>
void write_bin_points(const char* name, const tsurfel_set<T>& surfels);
template<class T>
void write_bin_points(FILE* f, const tsurfel_set<T>& surfels);
template<class T>
void append_bin_points(const char* name, const tsurfel_set<T>& surfels);
template<class T>
void append_bin_points(FILE* f, const tsurfel_set<T>& surfels);

void print(const char* prefix, const surfel_set& surfels);
void print(const char* prefix, const surfelset_view& sv);
void print(const char* prefix, const surfel_hierarchy& hierarchy);
void print(const char* prefix, const surfel_hierarchy_view& hv);
void print(const char* prefix, const point3_set& p3s);

GTB_END_NAMESPACE

#endif // __SURFELSETIO_HPP
