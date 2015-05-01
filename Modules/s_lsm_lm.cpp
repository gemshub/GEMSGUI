//-------------------------------------------------------------------
// $Id: s_lsm_lm.cpp 700 2006-04-18 06:52:07Z gems $
//
// Implementation of  TLMmin classe
//
// Solves or minimizes the sum of squares of m nonlinear
// functions of n variables.
//
// Use algorithm lmfit
//
// From public domain Fortran version
// of Argonne National Laboratories MINPACK
//     argonne national laboratory. minpack project. march 1980.
//     burton s. garbow, kenneth e. hillstrom, jorge j. more
// C translation by Steve Moshier
// Joachim Wuttke converted the source into C++ compatible ANSI style
// and provided a simplified interface
//
// Adapted by S.Dmytriyeva
// Copyright (C) 2005-2006  D.Kulik, S.Dmytriyeva
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include <cstdlib>
#include <climits>
#include <cmath>
#include "s_lsm.h"
#include "verror.h"

// ***** the following messages are referenced by the variable info.

const char *lm_shortmsg[] = {
        "invalid input",
        "success (f)",
        "success (p)",
        "success (f,p)",
        "degenerate",
        "call limit",
        "failed (f)",
        "failed (p)",
        "failed (o)",
        "no memory",
        "user break"
};

const char *lm_infmsg[] = {
    "improper input parameters",
    "the relative error in the sum of squares is at most tol",
    "the relative error between x and the solution is at most tol",
    "both errors are at most tol",
    "fvec is orthogonal to the columns of the jacobian to machine precision",
    "number of calls to fcn has reached or exceeded 200*(n+1)",
    "ftol is too small. no further reduction in the sum of squares is possible",
    "xtol too small. no further improvement in approximate solution x possible",
    "gtol too small. no further improvement in approximate solution x possible",
    "not enough memory",
    "break requested within function evaluation"
};


TLMmin::TLMmin():
   data(0), par(0)
{
    m_dat = 0;
    n_par = 0;
    fvec = 0;
    diag = 0;
    fjac = 0;
    qtf = 0;
    wa1 = 0;
    wa2 = 0;
    wa3 = 0;
    wa4 = 0;
    ipvt = 0;
    CVM = 0;
    chisq = 0.0;

}
TLMmin::TLMmin( double *aPar, TLMDataType *aData ):
   data(aData), par(aPar)
{
    m_dat = aData->getM_dat();
    n_par = aData->getN_par();
    fvec = 0;
    diag = 0;
    fjac = 0;
    qtf = 0;
    wa1 = 0;
    wa2 = 0;
    wa3 = 0;
    wa4 = 0;
    ipvt = 0;
    CVM = 0;
    chisq = 0.0;

}

TLMmin::~TLMmin()
{
  free_arrays();
}

// *** main function.
void TLMmin::Calc( double *sdpar, double *apar_ap,
                   double*ad_par, short *ad_type )
{
  if( !data )
    return;
#ifdef IPMGEMPLUGIN
    if( data->getInfo() == -1 ) //test_sizes
      return;
#endif
    par_ap = apar_ap;
    d_par = ad_par;
    d_type = ad_type;
    lm_minimize( sdpar );
}

// inmternakl functions

// *** allocate work space.
void TLMmin::alloc_arrays( )
{
    int n = n_par;
    int m = m_dat;

    fvec = new double[m];
    diag = new double[n];
    qtf = new double[n];
    fjac = new double[n*m];
    wa1 = new double[n];
    wa2 = new double[n];
    wa3 = new double[n];
    wa4 = new double[m];
    ipvt = new int[n];
    CVM = new double[n*n];

    if (!fvec || !diag || !qtf || !fjac||
        !wa1 || !wa2 || !wa3 || !wa4 || !ipvt || !CVM )
            control.info = 9;

}


// *** free work space.
void TLMmin::free_arrays( )
{
  if( fvec)
    { delete[] fvec; fvec = 0; }
  if( diag)
    { delete[] diag; diag = 0; }
  if( qtf)
    { delete[] qtf; qtf = 0; }
  if( fjac)
    { delete[] fjac; fjac = 0; }
  if( wa1)
    { delete[] wa1; wa1 = 0; }
  if( wa2)
    { delete[] wa2; wa2 = 0; }
  if( wa3)
    { delete[] wa3; wa3 = 0; }
  if( wa4)
    { delete[] wa4; wa4 = 0; }
  if( ipvt)
    { delete[] ipvt; ipvt = 0; }
  if( CVM)
    { delete[] CVM; CVM = 0; }
 }

void TLMmin::CheckLimits( double *p )
{
  int ii;
  double tmp;

  if( !d_type || !par_ap || !d_par )
   return;
  for( ii=0; ii<n_par; ii++ )
  {
    switch( d_type[ii] )
    {
      case 1:  /* lower bounds only */
              tmp = par_ap[ii] -  fabs(d_par[ii]);
              if( p[ii] < tmp )
                p[ii] =  tmp;
              break;
      case 3:
              tmp = par_ap[ii] -  fabs(d_par[ii]);
              if( p[ii] < tmp )
                p[ii] =  tmp;
      case 2:  /* upper bounds only */
              tmp = par_ap[ii] +  fabs(d_par[ii]);
              if( p[ii] > tmp )
                p[ii] =  tmp;
              break;
      case 0:
      default: break;
   }
  }

}


void TLMmin::ChTol( double& fTol, double& xTol )
{
  int ii;

  if( !d_type  )
   return;
  for( ii=0; ii<n_par; ii++ )
    if( d_type[ii] > 0 && d_type[ii]< 4 )
      break;
  if( ii < n_par)
  {
    fTol = 1e-10;
    xTol = 1e-10;
  }
}


