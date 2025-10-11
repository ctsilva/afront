/**
 *
 * file:	Matrix_3x3.h
 * description:	Defines some common functions used in a Matrix
 *		class designed specifically for 3 by 3 matrices.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _MATRIX_3x3_H_
#define _MATRIX_3x3_H_

// -- INCLUDES -- //
#include "Primitive_Structs.h"
#include "Global_Defs.h"
#include <math.h>
#include <vector>

// -- DEFINES -- //
#ifndef SIGN
#define SIGN(a,b) (b >= 0 ? a : -a)
#endif
#ifndef FMAX
#define FMAX(a,b) (a >= b ? a : b)
#endif
#ifndef ROTATE
#define ROTATE(a,i,j,k,l) g=a(i,j); h=a(k,l); a(i,j)=g-s*(h+g*tau);\
		          a(k,l) = h+s*(g-h*tau);
#endif
#ifndef EQL
#define EQL(a,b) (fabs(a-b) < EPSILON && fabs(a-b) > -EPSILON)
#endif

// -- CLASS DEFINITION -- //
class Matrix_3x3 {

 public:

  // constructors
		Matrix_3x3();
  explicit	Matrix_3x3( const float &init );
		Matrix_3x3( const float &in1, const float &in2, const float &in3,
			    const float &in4, const float &in5, const float &in6, 
                            const float &in7, const float &in8, const float &in9 );
		Matrix_3x3( const Matrix_3x3 &other );

  // accessors
  float		operator()( unsigned int row, unsigned int column ) const;
  float&	operator()( unsigned int row, unsigned int column );

  // useful operations
  void		operator/=( const float &value );
  void		operator*=( const float &value );

  // inverse computation functions
  void		svd_decomposition( Matrix_3x3 &U,
				   std::vector< float > &W,
				   Matrix_3x3 &V ) const;
  // void		qr_decomposition() const;
  // void		lu_decomposition() const;

  // eigen analysis
  int		eigen_analysis( Eigen_Pair &eigen1,
				Eigen_Pair &eigen2,
				Eigen_Pair &eigen3 ) const;

 private:
  float v_[9];		// stores a 3 by 3 matrix in a array of 9

  static float	helper_pythag( const float &a, const float &b );

};

#endif
