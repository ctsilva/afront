/* ======= OrthoNormalBasis.H =========================
 * Joel Daniels -- June 2005
 *
 * The orthonormal basis class maintains the operations
 * to store and manipulate an orthonormal basis.
 * This code is adapted/borrowed from Peter Shirley's
 * Realistic Ray Tracing 2nd Edition.
 * ==================================================== */

#ifndef _POINT_H_
#include "Point.h"
#endif

#ifndef _VECTOR_H_
#include "Vector.h"
#endif

#ifndef _ORTHONORMALBASIS_H_
#define _ORTHONORMALBASIS_H_

// includes
#include <iostream>

// defines

// class information
class OrthoNormalBasis {

public:
	// constructors
	OrthoNormalBasis() {}
	OrthoNormalBasis(const Vector &a, const Vector &b, const Vector &c ) { U_ = a; V_ = b; W_ = c; }

	// set the orthonormal basis
	void set(const Vector &a, const Vector &b, const Vector &c ) { U_ = a; V_ = b; W_ = c; }

	// initialize by passing a single vector
	void initFromU( const Vector& u );
	void initFromV( const Vector& v );
	void initFromW( const Vector& w );

	// calculate the Orthonormal basis from two vectors
	void initFromUV( const Vector &u, const Vector &v );
	void initFromUW( const Vector &u, const Vector &w );
	void initFromVU( const Vector &v, const Vector &u );
	void initFromVW( const Vector &v, const Vector &w );
	void initFromWU( const Vector &w, const Vector &u );
	void initFromWV( const Vector &w, const Vector &v );

	// friend istream &operator>>(istream &input, OrthoNormalBasis &t );
	// friend ostream &operator<<(ostream &output, const OrthoNormalBasis &t );
	friend bool operator == (const OrthoNormalBasis &o1, const OrthoNormalBasis &o2 );

	// get an axis from the ortho normal basis
	Vector u() const { return U_; }
	Vector v() const { return V_; }
	Vector w() const { return W_; }

protected:
	Vector U_, V_, W_;
};

#endif	// ORTHONORMALBASIS_H
