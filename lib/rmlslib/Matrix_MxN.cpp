/**
 *
 * file:	Matrix_MxN.cpp
 * description:	Defines some common functions used in a Matrix
 *		class designed for arbitrary matrices.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "Matrix_MxN.h"

/**
 * no argument constructor -- sets up a 0x0 matrix.
 */
Matrix_MxN::Matrix_MxN()
  : rows_(0),
    columns_(0)
{
  v_.clear();
}

/**
 * three argument constructor -- this function will initialize
 * the matrix to the number of rows and columns and will init
 * each value to the given value. zero by default. 
 */
Matrix_MxN::Matrix_MxN( const unsigned int &rows, 
			const unsigned int &cols,
			const float &init )
  : rows_( rows ),
    columns_( cols )
{
  v_.resize( rows*cols );
  for(unsigned int i=0; i<v_.size(); i++)
    {
      v_[i] = init;
    }
}

/**
 * vector constructor -- create the vector into a (4x1) mxn matrix
 */
Matrix_MxN::Matrix_MxN( const Vector &v )
  : rows_( 4 ),
    columns_( 1 )
{
  v_.resize(4);

  v_[0] = v.x();
  v_[1] = v.y();
  v_[2] = v.z();
  v_[3] = 1.0;
}

/**
 * point constructor -- create the point into a (4x1) mxn matrix
 */
Matrix_MxN::Matrix_MxN( const Point &p )
  : rows_( 4 ),
    columns_( 1 )
{
  v_.resize(4);

  v_[0] = p.x();
  v_[1] = p.y();
  v_[2] = p.z();
  v_[3] = 1.0;
}

/**
 * matrix 3x3 constructor -- create the 3x3 matrix into a mxn 
 * matrix class.
 */
Matrix_MxN::Matrix_MxN( const Matrix_3x3 &other )
  : rows_( 3 ),
    columns_( 3 )
{
  v_.resize(9);
  for(unsigned int i=0; i<9; i++)
    {
      v_[i] = other(i/3,i%3);
    }
}

/**
 * copy constructor -- copies the attributes from the given matrix 
 * into this one.
 */
Matrix_MxN::Matrix_MxN( const Matrix_MxN &other )
  : rows_( other.rows_ ),
    columns_( other.columns_ ),
    v_( other.v_ )
{}

/**
 * copy -- this function copies the information from the matrix mxn 
 * into this matrix.
 */
Matrix_MxN&
Matrix_MxN::operator=( const Matrix_MxN &other ) 
{
  rows_ = other.rows_;
  columns_ = other.columns_;
  v_.resize( other.v_.size() );
  for(unsigned int i=0; i<other.v_.size(); i++)
    {
      v_[i] = other.v_[i];
    }
  return *this;
}

/**
 * accessor () -- allows hte user to specify the desired row and column
 * of the matrix.  this function will perform a check to ensure that the
 * row and column indices are valid.
 */
float
Matrix_MxN::operator()( const unsigned int &row, const unsigned int &column ) const
{
  if (row >= rows_ || column >= columns_)
    {
      fprintf(stderr," [EXTREME DANGER] Matrix_MxN::operator() -- specified row column (%d,%d) out of bounds (%d,%d)!\n",row,column,rows_,columns_);
      return 0;
    }
  else
    {
      return v_[ row * columns_ + column ];
    }
}

/**
 * accessor () -- allows the user to specify the desired row and column of the
 * matrix and directly modify the value.  if this is out of bounds then we have 
 * some serious problems.
 */
float&	
Matrix_MxN::operator()( const unsigned int &row, const unsigned int &column )
{
  if (row >= rows_ || column >= columns_)
    {
      fprintf(stderr," [EXTREME DANGER] Matrix_MxN::operator() -- specified row column (%d,%d) out of bounds (%d,%d)!\n",row,column,rows_,columns_);
      return v_[0];
    }
  else
    {
      return v_[ row*columns_ + column ];
    }
}

