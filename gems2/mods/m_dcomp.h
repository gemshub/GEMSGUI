//-------------------------------------------------------------------
// $Id$
//
// Declaration of TDComp class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  
// Copyright (C) 1995-2001 S.Dmytriyeva, D.Kulik, V.Sinitsyn
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
#ifndef _m_dcomp_h_
#define _m_dcomp_h_

#include "v_mod.h"
#include "v_module.h"
#include "s_tpwork.h"

const int
MAXCPCOEF =      12,
  MAXCPFSCOEF=     9,
     MAXHKFCOEF =     8,
        MAXVTCOEF =      5,
           MAXODCOEF =     12, // Total number of V(T,P) coeffs BM Gottschalk
             MAXCRITPARAM =   7,
                 MAXEOSPARAM = 20;  // Maximum number of EoS coefficients

extern const double ZBALANCE_PREC,
    STANDARD_TC,
    STANDARD_P,
    DEF_REL_DEV,
    GAS_MV_STND,
    TK_DELTA,
    TEMPER_PREC,
    PRESSURE_PREC;

typedef struct
{ // Description  DCOMP
    char pstate[MAXSYMB],     // Aggregate state of Dependent Component (DC) at Tr,Pr
    psymb[MAXDRGROUP],   // Identifier of DC group
    dcn[MAXDCNAME],      // Name of Dependent Component
    atyp[MAXSYMB],       // Code of thermodynamic dataset

    pct[6],     // Mode of calculation of thermodynamic properties,T-dependency
    // P-dependency, EOS ID (3 reserved)
    PdcC,      // DC class code { TESWGVCHNIJM<digit>XYZABPQRO}
    PdcU,      // Units of energy  { j;   J c C N reserved }
    PdcV,      // Units of volume  { j;   c L a reserved }
    PdcP,      // Units of pressure Pr   { b;   B p P A reserved }
    PdcT,      // Units of temperature Tr   { C;  K F reserved }
    PdcMK,     //  Flag of Cp=f(T) coefficient array { + * - }
    PdcFT,     // Are there phase transitions? { + * - }
    PdcHKF,    // Flag of vector of HKF EOS coefficients { + * - }
    PdcVT,     // Flag of Vm=f(P,T) coefficient vector { + * - }"

    rmtm[MAXRMTM],// Date when this DC definition was created (in DB ? reserved)
    name[MAXFORMULA],    // Full name of DC
    form[MAXFORMULA];    // Chemical formula of DC

    short  NeCp,    // Numbers: of Cp(T) equations;
    Nft,     //  phase transitions;
    Nsd,     // SD references
    Nemp;    // EOS coefficients, usually MAXEOSPARAM

    float Zz,     // DC formula charge
    mwt,     // Molecular mass, g/mole
    mVs[2];  // Molar volume of DC at standard state (J/bar)
    // Uncertainty of DC molar volume (J/bar)
    double Gs[2], // Apparent free energy of DC formation from IC at standard state (J/mole)
    // Uncertainty of DC free energy of formation (J/mole)
    Hs[2];   // Apparent enthalpy of DC formation from IC at standard state (J/mole)
    // Uncertainty of DC enthalpy of formation (J/mole)
    float Ss[2],  // Absolute entropy of DC at standard state (J/mole/K)
    //  Uncertainty of DC entropy (J/mole/K)
    Cps[2],  // Heat capacity Cp of DC at standard state (J/mole/K)
    // Uncertainty of Cp (J/mole/K)
    Pst,     // Reference pressure Pr (bar)
    TCst,    // Reference temperature Tr (C)
    Smax,    // Entropy of lambda transition (J/mole/K)
    TCr,     // Temperature of lambda transition (C)
    Comp,    // Coefficient of isothermal compressibility
    Expa,    // Coefficient of isobaric expandability
    Der,     // Ion-size parameter a0, A (Debye-Hueckel aq species)
    DerB,   // Indiv.par. b (for III Debye-Hueckel aq species)

    *TCint, // Lower and upper T limits for ‘à=f(T) equation [NeCp][2]
    *Cp,    // Coeffs of Cp=f(T) (J,mole,K), one column per equation [MAXCPCOEF][NeCp] */
    *CpFS,  // reserved [MAXCPFSCOEF][NeCp]
    *HKFc,  // Empirical coefficients of HKF EOS [MAXHKFCOEF]
    *Vt,    // Coefficients of mV=f(T,P) [MAXVTCOEF]
    *CPg,   // Critical parameters (for FGL)  [MAXCRITPARAM]
    *ODc,   // Coeffs of V(T,P) Birch-Murnaghan 1947 Gottschalk [MAXODCOEF]
    *FtP,   // Column: TCf- at Pr; DltS,DltH,DltV; dV of phase transitions
    *FtBer, // Properties of phase transition (Berman): Tr; Tft; tilt; l1,l2 (reserved)
    // MAXCPFSCOEF
    *Cemp;  // Array of empirical EOS coefficients (CG EOS: MAXCGPARAM = 13)
    char (*FtTyp)[MAXSYMB], // List of phase transition type codes (reserved)
    (*sdref)[V_SD_RKLEN], // List of SDref keys to data sources
    (*sdval)[V_SD_VALEN]; // List of comments to data sources
    char *tprn;               // text buf
}

