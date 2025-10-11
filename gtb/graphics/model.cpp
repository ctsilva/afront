#include <gtb/gtb.hpp>
#ifndef WIN32
#include <gtb/graphics/model.hpp>
#endif // WIN32


#ifdef OUTLINE
#define inline
#include <gtb/graphics/model.ipp>
#undef inline
#endif


GTB_BEGIN_NAMESPACE


template<class T>
tModel<T>::tModel()
	: _is_bounding_box_valid(false),
	  _is_centroid_valid(false),
      _is_median_valid(false)
{
}


template<class T>
tModel<T>::~tModel()
{
}


//
// Apply a matrix to the vertices of the object
//
template<class T>
void tModel<T>::apply(const tMatrix4<T>& M)

{
    printf("tModel<T>::apply not implemented for this object type\n");
}

template<class T>
const typename tModel<T>::Box3 &tModel<T>::bounding_box() const
{
	if (!_is_bounding_box_valid) {
		compute_bounding_box();
		_is_bounding_box_valid = true;
	} 
	return _bounding_box;
}


template<class T>
const tPoint3<T> &tModel<T>::centroid() const
{
	if (!_is_centroid_valid) {
		compute_centroid();
		_is_centroid_valid = true;
	}
	return _centroid;
}

template<class T>
void tModel<T>::invalidate_all()
{
    _is_bounding_box_valid = _is_centroid_valid = _is_median_valid = false;
}

template<class T>
const tPoint3<T>& tModel<T>::median() const
{
    if (!_is_median_valid)
    {
        compute_median();
        _is_median_valid = true;
    }
    return _median;
}

template class tModel<float>;
template class tModel<double>;

GTB_END_NAMESPACE
