template <MP_INTEGER Base, MP_INTEGER Exponent> struct Power;

template <MP_INTEGER Base>
struct Power<Base, 0>
{
    enum { Value = 1 };
};

template <MP_INTEGER Base, MP_INTEGER Exponent>
struct Power
{
    enum { Value = Base * Power<Base, Exponent-1>::Value };
};
