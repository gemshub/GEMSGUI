//-------------------------------------------------------------------
// $Id: s_lsm_fun.cpp 700 2006-04-18 06:52:07Z gems $
//
// Implamentation of TLMDataType class
//
// Define Built in functions for task of minimization
//
// Written by S.Dmytriyeva
// Copyright (C) 2005-2006  D.Kulik, S.Dmytriyeva
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#ifndef IPMGEMPLUGIN

 #include "v_object.h"
 #include "v_mod.h"
 #include "visor.h"

#endif

#include <cmath>
#include "s_lsm.h"

TLMDataType::TLMDataType(   char afType, char aeType,
                   short am_dat,short atm_d, short an_par,
                   double *atdat, double *aydat,
                   double *acnst_y, double *awdat, // not nessassary data
                   char *arpn ): // for math script
   fType(afType), eType(aeType),
   m_dat(am_dat), tm_d(atm_d),  n_par(an_par),
   tdat(atdat),   ydat(aydat), cnst_y(acnst_y),
   wdat(awdat),   text_function(arpn)
{
    info=0;
    xi2=0.0;
    test_sizes();
}

TLMDataType::~TLMDataType()
{
#ifndef IPMGEMPLUGIN

  if( fType == MATHSCRIPT_FIT || eType == MATHSCRIPT_EVL )
  {
    // setup  internal objects  for 0
    aObj[ o_lms_yexp ].SetPtr( NULL );
    aObj[ o_lms_yexp ].SetDim( 0, 1 );
    aObj[ o_lms_tx ].SetPtr( NULL );
    aObj[ o_lms_tx ].SetDim( 0, tm_d );
    aObj[ o_lms_wexp ].SetPtr( NULL );
    aObj[ o_lms_wexp ].SetDim( 0, 1 );
    aObj[ o_lms_para ].SetPtr( NULL );
    aObj[ o_lms_para ].SetDim( 0, 1 );
   // free internal arrays
   //     aObj[ o_lms_delta ].Free();
   //    aObj[ o_lms_yfit  ].Free();
   //    aObj[ o_lms_paf ].Free();
   //     o_lms_jp, o_lms_kp, o_lms_itx
  }
#endif
}

int   TLMDataType::evaluate( double* par, double* fvec )
{
  switch( eType)
  {
     case TEST_EVL: lm_evaluate_default( par, fvec );
                    break;
#ifndef IPMGEMPLUGIN
     case MATHSCRIPT_EVL:   // using mathscript
                    rpn_evaluate( par, fvec );
                    break;
#endif
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
            {
               info = -1;
#ifndef IPMGEMPLUGIN
       Error( "Test",
          "Built in function is for 1 end members and 3 parameters only." );
#else
       printf( "Built in function is for 1 end members and 3 parameters only." );
#endif
             }
             break;

     case FUN_IPF_R:
     case FUN_IPF_G:
            if( tm_d != 2 && ( n_par > 6 || n_par <=0 ))
            {
               info = -1;
#ifndef IPMGEMPLUGIN
       Error( "Redlich-Kister_or_Guggenheim",
       "Built in function is for 2 end members only and upto 6 parameters." );
#else
       printf( "Built in function is for 2 end members only and upto 6 parameters." );
#endif
          }
            break;
     case FUN_IPF_T:
            if( tm_d != 2 &&  n_par != 2 )
          {
            info = -1;
#ifndef IPMGEMPLUGIN
       Error( "Thompson-Waldbaum",
       "Built in function is for 2 end members and 2 parameters only." );
#else
       printf( "Built in function is for 2 end members and 2 parameters only."  );
#endif
           }
            break;
     case MATHSCRIPT_FIT:   // using mathscript
#ifdef IPMGEMPLUGIN
           info = -1;
#endif
                    break;
     case FUN_IPF_M:   // Margules
     case FUN_IPF_V:   // Van Laar
     case FUN_IPF_B:   // Bale-Pelton dilute formalism
     default:
                   break;
     }

#ifndef IPMGEMPLUGIN
  if( fType == MATHSCRIPT_FIT || eType == MATHSCRIPT_EVL )
  {
    // setup  internal objects
    aObj[ o_lms_yexp ].SetPtr( ydat );
    aObj[ o_lms_yexp ].SetDim( m_dat, 1 );
    aObj[ o_lms_tx ].SetPtr( tdat );
    aObj[ o_lms_tx ].SetDim( m_dat, tm_d );
    aObj[ o_lms_wexp ].SetPtr( wdat );
    aObj[ o_lms_wexp ].SetDim( m_dat, 1 );
    aObj[ o_lms_para ].SetPtr( 0 );
    aObj[ o_lms_para ].SetDim( n_par, 1 );
   // allocate internal arrays
    aObj[ o_lms_delta ].Alloc( m_dat, 1, D_ );
    aObj[ o_lms_yfit  ].Alloc( m_dat, 1, D_ );
    aObj[ o_lms_paf  ].Alloc( n_par, 1, D_ );
   //     o_lms_jp, o_lms_kp, o_lms_itx, o_lms_para
   // translate equation after setup sizes
   rpn.GetEquat( text_function );
   }
#endif
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
#ifndef IPMGEMPLUGIN
     case MATHSCRIPT_FIT:   // using mathscript
                      r = rpn_function( i, p );
                      break;
#endif
     case FUN_IPF_M:   // Margules
     case FUN_IPF_V:   // Van Laar
     case FUN_IPF_B:   // Bale-Pelton dilute formalism


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
#ifndef IPMGEMPLUGIN
     case MATHSCRIPT_FIT:   // using mathscript
                       rpn_par_function( i, coef_p  );
                       break;
#endif
     case FUN_IPF_M:   // Margules
     case FUN_IPF_V:   // Van Laar
     case FUN_IPF_B:   // Bale-Pelton dilute formalism
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
            fvec[i] = (ydat[i] - function(i, getX(i), par))*sqrt(wdat[i]);
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
  {
	  rez += p[r] * pow( (t[0]-t[1]), r);
  }
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
    p[r]  = t[0]* t[1] * pow( (t[0]-t[1]), (double)r);
}

