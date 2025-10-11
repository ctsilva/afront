#ifndef GTB_RTPI_STRIP_INCLUDED
#define GTB_RTPI_STRIP_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/rtpi.hpp>


GTB_BEGIN_NAMESPACE


class RtpiStrip {
public:
	RtpiStrip();

	void add_point(const Rtpi &p);
	unsigned num_points() const;

	const Rtpi &operator[](unsigned i) const;
	Rtpi &operator[](unsigned i);

protected:
	std::vector<Rtpi> _data;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/rtpi_strip.ipp>
#endif

#endif // GTB_RTPI_STRIP_INCLUDED
