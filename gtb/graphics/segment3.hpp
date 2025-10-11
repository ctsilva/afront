#ifndef GTB_SEGMENT3_INCLUDED
#define GTB_SEGMENT3_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/point3.hpp>
#include <gtb/graphics/line3.hpp>

GTB_BEGIN_NAMESPACE

template<class T> class tBox3;

//class Vector3;
//class Box3;
//class Line3;

template<class T>
class tSegment3 {
public:
    typedef T value_type;
    typedef tPoint3<T> Point3;
    typedef tVector3<T> Vector3;
    typedef tLine3<T> Line3;

    tSegment3();
    tSegment3(const tSegment3 &s);
    tSegment3(const Point3 &source, const Point3 &target);
    tSegment3 &operator=(const tSegment3 &s);

    bool operator==(const tSegment3 &s) const;
    bool operator!=(const tSegment3 &s) const;

    const Point3 &source() const;
    const Point3 &target() const;

    const Point3 &min() const;
    const Point3 &max() const;

    value_type squared_length() const;
    value_type length() const;
    Vector3 direction() const;
    tLine3<T> supporting_line() const;
    tBox3<T> bounding_box() const;

    bool is_degenerate() const;
    bool contains(const Point3 &p) const;
    bool collinear_contains(const Point3 &p) const;

    tSegment3 operator-() const;

    // return the distance between the point and the line segment
    T distance(const Point3& p);
    T squared_distance(const Point3& p);

protected:
    Point3 _p, _q;
};

GTB_GENERATE_CLASS_TYPEDEFS(Segment3)

// typedef tSegment3<float> Segment3f;
// typedef tSegment3<double> Segment3d;
// #if defined(REAL_IS_FLOAT)
// typedef Segment3f Segment3;
// #else
// typedef Segment3d Segment3;
// #endif


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/segment3.ipp>
#endif

#endif // GTB_SEGMENT3_INCLUDED
