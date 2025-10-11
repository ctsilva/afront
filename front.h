

// the front is just a list of vertices.  by arbitrary convention, the area still to be triangulated
// is to the left of the front (looking at the next vertex in the list, with surface normal up).



#ifndef _FRONT_H
#define _FRONT_H

#include <list>
#include "common.h"

class Front;



#define PRIORITY_GROW_EDGE		0
#define PRIORITY_CONNECT		1


#define PRIORITY_OWA			100		// outside working area

#define PRIORITY_FAILSAFE_ANY		0x7ffffff0
#define PRIORITY_FAILSAFE		0x7ffffffe
#define PRIORITY_FAILSAFE_WILLFAIL	0x7fffffff

#define FRONT_FLAG_FROM_GROW		1
#define FRONT_FLAG_BOUNDARY		2
#define FRONT_FLAG_FEATURE		4


class ProjectionResult {
    public:
    Point3 position;
    Vector3 normal;
    volatile int result;   // success/boundary/fail/notfinished...
};

class FrontElement {
    public:

    typedef std::pair<int,real_type> priority_type;


    FrontElement() { }
    FrontElement(const Point3 &p, const Vector3 &n, int vi, real_type step)
	: position(p), normal(n), vertindex(vi), priority(0x7fffffff,1e34), max_step(step), flags(0) { }


    // info about the vertex
    Point3 position;
    Vector3 normal;

    // the index for the output file (triangles share vertices)
    int vertindex;

    // the priority of the edge starting at this vertex
    priority_type priority;
	
    // how far we can step from this spot
    real_type max_step;

    int flags;

	
    ProjectionResult proj_res;


    // keep track of the position in the heap so we can update it's priority
    int heap_position;

    // we need to know which front we're actually a part of
    Front *front;
};



class Front {

    public:

    typedef std::list<FrontElement> fel;	// front element list
    typedef fel::iterator feli;		// front element list iterator, basically just pointers to FrontElement's,
                                        // nice because they don't get invalidated when shuffeling them around in the lists


    Front();

    // add an element to the end of the list - useful for creating the inital fronts
    feli AddElement(FrontElement &e);



    // split the front at verts i1 and i2, return a pointer to the new front created
    static Front* Split(feli i1, feli i2, feli &n1, feli &n2);

    // merge two fronts, i1's front grows, i2's front will need to be deleted
    static void Merge(feli i1, feli i2, feli &n1, feli &n2);


    // get the next/prev element on the front
    feli FirstElement();
    static feli NextElement(feli i);
    static feli PrevElement(feli i);
    static void RemoveElement(feli i);
    static feli InsertElement(feli next, FrontElement &e);

    bool empty() { return elements.empty(); }
    void verify();

    private:
    // don't allow copies
    Front(const Front &rhs) {
	BREAK;
    }
    Front& operator=(const Front &rhs) {
	BREAK;
	return *this;
    }


    fel elements;

};




#endif





