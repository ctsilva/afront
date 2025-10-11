/**
 *
 * file:	Matrix_3x3.C
 * description:	Defines some common functions used in a Matrix
 *		class designed specifically for 3 by 3 matrices.
 *
 * author:	Joel Daniels
 * date:	March 2006
 *
 */

// -- INCLUDES -- //
#include "Matrix_3x3.h"

/**
 * no argument constructor - does nothing to values
 */
Matrix_3x3::Matrix_3x3() {}

/**
 * one argument constructor - assigns all values to the given init
 */
Matrix_3x3::Matrix_3x3( const float &init )
{
  for (unsigned int i=0; i<9; i++)
    {
      v_[i] = init;
    }
}

/** 
 * nine argumetn constructor - initializes each value separately
 * according to the given values.
 */
Matrix_3x3::Matrix_3x3( const float &in1, const float &in2, const float &in3, 
			const float &in4, const float &in5, const float &in6, 
			const float &in7, const float &in8, const float &in9 )
{
  v_[0] = in1;   v_[1] = in2;   v_[2] = in3;   
  v_[3] = in4;   v_[4] = in5;   v_[5] = in6;   
  v_[6] = in7;   v_[7] = in8;   v_[8] = in9;  
}

/**
 * copy constructor copies values from given matrix into this one
 */
Matrix_3x3::Matrix_3x3( const Matrix_3x3 &other )
{
  for(unsigned int i=0; i<9; i++)
    {
      v_[i] = other.v_[i];
    }
}

/**
 * accessor () - allows the user to get a copy of the value
 * at the indexed row and column in the matrix.
 */
float		
Matrix_3x3::operator()( unsigned int row, unsigned int column ) const
{
  if (row >= 3 || column >= 3) 
    {
      fprintf(stderr," [ERROR]: Matrix_3x3::operator() -- indices must be less than 3! (row = %d)(column = %d)\n",row,column);
      return 0;
    }
  else
    {
      return v_[ row*3 + column ];
    }
}

/**
 * accessor () - allows the user to get gain access to the value at
 * the indexed row and column in the matrix.
 */
float&	
Matrix_3x3::operator()( unsigned int row, unsigned int column )
{
  if (row >= 3 || column >= 3) 
    {
      fprintf(stderr," [ERROR] Matrix_3x3::operator() -- indices must be less than 3! (row = %d)(column = %d)\n",row,column);
      return v_[0];
    }
  else
    {
      return v_[ row*3 + column ];
    }
}

/**
 * operator /= - divides the scalar value through each value in the
 * matrix and assigns the results to that position.
 */
void		
Matrix_3x3::operator/=( const float &value )
{
  if (value < EPSILON && value > -EPSILON)
    {
      fprintf(stderr," [ERROR] Matrix_3x3::operator/= -- division by zero!\n");
      for( unsigned int i=0; i<9; i++)
	{
	  v_[i] = 0;
	} 
    }

  for( unsigned int i=0; i<9; i++)
    {
      v_[i] /= value;
    }
}

/** 
 * operator *= - multiplies the scalar value through each value in
 * the matrix and assigns the results to that position.
 */
void		
Matrix_3x3::operator*=( const float &value )
{
  for( unsigned int i=0; i<9; i++)
    {
      v_[i] *= value;
    }
}

/** 
 * svd_decomposition - performs SVD decomp and returns the results
 * without changes to the matrix.  This is borrowed by "Numerical
 * Recipes in C" Chapter 2: Solution to Linear Algebraic Equations
 * Section 2.6: Singular Value Decomposition.  The return
 * decomposition is given into the arguments passed.
 */
