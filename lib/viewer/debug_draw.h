#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include <gtb/graphics/point3.hpp>
#include <gtb/graphics/vector3.hpp>
#include <vector>
#include <viewer/viewer.h>

class DbgPoly {
public:
    template <class T>
	static void add(int win, const gtb::tPoint3<T> &p1, const gtb::tPoint3<T> &c1,
			const gtb::tPoint3<T> &p2, const gtb::tPoint3<T> &c2,
			const gtb::tPoint3<T> &p3, const gtb::tPoint3<T> &c3) {
	    std::vector<gtb::tPoint3<float> > p(3), c(3);
		p[0] = gtb::tPoint3<float>(p1[0],p1[1],p1[2]);	c[0] = gtb::tPoint3<float>(c1[0],c1[1],c1[2]);
		p[1] = gtb::tPoint3<float>(p2[0],p2[1],p2[2]);	c[1] = gtb::tPoint3<float>(c2[0],c2[1],c2[2]);
		p[2] = gtb::tPoint3<float>(p3[0],p3[1],p3[2]);	c[2] = gtb::tPoint3<float>(c3[0],c3[1],c3[2]);
	    add(win, p, c);
	}
    template <class T>
	static void add(const gtb::tPoint3<T> &p1, const gtb::tPoint3<T> &c1,
			const gtb::tPoint3<T> &p2, const gtb::tPoint3<T> &c2,
			const gtb::tPoint3<T> &p3, const gtb::tPoint3<T> &c3) {
	    std::vector<gtb::tPoint3<float> > p(3), c(3);
		p[0] = gtb::tPoint3<float>(p1[0],p1[1],p1[2]);	c[0] = gtb::tPoint3<float>(c1[0],c1[1],c1[2]);
		p[1] = gtb::tPoint3<float>(p2[0],p2[1],p2[2]);	c[1] = gtb::tPoint3<float>(c2[0],c2[1],c2[2]);
		p[2] = gtb::tPoint3<float>(p3[0],p3[1],p3[2]);	c[2] = gtb::tPoint3<float>(c3[0],c3[1],c3[2]);
	    add(p, c);
	}

	static void add(int win, const std::vector<gtb::tPoint3<float> > &p, const std::vector<gtb::tPoint3<float> > &c) 
    { 
	wins[win].madd(p, c); 
    }
	static void add(int win, const std::vector<gtb::tPoint3<double> > &p, const std::vector<gtb::tPoint3<double> > &c) 
    { 
	std::vector<gtb::tPoint3<float> > fp, fc;
	for (unsigned i=0; i<p.size(); ++i)
	    fp.push_back(gtb::tPoint3<float>(p[i]));
	for (unsigned i=0; i<c.size(); ++i)
	    fc.push_back(gtb::tPoint3<float>(c[i]));
	wins[win].madd(fp, fc); 
    }
    template <class T>
	static void add(const std::vector<gtb::tPoint3<T> > &p, const std::vector<gtb::tPoint3<T> > &c) 
    { 
	wins[0].madd(p, c); 
    }
	static void clear(int win) { wins[win].clear(); }
	static void draw(int win) { wins[win].draw(); }
	static void translate(int win, const gtb::tVector3<float> &t) 
    {
	wins[win].translate(t); 
    }

	void madd(const std::vector<gtb::tPoint3<float> > &p, const std::vector<gtb::tPoint3<float> > &c);
    void clear();
    void draw() const;
	void translate(const gtb::tVector3<float> &t);

private:
    std::vector< std::vector<gtb::tPoint3<float> > > points;
    std::vector< std::vector<gtb::tPoint3<float> > > colors;
	static DbgPoly wins[2];
};

