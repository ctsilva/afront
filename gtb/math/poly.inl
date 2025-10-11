template <int Degree,
	  class Real,
	  template <typename> class Basis>
Real Poly1<Degree, Real, Basis>::Eval(Real x) const
{
    Real result=(Real)0;
    for (int ix=0; ix <= Degree; ++ix)
	result += _coefs[imap(ix)] * TermEval(ix, x);
    return result;
}

template <int Degree,
	  class Real,
	  template <typename> class Basis>
Real Poly2<Degree, Real, Basis>::Eval(Real x, Real y) const
{
    Real result=(Real)0;
    for (int ix=0; ix <= Degree; ++ix)
	for (int iy = 0; iy <= Degree - ix; ++iy) {
	    result += _coefs[imap(ix, iy)] 
		* TermEval(ix, x)
		* TermEval(iy, y);
	    }
    return result;
}

template <int Degree,
	  class Real,
	  template <typename> class Basis>
Real Poly3<Degree, Real, Basis>::Eval
(Real x, Real y, Real z) const
{
    Real result=(Real)0;
    for (int ix=0; ix <= Degree; ++ix)
	for (int iy = 0; iy <= Degree - ix; ++iy)
	    for (int iz = 0; iz <= Degree - (iy+ix); ++iz) {
	    result += _coefs[imap(ix, iy, iz)]
		* this->TermEval(ix, x)
		* this->TermEval(iy, y)
		* this->TermEval(iz, z);
	    }
    return result;
}


template <int Degree,
	  class Real,
	  template <typename> class Basis>
void Poly1<Degree, Real, Basis>::LeastSquares
    (const std::vector<Real> &xs,
     const std::vector<Real> &fxs)
{
    AMat<double> a(xs.size(), CoefCount);
    AVec<double> b(fxs.size(), &fxs[0]);
    AVec<double> x(CoefCount);

    for (int i=0; i<(int)xs.size(); ++i)
	for (int j=0; j<=Degree; ++j) 
	    a(i,imap(j)) = TermEval(j, xs[i]);
    SVDSolve(a, b, x);
    for (int i=0; i<CoefCount; ++i)
	_coefs[i] = x[i];
}

template <int Degree,
	  class Real,
	  template <typename> class Basis>
void Poly2<Degree, Real, Basis>::LeastSquares
    (const std::vector<Real> &xs, const std::vector<Real> &ys,
     const std::vector<Real> &fxs)
{
    AMat<double> a(xs.size(), CoefCount);
    AVec<double> b(fxs.size(), &fxs[0]);
    AVec<double> x(CoefCount);

    for (int r=0; r<(int)xs.size(); ++r)
	for (int ix=0; ix<=Degree; ++ix) 
		for (int iy=0; iy<=Degree-ix; ++iy)
		    a(r,imap(ix,iy)) = TermEval(ix, xs[r]) * TermEval(iy, ys[r]);

    SVDSolve(a, b, x);
    for (int i=0; i<CoefCount; ++i)
	_coefs[i] = x[i];
}

template <int Degree,
	  class Real,
	  template <typename> class Basis>
void Poly3<Degree, Real, Basis>::LeastSquares
    (const std::vector<Real> &xs, const std::vector<Real> &ys, const std::vector<Real> &zs,
     const std::vector<Real> &fxs)
{
    AMat<double> a(xs.size(), CoefCount);
    AVec<double> b(fxs.size());
    AVec<double> x(CoefCount);

	for (unsigned i=0; i<fxs.size(); i++)
		b[(int)i] = fxs[i];

    for (int r=0; r<(int)xs.size(); ++r)
	for (int ix=0; ix<=Degree; ++ix) 
		for (int iy=0; iy<=Degree-ix; ++iy)
			for (int iz=0; iz<=Degree-(ix+iy); ++iz)
			    a(r,imap(ix,iy,iz)) = TermEval(ix, xs[r]) * TermEval(iy, ys[r]) * TermEval(iz, zs[r]);

    SVDSolve(a, b, x);
    for (int i=0; i<CoefCount; ++i)
	_coefs[i] = x[i];
}

template <int Degree,
	  class Real,
	  template <typename> class Basis>
void Poly3<Degree, Real, Basis>::LeastSquares
    (const std::vector<Real> &xs, const std::vector<Real> &ys, const std::vector<Real> &zs,
     const std::vector<Real> &fxs, const std::vector<Real> &weights, bool normeqns, poly_lls_solver solver)
{
    AMat<double> a(xs.size(), CoefCount);
    AVec<double> b(fxs.size());
    AVec<double> x(CoefCount);

	for (unsigned i=0; i<fxs.size(); i++)
		b[(int)i] = weights[i] * fxs[i];

    for (int r=0; r<(int)xs.size(); ++r)
	for (int ix=0; ix<=Degree; ++ix)
		for (int iy=0; iy<=Degree-ix; ++iy)
			for (int iz=0; iz<=Degree-(ix+iy); ++iz)
			    a(r,imap(ix,iy,iz)) = weights[r] * this->TermEval(ix, xs[r]) * this->TermEval(iy, ys[r]) * this->TermEval(iz, zs[r]);

	if (normeqns || solver!=POLY_LLS_SVD) {
		AMat<double> at = a.transposed();
		a = at*a;
		b = at*b;
	}

	if (solver==POLY_LLS_SVD) {
	    SVDSolve(a, b, x);
	} else if (solver==POLY_LLS_CHOLESKY) {

		double ar[CoefCount][CoefCount];
		for (int r=0; r<CoefCount; r++) {
			for (int c=0; c<CoefCount; c++) {
				ar[r][c] = a(r,c);
			}
		}
		cholesky<double,CoefCount>(ar, (double*)b, (double*)x);

	} else if (solver==POLY_LLS_HOUSEHOLDER) {

		double ar[CoefCount][CoefCount];
		for (int r=0; r<CoefCount; r++) {
			for (int c=0; c<CoefCount; c++) {
				ar[r][c] = a(r,c);
			}
		}
		householder<double,CoefCount>(ar, (double*)b, (double*)x);

	} else {
		std::cerr<<"no lls solver chosen!"<<std::endl;
	}


    for (int i=0; i<CoefCount; ++i)
		_coefs[i] = x[i];
}


