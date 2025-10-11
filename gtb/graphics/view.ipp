GTB_BEGIN_NAMESPACE


template <class T>
inline tView<T> &tView<T>::operator=(const tView &view)
{
    if (&view != this) {
        _camera = view._camera;
        _viewport = view._viewport;
    }
    return *this;
}

template <class T>
inline const typename tView<T>::Camera &tView<T>::camera() const
{
    return _camera;
}

template <class T>
inline const typename tView<T>::Camera &tView<T>::get_camera() const
{
    return _camera;
}

template <class T>
inline typename tView<T>::Camera &tView<T>::camera()
{
    return _camera;
}

template <class T>
inline typename tView<T>::Camera &tView<T>::get_camera()
{
    return _camera;
}

template <class T>
inline Viewport &tView<T>::viewport()
{
    return _viewport;
}

template <class T>
inline Viewport &tView<T>::get_viewport()
{
    return _viewport;
}

template <class T>
inline const Viewport &tView<T>::viewport() const
{
    return _viewport;
}

template <class T>
inline const Viewport &tView<T>::get_viewport() const
{
    return _viewport;
}


template <class T>
inline void tView<T>::set_camera(const Camera &cam)
{
    _camera = cam;
}

template <class T>
inline void tView<T>::init_exterior_view(const Box3 &box)
{
    _camera.init_exterior_view(box);
}


template <class T>
inline void tView<T>::init_interior_view(const Box3 &box)
{
    _camera.init_interior_view(box);
}


template <class T>
inline void tView<T>::init_image_view(unsigned width, unsigned height)
{
    _viewport.resize(0, 0, width, height);
    _camera.init_image_view();
}


GTB_END_NAMESPACE
