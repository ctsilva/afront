template <MP_INTEGER V> struct Factorial;

template <>
struct Factorial<0>
{
    enum { Value = 1 };
};

template <MP_INTEGER V>
struct Factorial
{
    enum { Value = Factorial<V-1>::Value * V };
};
