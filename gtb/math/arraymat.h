#ifndef __ARRAYMAT_H
#define __ARRAYMAT_H

GTB_BEGIN_NAMESPACE

/*
 * A wrapper that holds a pointer to a 1d array and treats it as a 2d array
 * Similar to image() with no ownership
 *
 * NOTE: this class is not the owner of the pointer
 */
template <class T>
class ArrayMat
{
public:
	ArrayMat(T* a, unsigned w, unsigned h) :
	  _a(a), W(w), H(h)
	{
	}

	T& operator()(unsigned y, unsigned x)
	{
		assert(y<H);
		assert(x<W);
		return _a[y*W+x];
	}

	T& operator()(unsigned idx)
	{
		assert(idx<W*H);
		return _a[idx];
	}

protected:
	T* _a;
	unsigned W, H;
};

GTB_END_NAMESPACE

#endif // __ARRAYMAT_H
