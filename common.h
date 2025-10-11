#ifndef _AFRONT_COMMON_H
#define _AFRONT_COMMON_H


#include <gtb/gtb.hpp>

#include <viewer/debug_draw.h>
#include <mlslib/CProjection.h>
#include <mlslib/poly2.h>


#include <vector>
#include <fstream>


#if defined(REAL_IS_FLOAT)
	typedef float real_type;
#else
	typedef double real_type;
#endif

#ifndef WIN32
#define assertnever(s) { assert(0 && s); }
#endif

typedef gtb::tVector3<real_type> Vector3;
typedef gtb::tVector2<real_type> Vector2;
typedef gtb::tPoint3<real_type> Point3;
typedef gtb::tPoint2<real_type> Point2;
typedef gtb::tBox3<real_type> Box3;
typedef gtb::tTriangle3<real_type> Triangle3;
typedef gtb::tTriangleMesh<real_type> TriangleMesh;
typedef gtb::TriangleMeshFace TriangleMeshFace;
typedef gtb::tPlane<real_type> Plane;
typedef gtb::plane_transformation<real_type> plane_transformation;
typedef gtb::tIndexedTriangleSet<real_type> IndexedTriangleSet;
typedef gtb::AMat<real_type> AMat;
typedef gtb::AVec<real_type> AVec;
typedef gtb::tMatrix4<double> Matrix4d;

typedef gtb::tsurfel_set<real_type> surfel_set;
typedef gtb::tsurfelset_view<real_type> surfelset_view;

using gtb::afree;
using gtb::sptr;
using gtb::aptr;
using gtb::CTimerOnce;

typedef mls::Poly2<real_type> Poly2;
typedef mls::CProjection<real_type> CProjection;
typedef mls::GaussianWeightFunction<real_type> GaussianWeightFunction;


using std::vector;
using std::cerr;
//using std::cout;	// don't write to cout - it is used for camera syncronization
using std::endl;



// sometimes we want static vectors to prevent const allocation/freeing, but want an easy way to change it since it's not thread safe
//#define static_vector(type,name) static vector<type> name; name.clear();
#define static_vector(type,name) vector<type> name; name.clear();


// keep acos in it's domain
inline double acossafe(double x) {
	if (x > 1.0) return acos( 1.0);
	if (x <-1.0) return acos(-1.0);
	return acos(x);
}
inline float acossafe(float x) {
	if (x > 1.0f) return acosf( 1.0f);
	if (x <-1.0f) return acosf(-1.0f);
	return acosf(x);
}


// redraw and wait for the user to push a key
void dbgClear();
void redrawAll();
void redrawAndWait(int key=0, bool force=false);
void waitForKey(int key=0);
int GetUILastKey();

// get the last point the user selected
bool getUISelectPoint(Point3 &p, int &win);


template <typename T>
void reverse(vector<T> &v) {
	for (unsigned i=0; i<v.size()/2; i++) 
		std::swap(v[i], v[v.size()-i-1]);
}


template <typename T>
T max_element(vector<T> &v) {
    T ret = v[0];
    for (unsigned i=1; i<v.size(); i++) {
	if (v[i] > ret)
	    ret = v[i];
    }
    return ret;
}

template <typename T>
T min_element(vector<T> &v) {
    T ret = v[0];
    for (unsigned i=1; i<v.size(); i++) {
	if (v[i] < ret)
	    ret = v[i];
    }
    return ret;
}


#define forvec(index, vector) for (unsigned index=0; index<(vector).size(); index++)


template <typename T>
T square(const T &x) {
    return x*x;
}


template <typename T>
    T clamp(T x, T low, T high) {
    if (x<low) return low;
    if (x>high) return high;
    return x;
}




// since gtb::nrran1f() is not thread safe!!
inline double myran1f(int t) {
#ifdef WIN32
  return (double)rand() / RAND_MAX;
#elif defined(__APPLE__)
  // macOS doesn't have drand48_r, use plain drand48 (not thread-safe but acceptable for this use)
  static bool first = true;
  if (first) {
    srand48(t * 50021);
    first = false;
  }
  return drand48();
#else

  static struct drand48_data data[100];
  static bool first = true;

  if (first) {
    static thlib::CSObject cs;
    cs.enter();
    if (first) {
      first = false;
      for (int i=0; i<100; i++) {
	srand48_r(i*50021, &data[i]);
      }
    }
    cs.leave();
  }


  double res;
  drand48_r(&data[t], &res);
  return res;
#endif
}


#endif // _AFRONT_COMMON_H
