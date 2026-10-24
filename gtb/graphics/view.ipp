
/*

Copyright 2007 University of Utah


This file is part of Afront.

Afront is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Afront is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA

*/


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
