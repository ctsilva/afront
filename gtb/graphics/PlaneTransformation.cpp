#include <gtb/gtb.hpp>
#include "PlaneTransformation.h"

GTB_BEGIN_NAMESPACE

template <class REAL>
void plane_transformation<REAL>::LoadFromPlaneOGLMatrix() const
{
    GLdouble m[16];
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if ((i==3) || (j==3)) m[i+j*4] = 0;
            else m[i+j*4] = Rt[i][j];
        
    m[15] = 1;
    glMultMatrixd(m);
    glTranslated(-T[0], -T[1], -T[2]);
}

template <>
void plane_transformation<float>::write(FILE* f) const
{
    m_plane.write(f);
    write_float(r[0], f);
    write_float(r[1], f);
    write_float(r[2], f);
    write_bool(_have_right_vector, f);
}

template <>
void plane_transformation<double>::write(FILE* f) const
{
    m_plane.write(f);
    write_double(r[0], f);
    write_double(r[1], f);
    write_double(r[2], f);
    write_bool(_have_right_vector, f);
}
    
template <>
void plane_transformation<float>::read(FILE* f)
{
    m_plane.read(f);
    read_float(&r[0], f);
    read_float(&r[1], f);
    read_float(&r[2], f);
    read_bool(&_have_right_vector, f);
    ComputeTransformation();
}

template <>
void plane_transformation<double>::read(FILE* f)
{
    m_plane.read(f);
    read_double(&r[0], f);
    read_double(&r[1], f);
    read_double(&r[2], f);
    read_bool(&_have_right_vector, f);
    ComputeTransformation();
}

template plane_transformation<float>;
template plane_transformation<double>;

GTB_END_NAMESPACE
