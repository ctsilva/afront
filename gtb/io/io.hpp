#ifndef GTB_IO_INCLUDED
#define GTB_IO_INCLUDED

#include <gtb/common.hpp>
#include <gtb/error/error.hpp>
#include <stdio.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

GTB_BEGIN_NAMESPACE


void read_unsigned(unsigned *i, FILE *fp);
void read_int(int *i, FILE *fp);
void read_float(float *f, FILE *fp);
void read_bool(bool *b, FILE *fp);

void write_unsigned(unsigned i, FILE *fp);
void write_int(int i, FILE *fp);
void write_float(float f, FILE *fp);
void write_bool(bool b, FILE *fp);

void write_double(double d, FILE *fp);
void read_double(double *d, FILE *fp);

void get_file_base_name(const char *file_name, char *base_name, unsigned size);
void get_file_base_name(const char *file_name,
			const char *suffix,
			char *base_name,
			unsigned size);
void get_file_extension(const char *file_name,
			char *extension,
			unsigned size);
FILE *xfopen(const char *file_name, const char *mode);
bool file_exists(const char *file_name);
bool dir_exists(const char *dir_name);

FILE* xfopenv(const char* file_name, int max_backup=5);

GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/io/io.ipp>
#endif

#endif // GTB_IO_INCLUDED
