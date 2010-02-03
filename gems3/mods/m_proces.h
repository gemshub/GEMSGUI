//-------------------------------------------------------------------
// $Id: m_proces.h 1184 2009-01-23 12:40:32Z gems $
//
// Declaration of TProcess class, config and calculation functions
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
#ifndef _m_process_h_
#define _m_process_h_

#include "v_mod.h"
#include "v_ipnc.h"
#include "graph.h"
#include "v_module.h"

const int PE_RKLEN = 80;

typedef struct
{ // Description  PROCESS
    char
    //      key[PE_RKLEN],           // Current key
    //mus[MAXMUNAME],        // Identifier of root multisystem <-RMULTS
    //pot[MAXTDPCODE],       // Symbol of minimized thermodynamic potential <-SYSTEM
    //symb[MAXSYSNAME],      // Identifier of Chemical System Definition <-SYSTEM
    //time_[MAXTIME], nchti, // CSD variant number <-SYSTEM
    //psymb[MAXPENAME],      // Identifier of PROCES definition
    //pcode[MAXPECODE],      // PROCES type code {T, G, R, S, P }

    name[MAXFORMULA],      // Name of PROCES simulator definition
    notes[MAXFORMULA],     // Notes
    xNames[MAXAXISNAME], // Abscissa name
    yNames[MAXAXISNAME]; // Ordinate name
 char (*lNam)[MAXGRNAME];    // List of ID of lines on Graph
 char (*lNamE)[MAXGRNAME];   // List of ID of lines of empirical data

 char
    Istat, // PROCES status: 0 or 5 -undefinite; 1or 6-start; 2 or 7-run;
           //    4 or 9 -end  ( > 5 using stepwise mode)
    PsTP,  // Is TPV of system changing in the process (+ -)
    PsBC,  //  Is there a change in bulk composition (+ -)
    PsRT,  // Are time dependent calculation and plotting mode (+ -)
    PsSY,  // Save changed SYSTEM definition records to PDB (+ -)
    PsGT,  // Is this a Sequential Reactor simulation (+ -)
    PsGR,  // Use graphics window (+ -)
    PsUX,  // Do PROCES depend on output equilibrium parameters (pH, Eh etc.)(+-)
    PsIN,  // Is this an inverse titration problem (+ -)
    PsKI,  // Is this a problem with kinetically-restricted DC (+ -)
    PsEqn, // Will IPN-equations be specified in PROCES definition (+ -)
    PsPro, // Is this a sequence of inverse titration points (+ -)

    Pvtm,  // Flags for optional arrays of values: CSD tm (+ - *)
    PvNV,  // MTPARM NV (+ - *)
    PvP,   // pressure P (+ - *)
    PvT,   // temperature TC (+ - *)
    PvV,   // volume V (+ - *)
    PvTau, // time Tau (+ - *)
    PvpXi, // process extent pXi (+ - *)
    PvNu,  // process extent Nu (+ - *)
    PvKin, // kinetic parameters vKin (+ - *)
    PvModc,// empirical parameters modC (+ - *)
    PvR1,  // Use previous EQSTAT result (+) or SIMPLEX initial approximation(+-)
    PvEF   // Use empirical data for graphics
    ;
  short  // N of points controled parameters of process
    // !!  Nnu,  NT, NV, NphH, Npe, R1 - not use; NP - intermal
    Ntim,Nxi,Nnu, // Ntim Number of mass-transport integration points
                  // Nxi number of points to be generated ;
                  // process extent variable(s)
    NP,NT,NV,     // N of points: along P; along T; along V
    NpH, Npe, R1, // N of inverse titration points: along pH; along pe(Eh);reserved
    Mode,    // Mode of points indexation { 0 - 8 }
    NR1,     // Numbers of: EQSTAT keys
    Nmc,     // N of process model coefficients
    Nrea,    // N of explicit reactions (reserved)
    Nrp,     // N of reaction parameters (reserved)
    Nsd,     // N of data source references
    Ntm, dNNV, dNP, dNV, dNT, NTau, NpXi, NNu,
    // Dimensions of arrays: vTm, vNV, vP, vV, vT, vTau, vpXi, vNu
    tmi[3],  // SYSTEM CSD definition #: start, end, step (initial)
    NVi[3],  // MTPARM variant #: start, end, step

    dimEF[2],    // Dimensions of array of empirical data
    dimXY[2],    // Dimensions of data sampler tables: col.1 - N of records;
    axisType[6],  // axis graph type, background(3), graph type, reserved

    *tm,    // Array of tm (SYSTEM CSD #) values [0:Ntm-1]
    *nv;    // Array of NV (MTPARM variant) values [0:NNV-1]

  double       // Units of measuremen get from start  SYSTEM
    Pi[3],    // Pressure P, bar: start, end, increment
    Ti[3],    // Temperature T, C: start, end, increment
    Vi[3],    // Volume of the system (L): start, end, increment
    Taui[3],  // Time (tau), arbitrary units: start, end, increment
    pXii[3],  // Process extent pXi = -log(Xi): start, end, increment
    Nui[3],   // Linear factor of extent Nu: start, end,
    // increment (precision for inverse titrations)
    pHi[3],   // pH values for inverse titration: start, end, increment
    pei[3],   // pe values for inverse titrations: start, end, increment

    // Arrays controled parameters of state
    *P,      // Array of P values [0:NP-1]
    *V,      // Array of V values [0:NV-1]
    *T,      // Array of TC values [0:NT-1]
    // Arrays controled parameters of process
    *Tau,    // Array of Tau (physical time) values [0:NTau-1]
    *pXi,    // Array of pXi (-log process extent) values [0:NpXi-1]
    *Nu,     // Array of Nu values [0:NNu-1]

    *Kin,    // Array of (reaction) kinetic parameters [0:Nrea-1][0:Nrp-1]
    *Modc    // Array of process script parameters?[0:Ntm-1][Nmc]
    ;
  double
     *x0,   // Vector of abscissa dimXY[][1]
     *y0,  // Sampled data array dimXY[][]
     *xE, *yE;         // Input empirical data XE, YE

  float
     size[2][4]; // Graph axis scale for region and fragment

  char
    *Expr,  // Text of process simulator equations
    *gr_expr,     // Text with IPN-expressions for data sampler
    gdkey[GD_RKLEN],   // reserved
    stkey[EQ_RKLEN+10],// SyStat
    tpkey[TP_RKLEN];
  char (*stl)[EQ_RKLEN]; // List of EQSTAT record keys [0:NeMax-1]

  char (*sdref)[V_SD_RKLEN]; // List of SDref keys to data sources
  char (*sdval)[V_SD_VALEN]; // Comments on data sources

  short NeMax,      // Max. N of iterations allowed for inverse titration
    Nst,          // N of completed EQSTAT computatitons
    Loop,         // Switch to continue (1) or finish (0) PROCESS iterations
    i, j,         // i, j index
    c_nrk,        // Current index of EQSTAT in record key list
    c_tm,         // Current Tm - SYSTEM CSD number
    c_NV;         // Current Nv - MTPARM variant number
 double    // Current values to control process
    c_P, c_V, c_T, c_TC,
    c_Tau, c_pXi, c_Xi, c_Nu,
    c_pH, c_pe, c_Eh;

    // work variables
    // IPNCalc rpn;       // IPN of equats of process  -- Expr
    time_t syt, stt, utt, // time to build of current records
    pet, tpt, mut;
 double ccTime;           // Added to control calc.time 15.01.09 DK
 char timep[16], TCp[16], Pp[16], NVp[16], Bnamep[16];

 char *tprn;               // internal bufer
}