/* *********************** high-level interface **************************** */
void TLMmin::lm_minimize( double* sdpar )
{

  alloc_arrays();
  // *** perform fit.

  control.info = 0;
  control.nfev = 0;

    // this goes through the modified legacy interface:
   lm_lmdif( m_dat, n_par, par, fvec, control.ftol, control.xtol, control.gtol,
              control.maxcall*(n_par+1), control.epsilon, diag, 1,
              control.stepbound, &(control.info),
              &(control.nfev), fjac, ipvt, qtf, wa1, wa2, wa3, wa4 );

// calc statistic
   control.fnorm = lm_enorm( m_dat, fvec);
   data->xi2     = control.fnorm * control.fnorm;
   data->xi2     /= (m_dat-n_par);
   lm_COVAR( CVM, data->xi2, n_par, m_dat);
   for(int ii=0; ii<n_par; ii++ )
    sdpar[ii] = CVM[ii*n_par+ii];

// output resalts
    if (control.info < 0 ) control.info = 10;
    data->lm_print_default( par, fvec, CVM, -1,
         control.info, control.nfev, data->xi2 );

// *** clean up.
  free_arrays();
}

/* ************************** implementation ******************************* */
// the following values seem good for an x86:
#define LM_MACHEP .555e-16 /* resolution of arithmetic */
#define LM_DWARF  9.9e-324 /* smallest nonzero number */
// the follwoing values should work on any machine:
// #define LM_MACHEP 1.2e-16
// #define LM_DWARF 1.0e-38

/*
// the squares of the following constants shall not under/overflow:
// these values seem good for an x86:
#define LM_SQRT_DWARF 1.e-160
#define LM_SQRT_GIANT 1.e150
// the following values should work on any machine:
// #define LM_SQRT_DWARF 3.834e-20
// #define LM_SQRT_GIANT 1.304e19
*/
#define MIN(a,b) (((a)<=(b)) ? (a) : (b))
#define MAX(a,b) (((a)>=(b)) ? (a) : (b))
#define SQR(x)   (x)*(x)


