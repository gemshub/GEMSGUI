//-------------------------------------------------------------------
// $Id:  $
//
// C/C++ Some functions from NUMERICAL RECIPES IN C
//
// Copyright (C) 2006 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------

#include <math.h>
#include "verror.h"

// the squares of the following constants shall not under/overflow:
// these values seem good for an x86:
#define LM_SQRT_DWARF 1.e-160
#define LM_SQRT_GIANT 1.e150
// the following values should work on any machine:
// #define LM_SQRT_DWARF 3.834e-20
// #define LM_SQRT_GIANT 1.304e19
#define SQR(x)   (x)*(x)

double enorm( int n, double *x )
{
/*     given an n-vector x, this function calculates the
 *     euclidean norm of x.
 *
 *     the euclidean norm is computed by accumulating the sum of
 *     squares in three different sums. the sums of squares for the
 *     small and large components are scaled so that no overflows
 *     occur. non-destructive underflows are permitted. underflows
 *     and overflows do not occur in the computation of the unscaled
 *     sum of squares for the intermediate components.
 *     the definitions of small, intermediate and large components
 *     depend on two constants, LM_SQRT_DWARF and LM_SQRT_GIANT. the main
 *     restrictions on these constants are that LM_SQRT_DWARF**2 not
 *     underflow and LM_SQRT_GIANT**2 not overflow.
 *
 *     parameters
 *
 *	n is a positive integer input variable.
 *
 *	x is an input array of length n.
 */
    int i;
    double agiant, s1, s2, s3, xabs, x1max, x3max, temp;

    if( n <= 0 )
      return 0.0;
    s1 = 0;
    s2 = 0;
    s3 = 0;
    x1max = 0;
    x3max = 0;
    agiant = LM_SQRT_GIANT/( (double) n);

    for ( i=0; i<n; i++ )
    {
        xabs = fabs(x[i]);
        if ( xabs > LM_SQRT_DWARF && xabs < agiant )
        {
// **  sum for intermediate components.
            s2 += xabs*xabs;
            continue;
        }

        if ( xabs >  LM_SQRT_DWARF )
        {
// **  sum for large components.
            if (xabs > x1max)
            {
                temp = x1max/xabs;
                s1 = 1 + s1*SQR(temp);
                x1max = xabs;
            }
            else
            {
                temp = xabs/x1max;
                s1 += SQR(temp);
            }
            continue;
        }
// **  sum for small components.
        if (xabs > x3max)
        {
            temp = x3max/xabs;
            s3 = 1 + s3*SQR(temp);
            x3max = xabs;
        }
        else
        {
            if (xabs != 0.)
            {
                temp = xabs/x3max;
                s3 += SQR(temp);
            }
        }
    }

// *** calculation of norm.

    if (s1 != 0)
        return x1max*sqrt(s1 + (s2/x1max)/x1max);
    if (s2 != 0)
    {
        if (s2 >= x3max)
            return sqrt( s2*(1+(x3max/s2)*(x3max*s3)) );
        else
            return sqrt( x3max*((s2/x3max)+(x3max*s3)) );
    }

    return x3max*sqrt(s3);
}

