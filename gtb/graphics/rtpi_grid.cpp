#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/rtpi_grid.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/rtpi_grid.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


RtpiGrid::RtpiGrid(const ModelRtpi &model)
{
	// find grid dimensions
	real_t w = ((model.max_theta() - model.min_theta()) /
		   (model.num_strips() - 1));
    	real_t min_theta = model.min_theta() - w / 2;
  	real_t min_phi = model.min_phi() - w / 2;
  	_num_rows = 1 + (int) ((model.max_phi() - min_phi) / w);
  	_num_cols = 1 + (int) ((model.max_theta() - min_theta) / w);
	assert(_num_rows > 0);
	assert(_num_cols > 0);
  	fprintf(stderr, "Grid is %dx%d\n", _num_rows, _num_cols);

	// allocate points
  	_data = new Rtpi *[_num_rows];
	assert(_data != NULL);
	for (unsigned i = 0; i < _num_rows; i++) {
		_data[i] = new Rtpi[_num_cols];
		assert(_data[i] != NULL);
	}

	// initialize points
	for (unsigned i = 0; i < model.num_strips(); i++) {
		const RtpiStrip &strip = model.strip(i);
		for (unsigned j = 0; j < strip.num_points(); j++) {
			int k = (int) ((strip[j].p() - min_phi) / w);
			int l = (int) ((strip[j].t() - min_theta) / w);
			assert((k >= 0) && (k < (int) _num_rows));
			assert((l >= 0) && (l < (int) _num_cols));
			_data[k][l] = strip[j];
		}
	}
}


RtpiGrid::~RtpiGrid()
{
	assert(_data != NULL);
	for (unsigned i = 0; i < num_rows(); i++) {
		assert(_data[i] != NULL);
		delete[] _data[i];
		_data[i] = NULL;
	}
	delete[] _data;
	_data = NULL;
}


GTB_END_NAMESPACE