/**
 * operator /= -- allows the user to divide the value into each element of hte 
 * matrix individually.  it directly modifies the matrix's values.
 */
void
Matrix_MxN::operator/=( const float &value )
{
  if (EQL(value,0.0))
    {
      fprintf(stderr," [WARNING] Matrix_MxN::operator/= -- division by zero requested!\n");
      for( unsigned int i=0; i<v_.size(); i++)
	v_[i] = 0.0;
    }
  else
    {
      for(unsigned int i=0; i<v_.size(); i++)
	{
	  v_[i] /= value;
	}
    }
}

/**
 * operator *= -- allows the user to multiply the value into each element of the
 * matrix individually.  it directly modifies the matrix values.
 */
void
Matrix_MxN::operator*=( const float &value )
{
  for(unsigned int i=0; i<v_.size(); i++)
    {
      v_[i] *= value;
    }
}

/**
 * operator * -- multiplies two matrices together.  this function will make sure
 * that the columns of the first match the rows of the second.  if they don't the
 * operation will fail, but we return a matrix with rows of first and columns of 
 * second with all values init to 0.
 */
Matrix_MxN	
Matrix_MxN::operator*( const Matrix_MxN &other ) const
{
  // initialize the results matrix to 0
  Matrix_MxN results( rows_, other.columns_, 0 );

  // only multiply if the conditions are right
  if (columns_ == other.rows_)
    {
      // loop through the results matrix and compute each value
      for(unsigned int i=0; i<results.rows_; i++)
	{
	  for(unsigned int j=0; j<results.columns_; j++)
	    {
	      // multiply and sum the components of this and other 
	      for(unsigned int k=0; k<columns_; k++)
		{
		  results(i,j) += (*this)(i,k) * other(k,j);
		}
	    }
	}
    }
  else
    {
      fprintf(stderr," [EXTREME DANGER] Matrix_MxN::operator* -- rows of matrix 1 don't match columns of matrix 2!\n");
    }

  // return the results matrix
  return results;
}

/**
 * operator / -- divides two matrices together. this function will make sure
 * that the columns of the first match the rows of the second.  if they don't 
 * the operation will fail, but the function returns the matrix with rows of 
 * the first and columns of the second all init to zero.
 */
Matrix_MxN	
Matrix_MxN::operator/( const Matrix_MxN &other ) const
{
  // initialize the results matrix to 0
  Matrix_MxN results( rows_, other.columns_, 0 );

  // only multiply if the conditions are right
  if (columns_ == other.rows_)
    {
      // loop through the results matrix and compute each value
      for(unsigned int i=0; i<results.rows_; i++)
	{
	  for(unsigned int j=0; j<results.columns_; j++)
	    {
	      // multiply and sum the components of this and other 
	      for(unsigned int k=0; k<columns_; k++)
		{
		  results(i,j) += (*this)(i,k) / other(k,j);
		}
	    }
	}
    }
  else
    {
      fprintf(stderr," [EXTREME DANGER] Matrix_MxN::operator/ -- rows of matrix 1 don't match columns of matrix 2!\n");
    }

  // return the results matrix
  return results;
}

/**
 * operator + -- this function adds each element together to give the 
 * new matrix.  If the two matrices do not match rows and columns then this
 * function will just return this matrix.
 */
Matrix_MxN	
Matrix_MxN::operator+( const Matrix_MxN &other ) const
{
  // initialize the results to this matrix
  Matrix_MxN results( rows_,columns_, 0 );

  // if rows and columns make sense then add up the elements
  if (rows_ == other.rows_ && columns_ == other.columns_)
    {
      for(unsigned int i=0; i<v_.size(); i++)
	{
	  results.v_[i] = v_[i] + other.v_[i];
	}
    }
  else
    {
      fprintf(stderr," [EXTREME DANGER] Matrix_MxN::operator+ -- can't add matrices with differing rows and columns\n");
    }

  // return the resultant matrix
  return results;
}

