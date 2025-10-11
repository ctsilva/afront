#ifndef GTB_COORDINATE_SYSTEM_INCLUDED
#define GTB_COORDINATE_SYSTEM_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/point3.hpp>
#include <gtb/graphics/vector3.hpp>
#include <gtb/graphics/line3.hpp>

GTB_BEGIN_NAMESPACE


template <class T>
class tCoordinateSystem {
public:
    typedef T value_type;
    typedef tPoint3<T> Point3;
    typedef tVector3<T> Vector3;
    typedef tLine3<T> Line3;

    tCoordinateSystem();
    tCoordinateSystem(const tCoordinateSystem &cs);
    tCoordinateSystem(const Point3 &origin,
                     const Vector3 &x,
                     const Vector3 &y,
                     const Vector3 &z);
    tCoordinateSystem &operator=(const tCoordinateSystem &cs);

    bool operator==(const tCoordinateSystem &cs);
    bool operator!=(const tCoordinateSystem &cs);

    const Point3 &origin() const;
    const Vector3 &x() const;
    const Vector3 &y() const;
    const Vector3 &z() const;

    tCoordinateSystem &reset(const Point3 &origin,
                            const Vector3 &x,
                            const Vector3 &y,
                            const Vector3 &z);

    tMatrix4<T> matrix() const;
    tMatrix4<T> inverse_matrix() const;

    tCoordinateSystem &rotate(const Line3 &l, value_type theta);
    tCoordinateSystem &translate(const Vector3 &t);
    tCoordinateSystem &move_to(const Point3 &p);

    void render(value_type length = 1.0) const;

protected:
    Point3 m_origin;
    Vector3 m_x;
    Vector3 m_y;
    Vector3 m_z;
};

typedef tCoordinateSystem<float> CoordinateSystemf;
typedef tCoordinateSystem<double> CoordinateSystemd;

#ifdef REAL_IS_FLOAT
typedef CoordinateSystemf CoordinateSystem;
#else
typedef CoordinateSystemd CoordinateSystem;
#endif

GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/coordinate_system.ipp>
#endif

#endif // GTB_COORDINATE_SYSTEM_INCLUDED