int N1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Solver of a system of linear equations using method of
// Cholesky Decomposition ( square roots N R in C 2.9
// if a square matrix A happens to be symmetric and positive defined)
// N - dimension of the matrix R (number of equations)
// R - input matrix of coeffs N*(N+1);
//     Last column is a vector of independent variables.
// X - solution vector (N).
// A - work matrix of  N(N+1) dimensions (internal copy of R )
// Return values: 0  - solved OK;
//                1  - no solution, degenerated or inconsistent system;
#define  r(i,j) (*(R+(j)+(i)*N1))
#define  a(i,j) (*(A+(j)+(i)*N1))
//
int CholeskyDecomposition( int N, double* R, double* X, double* A  )
{
  bool freeA = false;
  int ii, jj, kk;
  N1 = N + 1;
  double sum;

  if( !A )
  {   A = new double[N*N1];
      freeA = true;
  }

  // Copy data from matrix R to matrix A
  memset( A, 0, N*N1*sizeof(double));
  for( ii=0; ii<N; ii++)
   for( jj =0 ; jj<N; jj++ )
     a(ii,jj)= r(ii,jj);      /*       *(R+I*N1+J); */

  // Given a positive-definite symmetric matrix A[0..N][0..N],
  // this routine constructs its Cholesky decomposition, A = L · LT .
  // On input, only the upper triangle of A need be given; it is not modified.
  // The Cholesky factor L is returned in the lower triangle of A,
  // except for its diagonal elements which are returned in A[0..N][N] column.
  for( ii=0; ii<N; ii++ )
  {
    for( jj=ii; jj<N; jj++)
    {
      for( sum=a(ii,jj), kk=ii-1; kk >=0; kk-- )
        sum -= a(ii,kk)*a(jj,kk);
      if (ii == jj)
      {
         if (sum <= 0.0) // a, with rounding errors, is not positive definite.
         {
           if( freeA ) delete[] A;
           return 1;
         }
        a(ii,N)=sqrt(sum); // p[ii] = a(ii, N)
      }
      else a(jj,ii) = sum/a(ii, N);
   }
 }
 // Solves the set of n linear equations A · x = b,
 // where A is a positive-definite symmetric matrix.
 // A[0..N][0..N] and A[0..N][N] are input as the output of the routine choldc.
 // Only the lower subdiagonal portion of A is accessed.
 // R[0..N][N] is input as the right-hand side vector.
  for( ii=0; ii<N; ii++)
  { // Solve L · y = b, storing y in x.
    for( sum=r(ii,N), kk=ii-1; kk>=0; kk--)
       sum -= a(ii,kk)*X[kk];
    X[ii] = sum/a(ii,N);
  }
  for( ii=N-1; ii>=0; ii--)
  { // Solve LT · x = y.
    for( sum=X[ii], kk=ii+1; kk<N; kk++)
      sum -= a(kk,ii)*X[kk];
    X[ii] = sum/a(ii,N);
 }
 if( freeA ) delete[] A;
 return 0;
}

#undef a
#undef r

void LUdcmp( int N, double* A, int* indx, double& d);
void LUbksb( int N, double* A, int *indx, double* B);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Solver of a system of linear set of equations using method of
// LU Decomposition (  N R in C 2.3 :
// A = L*U , L - lower triangular ( has elements only on the diagonal and below)
// U - is upper triangular ( has elements only on the diagonal and above))
// Parameters:
// N - dimension of the matrix R (number of equations)
// A - input matrix of coeffs N*(N+1);
//     Last column is a vector of independent variables.
// X - solution vector (N).
// Return values: 0  - solved OK;
//                1  - no solution, degenerated or inconsistent system;

#define  a(i,j) (*(A+(j)+(i)*N1))
#define TINY 1.0e-20  // A small number.
//
int LUDecomposition( int N, double* A, double* X  )
{
  double d;
  int *indx, ii;
  N1 = N + 1;

  // set up start data
  indx = new int[N];
  for( ii=0; ii<N; ii++)
    X[ii]=  a(ii,N);            /*       *(A+I*N1+N); */

  // given LU decomposition
  LUdcmp( N, A, indx, d);
  LUbksb( N, A, indx, X); //  The answer x will be given back in b.
                          // Your original matrix A will have  been destroyed.
  delete[] indx;
  return 0;
}