PROCESS;

// Current Process
class TProcess : public TCModule
{
    PROCESS pe[1];

    GraphWindow *gd_gr;
    TPlotLine *plot;

    char *text_fmt;
    gstring sd_key;
    gstring filename;


protected:
    IPNCalc rpn[2];      // IPN of process simulator equations&&graph
    int pointShow;

    bool pe_dimValid();
    void pe_initiate();
    void pe_reset();

    void pe_next();
    void pe_qekey();
    void pe_test();
    void pe_text_analyze();
    void proc_titr();
    double f_proc( double x );
    void keyTest( const char *key );
    void _pr_tab( const char *key );
    void CalcEquat();
    void CalcPoint( int nPoint );
    void set_type_flags( char type);


public:

    static TProcess* pm;

    bool userCancel;
//    bool stopCalc;
    bool stepWise;
    bool calcFinished;

    PROCESS *pep;

    TProcess( int nrt );

    const char* GetName() const
    {
        return "Process";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    bool check_input( const char *key, int level=1 );
    gstring   GetKeyofRecord( const char *oldKey, const char *strTitle,
                              int keyType );


    void RecInput( const char *key );
    void MakeQuery();
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void RecordPlot( const char *key );
    bool SaveGraphData( GraphData* graph );

    void CmHelp();
    bool NoSave() const
    { return ( pep->PsSY == S_OFF ); }

   class UserCancelException {};
    void internalCalc();

};

enum pe_statcode {
    /*  types of process */
    P_TITR ='T', P_INV_TITR = 'G', P_REACT = 'R', P_SYST = 'S',
    P_PVT ='P', P_LIP = 'L'
};



#endif //   _m_proces_h
