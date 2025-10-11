#ifndef EXTINCTION_COEF_SET_INCLUDED
#define EXTINCTION_COEF_SET_INCLUDED

#include <vector>
#include <string>
#include "extinction_coef.hpp"

GTB_BEGIN_NAMESPACE


class extinction_coef_set {
public:
	extinction_coef_set();
	~extinction_coef_set();
	void init(const OctreeNode &node);
	void read(const char *model_file_name, int id);
	void write(const char *model_file_name, int id);
	extinction_coef closest(const Vector3 &v) const;

protected:
	static std::string get_file_name(const char *model_file_name, int id);
	static bool init_points();

	std::vector<extinction_coef> m_samples;
	static std::vector<Point3> m_points;
	static bool m_points_initialized;
};


GTB_END_NAMESPACE

#ifndef OUTLINE
#include "extinction_coef_set.ipp"
#endif

#endif // EXTINCTION_COEF_SET_INCLUDED
