#ifndef GTB_MODEL_INCLUDED
#define GTB_MODEL_INCLUDED

#include <gtb/common.hpp>
#include <gtb/graphics/box3.hpp>


GTB_BEGIN_NAMESPACE


template<class T>
class tModel {
public:
    virtual ~tModel();
    typedef tBox3<T> Box3;

    virtual const Box3 &bounding_box() const;
    virtual const tPoint3<T> &centroid() const;
    virtual const tPoint3<T>& median() const;


    //
    // Apply a matrix to the vertices of the object
    //
    virtual void apply(const tMatrix4<T>& M);

protected:
    tModel();

    virtual void compute_bounding_box() const = 0;
    virtual void compute_centroid() const = 0;
    virtual void compute_median() const {}

    void invalidate_all();

    mutable Box3 _bounding_box;
    mutable tPoint3<T> _centroid;
    mutable tPoint3<T> _median;
    mutable bool _is_bounding_box_valid;
    mutable bool _is_centroid_valid;
    mutable bool _is_median_valid;
};


#ifdef REAL_IS_FLOAT
typedef tModel<float> Model;
#else
typedef tModel<double> Model;
#endif


GTB_END_NAMESPACE

#ifndef OUTLINE
#include <gtb/graphics/model.ipp>
#endif

#endif // GTB_MODEL_INCLUDED
