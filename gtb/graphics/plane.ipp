GTB_BEGIN_NAMESPACE

template<class T>
inline tPlane<T>::tPlane()
//	: _n(tVector3<T>(0.0, 0.0, 1.0)),
//	  _D(0.0)
{
}


template<class T>
inline tPlane<T>::tPlane(const tVector3<T> &arg_n, value_type arg_D)
	: _n(arg_n),
	  _d(arg_D)
{
}

template<class T>
inline tPlane<T>::tPlane(const tVector3<T> &normal, const tPoint3<T>& origin_) :
    _n(normal),
    _d(-normal.dot(origin_))
{
}

template<class T>
inline tPlane<T>::tPlane(const tPoint3<T> &A, const tPoint3<T> &B, const tPoint3<T> &C)
{
	assert(!tPoint3<T>::collinear(A, B, C));
	_n = tPoint3<T>::normal(A, B, C);

//  	if (!real::is_equal(_n.squared_length(), 1.0, 0.00001)) {
//  		fprintf(stderr, "WARNING: bad plane normal\n");
//  		fprintf(stderr, "n = (%f %f %f)\n", _n.x(), _n.y(), _n.z());
//  		fprintf(stderr, "|n| = %f\n", _n.squared_length());
//  		//exit(EXIT_FAILURE);
//  	}

	_d = -(_n * A);

//  	if (!real::is_zero(_n * A + _D, 0.00001)) {
//  		fprintf(stderr, "WARNING: bad plane equation\n");
//  		fprintf(stderr, "np + D = %f\n", _n * A + _D);
//  		//exit(EXIT_FAILURE);
//  	}
}


template<class T>
inline tPlane<T>::tPlane(const tPlane &p)
	: _n(p._n),
	  _d(p._d)
{
}


template<class T>
inline tPlane<T>::tPlane(const tPolygon3<T> &p)
	: _n(p.normal()),
	  _d(p.D())
{
}

/*
 * D = sin(rhs.t) because that's the way the optimization function
 * computes things, the idea is to find a minimum of d, we do not
 * want d=infinity because of numerical problems.
 *
 */
template<class T>
inline tPlane<T>::tPlane(const tCPlaneRST<T>& rhs) :
    _n(cos(rhs.r)*cos(rhs.s), sin(rhs.r)*cos(rhs.s), sin(rhs.s)),
    _d(rhs.t) // WAS t(sin(rhs.t))
{
}

template<class T>
inline void tPlane<T>::fromRST(const tCPlaneRST<T>& rhs) {
	_n[0] = cos(rhs.r)*cos(rhs.s);
	_n[1] = sin(rhs.r)*cos(rhs.s);
	_n[2] = sin(rhs.s);
	_d = rhs.t;
}


template<class T>
tPlane<T> &tPlane<T>::operator=(const tPlane &p)
{
    if (&p != this) {
        _n = p._n;
        _d = p._d;
    }
    return *this;
}

template<class T>
bool tPlane<T>::contains(const tPoint3<T> &p) const
{
	return real::is_zero(p.x() * _n.x() +
			     p.y() * _n.y() +
			     p.z() * _n.z() +
			     _d);
}

//
// assume _n is normalized
template<class T>
tPoint3<T> tPlane<T>::origin() const
{
    return -_n*_d;
}


template<class T>
inline tVector3<T> &tPlane<T>::normal()
{
	return _n;
}

template<class T>
inline tVector3<T> &tPlane<T>::get_normal()
{
	return _n;
}

template<class T>
inline const tVector3<T> &tPlane<T>::normal() const
{
	return _n;
}

template<class T>
inline const tVector3<T> &tPlane<T>::get_normal() const
{
	return _n;
}

template<class T>
inline void tPlane<T>::set_normal(const tVector3<T>& n)
{
    _n = n;
}

template<class T>
inline void tPlane<T>::setD(value_type v_D)
{
	_d = v_D;
}

template<class T>
inline void tPlane<T>::set_D(value_type v_D)
{
	_d = v_D;
}

template<class T>
inline typename tPlane<T>::value_type tPlane<T>::D() const
{
	return _d;
}


template<class T>
inline bool tPlane<T>::operator==(const tPlane &p) const
{
	return ((_n == p._n) &&
		real::is_equal(_d, p._d));
}


template<class T>
inline bool tPlane<T>::operator!=(const tPlane &p) const
{
	return !(*this == p);
}


template<class T>
inline typename tPlane<T>::value_type tPlane<T>::signed_distance(const tPoint3<T> &p) const
{
	return (p.x() * _n.x() +
		p.y() * _n.y() +
		p.z() * _n.z()) + _d;
}


template<class T>
inline typename tPlane<T>::value_type tPlane<T>::distance(const tPoint3<T> &p) const
{
	return abs(signed_distance(p));
}


GTB_END_NAMESPACE
