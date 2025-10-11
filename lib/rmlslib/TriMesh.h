#ifndef __TRIMESH__H
#define __TRIMESH__H

#include "Primitive_Structs.h"
#include "Vector.h"
#include "Point.h"
#include "stdio.h"

typedef struct {
  unsigned int a_,b_,c_;
} Tri_Index;

class TriMesh {
 public:
  TriMesh();
  bool loadOFF(const char* filename);
  bool loadTXT(const char* filename);
  bool loadIV(const char *filename);
  int load3DS(const char* filename);
  std::vector<Point> pointCloud(unsigned int pts, float error_percent, float noiseHeight) const;
  std::vector<Point> pointCloud( const unsigned int &pts, const float &noiseHeight ) const;
  std::vector<Point> getVertices() { return vertices_;};
  ~TriMesh();

private:
  unsigned int nVertices_;
  unsigned int nTriangles_;
  std::vector<Point> vertices_;
  std::vector<Tri_Index> faces_;
};

#endif