// Thompson-Waldbaum
void TLMDataType::parThompsonWaldbaum( double *t, double* p )
{
  // test n_par == 2 and tm_d = 2
  p[0] = t[0]*t[1]*t[1];
  p[1] = t[0]*t[1]*t[0];
}

#ifndef IPMGEMPLUGIN


//------------------------------------------------------------------
// Working with mathscripts

double TLMDataType::rpn_function( int i, double* p )
{
    short ii=(short)i;
//     o_lms_jp, o_lms_kp, o_lms_itx, o_lms_para
    aObj[ o_lms_para ].SetPtr( p );
    aObj[ o_lms_jp ].SetPtr( &ii );
    rpn.CalcEquat();
    double ret = aObj[ o_lms_yfit ].Get( ii, 0 );
    return ret;
}

//get parametres coefficient functions
void
 TLMDataType::rpn_par_function( int i, double* p  )
{
    short ii = (short)i;
    aObj[ o_lms_jp ].SetPtr( &ii );  // current line in x
    rpn.CalcEquat();
    for( int j=0; j<n_par; j++)
     p[j] = aObj[ o_lms_paf ].Get( j, 0);}


void TLMDataType::rpn_evaluate( double* par, double* fvec )
{
  short ii;

    aObj[ o_lms_para ].SetPtr( par );
    aObj[ o_lms_jp ].SetPtr( &ii );  // current line in X


    for ( ii=0; ii<m_dat; ii++)
    {
         rpn.CalcEquat();
         fvec[ii] = aObj[ o_lms_delta ].Get(ii, 0);
    }
}


#endif

//------------------------------------------------------------------
// for comparing only

void TLMDataType::lm_print_default( double* par, double* /*fvec*/, double *CVM,
                      int iflag, int iter, int nfev, double norm  )
/*
 *       data  : for soft control of printout behaviour, add control
 *                 variables to the data struct
 *       iflag : 0 (init) 1 (outer loop) 2(inner loop) -1(terminated)
 *       iter  : outer loop counter
 *       nfev  : number of calls to *evaluate
 */
{
    gstring fname = pVisor->userGEMDir();
            fname += "fit_func.out";
    double f, y, *t;
    int i, j;

    fstream f_out(fname.c_str(), ios::out|ios::app  );
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
    printf ( " => norm: %12g\n", norm *lm_enorm( m_dat, fvec )* );
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
    f_out << "cvm = " << endl;
    for( i=0; i<n_par; i++ )
    {  for( j=0; j<n_par; j++ )
       f_out << CVM[i*n_par+j] << " ";
      f_out << endl;
    }
    f_out << endl;
    f_out << endl;
  }

 }

//--------------------- End of s_lsm_fun.cpp --------------------------

