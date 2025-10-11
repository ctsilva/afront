#ifndef GTB_RTPI_INCLUDED
#define GTB_RTPI_INCLUDED

#include <gtb/common.hpp>
#include <gtb/real/real.hpp>
#include <gtb/graphics/point3.hpp>


GTB_BEGIN_NAMESPACE


// Range, theta, phi, intensity
class Rtpi {
public:
	Rtpi();
	Rtpi(real_t r, real_t t, real_t p, int i);

	real_t r() const;
	real_t t() const;
	real_t p() const;
	int i() const;

	void set_r(real_t);
	void set_t(real_t);
	void set_p(real_t);
	void set_i(int);
	void reset(real_t r, real_t t, real_t p, int i);

	Point3 point() const;

	void read(FILE *fp);
	void write(FILE *fp) const;

protected:
	real_t _r, _t, _p;
	int _i;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/rtpi.ipp>
#endif

#endif // GTB_RTPI_INCLUDED
