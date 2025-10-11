
#ifndef LLS_WRAPPER_H
#define LLS_WRAPPER_H


#include "common.h"
#include "lsqr.h"

template <typename real_type>
class LLSWrapper {
public:
	LLSWrapper(int nrows, int ncols);
	~LLSWrapper();

	void InsertA(int r, int c, real_type val);
	void InsertB(int r, real_type val);
	void InsertX(int r, real_type val);
	real_type GetX(int r);

	void Solve();

private:

	class IndexValPair {
	public:
		IndexValPair() { }
		IndexValPair(int _i, real_type _v) : i(_i), v(_v) { }
		int i;
		real_type v;
	};

	// If MODE = 0, compute  y = y + A*x,
	void multA(dvec *x, dvec *y);

	// If MODE = 1, compute  x = x + A^T*y.
	void multAt(dvec *x, dvec *y);


	static void static_mult(long mode, dvec *x, dvec *y, void *ptr);


	vector< vector<IndexValPair> > cols;
	vector< vector<IndexValPair> > rows;

	vector<real_type> x;
	vector<real_type> b;
};

#include "lls_wrapper.inl"

#endif
