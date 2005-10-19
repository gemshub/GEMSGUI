//-------------------------------------------------------------------
// $Id:$
//
// Implamentation of TLMDataType class
//
// Solves or minimizes the sum of squares of m nonlinear
// functions of n variables.
//
// From public domain Fortran version
// of Argonne National Laboratories MINPACK
//     argonne national laboratory. minpack project. march 1980.
//     burton s. garbow, kenneth e. hillstrom, jorge j. more
// C translation by Steve Moshier
// Joachim Wuttke converted the source into C++ compatible ANSI style
// and provided a simplified interface
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2005  D.Kulik, S.Dmytriyeva
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
//

#include "s_lsm.h"

    TLMDataType::TLMDataType(   char afType, char aeType,
                   short am_dat,short atm_d, short an_par,
                   double *atdat, double *aydat,
                   double *acnst_y, double *awdat, // not nessassary data
                   double *awpar,
                   short *a_t):          // for math script object
   fType(afType), eType(aeType),
   m_dat(am_dat), tm_d(atm_d),  n_par(an_par),
   tdat(atdat),   ydat(aydat), cnst_y(acnst_y),
   wdat(awdat),   wpar(awpar), current_jm(a_t)
{
    info=0;
    xi2=0.0;
    test_sizes();
}

int   TLMDataType::evaluate( double* par, double* fvec )
{
  switch( eType)
  {
     case TEST_EVL: lm_evaluate_default( par, fvec );
                    break;
     case MATHSCRIPT_EVL:   // using mathscript
     default:
                   break;
 }
  return  info;
}

void TLMDataType::test_sizes()
{
  switch( fType)
  {
     case TEST_FIT:
     case TEST_FIT_SVD:
            if( tm_d != 1 &&  n_par != 3 )
       Error( "Test",
          "Built in function is for 1 end members and 3 parameters only." );
                    break;
     case FUN_IPF_R:
     case FUN_IPF_G:
                      if( tm_d != 2 && ( n_par > 6 || n_par <=0 ))
       Error( "Redlich-Kister_or_Guggenheim",
       "Built in function is for 2 end members only and upto 6 parameters." );
                       break;
     case FUN_IPF_T:
            if( tm_d != 2 &&  n_par != 2 )
       Error( "Thompson-Waldbaum",
       "Built in function is for 2 end members and 2 parameters only." );
                       break;
     case FUN_IPF_M:   // Margules
     case FUN_IPF_V:   // Van Laar
     case FUN_IPF_B:   // Bale-Pelton dilute formalism

     case MATHSCRIPT_FIT:   // using mathscript
     default:
                   break;
 }
}

double TLMDataType::function( int i, double* t, double* p )
{
  double r= 1.;
  switch( fType)
  {
     case TEST_FIT: r = my_fit_function( t[0], p );
                    break;
     case TEST_FIT_SVD: r = p[0] + p[1]*t[0] + p[2]*t[0]*t[0];
                    break;
     case FUN_IPF_R:   // Redlich-Kister
                       r = cnst_y[i];
     case FUN_IPF_G:   // Guggenheim
                       r *= Guggenheim( t, p );
                       break;
     case FUN_IPF_T:   // Thompson-Waldbaum
                       r = ThompsonWaldbaum( t, p );
                       break;
     case FUN_IPF_M:   // Margules
     case FUN_IPF_V:   // Van Laar
     case FUN_IPF_B:   // Bale-Pelton dilute formalism

     case MATHSCRIPT_FIT:   // using mathscript
     default:
                   break;
 }
 return r;
}


/*
 Calculate the coefficients of parametres
 returns the n_par basis function evaluated at t.
*/
void TLMDataType::par_funct( int i, double* t, double* coef_p )
{
  switch( fType)
  {
     case TEST_FIT_SVD:
     case TEST_FIT: my_par_funct( t[0], coef_p );
                    break;
     case FUN_IPF_R:   // Redlich-Kister
                       parGuggenheim( t, coef_p );
                       coef_p[0] *= cnst_y[i];
                       coef_p[1] *= cnst_y[i];
                       break;
     case FUN_IPF_G:   // Guggenheim
                       parGuggenheim( t, coef_p );
                       break;
     case FUN_IPF_T:   // Thompson-Waldbaum
                       parThompsonWaldbaum( t, coef_p );
                       break;
     case FUN_IPF_M:   // Margules
     case FUN_IPF_V:   // Van Laar
     case FUN_IPF_B:   // Bale-Pelton dilute formalism

     case MATHSCRIPT_FIT:   // using mathscript
     default:
                   break;
 }
}

