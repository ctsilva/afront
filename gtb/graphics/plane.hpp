#ifndef GTB_PLANE_INCLUDED
#define GTB_PLANE_INCLUDED

#include <gtb/common.hpp>
#include <gtb/real/real.hpp>
#include <gtb/math/math.hpp>
#include <gtb/graphics/vector3.hpp>
#include <gtb/graphics/segment3.hpp>
#include <gtb/graphics/line3.hpp>
#include <gtb/graphics/RSTPlane.hpp>
#include <gtb/graphics/ray3.hpp>

GTB_BEGIN_NAMESPACE


template <class T> class tSegment3;
template <class T> class tPolygon3;
template <class T> class tRay3;

template<class T>
class tPlane {
public:
    typedef T value_type;

    tPlane();
    tPlane(const tPlane &p);
    tPlane(const tVector3<T> &normal, value_type D);
    tPlane(const tVector3<T> &normal, const tPoint3<T>& origin_);
    tPlane(const tPoint3<T> &A, const tPoint3<T> &B, const tPoint3<T> &C);
    tPlane(const tPolygon3<T> &p);

    tPlane(const tCPlaneRST<T>& rhs);
	void fromRST(const tCPlaneRST<T>& rhs);

    tPlane &operator=(const tPlane &p);

    tVector3<T> &normal();
    tVector3<T> &get_normal();
    const tVector3<T> &normal() const;
    const tVector3<T> &get_normal() const;
    void set_normal(const tVector3<T>& n);
    value_type D() const;
    void setD(value_type v_D);
    void set_D(value_type v_D);

    tPoint3<T> origin() const;

    bool operator==(const tPlane &p) const;
    bool operator!=(const tPlane &p) const;

    value_type distance(const tPoint3<T> &) const;
    value_type signed_distance(const tPoint3<T> &) const;

    tPoint3<T> project(const tPoint3<T>& p) const; // project p on the plane

    void flip_orientation();

    bool contains(const tPoint3<T> &p) const;
    bool intersects(const tSegment3<T> &segment,
                    tPoint3<T> &hit_point,
                    value_type &hit_time) const;
	bool intersects(const tRay3<T> &ray,
			tPoint3<T> &hit_point,
			value_type &hit_time) const;


    void read(FILE *fp);
    void write(FILE *fp) const;

    bool intersect(const tPlane& P, tLine3<T>& l);

protected:
    // n.p + D = 0
    tVector3<T> _n;
    value_type _d;
};

typedef tPlane<float> Planef;
typedef tPlane<double> Planed;
#if defined(REAL_IS_FLOAT)
typedef Planef Plane;
#else
typedef Planed Plane;
#endif

GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/plane.ipp>
#endif

#endif // GTB_PLANE_INCLUDED
