template <MP_INTEGER N, MP_INTEGER K> struct Choose;

////////////////////////////////////////////////////////////
// Completely compile-time Choose
template <MP_INTEGER N, MP_INTEGER K>
    struct Choose;

template <MP_INTEGER N>
struct Choose<N, 0>
{
    enum { Value = 1 };
};

template <MP_INTEGER N, MP_INTEGER K>
struct Choose
{
    enum { Value = (N * Choose<N-1,K-1>::Value) / K };
};

////////////////////////////////////////////////////////////
// Choose with compile-time k

template <int K> struct ChooseK;
 
template <>
struct ChooseK<0>
{
    int operator()(int) { return 1; }
};

template <int K>
struct ChooseK
{
    // if n >= k, this never truncates!
    // Watch the parentheses order and prove by induction..
    int operator()(int n) {
	ChooseK<K-1> c;
	return (n * c(n-1)) / K;
    };
};

