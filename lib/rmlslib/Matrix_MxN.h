/**
 *
 * file:	Matrix_MxN.h
 * description:	Defines some common functions used in a Matrix
 *		class designed for arbitrary matrices.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

#ifndef _MATRIX_MxN_H_
#define _MATRIX_MxN_H_

// -- INCLUDES -- //
#include "Matrix_3x3.h"
#include "Global_Defs.h"
#include <math.h>
#include <vector>

// -- CLASS DEFINITION -- //
class Matrix_MxN {

 public:

  // constructors
		Matrix_MxN();
		Matrix_MxN( const unsigned int &rows, 
			    const unsigned int &cols, 
			    const float &init=0 );
  explicit	Matrix_MxN( const Vector &v );
  explicit	Matrix_MxN( const Point &p );
  explicit	Matrix_MxN( const Matrix_3x3 &other );
		Matrix_MxN( const Matrix_MxN &other );

  // copy
  Matrix_MxN&	operator=( const Matrix_MxN &other );

  // accessors
  float		operator()( const unsigned int &row, const unsigned int &column ) const;
  float&	operator()( const unsigned int &row, const unsigned int &column );

  unsigned int	number_rows() const { return rows_; };
  unsigned int&	number_rows() { return rows_; };

  unsigned int	number_cols() const { return columns_; };
  unsigned int& number_cols() { return columns_; };

  // useful operations
  void		operator/=( const float &value );
  void		operator*=( const float &value );

  Matrix_MxN	operator*( const Matrix_MxN &other ) const;
  Matrix_MxN	operator/( const Matrix_MxN &other ) const;
  Matrix_MxN	operator+( const Matrix_MxN &other ) const;
  Matrix_MxN	operator-( const Matrix_MxN &other ) const;

  Matrix_MxN	transpose() const;
  void		makeIdentity();

  // debugging print function
  void		print_matrix() const;

  // inverse computation functions
  bool		svd_decomposition( Matrix_MxN &U,
				   std::vector< float > &W,
				   Matrix_MxN &V ) const;
  Matrix_MxN	inverse() const;

  // eigen analysis
  int		eigen_analysis( std::vector< Eigen_Pair > &eigens ) const;

  // rotate and translate (4x4 matrices only!)
  void		rotate( const double &radians,
			const Vector &v );
  void		translate( const Vector &v );

  // helpful conversion functions
  Vector	makeVector() const;
  Point		makePoint() const;

 private:

  unsigned int rows_, columns_;	// save the row and column of the matrix
  std::vector< float > v_;	// stores the matrix as a vector

  static float	helper_pythag( const float &a, const float &b );
};

#endif
