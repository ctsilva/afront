#ifndef GTB_QUATERNION_INCLUDED_CSCHEID_20060130_1445
#define GTB_QUATERNION_INCLUDED_CSCHEID_20060130_1445

#include <gtb/graphics/vector3.hpp>

#include <gtb/common.hpp>
#include <gtb/real/real.hpp>
#include <gtb/math/math.hpp>

GTB_BEGIN_NAMESPACE

template <class T>
class tQuaternion {
public:
    typedef T value_type;
    tQuaternion();
    template <class T2> explicit tQuaternion(const tQuaternion<T2> &q);
    
    tQuaternion(const tVector3<T>&axis, T angle);
    tQuaternion(float x, float y, float z, float w);
    
    const tVector3<T> &get_v() const { return v_; };
    float get_w() const { return w_; };

    void set_v(const tVector3<T> &v) { v_ = v; };
    void set_w(float w) { w_ = w; };

    void normalize();
    tQuaternion<T> normalized() const;

    void conjugate();
    tQuaternion<T> conjugated() const;

    T length() const;
    T length2() const;
    
    //! \brief rotation concatenation
    tQuaternion<T>& operator*=(const tQuaternion<T>& q);

    //! \brief rotation of point or vector
    tPoint3<T> operator*=(const tPoint3<T> &p) const;
    tVector3<T> operator*=(const tVector3<T> &v) const;

protected:
    tVector3<T> v_;
    T w_;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const tQuaternion<T> &q);

template <class T>
tQuaternion<T> operator*(const tQuaternion<T> &q1, const tQuaternion<T> &q2);

template <class T>
tPoint3<T> operator*(const tQuaternion<T> &q1, const tPoint3<T> &q2);

template <class T>
tVector3<T> operator*(const tQuaternion<T> &q1, const tVector3<T> &q2);

typedef tQuaternion<float> Quaternionf;
typedef tQuaternion<double> Quaterniond;
#if defined(REAL_IS_FLOAT)
typedef Quaternionf Quaternion;
#else
typedef Quaterniond Quaternion;
#endif

GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/quaternion.ipp>
#endif

#endif
