//-------------------------------------------------------------------
// $Id:$
//
// Declaration of TLMDataType, TLMCalc classes
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

#ifndef _s_lmmin_h_
#define _s_lmmin_h_

#ifdef IPMGEMPLUGIN
  #include <stdio.h>
  #include <fstream>
  using namespace std;
  #include <math.h>
#else
 #include "v_ipnc.h"
#endif

class TLMDataType  // data for the task
// fitting data, function and evaluete function
{
  short info ;   // return status of calculation
  char fType;   // type of fit function
  char eType;   // type of evaluete function

  short m_dat;  // number of data points to use in fitting (<-nQ)
  short tm_d;   // number of arguments in tdat array per one data point  (<-nM(-1))
  short n_par;  // number of parameters to fit  (<-nP)  ?????

   double *tdat;  //  array of data arguments [nQ][nM]
   double *ydat;  //  array of data functions [nQ]

   double *wdat;  // array of weight factors for data  [nQ], optional
  //  *par,       // [nP] to LM fitter: guess parameters; from LM fitter - fitted parameters
   double *cnst_y;// [nQ] internal constants for yi calculatuion

 char * text_function;

protected:

#ifndef IPMGEMPLUGIN
  IPNCalc rpn;      // IPN of fit and evaluate functions

  void rpn_evaluate( double* par, double* fvec );
  void rpn_par_function( int i, double* p  );
  double rpn_function( int i, double* p );


#endif

  void test_sizes();
// evaluate functions
  void  lm_evaluate_default( double* par, double* fvec );

// fit functions
 double my_fit_function( double t, double* p );
 double Guggenheim( double *t, double* p );
 double ThompsonWaldbaum( double *t, double* p );

// get parametres coefficient functions
 void my_par_funct( double t, double* p );
 void parGuggenheim( double *t, double* p );
 void parThompsonWaldbaum( double *t, double* p );


public:

 double  xi2;  // final value of chi2 (quality of fit)

    TLMDataType(   char afType, char aeType,
                   short am_dat,short atm_d, short an_par,
                   double *atdat, double *aydat,
                   double *acnst_y, double *awdat = 0, // not nessassary data
                   char *arpn = 0);          // for math script text
   ~TLMDataType();

    short getM_dat() const
      { return m_dat;  }
    short getN_par() const
      { return n_par;  }
    short getInfo() const
      { return info;  }
    double* getX(int i) const
      { return tdat+(i*tm_d);  }
    double getY(int i) const
      { return ydat[i];  }
    double getWdat(int i) const
      { return wdat[i];  }
    double getXi2() const
      { return xi2;  }

   int   evaluate( double* par, double* fvec );
   double function( int i, double* t, double* p );
   void par_funct(  int i, double* t, double* coef_p );

   void lm_print_default( double* par, double* fvec, double *CVM,
                      int iflag, int iter, int nfev, double norm );

};


enum fit_func_types {
              TEST_FIT = '0',
              TEST_FIT_SVD = '2',
              MATHSCRIPT_FIT = '1',   // using mathscript
// for Dualth mode
              FUN_IPF_R = 'R',   // Redlich-Kister
              FUN_IPF_G = 'G',   // Guggenheim
              FUN_IPF_T = 'T',   // Thompson-Waldbaum
              FUN_IPF_M = 'M',   // Margules
              FUN_IPF_V = 'V',   // Van Laar
              FUN_IPF_B = 'B',   // Bale-Pelton dilute formalism
                    };

enum evl_func_types {
              TEST_EVL = '0',
              MATHSCRIPT_EVL = '1',   // using mathscript
                    };

enum limits_types {
    /* type of lmits */
    LM_NO_LIM = 0, LM_LOWER_LIM =1, LM_UPPER_LIM = 2, LM_BOTH_LIM =3,
                    };

// parameters for calling the high-level interface lmfit
//   ( lmfit.c provides lm_initialize_control which sets default values ):
struct lm_control_type{
        double ftol;       // relative error desired in the sum of squares.
        double xtol;       // relative error between last two approximations.
        double gtol;       // orthogonality desired between fvec and its derivs.
        double epsilon;    // step used to calculate the jacobian.
        double stepbound;  // initial bound to steps in the outer loop.
        double fnorm;      // norm of the residue vector fvec.
        int maxcall;       // maximum number of iterations.
        int nfev;          // actual number of iterations.
        int info;          // status of minimization.