void LUdcmp( int N, double* A, int* indx, double& d)
// Given a matrix A[0..N][0..N], this routine replaces it by the
// LU decomposition of a rowwise permutation of itself.
// A and N are input. A is output
// indx[0..N] is an output vector that records the row permutation effected by
// the partial pivoting;
// d is output as ±1 depending on whether the number of row interchanges
// was even or odd, respectively.
// This routine is used in combination with lubksb to solve linear equations
// or invert a matrix.
{
 int ii, imax, jj, kk;
 double big, dum, sum, temp;
 double *vv;         // vv stores the implicit scaling of each row.

 vv = new double[N];
 d = 1.0;            // No row interchanges yet.
 for( ii=0; ii<N; ii++)
 {  //Loop over rows to get the implicit scaling information
   big = 0.0;
   for( jj=0; jj<N; jj++)
    if( ( temp=fabs(a(ii,jj) ) ) > big)
      big=temp;
   if(big == 0.0)
   {
     // No nonzero largest element.
      delete[] vv;
      Error( "LUDecomposition", "Singular matrix in routine ludcmp");
   }
   vv[ii] = 1.0/big;   // Save the scaling.
 }
 for(jj=0; jj<N; jj++)   // This is the loop over columns of Crout’s method.
 {
    for( ii=0; ii<jj; ii++) // i<>j
    {
      sum = a(ii,jj);
      for( kk=0; kk<ii; kk++)
        sum -= a(ii,kk)*a(kk,jj);
      a(ii,jj) = sum;
    }
   big=0.0;      // Initialize for the search for largest pivot element.
   for( ii=jj; ii<N; ii++) // This is i = j  and i = j+1. . .N
   {
      sum = a(ii,jj);
      for( kk=0; kk<jj; kk++)
        sum -= a(ii,kk)*a(kk,jj);
      a(ii,jj) = sum;
      if( (dum=vv[ii]*fabs(sum)) >= big)
      { // Is the figure of merit for the pivot better than the best so far?
        big = dum;
        imax=ii;
      }
   }
   if(jj != imax)
    {  // Do we need to interchange rows?
       for ( kk=0; kk<N; kk++)
       {  // Yes, do so...
         dum = a(imax,kk);
         a(imax,kk) = a(jj,kk);
         a(jj,kk)=dum;
       }
       d = -d;  // ...and change the parity of d.
       vv[imax]=vv[jj];  // Also interchange the scale factor.
    }
  indx[jj]=imax;
  if( a(jj,jj) == 0.0 )
  //  a(jj,jj) = TINY;
  { delete[] vv;
    Error( "LUDecomposition", "Singular matrix in routine ludcmp");
  }
  //  If the pivot element is zero the matrix is singular
  // (at least to the precision of the algorithm).
  // For some applications on singular matrices, it is desirable to substitute TINY for zero.
  if( jj != N-1)
  {  //  Now, finally, divide by the pivot element.
    dum = 1.0/(a(jj,jj));
    for( ii=jj+1; ii<N; ii++)
      a(ii,jj) *= dum;
   }
  }  // Go back for the next column in the reduction.

  delete[] vv;
}

// Here is the routine for forward substitution and backsubstitution,
void LUbksb( int N, double* A, int *indx, double* B)
// Solves the set of n linear equations A·X = B.
// Here A[0..N][0..N] is input, not as the matrix A but rather as its LU
// decomposition, determined by the routine LUdcmp.
// indx[0..N] is input as the permutation vector returned by LUdcmp.
// B[0..N] is input as the right-hand side vector B,
// and returns with the solution vector X.
{
  int i, ii=-1, ip,j;
  double sum;

  // When ii is set to a positive value, it will become the
  // index of the first nonvanishing element of b.
  for( i=0; i<N; i++)
  { ip = indx[i];
    sum = B[ip];
    B[ip] = B[i];
    if( ii != -1 )
      for( j=ii; j<=i-1; j++)
         sum -= a(i,j)*B[j];
    else
       if(sum)     // A nonzero element was encountered, so from now on we
          ii = i; // will have to do the sums in the loop above.
    B[i]=sum;
  }
  for( i = N-1; i>=0; i--)
  {
    sum = B[i];
    for( j=i+1; j<N; j++)
       sum -= a(i,j)*B[j];
    B[i] = sum/a(i,i);  //  Store a component of the solution vector X.
  }
}

// Inverse of a Matrix using the above LU decomposition
// N - dimension of the matrix A (number of equations)
// A - input matrix of coeffs N*(N);
// Y - output matrix of coeffs N*(N);
void InverseofMatrix( int N, double* A, double* Y )
{
  double d, *col;
  int ii, jj, *indx;
  N1 = N;

  // alloc memory
  indx = new int[N];
  col = new double[N];
  if( !Y )
   Y = new double[N*N];

  // given LU decomposition
  LUdcmp( N, A, indx, d);

  for( jj=0; jj<N; jj++ )
  {   // Find inverse by columns
    for( ii=0; ii<N; ii++ )
     col[ii] =0.0;
    col[jj] =1.0;
    LUbksb( N, A, indx, col);
    for( ii=0; ii<N; ii++ )
     (*(Y+(jj)+(ii)*N)) = col[ii];
  }
  delete[] indx;
  delete[] col;
}

// Determinant of a Matrix using the above LU decomposition
// N - dimension of the matrix A (number of equations)
// A - input matrix of coeffs N*(N);
double DeterminantofMatrix( int N, double* A )
{
  double d;
  int jj, *indx;
  N1 = N;

  // alloc memory
  indx = new int[N];

  // given LU decomposition
  LUdcmp( N, A, indx, d);
  for( jj=0; jj<N; jj++ )
    d *= a( jj, jj);

  delete[] indx;
  return d;
}

#undef a

//-----------------------End of num_methods.cpp--------------------------