// ***** the low-level legacy interface for full control.
void TLMmin::lm_lmdif( int m, int n, double* x, double* fvec, double ftol, double xtol,
               double gtol, int maxfev, double epsfcn, double* diag, int mode,
               double factor, int *info, int *nfev,
               double* fjac, int* ipvt, double* qtf,
               double* wa1, double* wa2, double* wa3, double* wa4 )
{
/*
 *   the purpose of lmdif is to minimize the sum of the squares of
 *   m nonlinear functions in n variables by a modification of
 *   the levenberg-marquardt algorithm. the user must provide a
 *   subroutine evaluate which calculates the functions. the jacobian
 *   is then calculated by a forward-difference approximation.
 *
 *   the multi-parameter interface lm_lmdif is for users who want
 *   full control and flexibility. most users will be better off using
 *   the simpler interface lmfit provided above.
 *
 *   the parameters are the same as in the legacy FORTRAN implementation,
 *   with the following exceptions:
 *      the old parameter ldfjac which gave leading dimension of fjac has
 *        been deleted because this C translation makes no use of two-
 *        dimensional arrays;
 *      the old parameter nprint has been deleted; printout is now controlled
 *        by the user-supplied routine *printout;
 *      the parameter field *data and the function parameters *evaluate and
 *        *printout have been added; they help avoiding global variables.
 *
 *   parameters:
 *
 *	m is a positive integer input variable set to the number
 *	  of functions.
 *
 *	n is a positive integer input variable set to the number
 *	  of variables. n must not exceed m.
 *
 *	x is an array of length n. on input x must contain
 *	  an initial estimate of the solution vector. on output x
 *	  contains the final estimate of the solution vector.
 *
 *	fvec is an output array of length m which contains
 *	  the functions evaluated at the output x.
 *
 *	ftol is a nonnegative input variable. termination
 *	  occurs when both the actual and predicted relative
 *	  reductions in the sum of squares are at most ftol.
 *	  therefore, ftol measures the relative error desired
 *	  in the sum of squares.
 *
 *	xtol is a nonnegative input variable. termination
 *	  occurs when the relative error between two consecutive
 *	  iterates is at most xtol. therefore, xtol measures the
 *	  relative error desired in the approximate solution.
 *
 *	gtol is a nonnegative input variable. termination
 *	  occurs when the cosine of the angle between fvec and
 *	  any column of the jacobian is at most gtol in absolute
 *	  value. therefore, gtol measures the orthogonality
 *	  desired between the function vector and the columns
 *	  of the jacobian.
 *
 *	maxfev is a positive integer input variable. termination
 *	  occurs when the number of calls to lm_fcn is at least
 *	  maxfev by the end of an iteration.
 *
 *	epsfcn is an input variable used in determining a suitable
 *	  step length for the forward-difference approximation. this
 *	  approximation assumes that the relative errors in the
 *	  functions are of the order of epsfcn. if epsfcn is less
 *	  than the machine precision, it is assumed that the relative
 *	  errors in the functions are of the order of the machine
 *	  precision.
 *
 *	diag is an array of length n. if mode = 1 (see below), diag is
 *        internally set. if mode = 2, diag must contain positive entries
 *        that serve as multiplicative scale factors for the variables.
 *
 *	mode is an integer input variable. if mode = 1, the
 *	  variables will be scaled internally. if mode = 2,
 *	  the scaling is specified by the input diag. other
 *	  values of mode are equivalent to mode = 1.
 *
 *	factor is a positive input variable used in determining the
 *	  initial step bound. this bound is set to the product of
 *	  factor and the euclidean norm of diag*x if nonzero, or else
 *	  to factor itself. in most cases factor should lie in the
 *	  interval (.1,100.). 100. is a generally recommended value.
 *
 *	info is an integer output variable that indicates the termination
 *        status of lm_lmdif as follows:
 *
 *        info < 0  termination requested by user-supplied routine *evaluate;
 *
 *	  info = 0  improper input parameters;
 *
 *	  info = 1  both actual and predicted relative reductions
 *		    in the sum of squares are at most ftol;
 *
 *	  info = 2  relative error between two consecutive iterates
 *		    is at most xtol;
 *
 *	  info = 3  conditions for info = 1 and info = 2 both hold;
 *
 *	  info = 4  the cosine of the angle between fvec and any
 *		    column of the jacobian is at most gtol in
 *		    absolute value;
 *
 *	  info = 5  number of calls to lm_fcn has reached or
 *		    exceeded maxfev;
 *
 *	  info = 6  ftol is too small. no further reduction in
 *		    the sum of squares is possible;
 *
 *	  info = 7  xtol is too small. no further improvement in
 *		    the approximate solution x is possible;
 *
 *	  info = 8  gtol is too small. fvec is orthogonal to the
 *		    columns of the jacobian to machine precision;
 *
 *	nfev is an output variable set to the number of calls to the
 *        user-supplied routine *evaluate.
 *
 *	fjac is an output m by n array. the upper n by n submatrix
 *	  of fjac contains an upper triangular matrix r with
 *	  diagonal elements of nonincreasing magnitude such that
 *
 *		 t     t	   t
 *		p *(jac *jac)*p = r *r,
 *
 *	  where p is a permutation matrix and jac is the final
 *	  calculated jacobian. column j of p is column ipvt(j)
 *	  (see below) of the identity matrix. the lower trapezoidal
 *	  part of fjac contains information generated during
 *	  the computation of r.
 *
 *	ipvt is an integer output array of length n. ipvt
 *	  defines a permutation matrix p such that jac*p = q*r,
 *	  where jac is the final calculated jacobian, q is
 *	  orthogonal (not stored), and r is upper triangular
 *	  with diagonal elements of nonincreasing magnitude.
 *	  column j of p is column ipvt(j) of the identity matrix.
 *
 *	qtf is an output array of length n which contains
 *	  the first n elements of the vector (q transpose)*fvec.
 *
 *	wa1, wa2, and wa3 are work arrays of length n.
 *
 *	wa4 is a work array of length m.
 *
 *   the following parameters are newly introduced in this C translation:
 *
 *      evaluate is the name of the subroutine which calculates the functions.
 *        a default implementation lm_evaluate_default is provided in lm_eval.c;
 *        alternatively, evaluate can be provided by a user calling program.
 *        it should be written as follows:
 *
 *        void evaluate ( double* par, int m_dat, double* fvec,
 *                       void *data, int *info )
 *        {
 *           // for ( i=0; i<m_dat; ++i )
 *           //     calculate fvec[i] for given parameters par;
 *           // to stop the minimization,
 *           //     set *info to a negative integer.
 *        }
 *
 *      printout is the name of the subroutine which nforms about fit progress.
 *        a default implementation lm_print_default is provided in lm_eval.c;
 *        alternatively, printout can be provided by a user calling program.
 *        it should be written as follows:
 *
 *        void printout ( int n_par, double* par, int m_dat, double* fvec,
 *                       void *data, int iflag, int iter, int nfev )
 *        {
 *           // iflag : 0 (init) 1 (outer loop) 2(inner loop) -1(terminated)
 *           // iter  : outer loop counter
 *           // nfev  : number of calls to *evaluate
 *        }
 *
 *      data is an input pointer to an arbitrary structure that is passed to
 *        evaluate. typically, it contains experimental data to be fitted.
 *
 */
    int i, iter, j;
    double actred, delta, dirder, eps, fnorm, fnorm1, gnorm, par, pnorm,
        prered, ratio, step, sum, temp, temp1, temp2, temp3, xnorm;
    static double p1 = 0.1;
    static double p5 = 0.5;
    static double p25 = 0.25;
    static double p75 = 0.75;
    static double p0001 = 1.0e-4;

    *nfev = 0; // function evaluation counter
    iter = 1;  // outer loop counter
    par = 0;   // levenberg-marquardt parameter
    delta = 0; // just to prevent a warning (initialization within if-clause)
    xnorm = 0; // dito

    ChTol( ftol, xtol );
    CheckLimits( x );   // Srart array test
    temp = MAX(epsfcn,LM_MACHEP);
    eps = sqrt(temp); // used in calculating the Jacobian by forward differences

// *** check the input parameters for errors.

    if ( (n <= 0) || (m < n) || (ftol < 0.)
	|| (xtol < 0.) || (gtol < 0.) || (maxfev <= 0) || (factor <= 0.) )
    {
#ifndef IPMGEMPLUGIN
        Error( lm_shortmsg[0], lm_infmsg[0]);
#else
        *info = 0; // invalid parameter
        return;
#endif
    }
    if ( mode == 2 )  /* scaling by diag[] */
    {
	for ( j=0; j<n; j++ )  /* check for nonpositive elements */
        {
            if ( diag[j] <= 0.0 )
            {
#ifndef IPMGEMPLUGIN
              Error( lm_shortmsg[0], lm_infmsg[0]);
#else
                *info = 0; // invalid parameter
                return;
#endif
            }
        }
    }

// *** evaluate the function at the starting point and calculate its norm.

//    *info = 0;
//    (*evaluate)( x, m, fvec, data, info );
//    (*printout)( n, x, m, fvec, data, 0, 0, ++(*nfev) );

// SD oct 2005
    *info = data->evaluate( x, fvec );
    data->lm_print_default( x, fvec, 0, 0, 0, ++(*nfev), lm_enorm(m,fvec) );
//    if ( *info < 0 ) return; must be exeption if error
    fnorm = lm_enorm(m,fvec);

// *** the outer loop.

    do {

// O** calculate the jacobian matrix.

        for ( j=0; j<n; j++ )
        {
            temp = x[j];
            step = eps * fabs(temp);
            if (step == 0.) step = eps;
            x[j] = temp + step;
//            *info = 0;
//            (*evaluate)( x, m, wa4, data, info );
//            (*printout)( n, x, m, wa4, data, 1, iter, ++(*nfev) );
// SD oct 2005
    *info = data->evaluate( x, wa4 );
    data->lm_print_default( x, wa4, 0, 1, iter, ++(*nfev) , lm_enorm(m,wa4));
//            if ( *info < 0 ) return;  // user requested break must be exeption
            x[j] = temp;
            for ( i=0; i<m; i++ )
                fjac[j*m+i] = (wa4[i] - fvec[i]) / step;
        }

// O** compute the qr factorization of the jacobian.
         //   compute A = JtJ
        for(int  ii=0; ii<n; ii++)
        for(int jj=ii; jj<n; jj++)
        {
          double tmp = 0;
          for(int kk=0; kk<m; kk++ )
          tmp += fjac[kk*n+ii] * fjac[kk*n+jj];
          CVM[ii*n+jj] = CVM[jj*n+ii] = tmp;
        }

        lm_qrfac( m, n, fjac, 1, ipvt, wa1, wa2, wa3);
/*         //   compute A = JtJ
        for(int  ii=0; ii<n; ii++)
        for(int jj=ii; jj<n; jj++)
        {
          double tmp = 0;
          for(int kk=0; kk<m; kk++ )
          tmp += fjac[kk*n+ii] * fjac[kk*n+jj];
          CVM[ii*n+jj] = CVM[jj*n+ii] = tmp;
        }
*/
// O** on the first iteration ...

        if (iter == 1)
        {
            if (mode != 2)
//      ... scale according to the norms of the columns of the initial jacobian.
            {
                for ( j=0; j<n; j++ )
                {
                    diag[j] = wa2[j];
                    if ( wa2[j] == 0. )
                        diag[j] = 1.;
                }
            }

//      ... calculate the norm of the scaled x and
//          initialize the step bound delta.

            for ( j=0; j<n; j++ )
                wa3[j] = diag[j] * x[j];

            xnorm = lm_enorm( n, wa3 );
            delta = factor*xnorm;
            if (delta == 0.)
                delta = factor;
        }

// O** form (q transpose)*fvec and store the first n components in qtf.

        for ( i=0; i<m; i++ )
            wa4[i] = fvec[i];

        for ( j=0; j<n; j++ )
        {
            temp3 = fjac[j*m+j];
            if (temp3 != 0.)
            {
                sum = 0;
                for ( i=j; i<m; i++ )
                    sum += fjac[j*m+i] * wa4[i];
                temp = -sum / temp3;
                for ( i=j; i<m; i++ )
                    wa4[i] += fjac[j*m+i] * temp;
            }
            fjac[j*m+j] = wa1[j];
            qtf[j] = wa4[j];
        }

// O** compute the norm of the scaled gradient and test for convergence.

        gnorm = 0;
        if ( fnorm != 0 )
        {
            for ( j=0; j<n; j++ )
            {
                if ( wa2[ ipvt[j] ] == 0 ) continue;

                sum = 0.;
                for ( i=0; i<=j; i++ )
                    sum += fjac[j*m+i] * qtf[i] / fnorm;
                gnorm = MAX( gnorm, fabs(sum/wa2[ ipvt[j] ]) );
            }
        }

        if ( gnorm <= gtol )
        {
           *info = 4;
#ifndef IPMGEMPLUGIN
         Error( lm_shortmsg[*info], lm_infmsg[*info ]);
#else    // SD oct 2005
          return;
#endif
        }

// O** rescale if necessary.

        if ( mode != 2 )
        {
            for ( j=0; j<n; j++ )
                diag[j] = MAX(diag[j],wa2[j]);
        }

// O** the inner loop.

        do {

// OI* determine the levenberg-marquardt parameter.

            lm_lmpar( n,fjac,m,ipvt,diag,qtf,delta,&par,wa1,wa2,wa3,wa4 );

// OI* store the direction p and x + p. calculate the norm of p.

//SD insert check limits

           for ( j=0; j<n; j++ )
                wa2[j] = x[j] - wa1[j];
            CheckLimits( wa2 );
            for ( j=0; j<n; j++ )
                wa1[j] = x[j] - wa2[j];

// end check limits

           for ( j=0; j<n; j++ )
            {
                wa1[j] = -wa1[j];
                wa2[j] = x[j] + wa1[j];
                wa3[j] = diag[j]*wa1[j];
            }
            pnorm = lm_enorm(n,wa3);

// OI* on the first iteration, adjust the initial step bound.

            if ( *nfev<= 1+n ) // bug corrected by J. Wuttke in 2004
                delta = MIN(delta,pnorm);

// OI* evaluate the function at x + p and calculate its norm.

//            *info = 0;
//            (*evaluate)( wa2, m, wa4, data, info );
//            (*printout)( n, x, m, wa4, data, 2, iter, ++(*nfev) );
// SD oct 2005
    *info = data->evaluate( wa2, wa4 );
    data->lm_print_default( x, wa4, 0, 2, iter, ++(*nfev), lm_enorm(m,wa4) );
//            if ( *info < 0 ) return;  // user requested break exeption

            fnorm1 = lm_enorm(m,wa4);

// OI* compute the scaled actual reduction.

            if ( p1*fnorm1 < fnorm )
                actred = 1 - SQR( fnorm1/fnorm );
            else
                actred = -1;

// OI* compute the scaled predicted reduction and
//     the scaled directional derivative.

            for ( j=0; j<n; j++ )
            {
                wa3[j] = 0;
                for ( i=0; i<=j; i++ )
                    wa3[i] += fjac[j*m+i]*wa1[ ipvt[j] ];
            }
            temp1 = lm_enorm(n,wa3) / fnorm;
            temp2 = sqrt(par) * pnorm / fnorm;
            prered = SQR(temp1) + 2 * SQR(temp2);
            dirder = - ( SQR(temp1) + SQR(temp2) );

// OI* compute the ratio of the actual to the predicted reduction.

            ratio = prered!=0 ? actred/prered : 0;

// OI* update the step bound.

            if (ratio <= p25)
            {
                if (actred >= 0.)
                    temp = p5;
                else
                    temp = p5*dirder/(dirder + p5*actred);
                if ( p1*fnorm1 >= fnorm || temp < p1 )
                    temp = p1;
                delta = temp * MIN(delta,pnorm/p1);
                par /= temp;
            }
            else if ( par == 0. || ratio >= p75 )
            {
                delta = pnorm/p5;
                par *= p5;
            }

// OI* test for successful iteration...

            if (ratio >= p0001)
            {

//     ... successful iteration. update x, fvec, and their norms.

                for ( j=0; j<n; j++ )
                {
                    x[j] = wa2[j];
                    wa2[j] = diag[j]*x[j];
                }
                for ( i=0; i<m; i++ )
                    fvec[i] = wa4[i];
                xnorm = lm_enorm(n,wa2);
                fnorm = fnorm1;
                iter++;
            }
//#if BUG
//            else {
//                printf( "ATTN: iteration considered unsuccessful\n" );
//            }
//#endif

// OI* tests for convergence ( otherwise *info = 1, 2, or 3 )

            *info = 0; // do not terminate (unless overwritten by nonzero value)
            if ( fabs(actred) <= ftol && prered <= ftol && p5*ratio <= 1 )
                *info = 1;
            if (delta <= xtol*xnorm)
                *info += 2;
            if ( *info != 0)
                 return;       // OK

// OI* tests for termination and stringent tolerances.

            if ( *nfev >= maxfev)
                *info = 5;
            if ( fabs(actred) <= LM_MACHEP &&
                 prered <= LM_MACHEP && p5*ratio <= 1 )
                *info = 6;
            if (delta <= LM_MACHEP*xnorm)
                *info = 7;
            if (gnorm <= LM_MACHEP)
                *info = 8;
            if ( *info != 0)
#ifdef IPMGEMPLUGIN
                return;
#else
               Error( lm_shortmsg[*info], lm_infmsg[*info ]);
#endif
// OI* end of the inner loop. repeat if iteration unsuccessful.

        } while (ratio < p0001);

// O** end of the outer loop.

    } while (1);

}



