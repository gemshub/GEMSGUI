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

#include "num_methods.h"
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

// Random numbers ==========================================================

// uniform point
double randuni(double& x)
{ double m35=34359738368., m36=68719476736., m37=137438953472.;
  float a=0.,b=1.;
  x=x*5.;
  if(x>=m37) x=x-m37;
  if(x>=m36) x=x-m36;
  if(x>=m35) x=x-m35;
 return(x/m35*(b-a)+a);
}

// normal point
double randnorm(double& x)
{ double R1=0.;
  int j;
  for(j=0;j<101;j++)
    R1+=randuni(x);
      R1=(R1-101./2.)/pow(101./12.,0.5);
           R1=1./6.*(R1-(-3.0));
           if(R1<0.) R1=0.;
           if(R1>1.) R1=1.;
           return(R1);
/*return(1./9.*(R1-(-4.5)));*/
}

#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

// Long period (> 2 × 1018) random number generator of L’Ecuyer with Bays-Durham shuffle
// and added safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of
// the endpoint values). Call with idum a negative integer to initialize; thereafter, do not alter
// idum between successive deviates in a sequence. RNMX should approximate the largest floating
// value that is less than 1.
float ran2(long& idum)
{
   int j;
   long k;
   static long idum2=123456789;
   static long iy=0;
   static long iv[NTAB];
   float temp;
   if (idum <= 0)
   { // Initialize.
      if ( -idum < 1)
         idum=1; // Be sure to prevent idum = 0.
      else
         idum = -idum;
      idum2= idum;
     for (j=NTAB+7;j>=0;j--) // Load the shuffle table (after 8 warm-ups).
     {
       k = idum/IQ1;
       idum = IA1*(idum-k*IQ1)-k*IR1;
       if (idum < 0)
         idum += IM1;
       if (j < NTAB)
         iv[j] = idum;
     }
   iy=iv[0];
  }
   k = idum/IQ1;       //    Start here when not initializing.
   idum = IA1 * (idum-k*IQ1) - k*IR1;  //Compute idum=(IA1*idum) % IM1 without
                                       // overflows by Schrage’s  method.
   if ( idum < 0 )
      idum += IM1;
   k = idum2/IQ2;
   idum2 = IA2*(idum2-k*IQ2)-k*IR2;  //Compute idum2=(IA2*idum) % IM2 likewise.
   if (idum2 < 0)
      idum2 += IM2;
   j = iy/NDIV;                      // Will be in the range 0..NTAB-1.
   iy = iv[j]-idum2;                 // Here idum is shuffled, idum and idum2 are
   iv[j] = idum;                     // combined to generate output.
   if (iy < 1)
      iy += IMM1;
   if ( ( temp=AM*iy) > RNMX)
       return RNMX;                //  Because users don’t expect endpoint values.
   return temp;
}

#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)

// According to Knuth, any large MBIG, and any smaller (but still large) MSEED
// can be substituted for the above values.
// Returns a uniform random deviate between 0.0 and 1.0. Set idum to any negative value to
// initialize or reinitialize the sequence.
float ran3(long& idum)
{
  static int inext,inextp;
  static long ma[56];     // The value 56 (range ma[1..55]) is special and
  static int iff=0;       // should not be modified; see  Knuth.
  long mj,mk;
  int i,ii,k;
  if ( idum < 0 || iff == 0) // Initialization.
  {
     iff=1;
     mj = labs( MSEED - labs(idum));  // Initialize ma[55] using the seed idum
     mj %= MBIG;                      // and the large number MSEED.
     ma[55] = mj;
     mk = 1;
     for (i=1;i<=54;i++)   // Now initialize the rest of the table,
     {  ii=(21*i) % 55;    // in a slightly random order,
        ma[ii]=mk;         // with numbers that are not especially random.
        mk=mj-mk;
        if (mk < MZ)
          mk += MBIG;
        mj=ma[ii];
      }
      for (k=1;k<=4;k++)    // We randomize them by “warming upthe generator.”
        for(i=1;i<=55;i++)
        {
         ma[i] -= ma[1+(i+30) % 55];
         if (ma[i] < MZ)
            ma[i] += MBIG;
         }
       inext=0;     // Prepare indices for our first generated number.
       inextp=31;  //  The constant 31 is special; see Knuth.
       idum=1;
   }
   //  Here is where we start, except on initialization.
    if (++inext == 56)
       inext=1;           // Increment inext and inextp, wrapping around 56 to 1.
    if (++inextp == 56)
       inextp=1;
    mj = ma[inext]-ma[inextp];  // Generate a new random number subtractively.
    if (mj < MZ)
         mj += MBIG;    //  Be sure that it is in range.
    ma[inext]=mj;       // Store it,
    return mj*FAC;      // and output the derived uniform deviate.
}

//-----------------------End of num_methods.cpp--------------------------

