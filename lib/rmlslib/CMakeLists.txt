PROJECT(rmlslib)

SET(RMLSLIB_SRCS
    Camera.cpp
    FeatureEdge.cpp
    Octree.cpp
    Point.cpp
    Stage1.cpp
    Stage5.cpp
    Color.cpp
    HeightSurface.cpp
    OctreeNode.cpp
    PointCloud.cpp
    Stage2.cpp
    Stage6.cpp
    FLF_conversion.cpp
    Matrix_3x3.cpp
    OrthoNormalBasis.cpp
    Primitive_Functions.cpp
    Stage3.cpp
    TriMesh.cpp
    FLF_io.cpp
    Matrix_MxN.cpp
    PC_io.cpp
    QuadraticSurface.cpp
    Stage4.cpp
    Vector.cpp
)

ADD_LIBRARY(rmlslib STATIC ${RMLSLIB_SRCS})
