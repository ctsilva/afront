#ifndef GTB_RTPI_GRID_INCLUDED
#define GTB_RTPI_GRID_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/model_rtpi.hpp>


GTB_BEGIN_NAMESPACE


class RtpiGrid {
public:
	explicit RtpiGrid(const ModelRtpi &model);
  	~RtpiGrid();

	unsigned num_rows() const;
	unsigned num_cols() const;
	const Rtpi &sample(unsigned i, unsigned j) const;

protected:

	Rtpi **_data;
	unsigned _num_rows;
	unsigned _num_cols;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/rtpi_grid.ipp>
#endif

#endif // GTB_RTPI_GRID_INCLUDED