     lm_control_type()
    {
      maxcall = 100;
      epsilon = 1.e-14; //14
      stepbound = 100.;
      ftol = 1.e-14; //14
      xtol = 1.e-14; //14
      gtol = 1.e-14; //14
      info = 0;
      nfev = 0;
   }
};

#ifndef _LMDIF
extern char *lm_infmsg[];
extern char *lm_shortmsg[];
#endif


class TLMmin
{

  TLMDataType *data;
  lm_control_type control;

  short m_dat;  // number of data points to use in fitting (<-nQ)
  short n_par;  // number of parameters to fit  (<-nP)
  double  *par;   // [nP] to LM fitter: guess parameters; from LM fitter - fitted parameters

// limits of par
  double *par_ap;   // sentral point
  double *d_par;  // delta from sentral
  short  *d_type; // type of limits 0-No, 1 -Lower, 2 upper, 3 both

// internal arrays
    double *fvec;
    double *diag;
    double *fjac;
    double *qtf;
    double *wa1;
    double *wa2;
    double *wa3;
    double *wa4;
    int *ipvt;
    double *CVM;
    double chisq;


protected:

void alloc_arrays();
void free_arrays();

// compact high-level interface:
void lm_minimize ( double* par );
double lm_enorm( int, double* );

// low-level interface for full control:
void lm_lmdif( int m, int n, double* x, double* fvec, double ftol, double xtol,
               double gtol, int maxfev, double epsfcn, double* diag, int mode,
               double factor, int *info, int *nfev,
               double* fjac, int* ipvt, double* qtf,
               double* wa1, double* wa2, double* wa3, double* wa4 );

void lm_qrfac( int m, int n, double* a, int pivot, int* ipvt,
               double* rdiag, double* acnorm, double* wa);
void lm_qrsolv(int n, double* r, int ldr, int* ipvt, double* diag,
               double* qtb, double* x, double* sdiag, double* wa);
void lm_lmpar( int n, double* r, int ldr, int* ipvt, double* diag, double* qtb,
               double delta, double* par, double* x, double* sdiag,
               double* wa1, double* wa2);
int lm_COVAR(double *C, double sumsq, int m, int n);
void CheckLimits( double *p );
void ChTol( double& fTol, double& xTol );


public:

    TLMmin( double *par, TLMDataType *aData);
   ~TLMmin();

   void Calc( double *sdpar, double *par_ap=0, double*d_par=0, short *d_type=0 );
};

typedef double fd_type;

#define a(i,j) (A[(i)*n+(j)])
#define u(i,j) (U[(i)*n+(j)])
#define v(i,j) (V[(i)*n+(j)])
#define cvm(i,j) (CVM[(i)*n+(j)])

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define TOL 1.0e-12    // Default value for single precision and variables
                      // scaled to order unity.

class TSVDcalc
{

  TLMDataType *data;
  lm_control_type control;

  short m;  // number of data points to use in fitting (<-nQ)
  short n;  // number of parameters to fit  (<-nP)
  double  *par;   // [nP] to LM fitter: guess parameters; from LM fitter - fitted parameters
                  // or x from line arrays task
  float  *A;    // [m,n] matrix A
  double  *b;    //  [n] vector b

// internal arrays
    double *U;
    double *w;
    double *V;
    double *CVM;
    double chisq;

protected:

void alloc_arrays();
void free_arrays();

void svdGetXmore0( int ii, fd_type *V, fd_type x[]);

// compact high-level interface:
void svdGetX(fd_type *U, fd_type w[], fd_type *V, fd_type b[], fd_type x[]);
fd_type pyt_hag(fd_type a, fd_type b);
int svdGetUWV(fd_type *A, fd_type w[], fd_type *V);
void svdMin(  fd_type a1[], fd_type *U1, fd_type *V1,  fd_type w1[],  fd_type& chisq1 );
void svdStat(fd_type *V, fd_type w[], fd_type *CVM);


public:
    TSVDcalc( double *par, TLMDataType *aData);
    TSVDcalc( short M, short N, float *aA, double *ab, double *aX );
   ~TSVDcalc();

   void CalcMin( double *sdpar ); // return statistics
   int CalcSVD( bool tranp=false );
   void CalcB( bool transp, fd_type *bb );
};

inline
double ROUND_EXP(double x, int ex )
{    double dd = pow( 10., double(ex) );    
     return double( int( (x)*dd+.5 ) ) / dd;
}

#endif //_s_lmmin_h_
