//-------------------------------------------------------------------
// Id: gems/mods/m_syseq.h  version 2.0.0   2001
//
// Declaration of TSysEq class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010904
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik
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
#ifndef _m_syseq_h_
#define _m_syseq_h_
#include "m_param.h"

typedef struct
{ /* Packed system state description */
    char
    PhmKey[EQ_RKLEN],//Key of EQSTATe record to supply phases for b vector calculation
    name[MAXFORMULA],// Full name of this CSD definition
    notes[MAXFORMULA],// Comments
    switches[40],    /* SYSTEM switches line */
    Px[14];         /* Switches of profile index vectors */

    short DM[24]; /* Fact dimensions of system */
    float PPm[20];  // Parametres of System at all
    short /* Profile indices for non-zero increments to bulk composition */
    *nnc, /* IC     on  in  BI[]      [sy.Na]  */
    *llc, /* COMPOS on  in  XeA[]     [sy.La]  */
    *phc, /* PHASE  on  in  Phm[]     [sy.Fib] */
    *dcc, /* DC     on  in  XeD[]     [sy.Lb]  */

    /* Profile indices for configuration of components and phases */
    *nnf, /* IC    ON   in B[], system [sy.N]    */
    *phf, /* PHASE OFF  in system  [mu.FI-sy.Fi] */
    *dcf, /* DC    OFF  in system  [mu.L-sy.L]   */
    *pha, /* PHASE ON   adsorption EDL data [sy.Fia] */
    *dca, /* DC    ON   adsorption SAT data [sy.Lsor] */

    /* Profile indices for kinetic and metastability constraints */
    *phk, /* PHASE ON  in PUL[]/PLL[]     [sy.Fik] */
    *dck, /* DC    ON  in DUL[]/DLL[]     [sy.Lk]  */
    *dce, /* DC    ON  in GEX[], lnGmf[]  [sy.Le]  */
    *phe; /* PHASE ON  in YOF[], Aalp[]   [sy.Fie] */

    /* Dynamic data - packed for non-defaults DEFAULT: off */
    /* Non-zero increments to bulk composition */
    char
    *Acl,  // COMPOS selection switches { + * - } [0:mu.La-1]
    //  Scales of elements
    *BIun,  // Scales of IC quantity/concentration (bi_)[sy.Na]
    *XDun,  //Scales of DC quantity/concentration (xd_, dll_)[sy.Lb]
    *XAun,  // Scales of COMPOS quantity/concentration (xa_)[sy.La]
    *XPun   // Scales of phase quantity/concentration ( xp_, pll_[sy.Fib]
    ;
    double
    *BI,   // IC quantity/concentration to calculate  b vector [sy.Na]
    *XeA,  // COMPOS quanity/concentration to calculate  b vector [sy.La]
    *XeD,  // DC quantity/concentration to calculate  b vector [sy.Lb]
    *Phm, //PHASE(equilibrium) quantity/concentration to calculate b vector[sy.Fib]
    /* Configuration of components and phases
         IC: Default: OFF;   DC: Default ON;  PHASE: Default: ON; */
    *B; /* Non-zero elements of vector b    [sy.N] */
    float
    *delB, /* Vector b uncertainty (if present)[sy.N] */
    /* Additional constraints:  default OFF */
    *Pparc, // DC partial pressures, bar [sy.Le]
    *GEX,// Excess free energy increment for DC, J/mole [sy.Le]
    *lnGmf, // ln of initial DC activity coefficients [sy.Le]
    /* Phases */
    *Aalp,  // Specific surface area of dispersed phases, m2/g [sy.Fie]
    *YOF,   // Phase metastability parameter, J/g [sy.Fie]
    (*Sigm)[2],//Specific surface energy of solid-aqueous (gas) interface, J/m2 [sy.Fie][2]
    (*Xr0h0)[2],//Parameters r0 and h0 of particles (pores at r0<0), nm reserved [sy.Fie][2]
    (*XEpsC)[2];// Dielectric constant & conductivity of phase carrier, reserved [sy.Fie][2]
    /* Kinetic constraints: default OFF */
    char
    *RLC,  /* type of non-default constraints on x[j]  [sy.Lk] */
    *RSC,  /* scale codes for non-def. constr. on x[j]  [sy.Lk] */
    *RFLC, /* type of non-default constraints on XF[k] [sy.Fik] */
    *RFSC; /* scale codes for non-def. constr. on XF[k] [sy.Fik] */
    /* see file S_CLASS.H for codes */
    float
    /*VG*/  *DUL,// Vector of upper DC restrictions to x_j at eqstate (res)[sy.Lk]
    /*NG*/  *DLL,// Vector of lower DC restrictions to x_j at eqstate [sy.Lk]
    *PUL,//Vector of upper restrictions to phases X_a at eqstate(res.)[sy.Fik]
    *PLL;//Vector of lower restrictions to phases X_a at eqstate(res)[sy.Fik]
    /* Adsorption / EDL models     default OFF */
    /* DC parameters */
    char (*SATC)[2]; /* Classifier of methods of SAT calculation [sy.Lsor] */
    /* and allocation of sur DC to carrier components */
    float
    *MaSdj; /* Max surface DC density for non-compet.SAT 1/nm2 [sy.Lsor] */
    /* Phase parameters */
    char
    (*SCMT)[MNST]; /* Classifier of EDL models for surface types [sy.Fia][6] */
    float
    (*Nfsp)[MNST],// Fraction of surface types relative to carrier components [sy.Fia][6]
    (*MaSdt)[MNST],// Max.total reactive species density 1/nm2 [sy.Fia][6]
    (*XcapA)[MNST],// Inner-layer capacitance density (TLM,CCM) for surface types, F/m2 [sy.Fia][6]
    (*XcapB)[MNST],// Outer-layer capacitance sensity (TLM) for surface types, F/m2[sy.Fia][6]
    (*XcapF)[MNST], // Min.total reactive species density 1/nm2 [sy.Fia][6]
    (*XfIEC)[MNST], // Fix surf charge density or IEC mkeq/g    [sy.Fia][6]
    (*Xlam)[MNST];  // Factor of discretnes IDL 0 < 1 - in PHASE  [sy.Fia][6] */
    char
    (*sdref)[V_SD_RKLEN], /* List of SDref keys to sources of data [sy.Nsd] */
    (*sdval)[V_SD_VALEN]; /* Comments to data sources [sy.Nsd] */
}
SYSPACK;

