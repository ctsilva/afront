
#ifndef _UHEAP_H
#define _UHEAP_H


#include <vector>
#include "common.h"
#include <cassert>

// updatable heap

// heap class that calls a function heap_position(T*, int) when an element gets moved to a
// new position in the heap.  this allows you to track where an element is, and update the priority
// for just that element, which can be done in log time instead of remaking the whole heap in linear time


// maintains the heap property of each node's children being less than the node


// requires the following two global functions defined:
// bool operator<(const T &, const T &);
// void heap_position(T &, int);



template <typename T>
class UHeap {

public:

	UHeap() { }
	~UHeap() { }

	// put in at the end, and move it up the heap
	void push(T &x) {
		heap_position(x, heap.size());
		heap.push_back(x);
		update_position(heap.size()-1);
	}



	// move the back to the front, and push it back down the heap
	void pop() {
        assert(size() > 0);
		remove(0);
	}

	void remove(int i) {
        assert(i < size());
		heap_position(heap[i], -1);

		// swap the last element into this location
		if (i != (int)heap.size()-1) {
			heap[i] = heap.back();
			heap_position(heap[i], i);
		}
		heap.pop_back();

		// fix the heap
		if ((size() > 0) && (i != size()))
			update_position(i);
	}

	const T& top() {
		return heap.front();
	}




	// update the heap because the element at position i may be in the wrong place
	// usefull for pushing/popping/updating priorities
	void update_position(int i) {

        assert(i < size());

		// check if we have to move up
		int p = parent(i);
		if (p>=0 && heap[p]<heap[i]) {
			std::swap(heap[i], heap[p]);
			heap_position(heap[i], i);
			heap_position(heap[p], p);
			update_position(p);
		}


		// see if we have to move down
		int lc=left(i), rc=right(i);

		int maxc = -1;

		if (lc<(int)heap.size() && rc<(int)heap.size()) {
			maxc = (heap[lc] < heap[rc]) ? rc : lc;
		} else if (lc<(int)heap.size()) {
			maxc = lc;
		} else if (rc<(int)heap.size()) {
			maxc = rc;
		}

		// if we're smaller than the max child, swap with it
		if (maxc>=0 && heap[i]<heap[maxc]) {
			std::swap(heap[i], heap[maxc]);
			heap_position(heap[i], i);
			heap_position(heap[maxc], maxc);
			update_position(maxc);
		}
	}


	bool empty() const {
		return heap.empty();
	}

    int size()
    {
        return heap.size();
    }

	void verify(int i=0) const {

		if (i<(int)heap.size()) {

            assert(heap[i]->heap_position == i);

			int p=parent(i);
			if (p>=0) {
				if (heap[p] < heap[i])
					BREAK;
			}

			int lc=left(i);
			if (lc<(int)heap.size()) {
				if (heap[i] < heap[lc])
					BREAK;
				verify(lc);
			}

			int rc=right(i);
			if (rc<(int)heap.size()) {
				if (heap[i] < heap[rc])
					BREAK;
				verify(rc);
			}
		}
	}

	const std::vector<T>& contents() const {
		return heap;
	}



private:


	
    // zero-based heap
    int left(int ix) const   { return 2 * (ix + 1) - 1;    };
    int right(int ix) const  { return 2 * (ix + 1);        };
    int parent(int ix) const { return ((ix + 1) / 2) - 1;  };


	std::vector<T> heap;

};






#endif