/**
 * operator - -- this function subtracts the elements of the given matrix
 * from this matrix and returns the results.
 */
Matrix_MxN	
Matrix_MxN::operator-( const Matrix_MxN &other ) const
{
  // initialize the results to this matrix
  Matrix_MxN results( rows_,columns_, 0 );

  // if rows and columns make sense then add up the elements
  if (rows_ == other.rows_ && columns_ == other.columns_)
    {
      for(unsigned int i=0; i<v_.size(); i++)
	{
	  results.v_[i] = v_[i] - other.v_[i];
	}
    }
  else
    {
      fprintf(stderr," [EXTREME DANGER] Matrix_MxN::operator- -- can't add matrices with differing rows and columns\n");
    }

  // return the resultant matrix
  return results;
}

/**
 * transpose -- this function returns the transpose of the matrix
 */
Matrix_MxN
Matrix_MxN::transpose() const
{
  Matrix_MxN results( columns_,rows_ );
  for(unsigned int i=0; i<rows_; i++)
    {
      for(unsigned int j=0; j<columns_; j++)
	{
	  results( j,i ) = (*this)(i,j);
	}
    }
  return results;
}

/**
 * svd decomposition -- this function performs svd decomposition as borrowed 
 * from the numerical recipes for C.  the number of iterations is returned, -1 
 * if no convergence was found.
 */
