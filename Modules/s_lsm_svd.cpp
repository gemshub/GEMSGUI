//-------------------------------------------------------------------
// $Id: s_lsm_svd.cpp 700 2006-04-18 06:52:07Z gems $
//
// Implementation of  TSVDcalc classe
//
// Solves or minimizes the sum of squares of m nonlinear
// functions of n variables.
//
// Use algorithm of Singular Value Decomposition
// ( Adapted from JAMA, a Java Matrix Library, developed by jointly
//     by the Mathworks and NIST; see  http://math.nist.gov/javanumerics/jama).
//
// Written by S.Dmytriyeva
// Copyright (C) 2005-2006  D.Kulik, S.Dmytriyeva
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include <memory>
#include "s_lsm.h"
static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
(iminarg1) : (iminarg2))

TSVDcalc::TSVDcalc( double *aPar, TLMDataType *aData ):
   data(aData), par(aPar), A(0), b(0)
{
    m = aData->getM_dat();
    n = aData->getN_par();
    chisq = 0.0;
}

TSVDcalc::TSVDcalc( short M, short N, float *aA, double *ab, double *aX ):
   data(0), m(M), n(N), par(aX), A(aA), b(ab)
{
    chisq = 0.0;
}

TSVDcalc::~TSVDcalc()
{}

// *** main function.
void TSVDcalc::CalcMin( double *sdpar )
{
 //  int i, j;

//#ifdef IPMGEMPLUGIN
//    if( data->getInfo() == -1 ) //test_sizes
//      return;
//#endif

  svdMin( par );
  data->xi2     /= (m-n);

/* test out=========================================
fstream f_out("fit_func.out", ios::out|ios::app  );
  if( !f_out.good() )
   return;

 f_out << "V = " << endl;
    for( i=0; i<n; i++ )
    {  for( j=0; j<n; j++ )
       f_out << V[i][j] << " ";
      f_out << endl;
    }
 f_out << "W = " << endl;
    for( i=0; i<IMIN(m+1,n); i++ )
       f_out << w[i] << " ";
      f_out << endl;
// end test out=========================================*/

  svdStat();

  // calc statistic
   for(int ii=0; ii<n; ii++ )
    sdpar[ii] = CVM[ii][ii];

  data->lm_print_default( par, 0, CVM[0], -1, 0,0,data->xi2 );
}


// *** main function.
int TSVDcalc::CalcSVD(  bool transp )
{
 int i, j;

 AA = Array2D<double>(m, n);

 for(i=0;i<m;i++)
  for(j=0;j<n;j++)
  {
    if( transp )     // transpose input matrix  A
      AA[i][j]= (A[(j)*m+(i)]);
    else
      AA[i][j]=a(i,j);
  }
 svdGetUWV( AA );

/* test out=========================================
 fstream f_out("svd.out", ios::out  );
 if( !f_out.good() )
   return -1;

 // output u, v, w to file
 f_out << "u = " << endl;
 for( i=0; i<m; i++ )
 {  for( j=0; j<IMIN(m+1,n); j++ )
    f_out << U[i][j] << " ";
   f_out << endl;
 }
 f_out << "w = " << endl;
 for( j=0; j<IMIN(m+1,n); j++ )
    f_out << w[j] << " ";
 f_out << endl;
 f_out << "v = " << endl;
 for( i=0; i<n; i++ )
 {  for( j=0; j<n; j++ )
    f_out << V[i][j] << " ";
   f_out << endl;
 }
 // end test out=========================================*/

  int wj_zero = 0;
  double wmin, wmax=0.0; // Will be the maximum singular value obtained.
  for(j=0;j<IMIN(m+1,n);j++)
     if (w[j] > wmax) wmax=w[j];
// This is where we set the threshold for singular values allowed to be nonzero.
// The constant  is typical, but not universal.
// You have to experiment with your own application.
  wmin=wmax*1.0e-6;
  for(j=0;j<IMIN(m+1,n);j++)
    if (w[j] < wmin)
      {  wj_zero = (j+1);
         w[j]=0.0;
      }
  svdGetX( b, par ); // Now we can backsubstitute.

  if( wj_zero && transp )
    svdGetXmore0( wj_zero-1, par ); // Now we get x with elements >=0

/* test out=========================================
// output vectors w, x
 f_out << "w = " << endl;
 for( j=0; j<IMIN(m+1,n); j++ )
    f_out << w[j] << " ";
 f_out << endl;
 f_out << "x = " << endl;
 for( j=0; j<n; j++ )
    f_out << par[j] << " ";
 f_out << endl;
 // end test out=========================================*/

 return wj_zero;
}

void TSVDcalc::CalcB( bool transp, fd_type *bb )
{
  int j,i;
  float aa;

  for( i=0; i<m; i++ )
  {
    bb[i] = 0;
    for( j=0; j<n; j++ )
    {
       if( transp )     // transpose input matrix  A
         aa = (A[(j)*m+(i)]);
      else
         aa = a(i,j);
       bb[i] += aa * par[j];
     }
  }
}

// internal functions
//--------------------------------------------------------------------------
void TSVDcalc::svdGetXmore0( int ii, fd_type x[] )
{
 int j, jmin=0, jmax=0;
 fd_type a1, max=0, min=0;

 for (j=0;j<n;j++)
   if( x[j] < 0 )
   break;

 if( j == n ) // all elements x more 0
   return;

 for (j=0;j<n;j++)   //Select intervals
 {
   if( V[j][ii] > 0 )
   {
     a1 = (-x[j])/V[j][ii];
     if( !jmin || a1 > min )
     {
       min = a1; jmin = j+1;
     }
   }
   if( V[j][ii] < 0 )
   {
     a1 = (-x[j])/V[j][ii];
     if( !jmax || a1 < max )
     {
       max = a1; jmax = j+1;
     }
   }
 }
 if( min > max ) // no solutions in system
   return;

 a1 = (min+max)/2;
 for (j=0;j<n;j++)  // X multiply by a*V to get answer.
    x[j]  += V[j][ii]*a1;
}


