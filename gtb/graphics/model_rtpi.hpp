#ifndef GTB_MODEL_RTPI_INCLUDED
#define GTB_MODEL_RTPI_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/model.hpp>
#include <gtb/graphics/rtpi_strip.hpp>
#include <vector>


GTB_BEGIN_NAMESPACE


class ModelRtpi : public Model {
public:
	ModelRtpi();
	~ModelRtpi();

	void read(FILE *fp);
	void write(FILE *fp);

	unsigned num_strips() const;
	const RtpiStrip &strip(unsigned i) const;
	RtpiStrip &strip(unsigned i);
    void insert_strip(const RtpiStrip& strip);

	unsigned num_points() const; // total
	unsigned max_points() const; // per strip

	real_t min_range() const;
	real_t max_range() const;

	real_t min_theta() const;
	real_t max_theta() const;

	real_t min_phi() const;
	real_t max_phi() const;

	int min_intensity() const;
	int max_intensity() const;

	void histogram(std::vector<unsigned> &hist);
	void histogram_add(std::vector<unsigned> &hist);

	void equalize_histogram(unsigned hist_size = 256);

	void scale(real_t a);

	static void read_header(FILE *fp,
				unsigned &n_strips,
				unsigned &max_points);

protected:

	void compute_bounding_box() const;
	void compute_centroid() const;

  	std::vector<RtpiStrip *> _strips;
	unsigned _max_points;
	real_t _min_r, _max_r;
  	real_t _min_t, _max_t;
	real_t _min_p, _max_p;
	int _min_i, _max_i;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/model_rtpi.ipp>
#endif

#endif // GTB_MODEL_RTPI_INCLUDED