typedef struct
{ /* Packed equlibrium state description */
    short
    N,    // N of IC at equilibrium state
    L,    // N of DC at equilibrium state
    Fi,   // N of phases at equilibrium state
    Fis,  // N of multi-component phases at equilibrium state
    itIPM, // N of IPM iterations performed
    itPar, // N of parametric problem iterations (reserved)
    Psmt,  /* Switch of MTRAN data (0 off, 1 on) */
    Flags; /* work flags if Nres==1, record calculated Equstat */

    float
    P,T,V,S,H,UU, /* Values of system state variables: P (pressure), bar
                    T (temperature), K;  V (volume), cm3; S (entropy), J/K (reserv);
                            H (enthalpy), J (reser); G (Gibbs energy), J/RT */
    PCI,          // Value of Dikin's criterion upon convergence of IPM
    ParE,         // Ionic strength in aqueous phase, molal
    Scale,        /* Scale factor for B and V */
    Res1;         /* reserved */
    short
    *nnf, // RMULTS-indices of IC [N]
    *llf, // RMULTS-indices of DC [L]
    *phf; /* PROFILE indices of multicomp. phases in eqstate [Fis] */
    /* Optional at Psmt==1 */
    double
    *B,  // B ‚b vector of bulk chemical composition, moles(packed)[N][0]
    *U,  // U Dual solution u for IC (mole/mole), packed           [N][1]
    *Y, // Prime solution x for DC (moles), packed [L][0]
    *lnGam, //ln of DC activity coefficients, packed [L] [1]
    /* Arrays for phases and mass transport (optional at Psmt==1) */
    *Ba,     /* Bulk compos. of multicomp. phases     [Fis][N] */
    (*VM)[5],/* Va Volumes of multicomp. phases cm3   [Fis][0] */
    /* Ma Masses of multicomp. phases, g          [1] */
    /* MaC Mass of carriers of multicomp.ph.,g    [2] */
    /* Cpa Heat capacity of multicomp.ph. J/K/g   [3] */
    /* Enthalpy of multicomp. phases J/g          [4] */
    (*DIC)[2]; /* Diffusion coeffs. of IC in aq fluid   [N][0] */
    /*                         in gas phase     [1] */
    /* Work arrays */
    char
    (*SBp)[MAXICNAME+MAXSYMB], // List of IC names, packed [N] */
    (*SMp)[MAXDCNAME],         // List of IC names, packed [L] */
    (*SFp)[MAXPHNAME];      /* Compressed list of phase names [Fis] */

}
EQSTAT;



// Current System+Equstat
class TSysEq : public TCModule
{
    SYSPACK ss[2];
    EQSTAT st[2];

protected:

    void  resetFlags();
    void newSizeifChange();

public:

    static TSysEq* pm;

    SYSPACK *ssp;
    EQSTAT *stp;

    TSysEq( int nrt );

    const char* GetName() const
    {
        return "SysEq";
    }
    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    void RecInput( const char *key );
    int RecBuild( const char *key );
    void RecCalc( const char *key );
    void RecSave( const char *key, bool onOld );
    void RecordPrint( const char *key=0 );

    // set to 1 if have been calculated equation state for this record
    bool ifCalcFlag();
    void setCalcFlag( bool ifC );
    void keyTest( const char *key );

    void InsertChanges( TIArray<CompItem>& aIComp,
                        TIArray<CompItem>& aCompos, TIArray<CompItem>& aPhase,
                        TIArray<CompItem>&aDComp );
    bool MakeRecord( const char *key );

};
#endif   //   _m_syseq_h