DCOMP;

typedef struct
{//aqsref - standart t/d parametres fo water solution HKF-model
    double Gfaqs;
    double Hfaqs;
    double SPrTra;
    double A[4];
    double C[2];
    double wref;
    double chg;
}
AQSREF;

struct elmWindowData;
struct dcSetupData;

// Current IComp
class TDComp : public TCModule
{
    DCOMP dc[2];

protected:
// Generic corrections
    void calc_tpcv( int q, int p, int CE, int CV );
    void calc_voldp( int q, int p, int CE, int CV );
// For Gottschalk's database
    void BirchMurnaghan( double Pst, double P, double Tst, double T, double Vst,
         const float *BMcoef, double &VV0, double &alpha, double &beta,
         double &GG0, double &HH0, double &SS0 );
    double BM_Volume( double P, double vt, double kt0, double kp, double kpp,
       double vstart);
    double BM_IntVol(double P, double Pref, double vt, double vpt, double kt0,
                 double kp, double kpp);
// From SUPCRT92
    void calc_tpH2O( int pst );
    void calc_tphkf( int q, int p );
    void calc_thkf( AQSREF& arf, double P, double T, double Dw, double betaw,
                    double alphaw, double daldTw, double Z, double Q,
                    double Y, double X, int geqn);

    void gfun92(double TdegC, double Pbars, double Dgcm3, double betab,
                double alphaK, double daldT, double *g, double *dgdP,
                double *dgdT, double *d2gdT2, int geqn);
    void gShok2(double T, double P, double D, double beta, double alpha,
                double daldT, double *g, double *dgdP, double *dgdT,
                double *d2gdT2);
    void omeg92(double g, double dgdP, double dgdT, double d2gdT2,
                double wref, double ZZ, double *W, double *dwdP,
                double *dwdT, double *d2wdT2);
    // void S_import( short Mode, fstream& imp );
    // void B_import( int Nft, fstream& imp );
    // From PRONSPREP97
    void ParCor();

public:

    static TDComp* pm;

    DCOMP *dcp;

    TDComp( int nrt );

    const char* GetName() const
    {
        return "DComp";
    }

    bool firstDc() const
    {
        return dcp==&dc[0];
    }
    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    void RecInput( const char *key );
    void MakeQuery();
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void TryRecInp( const char *key, time_t& time_s, int q );
    void CmHelp();

    // necessary new command in menu
    void DCthermo( int q, int p);

    void CopyRecords( const char * prfName, TCIntArray& cnt,
                 elmWindowData el_data, dcSetupData st_data );

};

#endif     // _m_dcomp.h
