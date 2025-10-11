/* ======= OrthoNormalBasis.H =========================
 * Joel Daniels -- June 2005
 *
 * The orthonormal basis class maintains the operations
 * to store and manipulate an orthonormal basis.
 * This code is adapted/borrowed from Peter Shirley's
 * Realistic Ray Tracing 2nd Edition.
 * ==================================================== */

// includes
#include "OrthoNormalBasis.h"

// define
#define ONB_EPSILON	0.01

// -- initFrom X --
// initialize by passing a single vector, automatically sets up the other 
// vectors to form an ortho normal basis
void OrthoNormalBasis::initFromU( const Vector& u )
{
	Vector n(1.0,0.0,0.0);
	Vector m(0.0,1.0,0.0);
	U_ = u.unit_vector();
	V_ = U_.cross_product(n);
	if (V_.length() < ONB_EPSILON)
		V_ = U_.cross_product(m);
	W_ = U_.cross_product(V_);
}
void OrthoNormalBasis::initFromV( const Vector& v )
{
	Vector n(1.0,0.0,0.0);
	Vector m(0.0,1.0,0.0);
	V_ = v.unit_vector();
	U_ = V_.cross_product(n);
	if (U_.length() < ONB_EPSILON)
		U_ = V_.cross_product(m);
	W_ = U_.cross_product(V_);
}
void OrthoNormalBasis::initFromW( const Vector& w )
{
	Vector n(1.0,0.0,0.0);
	Vector m(0.0,1.0,0.0);
	W_ = w.unit_vector();
	U_ = W_.cross_product(n);
	if (U_.length() < ONB_EPSILON)
		U_ = W_.cross_product(m);
	V_ = W_.cross_product(U_);
}

// calculate the Orthonormal basis from two vectors
void OrthoNormalBasis::initFromUV( const Vector &u, const Vector &v )
{
	U_ = u.unit_vector();
	W_ = u.unit_cross_product(v);
	V_ = W_.cross_product(U_);
}
void OrthoNormalBasis::initFromUW( const Vector &u, const Vector &w )
{
	U_ = u.unit_vector();
	V_ = w.unit_cross_product(u);
	W_ = U_.cross_product(V_);
}
void OrthoNormalBasis::initFromVU( const Vector &v, const Vector &u )
{
	V_ = v.unit_vector();
	W_ = u.unit_cross_product(v);
	U_ = V_.cross_product(W_);
}
void OrthoNormalBasis::initFromVW( const Vector &v, const Vector &w )
{
	V_ = v.unit_vector();
	U_ = v.unit_cross_product(w);
	W_ = U_.cross_product(V_);
}
void OrthoNormalBasis::initFromWU( const Vector &w, const Vector &u )
{
	W_ = w.unit_vector();
	V_ = w.unit_cross_product(u);
	U_ = V_.cross_product(W_);
}
void OrthoNormalBasis::initFromWV( const Vector &w, const Vector &v )
{
  W_ = w.unit_vector();
  U_ = v.unit_cross_product(w);
  V_ = W_.cross_product(U_);
}

// // -- operator>> --
// // overload the input operator in order to read in an ortho normal basis
// istream &operator>>(istream &input, OrthoNormalBasis &t )
// {
// 	Vector new_u, new_v, new_w;
// 	input >> new_u >> new_v >> new_w;
// 	t.initFromUV( new_u, new_v );
// 	return input;
// }

// // -- operator<< --
// // overlad the output operator in order to output an ortho normal basis
// ostream &operator<<(ostream &output, const OrthoNormalBasis &t )
// {
// 	output << t.u() << "\n" << t.v() << "\n" << t.w() << "\n";
// 	return output;
// }

// -- operator == --
// overload the boolean equal to test
bool operator == (const OrthoNormalBasis &o1, const OrthoNormalBasis &o2 )
{  return (o1.u().approx_equal(o2.u()) && o1.v().approx_equal(o2.v()) && o1.w().approx_equal(o2.w())); }