void		
Matrix_3x3::svd_decomposition( Matrix_3x3 &U,
			       std::vector< float > &W,
			       Matrix_3x3 &V ) const
{
  // some variables used throughout
  int flag,i,its,j,jj,k,l,nm;
  float anorm,c,f,g,h,s,scale,x,y,z;
  std::vector< float > rv1( 3 );

  // initialize W and U
  W.clear(); 
  W.resize(3);
  for( i=0; i<9; i++ ) U.v_[i] = v_[i];

  // Householder reduction to bidiagnol form
  g = scale = anorm = 0.0;
  for( i=0; i<3; i++ ) 
    {
      l = i+1;
      rv1[i] = scale*g;
      g = s = scale = 0.0;

      // removed if statement (known to be 3by3!)
      for( k=i; k<3; k++)
	{
	  scale += fabs(U.v_[k*3 + i]);
	}

      if (scale)
	{
	  for (k=i; k<3; k++)
	    {
	      U.v_[k*3 + i] /= scale;
	      s += U.v_[k*3 + i]*U.v_[k*3 + i];
	    }
	  f = U.v_[i*3 + i];
	  g = -SIGN( sqrt(s),f );
	  h = f*g-s;
	  U.v_[i*3 + i] = f-g;
	  for( j=l; j<3; j++ )
	    {
	      for( s=0.0,k=i; k<3; k++ )
		{
		  s += U.v_[k*3 +i]*U.v_[k*3 + j];
		}
	      f = s/h;
	      for( k=i; k<3; k++ )
		{
		  U.v_[k*3 + j] += f*U.v_[k*3 + i];
		}
	    }
	  for( k=i; k<3; k++ )
	    {
	      U.v_[k*3 + i] += scale;
	    }
	}

      W[i] = scale * g;
      g = s = scale = 0.0;
      if (i<2)
	{
	  for( k=l; k<3; k++)
	    {
	      scale += fabs( U.v_[i*3 + k] );
	    }
	  if (scale) 
	    {
	      for (k=l; k<3; k++)
		{
		  U.v_[i*3 + k] /= scale;
		  s += U.v_[i*3 + k]*U.v_[i*3 + k];
		}
	      f = U.v_[i*3 + l];
	      g = -SIGN(sqrt(s),f);
	      h = f*g-s;
	      U.v_[i*3 + l] = f-g;
	      for (k=l; k<3; k++)
		{
		  rv1[k] = U.v_[i*3 + k]/h;
		}
	      for (j=l; j<3; j++)
		{
		  for (s=0.0,k=l; k<3; k++)
		    {
		      s += U.v_[j*3 + k]*U.v_[i*3 + k];
		    }
		  for (k=l; k<3; k++)
		    {
		      U.v_[j*3 + k] += s*rv1[k];
		    }
		}
	      for (k=l;k<3; k++)
		{
		  U.v_[i*3 + k] *= scale;
		}
	    }
	}
      anorm = FMAX( anorm,(fabs(W[i])+fabs(rv1[i])) );
    }

  // Accumulation of right-hand transformation
  for (i=2; i>=0; i--)
    {
      if (i<2)
	{
	  if (g) {
	    // double division to avoid underflow
	    for( j=l; j<3; j++)
	      {
		V.v_[j*3 + i] = (U.v_[i*3 + j]/U.v_[i*3 + l])/g;
	      }
	    for( j=l; j<3; j++)
	      {
		for (s=0.0,k=l; k<3; k++)
		  {
		    s += U.v_[i*3 + k]*V.v_[k*3 + j];
		  }
		for( k=l; k<3; k++)
		  {
		    V.v_[k*3 + j] = s*V.v_[k*3 + i];
		  }
	      }
	  }
	  for (j=l; j<3; j++)
	    {
	      V.v_[i*3 + j] = V.v_[j*3 + i] = 0.0;
	    }
	}
      V.v_[i*3 + i] = 1.0;
      g = rv1[i];
      l=i;
    }

  // Accumulation of left-hand transformation
  for( i=2; i>=0; i-- )
    {
      l=i+1;
      g = W[i];
      for( j=l; j<3; j++)
	{
	  U.v_[i*3 + j] = 0.0;
	}
      if (g) 
	{
	  g = 1.0/g;
	  for (j=l; j<3; j++)
	    {
	      for (s=0.0,k=l; k<3; k++)
		{
		  s += U.v_[k*3 + i]*U.v_[k*3 + j];
		}
	      f = (s/U.v_[i*3 + i])*g;
	      for(k=i;k<3;k++)
		{
		  U.v_[k*3 + j] += f*U.v_[k*3 + i];
		}
	    }
	  for (j=i; j<3; j++)
	    {
	      U.v_[j*3 + i] *= g;
	    }
	}
      else
	{
	  for (j=i; j<3; j++)
	    {
	      U.v_[j*3 + i] *= g;
	    }
	}
      ++U.v_[i*3 + i];
    }

  // Diagnolization of the bidiagnol form: Loop over
  nm = 1;  // init to remove warning (no real need to do so tho)
  for (k=2; k>=0; k--)
    {
      // singular values and over allowed iterations
      for (its=0; its<30; i++)
	{
	  flag = 1;

	  // test for splitting
	  for (l=k; l>=0; l--)
	    {
	      nm = l-1;			// note rv1[0] always zero.
	      if ((float)(fabs(rv1[l])+anorm) == anorm)
		{
		  flag = 0;
		  break;
		}
	      if ((float)(fabs(W[nm])+anorm) == anorm) 
		{
		  break;
		}
	    }
	  if (flag)
	    {
	      // cancellation of rv1[l], if l > 1.
	      c = 0.0;
	      s = 1.0;
	      for (i=l; i<k; i++)
		{
		  f = s*rv1[i];
		  rv1[i] *= c;
		  if ((float)(fabs(f)+anorm) == anorm)
		    {
		      break;
		    }
		  g = W[i];
		  h = helper_pythag(f,g);
		  W[i] = h;
		  h = 1.0/h;
		  c = g*h;
		  s = -f*h;
		  for (j=0; j<3; j++)
		    {
		      y = U.v_[j*3 + nm];
		      z = U.v_[j*3 + i];
		      U.v_[j*3 + nm] = y*c+z*s;
		      U.v_[j*3 + i] = z*c-y*s;
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
		  for (j=0; j<3; j++)
		    {
		      V.v_[j*3 + k] = -V.v_[j*3 + k];
		    }
		}
	      break;
	    }

	  if (its == 29)
	    {
	      fprintf(stderr," [ERROR] Matrix_3x3::svd_decomposition() -- no convergence in 30 iterations!\n");
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
	  for( j=l; j<=nm; j++ )
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
	      for ( jj=1; jj<3; jj++ )
		{
		  x = V.v_[jj*3 + j];
		  z = V.v_[jj*3 + i];
		  V.v_[jj*3 + j] = x*c + z*s;
		  V.v_[jj*3 + i] = z*c - x*s;
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
	      for (jj=0; jj<3; jj++)
		{
		  y = U.v_[jj*3 + j];
		  z = U.v_[jj*3 + i];
		  U.v_[jj*3 + j] = y*c + z*s;
		  U.v_[jj*3 + i] = z*c - y*s;
		}
	    }
	  rv1[l] = 0.0;
	  rv1[k] = f;
	  W[k] = x;
	}
    }
}

/**
 * helper pythag -- used by the numerical recipes svd decomposition
 * function.
 */
float
Matrix_3x3::helper_pythag( const float &a,
			   const float &b )
{
  float absa = fabs(a);
  float absb = fabs(b);

  if (absa > absb)
    {
      float value = absb/absa;
      return absa*sqrt( 1.0 + value*value );
    }
  else if (absb == 0)
    {
      return 0.0;
    }
  else
    {
      float value = absa/absb;
      return absb*sqrt( 1.0 + value*value );
    }
}
// void		Matrix_3x3::qr_decomposition() const;
// void		Matrix_3x3::lu_decomposition() const;

/**
 * eigen value and vectors - computes the eigen information for
 * the matrix and returns the results via the input eigen pairs.
 * There are at most 3 eigen vectors in a 3x3 matrix.  All zero
 * vectors will still be returned.  The returned vectors are sorted
 * in importance of their eigen values (largest = 1, smallest = 3)
 * Borrowed the code from numerical recipes for C.  Returns the
 * number of Jacobian iterations required (-1 if failed!)
 */
int		
Matrix_3x3::eigen_analysis( Eigen_Pair &eigen1,
			    Eigen_Pair &eigen2,
			    Eigen_Pair &eigen3 ) const
{

  // implement jacobian transformations to perform eigen analysis
  int j=0,iq=0,ip=0,i=0;
  float tresh=0,theta=0,tau=0,t=0,sm=0,s=0,h=0,g=0,c=0,b[3],z[3];
  Matrix_3x3 a( *this );
  Matrix_3x3 e_vectors(0.0);
  float e_values[3];

  for(ip=0; ip<3; ip++)
    {
      // initialize the eigen vectors matrix
      e_vectors.v_[ip*3 + ip] = 1.0;

      // initialize b and the eigen values 
      b[ip] = e_values[ip] = this->v_[ip*3 + ip];
      z[ip] = 0.0;
    }

  // start the looping process (bound so it won't run forever)
  int nRotations = 0;
  for (i=0; i<50; i++)
    {
      // sum off-diagnol elements
      sm = 0.0;
      for(ip = 0; ip<2; ip++)
	{
	  for(iq=ip+1; iq<3; iq++)
	    {
	      sm += fabs(a.v_[ip*3 + iq]);
	    }
	}

      // the normal return, which relies on quadratic convergence to machine underflow
      if ( EQL(sm,0) )
	{
	  // store the eigen pairs then sort them into order
	  std::vector< Eigen_Pair > remaining(3);
	  remaining[0].vector_ = Vector( e_vectors.v_[0], e_vectors.v_[3], e_vectors.v_[6] );
	  remaining[0].value_ = e_values[0];
	  remaining[1].vector_ = Vector( e_vectors.v_[1], e_vectors.v_[4], e_vectors.v_[7] );
	  remaining[1].value_ = e_values[1];
	  remaining[2].vector_ = Vector( e_vectors.v_[2], e_vectors.v_[5], e_vectors.v_[8] );
	  remaining[2].value_ = e_values[2];

	  // sort the remaining into order
	  while( remaining.size() > 0 )
	    {
	      std::vector< Eigen_Pair >::iterator maxIter = remaining.begin();
              std::vector< Eigen_Pair >::iterator currentIter = remaining.begin();
	      while( currentIter != remaining.end() )
		{
		  if (maxIter->value_ < currentIter->value_)
		    maxIter = currentIter;
		  currentIter++;
		}

	      // copy the maximum value and delete it out of the remaining
	      if (remaining.size() == 3)
		{
		  eigen1.vector_ = maxIter->vector_;
		  eigen1.value_ = maxIter->value_;
		}
	      else if (remaining.size() == 2)
		{
		  eigen2.vector_ = maxIter->vector_;
		  eigen2.value_ = maxIter->value_;
		}
	      else if (remaining.size() == 1)
		{
		  eigen3.vector_ = maxIter->vector_;
		  eigen3.value_ = maxIter->value_;
		}

	      // delete it from the remaining vectors
	      remaining.erase( maxIter );
	    }
	  return nRotations;
	}

      if (i<4)		// ... on the first three sweeps through ...
	tresh = .2*sm/16.0;
      else		// ... thereafter ...
	tresh = 0.0;

      for (ip=0; ip<2; ip++)
	{
	  for (iq=ip+1; iq<3; iq++)
	    {
	      g = 100.0 * fabs( a.v_[ip*3 + iq] );

	      // after four sweeps, skip the rotation if the off-diagnol element is small
	      if ( i>3 &&
		   EQL((float)(fabs(e_values[ip])+g), (float)(fabs(e_values[ip]))) &&
		   EQL((float)(fabs(e_values[iq])+g), (float)(fabs(e_values[iq]))) )
		{
		  a.v_[ip*3 + iq] = 0.0;
		}
	      else if (fabs(a.v_[ip*3 + iq]) > tresh)
		{
		  h = e_values[iq] - e_values[ip];
		  if ( EQL((float)(fabs(h)+g), (fabs(h))) )
		    {
		      t = (a.v_[ip*3+iq])/h;
		    }
		  else 
		    {
		      theta = 0.5*h / (a.v_[ip*3 + iq]);
		      t = 1.0 / (fabs(theta) + sqrt( 1.0+theta*theta ));
		      if (theta < 0.0) t = -t;
		    }
		  c = 1.0/sqrt(1+t*t);
		  s = t*c;
		  tau = s/(1.0+c);
		  h = t*a.v_[ip*3 + iq];
		  z[ip] -= h;
		  z[iq] += h;
		  e_values[ip] -= h;
		  e_values[iq] += h;
		  a.v_[ip*3 + iq] = 0.0;

		  // case of rotations 1<=j<p
		  for (j=0; j<ip-1; j++)
		    {
		      ROTATE(a,j,ip,j,iq);
		    }

		  // case of rotations p<j<q
		  for (j=ip+1; j<=iq-1; j++)
		    {
		      ROTATE(a,ip,j,j,iq);
		    }

		  // case of rotations q<j<3
		  for (j=iq+1; j<3; j++)
		    {
		      ROTATE(a,ip,j,iq,j);
		    }
		  for(j=0; j<3; j++)
		    {
		      ROTATE(e_vectors,j,ip,j,iq);
		    }
		  nRotations++;
		}
	    }
	}

      // update eigen values with the sum of tapq, and reinitialize z
      for (ip=0; ip<3; ip++)
	{
	  b[ip] += z[ip];
	  e_values[ip] = b[ip];
	  z[ip] = 0.0;
	}
    }

  // failed to get the eigens
  return -1;
}

