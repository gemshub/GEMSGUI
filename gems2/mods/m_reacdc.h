//-------------------------------------------------------------------
// Id: gems/mods/m_reacdc.h  version 2.0.0   2001
//
// Declaration of TReacDC class, config and calculation functions
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
//
#ifndef _m_reacdc_h_
#define _m_reacdc_h_

#include "v_mod.h"
#include "v_module.h"

const int RE_RKLEN = 32;

typedef struct
{ // Description  REACDC
    char pstate[MAXSYMB],// Aggregate state of reaction-defined DC at Tr,Pr
    psymb[MAXDRGROUP],   // Identifier of REACDC group
    dcn[MAXDCNAME],      // Name of reaction-defined DC
    atyp[MAXSYMB],       // Code of thermodynamic dataset
    pct[6], //Mode of calculation of thermodynamic properties{CHKSIX}
    //Mode of T-dependency { SHBKWALX123ZREGDC }
    //Mode of P-dependency { N0CKVBEGFAZ }
    // reserved
    PreC,   // DC class code { TESWGVCHNIJM0123456789XYZABPQRO }
    PreU,      // Units of energy   { j;  JcCN reserved }
    PreV,      // Units of volume   { j;  cLa reserved }
    PreP,      // Units of pressure Pr   { b;  BpPA reserved }
    PreT,      // Units of temperature Tr   { C; KF reserved }
    PreR1,     // reserved

    PreKT,// Flag for lgK=f(T) coefficients { +*- }
    PreKP,// Flag for lgK(Tf,Pf) array for lgK interpolation {+*-}
    PreDS,// Flag for coefficieents of electrostatic models {+*-}(reserved)
    PreDV,     //Flag for dVr=f(P,T) coefficients {+*-}
    PreDC,     // Flag for dCpr=f(T) coefficients  {+*-}
    PrSVC,     //Will TP vary along SVC curve only? {+-} (reserved)
    PrAki,     // Flag for coefficients of HKF EOS {+-} (reserved)
    PrIso,     // Is reaction-defined DC an isotope form? {+-}

    rmtm[MAXRMTM],     // Date when this REACDC definition was created
    name[MAXFORMULA],  // Full name of reaction and reaction-defined DC
    form[MAXFORMULA],  // Chemical formula of reaction-defined DC
    (*DCk)[DC_RKLEN],// List of DC keys (names) involved in reaction [0:nDC-1]
    *rDC;   // DC codes: n-new r-REACDC d-DCOMP f-fictive
    short nDC,     // N of DC in reaction, reaction-defined DC is the last one
    NcKt,     // N of nonzero coeffs of logK = f(T) equation
    NcKp,     // N of nonzero coeffs of dVr = f(P,T) equation
    NcSt,     //N of nonzero coeffs of electrostatic equations (reserved)
    NcCt,     // N of nonzero coeffs of dCpr = f(T) equation
    nTp,      // N of interpolation points along T
    nPp,      // N of interpolation points along P
    Nsd;      // N of references to Data Sources
    float Zz,      // Reaction-defined DC formula charge
    mwt;     // Molecular mass of reaction-defined DC, g/mole
    double Ks[3],  //K (stability constant) of reaction at Tr,Pr(isotopic:alpha)
    //log K of reaction at Tr,Pr (isotopic: 1000ln(alpha))
    // Uncertainty of logK
    Gs[3],   // dG of reaction at Tr, Pr  (J/mole)
    //Apparent standard free energy of reacdef-DC formation from IC g0 (J/mole)
    // Uncertainty of g0  dev(g0) (J/mole)
    Hs[3];   // dH of reaction at Tr, Pr (J/mole)
    //Apparent standard enthalpy of reacdef-DC formation from IC h0 (J/mole)
    // Uncertainty of h0  dev(h0) (J/mole)
    float  Ss[3],//dS of reaction at Tr, Pr (J/mole/K)
    //Absolute entropy of reacdef-DC at standard state S0 (J/mole/K)
    //Uncertainty of S0  dev(S0) (J/mole/K)
    Cps[3],// dCp of reaction at Tr, Pr (J/mole/K)
    //Heat capacity of reacdef-DC at standard state Cp0 (J/mole/K)
    //Uncertainty of Cp0  dev(Cp0) (J/mole/K)
    Vs[3], // dV of reaction at Tr, Pr (J/bar)
    //Molar volume of reacdef-DC at standard state Vm0 (J/bar)
    //Uncertainty of Vm0  dev(Vm0) (J/bar)
    Nix[3],//Mole fraction of counter-isotope in natural mixture (element)
    //  reserved; reserved
    Pst,     // Reference pressure Pr (bar)
    TCst,    // Reference temperature Tr (‘)
    Comp,    // Coefficient of isothermal compressibility of reacdef DC
    Expa,    // Coefficient of isobaric expandability of reacdef DC
    Der,// Ion-size par.a0,A(Debye-Hueckel aq species);ƒmax,1/nm2(sorbates)
    DerB;    // Indiv.par. b (for III Debye-Hueckel aq species)
    double *scDC; // Stoichiometry coefficients of DC in reaction (reagents < 0)
    float *TCint, // Vector or interval of temperature T  [0:nTp-1 > 1]
    *Pint,  // Vector or interval of pressure P  [0:nPp-1 > 1]
    *DCp,   // Coefficients of dCpr = f(T) equation (reserved) [MAXCPCOEF]
    *HKFc,  // Coeffs a1-a4, c1,c2, w, rAB for HKF EOS of reaction (reserved)
    // [MAXHKFCOEF]
    *DVt,   // Coeffs for dVr = f(P,T) equation (reserved) [MAXVTCOEF]
    *DSt,   // Coeffs for electrostatic model equations(reserved)[MAXCPCOEF]
    *pKt,//Coefficients of logK = f(T) or 1000ln(alpha) = f(T) equation:a0
    //  a1 *T    (isotopic: a1/T)
    //  a2 /T    (isotopic: a2/T^2)
    // a3 *lnT  (isotopic: a3/T^3)
    // a4 /T^2  (isotopic: a4/T^4)
    // a5 *T^2
    // a6 /T^0.5            [MAXCPCOEF]
    *logK // Array of logK(T,P) values for interpolation[0:nTp-1][0:nPp-1]
    ;
    char (*sdref)[V_SD_RKLEN], // List of Data Source SDref keys
    (*sdval)[V_SD_VALEN]; // Comments to Data Source references 0:Nsd-1
    // work arrays
    double (*ParDC)[6]; // Gs, Hs, Ss, Cps, Vs values for DC in reaction (columns)
    char *tprn;               // internal
}