void TLMmin::lm_lmpar(int n, double* r, int ldr, int* ipvt, double* diag, double* qtb,
              double delta, double* par, double* x, double* sdiag,
              double* wa1, double* wa2)
{
/*     given an m by n matrix a, an n by n nonsingular diagonal
 *     matrix d, an m-vector b, and a positive number delta,
 *     the problem is to determine a value for the parameter
 *     par such that if x solves the system
 *
 *	    a*x = b ,	  sqrt(par)*d*x = 0 ,
 *
 *     in the least squares sense, and dxnorm is the euclidean
 *     norm of d*x, then either par is 0. and
 *
 *	    (dxnorm-delta) .le. 0.1*delta ,
 *
 *     or par is positive and
 *
 *	    abs(dxnorm-delta) .le. 0.1*delta .
 *
 *     this subroutine completes the solution of the problem
 *     if it is provided with the necessary information from the
 *     qr factorization, with column pivoting, of a. that is, if
 *     a*p = q*r, where p is a permutation matrix, q has orthogonal
 *     columns, and r is an upper triangular matrix with diagonal
 *     elements of nonincreasing magnitude, then lmpar expects
 *     the full upper triangle of r, the permutation matrix p,
 *     and the first n components of (q transpose)*b. on output
 *     lmpar also provides an upper triangular matrix s such that
 *
 *	     t	 t		     t
 *	    p *(a *a + par*d*d)*p = s *s .
 *
 *     s is employed within lmpar and may be of separate interest.
 *
 *     only a few iterations are generally needed for convergence
 *     of the algorithm. if, however, the limit of 10 iterations
 *     is reached, then the output par will contain the best
 *     value obtained so far.
 *
 *     parameters:
 *
 *	n is a positive integer input variable set to the order of r.
 *
 *	r is an n by n array. on input the full upper triangle
 *	  must contain the full upper triangle of the matrix r.
 *	  on output the full upper triangle is unaltered, and the
 *	  strict lower triangle contains the strict upper triangle
 *	  (transposed) of the upper triangular matrix s.
 *
 *	ldr is a positive integer input variable not less than n
 *	  which specifies the leading dimension of the array r.
 *
 *	ipvt is an integer input array of length n which defines the
 *	  permutation matrix p such that a*p = q*r. column j of p
 *	  is column ipvt(j) of the identity matrix.
 *
 *	diag is an input array of length n which must contain the
 *	  diagonal elements of the matrix d.
 *
 *	qtb is an input array of length n which must contain the first
 *	  n elements of the vector (q transpose)*b.
 *
 *	delta is a positive input variable which specifies an upper
 *	  bound on the euclidean norm of d*x.
 *
 *	par is a nonnegative variable. on input par contains an
 *	  initial estimate of the levenberg-marquardt parameter.
 *	  on output par contains the final estimate.
 *
 *	x is an output array of length n which contains the least
 *	  squares solution of the system a*x = b, sqrt(par)*d*x = 0,
 *	  for the output par.
 *
 *	sdiag is an output array of length n which contains the
 *	  diagonal elements of the upper triangular matrix s.
 *
 *	wa1 and wa2 are work arrays of length n.
 *
 */
    int i, iter, j, nsing;
    double dxnorm, fp, fp_old, gnorm, parc, parl, paru;
    double sum, temp;
    static double p1 = 0.1;
    static double p001 = 0.001;


// *** compute and store in x the gauss-newton direction. if the
//     jacobian is rank-deficient, obtain a least squares solution.

    nsing = n;
    for ( j=0; j<n; j++ )
    {
	wa1[j] = qtb[j];
	if ( r[j*ldr+j] == 0 && nsing == n )
            nsing = j;
	if (nsing < n)
            wa1[j] = 0;
    }
    for ( j=nsing-1; j>=0; j-- )
    {
        wa1[j] = wa1[j]/r[j+ldr*j];
        temp = wa1[j];
        for ( i=0; i<j; i++ )
            wa1[i] -= r[j*ldr+i]*temp;
    }

    for ( j=0; j<n; j++ )
	x[ ipvt[j] ] = wa1[j];

// *** initialize the iteration counter.
//     evaluate the function at the origin, and test
//     for acceptance of the gauss-newton direction.

    iter = 0;
    for ( j=0; j<n; j++ )
	wa2[j] = diag[j]*x[j];
    dxnorm = lm_enorm(n,wa2);
    fp = dxnorm - delta;
    if (fp <= p1*delta)
    {
        *par = 0;
        return;
    }

// *** if the jacobian is not rank deficient, the newton
//     step provides a lower bound, parl, for the 0. of
//     the function. otherwise set this bound to 0..

    parl = 0;
    if (nsing >= n)
    {
	for ( j=0; j<n; j++ )
            wa1[j] = diag[ ipvt[j] ] * wa2[ ipvt[j] ] / dxnorm;

	for ( j=0; j<n; j++ )
        {
            sum = 0.;
            for ( i=0; i<j; i++ )
                sum += r[j*ldr+i]*wa1[i];
            wa1[j] = (wa1[j] - sum)/r[j+ldr*j];
        }
	temp = lm_enorm(n,wa1);
	parl = fp/delta/temp/temp;
    }

// *** calculate an upper bound, paru, for the 0. of the function.

    for ( j=0; j<n; j++ )
    {
	sum = 0;
	for ( i=0; i<=j; i++ )
            sum += r[j*ldr+i]*qtb[i];
	wa1[j] = sum/diag[ ipvt[j] ];
    }
    gnorm = lm_enorm(n,wa1);
    paru = gnorm/delta;
    if (paru == 0.)
	paru = LM_DWARF/MIN(delta,p1);

// *** if the input par lies outside of the interval (parl,paru),
//     set par to the closer endpoint.

    *par = MAX( *par,parl);
    *par = MIN( *par,paru);
    if ( *par == 0.)
	*par = gnorm/dxnorm;

// *** iterate.

    for ( ; ; iter++ ) {

// *** evaluate the function at the current value of par.

        if ( *par == 0.)
            *par = MAX(LM_DWARF,p001*paru);
        temp = sqrt( *par );
        for ( j=0; j<n; j++ )
            wa1[j] = temp*diag[j];
        lm_qrsolv( n, r, ldr, ipvt, wa1, qtb, x, sdiag, wa2);
        for ( j=0; j<n; j++ )
            wa2[j] = diag[j]*x[j];
        dxnorm = lm_enorm(n,wa2);
        fp_old = fp;
        fp = dxnorm - delta;

// ***	 if the function is small enough, accept the current value
//	 of par. also test for the exceptional cases where parl
//	 is 0. or the number of iterations has reached 10.

        if ( fabs(fp) <= p1*delta
             || (parl == 0. && fp <= fp_old && fp_old < 0.)
             || iter == 10 )
            break; // the only exit from this loop

// *** compute the Newton correction.

        for ( j=0; j<n; j++ )
            wa1[j] = diag[ ipvt[j] ] * wa2[ ipvt[j] ] / dxnorm;

        for ( j=0; j<n; j++ )
        {
            wa1[j] = wa1[j]/sdiag[j];
            for ( i=j+1; i<n; i++ )
                wa1[i] -= r[j*ldr+i]*wa1[j];
        }
        temp = lm_enorm( n, wa1);
        parc = fp/delta/temp/temp;

// *** depending on the sign of the function, update parl or paru.

        if (fp > 0)
            parl = MAX(parl, *par);
        else if (fp < 0)
            paru = MIN(paru, *par);
        // the case fp==0 is precluded by the break condition

// *** compute an improved estimate for par.

        *par = MAX(parl, *par + parc);

    }

}