bool
Matrix_MxN::svd_decomposition( Matrix_MxN &U,
			       std::vector< float > &W,
			       Matrix_MxN &V ) const
{
  // some variables used throughout
  int flag,its,i,j,jj,k,l,nm;
  float anorm,c,f,g,h,s,scale,x,y,z;
  std::vector< float > rv1( columns_ );

  // initialize W,U and V
  W.clear(); 
  W.resize(columns_);
  U = Matrix_MxN( *this );
  V = Matrix_MxN( columns_,columns_,0 );

  // Householder reduction to bidiagnol form
  g = scale = anorm = 0.0;
  l = 1;
  for( i=0; i<(signed int)columns_; i++ ) 
    {
      l = i+1;
      rv1[i] = scale*g;
      g = s = scale = 0.0;

      // if i is less than the row count
      if (i<(signed int)rows_)
	{
	  for( k=i; k<(signed int)rows_; k++)
	    {
	      scale += fabs(U.v_[k*columns_ + i]);
	    }

	  if (scale)
	    {
	      for (k=i; k<(signed int)rows_; k++)
		{
		  U.v_[k*columns_ + i] /= scale;
		  s += U.v_[k*columns_ + i]*U.v_[k*columns_ + i];
		}
	      f = U.v_[i*columns_ + i];
	      g = -SIGN( sqrt(s),f );
	      h = f*g-s;
	      U.v_[i*columns_ + i] = f-g;
	      for( j=l; j<(signed int)columns_; j++ )
		{
		  for( s=0.0,k=i; k<(signed int)rows_; k++ )
		    {
		      s += U.v_[k*columns_ +i]*U.v_[k*columns_ + j];
		    }
		  f = s/h;
		  for( k=i; k<(signed int)rows_; k++ )
		    {
		      U.v_[k*columns_ + j] += f*U.v_[k*columns_ + i];
		    }
		}
	      for( k=i; k<(signed int)rows_; k++ )
		{
		  U.v_[k*columns_ + i] *= scale;
		}
	    }
	}

      W[i] = scale * g;
      g = s = scale = 0.0;
      if (i<(signed int)rows_ && i!=(signed int)columns_-1)
	{
	  for( k=l; k<(signed int)columns_; k++)
	    {
	      scale += fabs( U.v_[i*columns_ + k] );
	    }
	  if (scale) 
	    {
	      for (k=l; k<(signed int)columns_; k++)
		{
		  U.v_[i*columns_ + k] /= scale;
		  s += U.v_[i*columns_ + k]*U.v_[i*columns_ + k];
		}
	      f = U.v_[i*columns_ + l];
	      g = -SIGN(sqrt(s),f);
	      h = f*g-s;
	      U.v_[i*columns_ + l] = f-g;
	      for (k=l; k<(signed int)columns_; k++)
		{
		  rv1[k] = U.v_[i*columns_ + k]/h;
		}
	      for (j=l; j<(signed int)rows_; j++)
		{
		  for (s=0.0,k=l; k<(signed int)columns_; k++)
		    {
		      s += U.v_[j*columns_ + k]*U.v_[i*columns_ + k];
		    }
		  for (k=l; k<(signed int)columns_; k++)
		    {
		      U.v_[j*columns_ + k] += s*rv1[k];
		    }
		}
	      for (k=l; k<(signed int)columns_; k++)
		{
		  U.v_[i*columns_ + k] *= scale;
		}
	    }
	}
      anorm = FMAX( anorm,(fabs(W[i])+fabs(rv1[i])) );
    }

  // Accumulation of right-hand transformation
  for (i=columns_-1; i>=0; i--)
    {
      if (i<(signed int)columns_-1)
	{
	  if (g) {
	    // double division to avoid underflow
	    for( j=l; j<(signed int)columns_; j++)
	      {
		V.v_[j*columns_ + i] = (U.v_[i*columns_ + j]/U.v_[i*columns_ + l])/g;
	      }
	    for( j=l; j<(signed int)columns_; j++)
	      {
		for (s=0.0,k=l; k<(signed int)columns_; k++)
		  {
		    s += U.v_[i*columns_ + k]*V.v_[k*columns_ + j];
		  }
		for( k=l; k<(signed int)columns_; k++)
		  {
		    V.v_[k*columns_ + j] += s*V.v_[k*columns_ + i];
		  }
	      }
	  }
	  for (j=l; j<(signed int)columns_; j++)
	    {
	      V.v_[i*columns_ + j] = V.v_[j*columns_ + i] = 0.0;
	    }
	}
      V.v_[i*columns_ + i] = 1.0;
      g = rv1[i];
      l=i;
    }

  // Accumulation of left-hand transformation
  for( i=MIN(rows_,columns_)-1; i>=0; i-- )
    {
      l=i+1;
      g = W[i];
      for( j=l; j<(signed int)columns_; j++)
	{
	  U.v_[i*columns_ + j] = 0.0;
	}
      if (g) 
	{
	  g = 1.0/g;
	  for (j=l; j<(signed int)columns_; j++)
	    {
	      for (s=0.0,k=l; k<(signed int)rows_; k++)
		{
		  s += U.v_[k*columns_ + i]*U.v_[k*columns_ + j];
		}
	      f = (s/U.v_[i*columns_ + i])*g;
	      for(k=i;k<(signed int)rows_;k++)
		{
		  U.v_[k*columns_ + j] += f*U.v_[k*columns_ + i];
		}
	    }
	  for (j=i; j<(signed int)rows_; j++)
	    {
	      U.v_[j*columns_ + i] *= g;
	    }
	}
      else
	{
	  for (j=i; j<(signed int)rows_; j++)
	    {
	      U.v_[j*columns_ + i] = 0.0;
	    }
	}
      ++U.v_[i*columns_ + i];
    }

  // Diagnolization of the bidiagnol form: Loop over
  nm = 1;  // init to remove warning (no real need to do so tho)
  for (k=columns_-1; k>=0; k--)
    {
      // singular values and over allowed iterations
      for (its=0; its<30; its++)
	{
	  flag = 1;

	  // test for splitting
	  for (l=k; l>=0; l--)
	    {
	      nm = l-1;			// note rv1[0] always zero.
	      if (EQL((float)(fabs(rv1[l])+anorm),anorm) || l==0)
		{
		  flag = 0;
		  break;
		}
	      if (EQL((float)(fabs(W[nm])+anorm),anorm))
		{
		  break;
		}
	    }
	  if (flag)
	    {
	      // cancellation of rv1[l], if l > 1.
	      c = 0.0;
	      s = 1.0;
	      for (i=l; i<=k; i++)	//i<k ???
		{
		  f = s*rv1[i];
		  rv1[i] *= c;
		  if (EQL((float)(fabs(f)+anorm), anorm))
		    {
		      break;
		    }
		  g = W[i];
		  h = helper_pythag(f,g);
		  W[i] = h;
		  h = 1.0/h;
		  c = g*h;
		  s = -f*h;
		  for (j=0; j<(signed int)rows_; j++)
		    {
		      y = U.v_[j*columns_ + nm];
		      z = U.v_[j*columns_ + i];
		      U.v_[j*columns_ + nm] = y*c+z*s;
		      U.v_[j*columns_ + i] = z*c-y*s;
		    }
		}
	    }
	  z = W[k];

	  // convergence
	  if (l==k)
	    {
	      // singular value is made non negative
	      if ( z<0.0 )
		{
		  W[k] = -z;
		  for (j=0; j<(signed int)columns_; j++)
		    {
		      V.v_[j*columns_ + k] = -V.v_[j*columns_ + k];
		    }
		}
	      break;
	    }

	  if (its == 29)
	    {
	      fprintf(stderr," [ERROR] Matrix_3x3::svd_decomposition() -- no convergence in 30 iterations!\n");
	      return false;
	    }

	  // shift from bottom 2by2 minor
	  x = W[l];
	  nm = k-1;
	  y = W[nm];
	  g = rv1[nm];
	  h = rv1[k];
	  f = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
	  g = helper_pythag( f,1.0 );
	  f = ((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
	  c = s = 1.0;

	  // next qr transformation:
	  for( j=l; j<=nm; j++ )	// j<nm
	    {
	      i = j+1;
	      g = rv1[i];
	      y = W[i];
	      h = s*g;
	      g = c*g;
	      z = helper_pythag( f,h );
	      rv1[j] = z;
	      c = f/z;
	      s = h/z;
	      f = x*c + g*s;
	      g = g*c - x*s;
	      h = y*s;
	      y *= c;
	      for ( jj=0; jj<(signed int)columns_; jj++ )
		{
		  x = V.v_[jj*columns_ + j];
		  z = V.v_[jj*columns_ + i];
		  V.v_[jj*columns_ + j] = x*c + z*s;
		  V.v_[jj*columns_ + i] = z*c - x*s;
		}
	      z = helper_pythag( f,h );
	      W[j] = z;			// rotation can be arbitrary if z = 0.
	      if (z) 
		{
		  z = 1.0/z;
		  c = f*z;
		  s = h*z;
		}
	      f = c*g + s*y;
	      x = c*y - s*g;
	      for (jj=0; jj<(signed int)rows_; jj++)
		{
		  y = U.v_[jj*columns_ + j];
		  z = U.v_[jj*columns_ + i];
		  U.v_[jj*columns_ + j] = y*c + z*s;
		  U.v_[jj*columns_ + i] = z*c - y*s;
		}
	    }
	  rv1[l] = 0.0;
	  rv1[k] = f;
	  W[k] = x;
	}
    }

  return true;
}

/** 
 * print -- prints out the matrix information.
 */
void
Matrix_MxN::print_matrix() const
{
  for(unsigned int i=0; i<rows_; i++)
    {
      fprintf(stderr,"[ ");
      for(unsigned int j=0; j<columns_; j++)
	{
	  fprintf(stderr,"%g ",v_[i*columns_ + j]);
	}
      fprintf(stderr,"]\n");
    }
}

/**
 * inverse -- this function computes the inverse of the matrix and returns the
 * results. the inverse of the matrix is computed via the svd decomposition
 */
Matrix_MxN
Matrix_MxN::inverse() const
{
  // perform svd decomposition
  Matrix_MxN V,U;
  std::vector< float > weights;
  if (!this->svd_decomposition( U,weights,V ))
    {
      // report warning and return the identity matrix
      fprintf(stderr,"[WARNING] Matrix_MxN::inverse() -- unable to complete svd_decomp!\n");
      Matrix_MxN garbage( columns_,rows_,0.0 );
      for(unsigned int i=0; i<columns_ && i<rows_; i++)
	{
	  garbage(i,i) = 1.0;
	}
    }

  // create the W matrix
  Matrix_MxN W( weights.size(),weights.size(), 0.0 );
  for(unsigned int i=0; i<weights.size(); i++)
    {
      // zero out weights that are very small
      if (weights[i] > 0.000001)
	W(i,i) = 1.0 / weights[i];
      else
	W(i,i) = 0.0;
    }

//   U(0,0) = -0.1525;
//   U(0,1) = -0.8226;
//   U(1,0) = -0.3499;
//   U(1,1) = -0.4214;
//   U(2,0) = -0.5474;
//   U(2,1) = -0.0201;
//   U(3,0) = -0.7448;
//   U(3,1) = 0.3812;

//   W(0,0) = 1.0/14.2691;
//   W(1,1) = 1.0/0.6268;

//   V(0,0) = -0.6414;
//   V(0,1) = 0.7672;
//   V(1,0) = -0.7672;
//   V(1,1) = -0.6414;

  // the inverse is V*W*U_transpose
  return V*W*U.transpose();
}

/**
 * eigen analysis -- this function will perform the jacobian rotations as in
 * numerical recipes for C to compute the eigen vectors for the matrix.
 */
int
Matrix_MxN::eigen_analysis( std::vector< Eigen_Pair > &eigens ) const
{
  return (signed int) eigens.size();
}

/**
 * helper pythag -- this is a helper function required of the numerical recipes
 * functions.
 */
float
Matrix_MxN::helper_pythag( const float &a, const float &b )
{
  float absa = fabs(a);
  float absb = fabs(b);

  if (absa > absb)
    {
      float value = absb/absa;
      return absa*sqrt( 1.0 + value*value );
    }
  else
    {
      float value = absa/absb;
      return ( absb==0.0 ? 0.0 : absb*sqrt( 1.0 + value*value ) );
    }
}

/**
 * adds a post rotation to the matrix that rotates "radian" units
 * around the v vector.
 */
void 
Matrix_MxN::rotate( const double &radians,
		    const Vector &v ) 
{
  // if not a four by four then quit!
  if ( columns_ != 4 || rows_ != 4)
    {
      fprintf(stderr,"[ERROR] Matrix_MxN::rotate() - must be a 4x4 matrix!\n");
      return;
    }

  // if the vector is zero don't know where to rotate!
  if ( EQL(v.length(),0.0) )
    {
      fprintf(stderr,"[ERROR] Matrix_MxN::rotate() - zero vector length specified!\n");
      return;
    }

  // otherwise compute the rotation matrix
  float sine = sin(radians);
  float cosine = cos(radians);
  float other = 1.0 - cosine;

  Vector tmpVector = v.unit_vector();
  float v_xx = tmpVector.x() * tmpVector.x();
  float v_yy = tmpVector.y() * tmpVector.y();
  float v_zz = tmpVector.z() * tmpVector.z();
  float v_xy = tmpVector.x() * tmpVector.y();
  float v_xz = tmpVector.x() * tmpVector.z();
  float v_yz = tmpVector.y() * tmpVector.z();

  Matrix_MxN tempMatrix(4,4);
  tempMatrix.v_[0*4 + 0] = ((other*v_xx) + cosine);
  tempMatrix.v_[0*4 + 1] = ((other*v_xy) + (sine*tmpVector.z()));
  tempMatrix.v_[0*4 + 2] = ((other*v_xz) - (sine*tmpVector.y()));
  tempMatrix.v_[0*4 + 3] = 0.0;

  tempMatrix.v_[1*4 + 0] = ((other*v_xy) - (sine*tmpVector.z()));
  tempMatrix.v_[1*4 + 1] = ((other*v_yy) + cosine);
  tempMatrix.v_[1*4 + 2] = ((other*v_yz) + (sine*tmpVector.x()));
  tempMatrix.v_[1*4 + 3] = 0.0;

  tempMatrix.v_[2*4 + 0] = ((other*v_xz) + (sine*tmpVector.y()));
  tempMatrix.v_[2*4 + 1] = ((other*v_yz) - (sine*tmpVector.x()));
  tempMatrix.v_[2*4 + 2] = ((other*v_zz) + cosine);
  tempMatrix.v_[2*4 + 3] = 0.0;

  tempMatrix.v_[3*4 + 0] = 0.0;
  tempMatrix.v_[3*4 + 1] = 0.0;
  tempMatrix.v_[3*4 + 2] = 0.0;
  tempMatrix.v_[3*4 + 3] = 1.0;

  *this = tempMatrix*(*this);
}

/**
 * translate matrix will set up the translation on a 4x4 matrix only
 */
void 
Matrix_MxN::translate( const Vector &v ) 
{
  // if not a four by four then quit!
  if ( columns_ != 4 || rows_ != 4)
    {
      fprintf(stderr,"[ERROR] Matrix_MxN::rotate() - must be a 4x4 matrix!\n");
      return;
    }

  // quit early if no move!
  if ( EQL(v.length(),0.0) )
    {
      return;
    }

  Matrix_MxN tempMatrix(4,4);
  tempMatrix.v_[0*4+0] = 1.0;
  tempMatrix.v_[0*4+1] = 0.0;
  tempMatrix.v_[0*4+2] = 0.0;
  tempMatrix.v_[0*4+3] = v.x();

  tempMatrix.v_[1*4+0] = 0.0;
  tempMatrix.v_[1*4+1] = 1.0;
  tempMatrix.v_[1*4+2] = 0.0;
  tempMatrix.v_[1*4+3] = v.y();

  tempMatrix.v_[2*4+0] = 0.0;
  tempMatrix.v_[2*4+1] = 0.0;
  tempMatrix.v_[2*4+2] = 1.0;
  tempMatrix.v_[2*4+3] = v.z();

  tempMatrix.v_[3*4+0] = 0.0;
  tempMatrix.v_[3*4+1] = 0.0;
  tempMatrix.v_[3*4+2] = 0.0;
  tempMatrix.v_[3*4+3] = 1.0;

  *this = tempMatrix*(*this);
}

/**
 * conversion function -- turn the matrix into a vector if possible
 */
Vector
Matrix_MxN::makeVector() const
{
  // check if possible
  if (columns_ != 1 || rows_ != 4)
    {
      fprintf(stderr,"[ERROR] Matrix_MxN::makeVector() - invalid dimension values (%dx%d not 4x1)!\n",rows_,columns_);
      return Vector(0,0,0);
    }

  // return the vector
  return Vector(v_[0],v_[1],v_[2]);
}

/**
 * conversion function -- turn the matrix into a point if possible
 */
Point
Matrix_MxN::makePoint() const
{
  // check if possible
  if (columns_ != 1 || rows_ != 4)
    {
      fprintf(stderr,"[ERROR] Matrix_MxN::makePoint() - invalid dimension values (%dx%d not 4x1)!\n",rows_,columns_);
      return Point(0,0,0);
    }

  // return the vector
  return Point(v_[0],v_[1],v_[2]);
}

/**
 * make the identity matrix
 */
void
Matrix_MxN::makeIdentity()
{
  for(unsigned int i=0; i<rows_; i++)
    {
      for(unsigned int j=0; j<columns_; j++)
	{
	  if (i==j) v_[i*rows_+j] = 1.0;
	  else v_[i*rows_+j] = 0.0;
	}
    }
}