void TSVDcalc::svdGetX( fd_type b1[], fd_type x[] )
// Solves A�x = b1 for a vector x, where A is specified by the arrays
// U, w, V as returned by svdGetUWV.
//  b1 is the input right-hand side.
//  x is the output solution vector.
{
  int jj,j,i;
  fd_type s=0.;
  Array1D<double> tmp(n, s);

  for( j=0;j<IMIN(m+1,n);j++ )   // Calculate UTB.
  {
     s = 0.0;
     if ( w[j] )  // Nonzero result only if wj is nonzero.
     {
       for (i=0;i<m;i++)
           s += U[i][j]*b1[i];
        s /= w[j]; // This is the divide by wj .
     }
     tmp[j] = s;
  }
  for ( j=0;j<n;j++ )  // Matrix multiply by V to get answer.
  {
     s = 0.0;
     for (jj=0;jj<n;jj++)
        s += V[j][jj]*tmp[jj];
     x[j] = s;
  }
}

void TSVDcalc::svdGetUWV( const Array2D<double> &Arg )
/** Singular Value Decomposition.

For an m-by-n matrix A with m >= n, the singular value decomposition is
an m-by-n orthogonal matrix U, an n-by-n diagonal matrix S, and
an n-by-n orthogonal matrix V so that A = U*S*V'.

The singular values, sigma[k] = S[k][k], are ordered so that
sigma[0] >= sigma[1] >= ... >= sigma[n-1].

The singular value decompostion always exists, so the constructor will
never fail.  The matrix condition number and the effective numerical
rank can be computed from this decomposition.
( Adapted from JAMA, a Java Matrix Library, developed by jointly
  by the Mathworks and NIST; see  http://math.nist.gov/javanumerics/jama).
*/
{
  /*   m = Arg.dim1();
       n = Arg.dim2();
       int nu = min(m,n);
       s = Array1D<Real>(min(m+1,n));
       U = Array2D<Real>(m, nu, Real(0));
       V = Array2D<Real>(n,n);
  */
 SVD<double> data1(Arg);

 data1.getU( U ); // m * min( m+1, n )
 // Return the right singular vectors
 data1.getV( V ); // n*n
 // Return the one-dimensional array of singular values
 data1.getSingularValues( w ); // min(m+1,n)
}

void TSVDcalc::svdMin(  fd_type a[] )
// Given a set of data points x[1..m],y[1..m] with individual coeff
// wpar[1..ndata], use ��2 minimization to determine the coefficients a[1..ma]
// of the fitting  function y = Sumi (ai*afunci(x)).
// Here we solve the fitting equations.
// Arrays u[1..m][1..n], v[1..n][1..n],
// and w[1..n] provide workspace on input; on output they define the
// singular value decomposition, and can be used to obtain the covariance matrix.
// The program  returns values for the ma fit parameters a, and X, chisq.
// The user supplies a routine funcs(x,afunc,ma) that returns the ma basis
// functions evaluated at x = x in the array afunc[1..n].
{

   fd_type wmax,tmp,thresh,sum,*b1,*afunc;
   int i, j;

   b1 = new fd_type[m];
   afunc = new fd_type[n];
   AA = Array2D<double>(m, n);

   for (i=0;i<m;i++)  // Accumulate coefficients of the fitting matrix
   {
       data->par_funct( i, data->getX(i),afunc );
       tmp= data->getWdat(i); // 1.0/sig[i];
       for (j=0;j<n;j++)
         AA[i][j] = afunc[j]*tmp;
       b1[i] = data->getY(i) * tmp;
    }

   svdGetUWV( AA );    //Singular value decomposition.

   wmax=0.0;                     // Edit the singular values, given TOL from
   for (j=0;j<IMIN(m+1,n);j++)
       if (w[j] > wmax) wmax=w[j];
   thresh=TOL*wmax;
   for (j=0;j<n;j++)
       if (w[j] < thresh) w[j]=0.0;

   svdGetX( b1, a );

   chisq=0.0;                    // Evaluate chi-square.
   for (i=0;i<m;i++)
   {
       data->par_funct( i, data->getX(i),afunc );
       for (sum=0.0,j=0;j<n;j++)
         sum += a[j]*afunc[j];
       tmp=(data->getY(i)-sum)*data->getWdat(i);
       chisq += tmp*tmp;
   }
   //chisq = sqrt(chisq);
   data->xi2 = chisq;

   delete[] afunc;
   delete[] b1;
}

void TSVDcalc::svdStat()
// To evaluate the covariance matrix CVM
// call this routine with matrices V, w
{
   int k,j,i;
   fd_type sum, tmp =0.;
   Array1D<double> wti(n, tmp);

   CVM = Array2D<double>(n, n, tmp);
   for( i=0;i<IMIN(m+1,n);i++ )
   {
       wti[i]=0.0;
       if(w[i])
         wti[i]=1.0/(w[i]*w[i]);
   }

   for(i=0;i<n;i++) // Sum contributions to covariance matrix
     for(j=0;j<=i;j++)
     {
       for (sum=0.0,k=0;k<n;k++)
          sum += V[i][k]*V[j][k]*wti[k];
       tmp = sqrt(fabs(sum));
       CVM[j][i] = CVM[i][j] = SIGN(tmp, sum);
     }
}

// ------------------- End of s_lsm_svd.cpp --------------------------