void TLMmin::lm_qrfac(int m, int n, double* a, int pivot, int* ipvt,
           double* rdiag, double* acnorm, double* wa)
{
/*
 *     this subroutine uses householder transformations with column
 *     pivoting (optional) to compute a qr factorization of the
 *     m by n matrix a. that is, qrfac determines an orthogonal
 *     matrix q, a permutation matrix p, and an upper trapezoidal
 *     matrix r with diagonal elements of nonincreasing magnitude,
 *     such that a*p = q*r. the householder transformation for
 *     column k, k = 1,2,...,min(m,n), is of the form
 *
 *			    t
 *	    i - (1/u(k))*u*u
 *
 *     where u has 0.s in the first k-1 positions. the form of
 *     this transformation and the method of pivoting first
 *     appeared in the corresponding linpack subroutine.
 *
 *     parameters:
 *
 *	m is a positive integer input variable set to the number
 *	  of rows of a.
 *
 *	n is a positive integer input variable set to the number
 *	  of columns of a.
 *
 *	a is an m by n array. on input a contains the matrix for
 *	  which the qr factorization is to be computed. on output
 *	  the strict upper trapezoidal part of a contains the strict
 *	  upper trapezoidal part of r, and the lower trapezoidal
 *	  part of a contains a factored form of q (the non-trivial
 *	  elements of the u vectors described above).
 *
 *	pivot is a logical input variable. if pivot is set true,
 *	  then column pivoting is enforced. if pivot is set false,
 *	  then no column pivoting is done.
 *
 *	ipvt is an integer output array of length lipvt. ipvt
 *	  defines the permutation matrix p such that a*p = q*r.
 *	  column j of p is column ipvt(j) of the identity matrix.
 *	  if pivot is false, ipvt is not referenced.
 *
 *	rdiag is an output array of length n which contains the
 *	  diagonal elements of r.
 *
 *	acnorm is an output array of length n which contains the
 *	  norms of the corresponding columns of the input matrix a.
 *	  if this information is not needed, then acnorm can coincide
 *	  with rdiag.
 *
 *	wa is a work array of length n. if pivot is false, then wa
 *	  can coincide with rdiag.
 *
 */
    int i, j, k, kmax, minmn;
    double ajnorm, sum, temp;
    static double p05 = 0.05;

// *** compute the initial column norms and initialize several arrays.

    for ( j=0; j<n; j++ )
    {
	acnorm[j] = lm_enorm(m, &a[j*m]);
	rdiag[j] = acnorm[j];
	wa[j] = rdiag[j];
	if ( pivot )
            ipvt[j] = j;
    }

// *** reduce a to r with householder transformations.

    minmn = MIN(m,n);
    for ( j=0; j<minmn; j++ )
    {
        if ( !pivot ) goto pivot_ok;

// *** bring the column of largest norm into the pivot position.

        kmax = j;
        for ( k=j+1; k<n; k++ )
            if (rdiag[k] > rdiag[kmax])
		kmax = k;
        if (kmax == j) goto pivot_ok; // bug fixed in rel 2.1

        for ( i=0; i<m; i++ )
	{
            temp        = a[j*m+i];
            a[j*m+i]    = a[kmax*m+i];
            a[kmax*m+i] = temp;
	}
        rdiag[kmax] = rdiag[j];
        wa[kmax] = wa[j];
        k = ipvt[j];
        ipvt[j] = ipvt[kmax];
        ipvt[kmax] = k;

    pivot_ok:

// *** compute the Householder transformation to reduce the
//     j-th column of a to a multiple of the j-th unit vector.

        ajnorm = lm_enorm( m-j, &a[j*m+j] );
        if (ajnorm == 0.)
        {
            rdiag[j] = 0;
            continue;
        }

        if (a[j*m+j] < 0.)
            ajnorm = -ajnorm;
        for ( i=j; i<m; i++ )
            a[j*m+i] /= ajnorm;
        a[j*m+j] += 1;

// *** apply the transformation to the remaining columns
//     and update the norms.

        for ( k=j+1; k<n; k++ )
        {
            sum = 0;

            for ( i=j; i<m; i++ )
                sum += a[j*m+i]*a[k*m+i];

            temp = sum/a[j+m*j];

            for ( i=j; i<m; i++ )
                a[k*m+i] -= temp * a[j*m+i];

            if ( pivot && rdiag[k] != 0. )
            {
                temp = a[m*k+j]/rdiag[k];
                temp = MAX( 0., 1-temp*temp );
                rdiag[k] *= sqrt(temp);
                temp = rdiag[k]/wa[k];
                if ( p05*SQR(temp) <= LM_MACHEP )
                {
                    rdiag[k] = lm_enorm( m-j-1, &a[m*k+j+1]);
                    wa[k] = rdiag[k];
                }
            }
        }

	rdiag[j] = -ajnorm;
    }
}