class DbgPLines {
public:
    template <class T>
	static void add(int win, const std::vector<gtb::tPoint3<T> > &a, int color=3) { 
	std::vector<gtb::tPoint3<float> > fa;
	for (unsigned i=0; i<a.size(); ++i)
	    fa.push_back(gtb::tPoint3<float>(a[i]));
	wins[win].madd(fa, color); 
    }
    template <class T>
	static void add(const std::vector<gtb::tPoint3<T> > &a, int color=3) { 
	add(0, a, color); 
    }

	static void clear(int win) { wins[win].clear(); }
	static void draw(int win) { wins[win].draw(); }
	static void translate(int win, const gtb::tVector3<float> &t) { 
	    wins[win].translate(t); 
	}
	static void translate(int win, const gtb::tVector3<double> &t) { 
	    wins[win].translate(gtb::tVector3<float>(t)); 
	}

	void madd(const std::vector<gtb::tPoint3<float> > &a, int color);
    void clear();
    void draw() const;
	void translate(const gtb::tVector3<float> &t);

private:
    std::vector< std::vector<gtb::tPoint3<float> > > lines;
	std::vector<int> color;
	static DbgPLines wins[2];
};

class DbgPoints {
public:
    template <class T>
	static void add(int win, const gtb::tPoint3<T> &p, float r, float g, float b) { 
	wins[win].madd(gtb::tPoint3<float>(p), r, g, b); 
    }
    template <class T>
	static void add(const gtb::tPoint3<T> &p, float r, float g, float b) { wins[0].madd(gtb::tPoint3<float>(p), r, g, b); }
	static void clear(int win) { wins[win].clear(); }
	static void draw(int win) { wins[win].draw(); }
	static void translate(int win, const gtb::tVector3<float> &t) { 
	    wins[win].translate(t); 
	}

	void madd(const gtb::tPoint3<float> &p, float r, float g, float b);
    void clear();
    void draw() const;
	void translate(const gtb::tVector3<float> &t);

private:
    std::vector<gtb::tPoint3<float> > points;
	std::vector<float> color;

	static DbgPoints wins[2];
};


// oriented debug points
class DbgOPoints {
public:
    template <class T>
	static void add(int win, const gtb::tPoint3<T> &p, const gtb::tVector3<T> &n, float r, float g, float b) { 
	wins[win].madd(gtb::tPoint3<float>(p), gtb::tVector3<float>(n), r, g, b); 
    }
    template <class T>
	static void add(const gtb::tPoint3<T> &p, const gtb::tVector3<T> &n, float r, float g, float b) { wins[0].madd(gtb::tPoint3<float>(p), gtb::tVector3<float>(n), r, g, b); }
	static void clear(int win) { wins[win].clear(); }
	static void draw(int win) { wins[win].draw(); }
	static void translate(int win, const gtb::tVector3<float> &t) { 
	    wins[win].translate(t); 
	}

	void madd(const gtb::tPoint3<float> &p, const gtb::tVector3<float> &n, float r, float g, float b);
    void clear();
    void draw() const;
	void translate(const gtb::tVector3<float> &t);

private:
    std::vector<gtb::tPoint3<float> > points;
    std::vector<gtb::tVector3<float> > normals;
	std::vector<float> color;

	static DbgOPoints wins[2];
};


class DbgSpheres {
public:
    template <class T>
	static void add(int win, const gtb::tPoint3<T> &p, float rad, float r, float g, float b, float a) { 
	wins[win].madd(gtb::tPoint3<float>(p), rad, r, g, b, a); 
    }
    template <class T>
	static void add(const gtb::tPoint3<T> &p, float rad, float r, float g, float b, float a) { wins[0].madd(gtb::tPoint3<float>(p), rad, r, g, b, a); }
	static void clear(int win) { wins[win].clear(); }
	static void draw(int win) { wins[win].draw(); }

	void madd(const gtb::tPoint3<float> &p, float rad, float r, float g, float b, float a);
    void clear();
    void draw() const;

private:
    std::vector<gtb::tPoint3<float> > points;
	std::vector<float> radius;
	std::vector<float> color;

	static DbgSpheres wins[2];
};


#endif

