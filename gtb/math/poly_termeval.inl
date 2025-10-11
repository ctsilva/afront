////////////////////////////////////////////////////////////
// TermEval simply evaluates each term

template <class Real, template <typename> class Basis, int L> 
struct TE;

//template <class Real, template <typename> class Basis, int L> 
//inline Real TermEval(const mp::Tuple<int, L> &degrees,
//		     const mp::Tuple<real_t, L> &values);

template <class Real, template <typename> class Basis>
struct TE<Real, Basis, 1>
{
    static inline Real Eval
	(const mp::Tuple<int, 1> &degrees, 
	 const mp::Tuple<Real, 1> &values)
    {
	return Basis<Real>::TermEval(degrees._value, values._value);
    }

    static inline Real Eval
	(const int *degrees, const Real *values)
    {
	return Basis<Real>::TermEval(*degrees, *values);
    }
};

template <class Real, template <typename> class Basis, int L>
struct TE
{
    static inline Real Eval
	(const mp::Tuple<int, L> &degrees,
	 const mp::Tuple<Real, L> &values)
    {
	return Basis<Real>::TermEval(degrees._value, values._value) *
	    TE<Real, Basis, L-1>::Eval(degrees._rest, values._rest);
    }

    static inline Real Eval
	(const int *degrees, const Real *values)
    {
	return Basis<Real>::TermEval(*degrees, *values) *
	    TE<Real, Basis, L-1>::Eval(degrees+1, values+1);
    }
};

////////////////////////////////////////////////////////////
// PolyEval evaluates the entire polynomial, summing up
// terms one at a time

template 
    <// Poly types
     int PDimension, int PDegree, 
     class Real, template <typename> class Basis,
     // metaprog variables
     int RemDegrees, int CurDimensions
     >
struct PolyEval;

// Helper function
template
    <int PDimension, int PDegree,
     class Real, template <typename> class Basis,
     int RemDegrees, int CurDimensions, int i>
struct SumPolyTerms;

// Base case for PolyEval
template 
    <int PDimension, int PDegree, 
     class Real, template <typename> class Basis,
     int RemDegrees>
struct PolyEval<PDimension, PDegree, Real, Basis,
		RemDegrees, PDimension> // Specialize on CurDimensions = PDimension
{
    static inline Real Eval
	(const Poly<PDimension, PDegree, Real, Basis> &poly,
	 const mp::Tuple<Real, PDimension> &values,
	 const mp::Tuple<int, PDimension> &degrees)
    {
	return poly.Get(degrees) * 
	    TE<Real, Basis, PDimension>::Eval(degrees, values);
    }
};

// Recursion for PolyEval
// Recurse: PolyEval<d, n>(p) = \sum_{i=0}^n PolyEval<d-1,n-i>([i,p])
// We'll call a helper function for the sum
template 
    <// Poly types
     int PDimension, int PDegree, 
     class Real, template <typename> class Basis,
     // metaprog variables
     int RemDegrees, int CurDimensions
     >
struct PolyEval
{
    static inline Real Eval(const Poly<PDimension, PDegree, Real, Basis> &poly,
		     const mp::Tuple<Real, PDimension> &values,
		     const mp::Tuple<int, CurDimensions> &degrees) {
	return SumPolyTerms<PDimension, PDegree, Real, Basis,
	    RemDegrees, CurDimensions, 0>::Eval(poly, values, degrees);
    };
};

// Final case, i == RemDegrees
template
    <int PDimension, int PDegree,
     class Real, template <typename> class Basis,
     int RemDegrees, int CurDimensions>
struct SumPolyTerms
<PDimension, PDegree, Real, Basis, RemDegrees, CurDimensions, RemDegrees>
{
    static inline Real Eval(const Poly<PDimension, PDegree, Real, Basis> &poly,
		     const mp::Tuple<Real, PDimension> &values,
		     const mp::Tuple<int, CurDimensions> &degrees) 
    {
	return PolyEval<PDimension, PDegree, Real, Basis, 0, CurDimensions+1>::Eval
	    (poly, values,mp::Tuple<int, CurDimensions+1>(RemDegrees, degrees));
    }
};
template
    <int PDimension, int PDegree,
     class Real, template <typename> class Basis,
     int RemDegrees, int CurDimensions, int i>
struct SumPolyTerms
{
    static inline Real Eval(const Poly<PDimension, PDegree, Real, Basis> &poly,
		     const mp::Tuple<Real, PDimension> &values,
		     const mp::Tuple<int, CurDimensions> &degrees) 
    {
	return PolyEval<PDimension, PDegree, Real, Basis, RemDegrees-i, CurDimensions+1>::Eval
	    (poly, values, mp::Tuple<int, CurDimensions+1>(i, degrees))
	    + SumPolyTerms<PDimension, PDegree, Real, Basis, RemDegrees, CurDimensions, i+1>::Eval
	    (poly, values, degrees);
    }
};