void TLMmin::lm_qrsolv(int n, double* r, int ldr, int* ipvt, double* diag,
              double* qtb, double* x, double* sdiag, double* wa)
{
/*
 *     given an m by n matrix a, an n by n diagonal matrix d,
 *     and an m-vector b, the problem is to determine an x which
 *     solves the system
 *
 *	    a*x = b ,	  d*x = 0 ,
 *
 *     in the least squares sense.
 *
 *     this subroutine completes the solution of the problem
 *     if it is provided with the necessary information from the
 *     qr factorization, with column pivoting, of a. that is, if
 *     a*p = q*r, where p is a permutation matrix, q has orthogonal
 *     columns, and r is an upper triangular matrix with diagonal
 *     elements of nonincreasing magnitude, then qrsolv expects
 *     the full upper triangle of r, the permutation matrix p,
 *     and the first n components of (q transpose)*b. the system
 *     a*x = b, d*x = 0, is then equivalent to
 *
 *		   t	   t
 *	    r*z = q *b ,  p *d*p*z = 0 ,
 *
 *     where x = p*z. if this system does not have full rank,
 *     then a least squares solution is obtained. on output qrsolv
 *     also provides an upper triangular matrix s such that
 *
 *	     t	 t		 t
 *	    p *(a *a + d*d)*p = s *s .
 *
 *     s is computed within qrsolv and may be of separate interest.
 *
 *     parameters
 *
 *	n is a positive integer input variable set to the order of r.
 *
 *	r is an n by n array. on input the full upper triangle
 *	  must contain the full upper triangle of the matrix r.
 *	  on output the full upper triangle is unaltered, and the
 *	  strict lower triangle contains the strict upper triangle
 *	  (transposed) of the upper triangular matrix s.
 *
 *	ldr is a positive integer input variable not less than n
 *	  which specifies the leading dimension of the array r.
 *
 *	ipvt is an integer input array of length n which defines the
 *	  permutation matrix p such that a*p = q*r. column j of p
 *	  is column ipvt(j) of the identity matrix.
 *
 *	diag is an input array of length n which must contain the
 *	  diagonal elements of the matrix d.
 *
 *	qtb is an input array of length n which must contain the first
 *	  n elements of the vector (q transpose)*b.
 *
 *	x is an output array of length n which contains the least
 *	  squares solution of the system a*x = b, d*x = 0.
 *
 *	sdiag is an output array of length n which contains the
 *	  diagonal elements of the upper triangular matrix s.
 *
 *	wa is a work array of length n.
 *
 */
    int i, kk, j, k, nsing;
    double qtbpj, sum, temp;
    double sin, cos, tan, cotan; // these are local variables, not functions
    static double p25 = 0.25;
    static double p5 = 0.5;

// *** copy r and (q transpose)*b to preserve input and initialize s.
//     in particular, save the diagonal elements of r in x.

    for ( j=0; j<n; j++ )
    {
	for ( i=j; i<n; i++ )
            r[j*ldr+i] = r[i*ldr+j];
	x[j] = r[j*ldr+j];
	wa[j] = qtb[j];
    }

// *** eliminate the diagonal matrix d using a givens rotation.

    for ( j=0; j<n; j++ )
    {

// ***	 prepare the row of d to be eliminated, locating the
// 	 diagonal element using p from the qr factorization.

        if (diag[ ipvt[j] ] == 0.)
            goto L90;
        for ( k=j; k<n; k++ )
            sdiag[k] = 0.;
        sdiag[j] = diag[ ipvt[j] ];

// ***	 the transformations to eliminate the row of d
//	 modify only a single element of (q transpose)*b
//	 beyond the first n, which is initially 0..

        qtbpj = 0.;
        for ( k=j; k<n; k++ )
	{

//	    determine a givens rotation which eliminates the
//	    appropriate element in the current row of d.

            if (sdiag[k] == 0.)
		continue;
            kk = k + ldr * k; // <! keep this shorthand !>
            if ( fabs(r[kk]) < fabs(sdiag[k]) )
            {
		cotan = r[kk]/sdiag[k];
		sin = p5/sqrt(p25+p25*SQR(cotan));
		cos = sin*cotan;
            }
            else
            {
		tan = sdiag[k]/r[kk];
		cos = p5/sqrt(p25+p25*SQR(tan));
		sin = cos*tan;
            }

// ***	    compute the modified diagonal element of r and
//	    the modified element of ((q transpose)*b,0).

            r[kk] = cos*r[kk] + sin*sdiag[k];
            temp = cos*wa[k] + sin*qtbpj;
            qtbpj = -sin*wa[k] + cos*qtbpj;
            wa[k] = temp;

// *** accumulate the tranformation in the row of s.

            for ( i=k+1; i<n; i++ )
            {
                temp = cos*r[k*ldr+i] + sin*sdiag[i];
                sdiag[i] = -sin*r[k*ldr+i] + cos*sdiag[i];
                r[k*ldr+i] = temp;
            }
	}
    L90:

// *** store the diagonal element of s and restore
//     the corresponding diagonal element of r.

	sdiag[j] = r[j*ldr+j];
	r[j*ldr+j] = x[j];
    }

// *** solve the triangular system for z. if the system is
//     singular, then obtain a least squares solution.

    nsing = n;
    for ( j=0; j<n; j++ )
    {
	if ( sdiag[j] == 0. && nsing == n )
            nsing = j;
	if (nsing < n)
            wa[j] = 0;
    }

    for ( j=nsing-1; j>=0; j-- )
    {
	sum = 0;
        for ( i=j+1; i<nsing; i++ )
            sum += r[j*ldr+i]*wa[i];
	wa[j] = (wa[j] - sum)/sdiag[j];
    }

// *** permute the components of z back to components of x.

    for ( j=0; j<n; j++ )
	x[ ipvt[j] ] = wa[j];
}