//--------------------------------------------------------
//  evaluate functions
/*
 * This file contains default implementation of the evaluate and printout
 * routines. In most cases, customization of lmfit can be done by modifying
 * these two routines. Either modify them here, or copy and rename them.
 */

void TLMDataType::lm_evaluate_default( double* par, double* fvec )
/*
 *	par is an input array. At the end of the minimization, it contains
 *        the approximate solution vector.
 *
 *	fvec is an output array of length m_dat which contains the function
 *        values the square sum of which ought to be minimized.
 *
 */
{
    for (short i=0; i<m_dat; i++)
            fvec[i] = ydat[i] - function(i, getX(i), par);
    /* if <parameters drifted away> { info = -1; } */
}

//--------------------------------------------------------
//  fit functions

// this test program demonstrates the least-squares standard task:
// optimize p to fit some data y(t) by my_fit_function(t;p).

double TLMDataType::my_fit_function( double t, double* p )
{
    return ( p[0]*t + (1-p[0]+p[1]+p[2])*t*t ) /
        ( 1 + p[1]*t + p[2]*t*t );
}

// Redlich-Kister and Guggenheim
double TLMDataType::Guggenheim( double *t, double* p )
{
  // test tm_d = 2
  double rez = 0;
  for( int r=0; r<n_par; r++)
    rez += p[r] * pow( (t[0]-t[1]), r);
  rez *= t[0]*t[1];
  return rez;
}

// Thompson-Waldbaum
double TLMDataType::ThompsonWaldbaum( double *t, double* p )
{
  // test n_par == 2 and tm_d = 2
  double rez = p[0]*t[1]+p[1]*t[0];
  rez *= t[0]*t[1];
  return rez;
}

//------------------------------------------------------------------
//get parametres coefficient functions
void
 TLMDataType::my_par_funct( double t, double* p  )
{
    p[0]=1.0;
    for( int j=1;j<n_par;j++)
       p[j]=p[j-1]*t;
}

// Redlich-Kister and Guggenheim
void TLMDataType::parGuggenheim( double *t, double* p )
{
  // test tm_d = 2
  for( int r=0; r<n_par; r++)
    p[r]  = t[0]* t[1] * pow( (t[0]-t[1]), r);
}

// Thompson-Waldbaum
void TLMDataType::parThompsonWaldbaum( double *t, double* p )
{
  // test n_par == 2 and tm_d = 2
  p[0] = t[0]*t[1]*t[1];
  p[1] = t[0]*t[1]*t[0];
}

//------------------------------------------------------------------
// for comparing only

void TLMDataType::lm_print_default( double* par, double* fvec,
                      int iflag, int iter, int nfev, double norm  )
/*
 *       data  : for soft control of printout behaviour, add control
 *                 variables to the data struct
 *       iflag : 0 (init) 1 (outer loop) 2(inner loop) -1(terminated)
 *       iter  : outer loop counter
 *       nfev  : number of calls to *evaluate
 */
{
    double f, y, *t;
    int i;

 fstream f_out("fit_func.out", ios::app  );
 if( !f_out.good() )
   return;

 /*   if (iflag==2) {
        printf ("trying step in gradient direction\n");
    } else if (iflag==1) {
        printf ("determining gradient (iteration %d)\n", iter);
    } else if (iflag==0) {
        printf ("starting minimization\n");
    } else if (iflag==-1) {
        printf ("terminated after %d evaluations\n", nfev);
    }

    printf( "  par: " );
    for( i=0; i<n_par; ++i )
        printf( " %12g", par[i] );
    printf ( " => norm: %12g\n", norm /*lm_enorm( m_dat, fvec )* );
 */
 if ( iflag == -1 ) {

    f_out << "  par: " ;
    for( i=0; i<n_par; ++i )
        f_out <<  par[i] << "    " ;
    f_out << " => norm: " << norm << endl;

    f_out << "  fitting data as follows:" << endl;
        for( i=0; i<m_dat; ++i ) {
            t = getX(i);
            y = getY(i);
            f = function( i, t, par );
            f_out << "  t[ " << i << " ]= " << t[0] << "  y= ";
            f_out <<  y << " fit = " << f << " residue =  " << (y-f) << endl;
        }

    f_out << endl;
    f_out << " status: " << lm_shortmsg[iter];
    f_out  << " after " << nfev << " iterations" << endl;
    f_out << endl;
    f_out << endl;
   }
   
 }

//--------------------- End of s_lmeval.cpp ---------------------------

