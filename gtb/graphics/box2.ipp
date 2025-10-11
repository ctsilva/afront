GTB_BEGIN_NAMESPACE

template<class T>
inline bool tBox2<T>::is_order_correct() const
{
	return((_min_pt.x() <= _max_pt.x()) &&
	       (_min_pt.y() <= _max_pt.y()));
}


template<class T>
inline tBox2<T>::tBox2()
{
}


template<class T>
inline tBox2<T>::tBox2(const tBox2 &b)
	: _min_pt(b._min_pt),
	  _max_pt(b._max_pt)
{
	assert(is_order_correct());
}


template<class T>
inline tBox2<T>::tBox2(const tPoint2<T> &min_pt, const tPoint2<T> &max_pt)
	: _min_pt(min_pt),
	  _max_pt(max_pt)
{
	assert(is_order_correct());
}


template<class T>
inline tBox2<T>::tBox2(value_type xmin, value_type ymin,
	   value_type xmax, value_type ymax)
	: _min_pt(xmin, ymin),
	  _max_pt(xmax, ymax)
{
	assert(is_order_correct());
}


template<class T>
inline tBox2<T>::tBox2(const tPoint2<T> &c, value_type lx, value_type ly)
	: _min_pt(c.x() - lx / 2.0, c.y() - ly / 2.0),
	  _max_pt(c.x() + lx / 2.0, c.y() + ly / 2.0)
{
    assert(is_order_correct());
}


template<class T>
inline tBox2<T> &tBox2<T>::operator=(const tBox2 &b)
{
    if (&b != this) {
        _min_pt = b._min_pt;
        _max_pt = b._max_pt;
        assert(is_order_correct());
    }
    return *this;
}


template<class T>
inline bool tBox2<T>::operator==(const tBox2 &b) const
{
    return ((_min_pt == b._min_pt) &&
            (_max_pt == b._max_pt));
}


template<class T>
inline bool tBox2<T>::operator!=(const tBox2 &b) const
{
    return !(*this == b);
}


template<class T>
inline bool tBox2<T>::is_empty() const
{
    return ((_min_pt.x() >= _max_pt.x()) ||
            (_min_pt.y() >= _max_pt.y()));
}


template<class T>
inline const tPoint2<T> &tBox2<T>::min_point() const
{
    return _min_pt;
}


template<class T>
inline const tPoint2<T> &tBox2<T>::get_min_point() const
{
    return _min_pt;
}


template<class T>
inline const tPoint2<T> &tBox2<T>::max_point() const
{
    return _max_pt;
}


template<class T>
inline const tPoint2<T> &tBox2<T>::get_max_point() const
{
    return _max_pt;
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::x_min() const
{
    return _min_pt.x();
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::y_min() const
{
	return _min_pt.y();
}

template<class T>
inline typename tBox2<T>::value_type tBox2<T>::x_max() const
{
    return _max_pt.x();
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::y_max() const
{
    return _max_pt.y();
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::x_length() const
{
    return _max_pt.x() - _min_pt.x();
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::y_length() const
{
    return _max_pt.y() - _min_pt.y();
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::diagonal_length() const
{
    return (_max_pt - _min_pt).length();
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::area() const
{
    return x_length() * y_length();
}


template<class T>
inline tPoint2<T> tBox2<T>::centroid() const
{
    return tPoint2<T>((_min_pt.x() + _max_pt.x()) / 2.0,
		      (_min_pt.y() + _max_pt.y()) / 2.0);
}


template<class T>
inline bool tBox2<T>::contains(const tPoint2<T> &p) const
{
    return ((p.x() >= _min_pt.x()) && (p.x() <= _max_pt.x()) &&
            (p.y() >= _min_pt.y()) && (p.y() <= _max_pt.y()));
}


template<class T>
inline void tBox2<T>::reset(const tPoint2<T> &min_pt, const tPoint2<T> &max_pt)
{
    _min_pt = min_pt;
    _max_pt = max_pt;
    assert(is_order_correct());
}


template<class T>
inline void tBox2<T>::reset(value_type xmin, value_type ymin,
		 value_type xmax, value_type ymax)
{
    _min_pt.reset(xmin, ymin);
    _max_pt.reset(xmax, ymax);
    assert(is_order_correct());
}

template<class T>
inline void tBox2<T>::set_min(int axis, value_type value)
{
    _min_pt[axis] = value;
}

template<class T>
inline void tBox2<T>::set_max(int axis, value_type value)
{
	_max_pt[axis] = value;
}

template<class T>
inline void tBox2<T>::scale(value_type s)
{
    tPoint2<T> origin = centroid();
    _min_pt.scale(origin, s);
    _max_pt.scale(origin, s);
    assert(is_order_correct());
}


template<class T>
inline tPoint2<T> tBox2<T>::vertex(unsigned i) const
{
    assert(i < 4);
    value_type x = (i & RIGHT_MASK) ? _max_pt.x() : _min_pt.x();
    value_type y = (i & UP_MASK) ?    _max_pt.y() : _min_pt.y();
    return tPoint2<T>(x, y);
}


template<class T>
inline tPoint2<T> tBox2<T>::vertex(unsigned f, unsigned i) const
{
    assert(f < 4);
    assert(i < 2);
    return vertex(_vertex_indices[f][i]);
}


template<class T>
inline tVector2<T> tBox2<T>::normal(unsigned i) const
{
    assert(i < 6);
    return tPoint2<T>::normal(vertex(_vertex_indices[i][0]),
			      vertex(_vertex_indices[i][1]));
}

template<class T>
inline void tBox2<T>::read(FILE *fp)
{
    _min_pt.read(fp);
    _max_pt.read(fp);
}


template<class T>
inline void tBox2<T>::write(FILE *fp) const
{
    _min_pt.write(fp);
    _max_pt.write(fp);
}


template<class T>
inline typename tBox2<T>::value_type tBox2<T>::distance(const tPoint2<T> &p) const
{
    tVector2<T> dl = _min_pt - p;
    tVector2<T> dr = p - _max_pt;
    for (int i = 0; i < 3; i++) {
        if (dl[i] < 0) {
                    dl[i] = 0;
        }
        if (dr[i] < 0) {
            dr[i] = 0;
        }
    }
    tVector2<T> diff = dl + dr;
    value_type d = diff.length();
    return d;
}


template<class T>
inline tBox2<T> tBox2<T>::bounding_box(const tPoint2<T> &a,
			       const tPoint2<T> &b,
			       const tPoint2<T> &c)
{
    return tBox2(std::min(std::min(a[0], b[0]), c[0]),
                 std::min(std::min(a[1], b[1]), c[1]),
                 std::max(std::max(a[0], b[0]), c[0]),
                 std::max(std::max(a[1], b[1]), c[1]));
}


template<class T>
inline void tBox2<T>::update(const tPoint2<T>& p)
{
    for (int i = 0; i < 2; ++i)
    {
        _min_pt[i] = std::min(_min_pt[i], p[i]);
        _max_pt[i] = std::max(_max_pt[i], p[i]);
    }
}

GTB_END_NAMESPACE