/*
double TLMmin::lm_enorm( int n, double *x )
{
*     given an n-vector x, this function calculates the
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
 *
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
*/
//=================================================================

/*
 * This function computes in C the covariance matrix corresponding to a least
 * squares fit. JtJ is the approximate Hessian at the solution (i.e. J^T*J, where
 * J is the jacobian at the solution), sumsq is the sum of squared residuals
 * (i.e. goodnes of fit) at the solution, m is the number of parameters (variables)
 * and n the number of observations. JtJ can coincide with C.
 *
 * if JtJ is of full rank, C is computed as sumsq/(n-m)*(JtJ)^-1
 * otherwise and if LAPACK is available, C=sumsq/(n-r)*(JtJ)^+
 * where r is JtJ's rank and ^+ denotes the pseudoinverse
 * The diagonal of C is made up from the estimates of the variances
 * of the estimated regression coefficients.
 * See the documentation of routine E04YCF from the NAG fortran lib
 *
 * The function returns the rank of JtJ if successful, 0 on error
 *
 * A and C are mxm
 *
 */

int
  TLMmin::lm_COVAR( double *C, double sumsq, int m, int n)
{
  int i, j, k, l;
  int *idx, maxi=-1;
  double *a, *x, *work, max, sum, tmp;


 // allocate internal memory
 idx = new int[m];
 a = new double[m*m];
 x = new double[m];
 work = new double[m];

 //   compute A = JtJ
 /*for( i=0; i<m; i++)
    for( j=i; j<m; j++)
    {
       tmp = 0;
       for( k=0; k<n; k++ )
        tmp += J[k*m+i] * J[k*m+j];
       a[i*m+j] = a[j*m+i] = tmp;
    }
  */
 //   computed before CVM as JtJ
 for( i=0; i<m*m; i++)
       a[i] = C[i];

 //  computes the inverse of A in C. A and B can coincide

 // compute the LU decomposition of a row permutation of matrix a;
 // the permutation itself is saved in idx[]
 for(i=0; i<m; ++i)
 {
   max=0.0;
   for(j=0; j<m; ++j)
     if((tmp=fabs(a[i*m+j]))>max)
        max=tmp;
   if(max==0.0)
   {
     delete[] idx;
     delete[] a;
     delete[] x;
     delete[] work;

#ifdef IPMGEMPLUGIN
        fprintf(stderr, "Singular matrix A in lm_COVAR()!\n");
        return 0;
#else
       Error( "lm_COVAR", "Singular matrix A."  );
#endif
    }
      work[i]=1.0/max;
 }

 for(j=0; j<m; ++j)
 {
     for(i=0; i<j; ++i)
     {
        sum=a[i*m+j];
	for(k=0; k<i; ++k)
           sum-=a[i*m+k]*a[k*m+j];
        a[i*m+j]=sum;
     }
     max=0.0;
     for(i=j; i<m; ++i)
     {
     	sum=a[i*m+j];
     	for(k=0; k<j; ++k)
          sum-=a[i*m+k]*a[k*m+j];
       	a[i*m+j]=sum;
        if((tmp=work[i]*fabs(sum))>=max)
        {
	   max=tmp;
	   maxi=i;
        }
     }
     if(j!=maxi)
     {
      for(k=0; k<m; ++k)
      {
      	tmp=a[maxi*m+k];
      	a[maxi*m+k]=a[j*m+k];
      	a[j*m+k]=tmp;
      }
      work[maxi]=work[j];
     }
     idx[j]=maxi;
     if(a[j*m+j]==0.0)
       a[j*m+j]=2.22 *1e-16; // DBL_EPSILON;
     if(j!=m-1)
     {
        tmp=1.0/(a[j*m+j]);
        for(i=j+1; i<m; ++i)
          a[i*m+j]*=tmp;
     }
 }

 // The decomposition has now replaced a. Solve the m linear systems using
 // forward and back substitution

  for(l=0; l<m; ++l)
  {
    for(i=0; i<m; ++i)
       x[i]=0.0;
    x[l]=1.0;

    for(i=k=0; i<m; ++i)
    {
       j=idx[i];
       sum=x[j];
       x[j]=x[i];
       if(k!=0)
         for(j=k-1; j<i; ++j)
             sum-=a[i*m+j]*x[j];
       else
        if(sum!=0.0)
           k=i+1;
       x[i]=sum;
    }
    for(i=m-1; i>=0; --i)
    {
      sum=x[i];
      for(j=i+1; j<m; ++j)
        sum-=a[i*m+j]*x[j];
      x[i]=sum/a[i*m+i];
    }

    for(i=0; i<m; ++i)
      C[i*m+l]=x[i];
  }

// free memory
     delete[] idx;
     delete[] a;
     delete[] x;
     delete[] work;

//

  int rnk = m; /* assume full rank */

  // double fact = sumsq/(double)(n-rnk);
  for(i=0; i<m*m; i++)
  {  // C[i] *= sumsq;
     tmp = sqrt(fabs(C[i]));
     C[i] = SIGN(tmp, C[i]);
  }
  return rnk;
}
//====================================================================

// the squares of the following constants shall not under/overflow:
// these values seem good for an x86:
#define LM_SQRT_DWARF 1.e-160
#define LM_SQRT_GIANT 1.e150
// the following values should work on any machine:
// #define LM_SQRT_DWARF 3.834e-20
// #define LM_SQRT_GIANT 1.304e19
#define SQR(x)   (x)*(x)

// This function calculates Euclidean norm of a vector x of length n
// Modified from ??????????
//
double enorm( int n, double *x )
{
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
// sum for intermediate components.
            s2 += xabs*xabs;
            continue;
        }

        if ( xabs >  LM_SQRT_DWARF )
        {
// sum for large components.
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
// sum for small components.
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

// calculation of norm.

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

//--------------------- End of s_lsm_lm.cpp --------------------------