REACDC;

struct elmWindowData;
struct rdSetupData;

// Current ReacDC
class TReacDC : public TCModule
{
    REACDC rc[8];

protected:

    void w_dyn_kill();
    void w_dyn_new();
    void Convert_Cp_to_KT( int CE );
    void Convert_KT_to_Cp( int CE );
    void Recalc( int q, const char *key  );
    void Recalc_rDCN( double foS );
    void Recalc_rDCD();
    void Recalc_ISO1( double foS );
    void Recalc_ISO2( double foS );
    void Calc_rDCD( int q, int p );
    void calc_tphkf_r( int q, int p );
    void calc_akinf_r( int q, int p, int CE, int CV );
    void calc_lgk_r( int q, int p, int CE, int CV );
    void calc_r_interp( int q, int p, int CE, int CV );
    float lagr(float *a, float *x, float *y, float x1, float x2,  int p0,int p1);
    void calc_dissoc_r( int q, int p, int CE, int CV );
    void calc_iso_a( int q, int p );
    void calc_exion_r( int q, int p );
    void calc_tpcv_r( int q, int p, int CM, int CV );
    void PronsPrep( const char *key );


public:

    static TReacDC* pm;

    REACDC *rcp;

    TReacDC( int nrt );

    const char* GetName() const
    {
        return "ReacDC";
    }

    bool firstRc() const
    {
        return rcp==&rc[0];
    }
    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    bool check_input( const char *key, int level=1 );

    void RecInput( const char *key );
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char* key );
    void TryRecInp( const char *key, time_t& time_s, int q );
    void RecordPrint( const char *key=0 );
    void CmHelp(); 

    void RCthermo( int q, int p );

    void CopyRecords( const char * prfName, TCIntArray& cnt,
                      elmWindowData el_data, rdSetupData st_data );

};

enum pardc_ndx {  /* indexes of work arrays  */
    _Gs_,  /* Gs comp.reactions */
    _Hs_,  /* Hs  */
    _Ss_,  /* Ss  */
    _Cps_, /* Cps */
    _Vs_,  /* Vs  */
    _Zs_,  /* Z */
    _dGs_, /* delta Gs */
    _dHs_, /* delta Hs */
    _dSs_, /* delta Ss */
    _dCps_,/* delta Cps */
    _dVs_  /* delta Vs */
};

#endif    //  _m_reacdc_h

