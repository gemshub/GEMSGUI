//-------------------------------------------------------------------
// Id: gems/mods/m_probe.h  version 2.0.0   2001
// Under construction, to be included in version 3.0 (2003)
// Declaration of TProbe class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
// Copyright (C) 1995-2001 K.Chudnenko, S.Dmytriyeva, D.Kulik
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
#ifndef _m_probe_h_
#define _m_probe_h_

#include "m_param.h"
#include "m_gtdemo.h"

const int PB_RKLEN = 62;

typedef struct
{ // Description  PROBE
    char
    //   stkey1[EQ_RKLEN],  // Parent EQSTAT record key
    //   NUV[MAXNV], nchz,  // PROBE task variant #

    name[MAXFORMULA],      // Full name of PROBE task
    notes[MAXFORMULA],     // Comments
    zond[12],   /* PROBE varying G0, S0, b, T, P, V0(DC), GAMMA,
                         Gex,  V(system) (+ -), reserved, reserved,
                         Use previous EQSTAT result (+/-)  */
    Zexpr,      // Use IPN-equations (+ -)
    pbcalc;     // Flag to continue iterations

    short N,         // N of selected IC
    L,         // N of selected DC
    FI,        // N of selected phases
    FIS,       // N of selected multi-component phases
    Nsd,       // N of data source references
    QT,        // N of PROBE points
    kGN,       // N of PROBE groups using normal distribution law
    kGR,       // N of PROBE groups using uniform distribution law
    kG,        // Total N of PROBE groups kGR+kGN
    Nres1,     // reserved
    NgT,       // Group # for T, 0- no  analyse
    NgP,       // Group # for P, 0- no  analyse
    NgV,       // Group # for V, 0- no  analyse

    nI,        // size of vector statistic   (internal)
    nF,        // part of vector statistic to get phases (internal)
    Nres4,
    *L1,         // N of IC selected in each phase [FI]
    *nnf,        // RMULTS-indices of selected IC [N]
    *llf,        // RMULTS-indices of selected DC [L]
    *kkf,        // RMULTS-indices of selected phases [FI]
    *NgLg,    // Group indices for G0 variation [L], 0 - no zonded
    *NgLs,    // Group indices for S0 variation [L], 0 - no zonded
    *NgLv,    // Group indices for V0 variation [L], 0 - no zonded
    *NgLgam,  // Group indices for GAMMA variation [L], 0 - no zonded
    *NgLgex,  // Group indices for Gex variation [L], 0 - no zonded
    *NgN,     // Group indices for b variation [N], 0 - no zonded
    *iopt      // Vector of phase selection
    ;
    double *Bs;    // Deteministic values of b [N]

    float  T,       // Deterministic value of T
    IntT,    // uncertainty half-interval of T
    P,       // Deterministic value of P
    IntP,    // uncertainty half-interval of P
    V,       // Deterministic value of V
    IntV,    // uncertainty half-interval of V
    *IntLg,    // Uncertainty half-interval of G0 [L]
    *IntLs,    // Uncertainty half-interval of S [L]
    *IntLv,    // Uncertainty half-interval of V [L]
    *IntLgam,  // Uncertainty half-interval of GAMMA [L]
    *IntLgex,  // Uncertainty half-interval of Gex [L]
    *IntN,     // Uncertainty half-interval of b [N]
    *Gs,       // Deteministic values of G0 [L]
    *Ss,       // Deteministic values of S0 [L]
    *Vs,       // Deteministic values of V0 [L]
    *GAMs,     // Deteministic values of GAMMA [L]
    *GEXs,     // Deteministic values of Gex [L]
    *OVR;      // Pseudo-random numbers of uniform distribution [kGR]

    char
    *ZPg,      // Code of distribution law for groups [kG]
    // 0-uniform distribution law, 1-normal distribution law
    *Expr,     // Text with IPN equations
    (*SGp)[MAXPHNAME],    //List of PROBE group names [kG]
    (*stl)[EQ_RKLEN],     // List of generated EQSTAT records [QT]
    (*sdref)[V_SD_RKLEN], // List of SDref keys to data sources [Nsd]
    (*sdval)[V_SD_VALEN],  // Comments to data sources [Nsd]

    /* work arrays and parametres  */
    (*SBp)[MAXICNAME+MAXSYMB], // Compressed list of IC names [N]
    (*SMp)[MAXDCNAME],         // Compressed list of DC names [L]
    (*SFp)[MAXPHNAME],         // Compressed list of phase names [FI]
    //       sykey[SY_RKLEN],           // current key
    //       tpkey[TP_RKLEN],
    stkey[EQ_RKLEN+10];  // current SysEq key
    short
    nMV,       // Current index of point in AUI procedure
    Loop,      // Flag of finishing AUI iterations
    c_nrk,     // Current # of EQSTAT rkey in the list
    c_tm,      // Current # of created SYSTEM record
    c_NV;      // Current # of created MTPARM record

    char timep[16], TCp[16], Pp[16], NVp[16], Bnamep[16];
    char *tprn;               // internal
}

PROBE;

// Current Probe
class TProbe : public TCModule
{
    PROBE pr[1];

protected:

    //IPNCalc rpn;       // IPN of equats of process  -- Expr
    void keyTest( const char *key );
    short get_simple( short oldSimp );
    bool is_simple( short  Simp );
    float UnsG( float x, float dx );
    void pr_init();
    void BELOV();
    int WL(int i,int j,int QT);
    float PNTBEL(int J,int QT,int OV);
    float PNTNOR(int I,int J,int QT);
    void NexT( int J, int iT, int iP );
    short  pb_min( int N, double *x);
//    void  _pr_tab( const char *key );

public:

    static TProbe* pm;

    PROBE *prp;

    TProbe( int nrt );

    const char* GetName() const
    {
        return "Probe";
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
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void RecordPrint( const char *key=0);
    void CmHelp();  

    //changes
    void InsertChanges( TIArray<CompItem>& aIComp,
                        TIArray<CompItem>& aPhase,  TIArray<CompItem>&aDComp );
    void newSizeifChange();

    //STATISTIC
    void paragen( int N, GDSTAT gst );
    void pb_matr( int QT, GDSTAT gst );

    short GetnI() const
    {
        return prp->nI;
    }
    short GetnF() const
    {
        return prp->nF;
    }
    short GetQT() const
    {
        return prp->QT;
    }
    short *GetIopt() const
    {
        return prp->iopt;
    }

};

typedef enum {  // types of  PROBE groups using distribution law
    PRB_UNIFORM  = 'u',     /* uniform */
    PRB_NORMAL   = 'n'      /* normal */
} PRB_CLASSES;


#endif  // _m_probe_h

