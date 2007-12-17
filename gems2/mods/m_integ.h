//-------------------------------------------------------------------
// $Id$
//
// Declaration of TInteg class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#ifndef _nteg_h_
#define _nteg_h_

#include "v_module.h"
#include "v_ipnc.h"

const int
MAXITER = 800, // max number steps
          MAXINTEGNAME = 70,
                         INT_BLOK =80;

// The task solution of system equations
class TInteg : public TCModule
{
    char keywd[24];             // task key

    // inital parameters of task
    char name[2][MAXINTEGNAME]; // task name
    int  Nequ;      //  number equations in system
    double x_bg,    //  span of integration
    x_end,
    step_bg, //  inital value of step
    Eps;     //  epsilon of solution
    double *param;  // work paramters of task
    double * y_bg;  // inital values
    char * TxtEqu;  // text of equations

    // working paraters
    double step;   // current step
    bool FlagGraf; // grafical or text mode
    int  MaxIter,  // max number of iterations
    nfcn,      // number of functional estimates
    nstep,     // number of steps
    naccept,   // number of permissible steps
    nrejct;    // number of unpermissible steps
    double arg_x,    // current value of argument
    * val_y,    // current value of functins
    * valdy;    // current value of differential
    int  Bsize;    // size of allx, ally, allst, allpr
    double  * allx,
    * ally,
    *allst;
    short   *allpr;

    //
    IPNCalc rpn;

protected:

    void Solut( int ix, int id, int it, double *t1, double *t2, double t3 );
    void MIDEX( int j, double t, double h, int ix, int id, int it );
    void PR( int Ni,int pr, double x, double step, double *y );
    void INTEG( int iy, int id, int ix, double eps,
                double& step, double t_begin, double t_end );

public:

    static TInteg* pm;

    TInteg( int nrt );
    const char* GetName() const
    {
        return "Integ";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    void RecInput( const char *key );
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void RecordPlot( const char */*key*/ );

    void CmHelp();
 //  virtual gstring  GetKeyofRecord( const char *oldKey, const char *strTitle,
 //      int keyType );

};

#endif  // _integ_h

outField TGEM2MT_static_fields[19] =  {
 { "Mode", 1,0 },
 { "PvFDL", 0,0 },  // PvPGD, PvFDL, PvSFL
 { "PvGrid", 0,0 },
 { "Size" , 1,0 },
 { "nFD" , 0,0 },
 { "nPG" , 0,0 },
 { "nSFD" , 0,0 },
 { "MaxSteps", 1,0 },
 { "nPTypes", 0,0 },
 { "nProps", 0,0 },
 { "Tau", 1,0 },
 { "LSize", 0,0 },
 { "fVel", 1,0 },
 { "cLen", 1,0 },
 { "tf", 1,0 },
 { "cdv", 1,0 },
 { "cez", 1,0 },
 { "al_in", 1,0 },
 { "Dif_in", 1,0 }
 };

outField TGEM2MT_dynamic_fields[3] =  {
 { "DiCp", 1, 0 },
 { "HydP", 1, 0 },
 { "NPmean", 1, 0 },
 { "NPmin", 1, 0 },
 { "NPmax", 1, 0 },
 { "mGrid", 1, 0 }
 };

