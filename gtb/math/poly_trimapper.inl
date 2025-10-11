// Map a tuple of triangular (pyramidal, etc) coordinates 
// to a 1D layout

template <int D>
inline int triangular_map_sum(const mp::Tuple<int, D> &x);
template <int D>
inline int triangular_map(const mp::Tuple<int, D> &x);
template <int D>
inline int triangular_map(const int *x);
template <int D>
inline int triangular_map_sum(const int *x);

template <>
inline int triangular_map_sum(const mp::Tuple<int, 1> &x)
{
    return x._value;
}
template <int D>
inline int triangular_map_sum(const mp::Tuple<int, D> &x)
{
    return x._value + triangular_map_sum(x._rest);
}

template <>
inline int triangular_map_sum<1>(const int *x)
{
    return *x;
}
template <int D>
inline int triangular_map_sum(const int *x)
{
    return *x + triangular_map_sum<D-1>(x+1);
}

template <>
inline int triangular_map(const mp::Tuple<int, 1> &x)
{
    return x._value;
}

template <int D>
inline int triangular_map(const mp::Tuple<int, D> &x)
{
    mp::ChooseK<D> p;
    return triangular_map(x._rest) + 
	p(triangular_map_sum(x)+D-1);
}

template <>
inline int triangular_map<1>(const int *x) { return *x; }

template <int D>
inline int triangular_map(const int *x) 
{ 
    mp::ChooseK<D> p;
    return triangular_map<D-1>(x+1) + p(triangular_map_sum<D>(x)+D-1);
}