template <int Degree,
	  class Real,
	  template <typename> class Basis>
void Poly1<Degree,Real,Basis>::write(std::ostream &os)
{
    for (int i=0; i<=Degree; ++i)
	os << " +" << _coefs[imap(i)] << " b_" << i;
}

template 
<int Dimension, int Degree, class Real, template <typename> class Basis>
std::ostream &operator<<
(std::ostream &os, const Poly<Dimension,Degree,Real,Basis> &p) {
    p.write(os);
    return os;
}
// Bump the "degree permutation"

template <int Dimension, int Degree>
inline int bump_degrees(int *degrees, int sum_deg)
{
    int cur_deg = 0;
    while (cur_deg < Dimension) {
	degrees[cur_deg]++;
	sum_deg++;
	if (sum_deg > Degree) {
	    sum_deg -= degrees[cur_deg];
	    degrees[cur_deg] = 0;
	    cur_deg++;
	} else
	    return sum_deg;
    }
    return sum_deg;
}

template <int Dimension,
	  int Degree,
	  class Real,
	  template <typename> class Basis>
void Poly<Dimension,Degree,Real,Basis>::write(std::ostream &os) const
{
    int degrees[Dimension] = {0,}, sum_deg=0;
    os << "ListPlot[{";
    for (int i=0; i<CoefCount; ++i) {
	os << "{";
	for (int j=0; j<Dimension; ++j)
	    os << degrees[j] << ", ";
//	os << fixed << setprecision(10) << _coefs[imap(degrees)] << "}";	// fixme!!
	bump_degrees<Dimension, Degree>(degrees,sum_deg);
	if (i < CoefCount-1)
	    os << ", ";
    }
    os << "}, {PlotStyle->{AbsolutePointSize[5]}, PlotRange->All}]" << std::endl;
}

template <int Dimension, int Degree,
	  class Real, template <typename> class Basis>
void Poly<Dimension, Degree, Real, Basis>::LeastSquares
(const gtb::AMat<Real> &xs, const gtb::AVec<Real> &fxs
 )
{
    assert(xs.rows() == fxs.size());
    assert(xs.columns() == Dimension);

    AMat<double> a(xs.rows(), CoefCount);
    AVec<double> b(fxs.size());
    AVec<double> x(CoefCount);
    int degrees[Dimension] = {0,}, d_count = 0;

    for (int r=0; r<(int)xs.rows(); ++r) {
	b(r) = fxs(r);
	for (int c=0; c<CoefCount; ++c) {
	    Real t = TE<Real, Basis, Dimension>::Eval(degrees, &xs(r,0));
	    a(r, c) = t;
	    d_count = bump_degrees<Dimension, Degree>(degrees, d_count);
	}
    }

    AMat<double> at = a.transposed();
    AMat<double> na = at * a;
    AVec<double> nb = at * b;
    
    SVDSolve(na, nb, x);
    for (int i=0; i<CoefCount; ++i) {
	_coefs[imap(degrees)] = (real_t)x(i);
	d_count = bump_degrees<Dimension, Degree>(degrees, d_count);
    }
}


template <int Dimension, int Degree,
	  class Real, template <typename> class Basis>
void Poly<Dimension, Degree, Real, Basis>::LeastSquares
(const gtb::AMat<Real> &xs, 
 const gtb::AVec<Real> &fxs,
 const gtb::AVec<Real> &weights)
{
    assert(xs.rows() == fxs.size());
    assert(xs.columns() == Dimension);

    AMat<double> a(xs.rows(), CoefCount);
    AVec<double> b(fxs.size());
    AVec<double> x(CoefCount);
    int degrees[Dimension] = {0,}, d_count = 0;

    for (int r=0; r<(int)xs.rows(); ++r) {
	b(r) = fxs(r) * weights(r);
	for (int c=0; c<CoefCount; ++c) {
	    Real t = TE<Real, Basis, Dimension>::Eval(degrees, &xs(r,0));
	    a(r, c) = t * weights(r);
	    d_count = bump_degrees<Dimension, Degree>(degrees, d_count);
	}
    }
    SVDSolve(a, b, x);
    for (int i=0; i<CoefCount; ++i) {
	Set(imap(degrees), (real_t)x(i));
	d_count = bump_degrees<Dimension, Degree>(degrees, d_count);
    }
}
