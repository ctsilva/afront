#include <assert.h>


GTB_BEGIN_NAMESPACE


inline void read_unsigned(unsigned *i, FILE *fp)
{
	assert(4 == sizeof(unsigned));
	unsigned char buf[4];
	if (fread(buf, 1, 4, fp) != 4) {
		GTB_ERROR("fread failed");
	}
	*i = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}


inline void read_int(int *i, FILE *fp)
{
	assert(4 == sizeof(int));
	read_unsigned((unsigned *) i, fp);
}


inline void read_float(float *f, FILE *fp)
{
	assert(4 == sizeof(float));
	unsigned i;
	read_unsigned(&i, fp);
	*f = *(float *) &i;
}


inline void read_bool(bool *b, FILE *fp)
{
	if (fread(b, sizeof(bool), 1, fp) != 1) {
		GTB_ERROR("fread failed");
	}
}


inline void write_unsigned(unsigned i, FILE *fp)
{
	assert(4 == sizeof(unsigned));
	unsigned char buf[4];
	buf[0] = i & 0xff;
	buf[1] = (i >> 8) & 0xff;
	buf[2] = (i >> 16) & 0xff;
	buf[3] = (i >> 24) & 0xff;
	if (fwrite(buf, 1, 4, fp) != 4) {
		GTB_ERROR("fwrite failed");
	}
}


inline void write_int(int i, FILE *fp)
{
	assert(4 == sizeof(int));
	write_unsigned((unsigned) i, fp);
}


inline void write_float(float f, FILE *fp)
{
	assert(4 == sizeof(float));
	unsigned i = *(unsigned *) &f;
	write_unsigned(i, fp);
}

//
// Bad implementation
//
inline void write_double(double d, FILE *fp)
{
	fwrite(&d, sizeof(d), 1, fp);
}
inline void read_double(double *d, FILE *fp)
{
	fread(d, sizeof(double), 1, fp);
}

inline void write_bool(bool b, FILE *fp)
{
	if (fwrite(&b, sizeof(bool), 1, fp) != 1) {
		GTB_ERROR("fwrite failed");
	}
}


GTB_END_NAMESPACE
