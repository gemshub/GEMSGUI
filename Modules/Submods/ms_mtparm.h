//-------------------------------------------------------------------
// $Id: ms_mtparm.h 1377 2009-07-24 13:12:02Z gems $
//
// Declaration of TMTparm class, config and  load functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2012 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Selektor GUI GUI DBMS library, gems/lib/gemvizor.lib
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#ifndef _ms_mtparm_h_
#define _ms_mtparm_h_

#include "v_mod.h"
#include "v_module.h"
#include "GEMS3K/datach.h"

enum SQUEEZE_CODES { /*delete DC in MTPARM */
    TPMARK_VALID = 'V',    TPMARK_PMODF = 'P',     TPMARK_ALLDC = 'D',
    CP_NOT_VALID = 'e',    PM_NOT_EXIST = '-'
};


typedef struct
{ // MTPARM is base to Project (t/d parametres for DC)
    char //symb[MAXMUNAME],    // Identifier of root multisystem <- RMULTS
    //P_[MAXPTN], nchp,   // Pressure P, bar or 0 (Psat H2O)
    //TC_[MAXPTN], ncht,  // Temperature T, deg.C
    //NV_[MAXNV], nchn,   // Variant number of MTPARM data

    PunE,          // Units of energy   { j;  J c C N reserved }
    PunV,          // Units of volume   { j;  c L a reserved }
    PunP,          //  Units of pressure  { b;  B p P A reserved }
    PunT,          // Units of temperature  { C; K F reserved }
    PeosW,   // Account for EOS H2O { + - }
    P_HKF,   // Link to HKF EOS functions { + - }
    Pbg,     // flag for constant (0) or variable b_gamma in DH eq (1-NaCl, 2-KCl, 3-NaOH, 4-KOH)
    Pres1,   // Include vector of quality for TP dependencies of DC { + * - }

    // indicators for states of project arrays (Flags for the result data vectors)
    PtvG, PtvdG,   /* G, delG, + - * (if * recalc) */
    PtvH, PtvdH,   /* H, delH, + - *     */
    PtvS, PtvdS,   /* S, delS, + - *     */
    PtvCp, PtvdCp, /* Cp, delCp, + - *   */
    PtvVm, PtvdVm, /* Vm, delVm, + - *   */
    PtvF, PtvU,    /* F, U, + - *        */
    PtCv,          /* Cv,  + - *         */
    PtvA, PtvB,    /* Alp, Bet, + - *    */
    PtvWb, PtvWr,  /* Wborn, Wrad, + - * */
    PtvFg, PtvdVg, /* Fugg, dVg + - *    */
    PtvR1,         /*  reserved, + - *      */

    *mark   // vector of quality for TP dependencies of DC{ +-e }[0:L-1]
    ;
    short L,  //Numbers: L of DC = mu.L
    Ls,   // Ls of DC in multi-component phases
    Lg,   // PG of DC in gas phase (for FGL)
    La    // Laq number of aqueous species and water - changed from Lx on 05.01.05
    ;
    double  /* genP[8] */
    curT, curP, // current T & P
    T,    	// Temperature T, C
    P,    	// Pressure P, bar
    RT,   	// RT factor (R=8.31451 J/mole/K)
    TK,   	// T in Kelvins, added 27.11.00 by DAK
    RoW, RoV,  // Density of H2O g/cm3 (liquid; vapour)
    dRdTW, dRdTV,
    d2RdT2W, d2RdT2V,
    dRdPW, dRdPV,
    d2RdP2W, d2RdP2V,
    EpsW, EpsV,  // Dielectric constant of H2O (liquid; vapour)
    dEdTW, dEdTV,
    d2EdT2W, d2EdT2V,
    dEdPW, dEdPV,
    d2EdP2W, d2EdP2V,
      VisW, VisV; // Viscosity of H2O (dynamic), kg/m*sec (water; steam)


// Arrays loaded from current data, not from DB !!!!
    double        /* size [0:L-1] */
    *G,    // Partial molar(molal) Gibbs energy g(TP) (always), J/mole
    *devG, // Uncertainty of g(TP)
    *H,    // Partial molar(molal) enthalpy h(TP), J/mole
    *devH, // Uncertainty of h(TP)
    *S,    // Partial molar(molal) entropy s(TP), J/mole/K
    *devS, // Uncertainty of s(TP)
    *Cp,   // Partial molar(molal) heat capacity Cp(T), J/mole/K
    *devC, // Uncertainty Cp (Cv)
    *Vm,   // Partial molar(molal) volume Vm(TP) (always), J/bar
    *devV, // Uncertainty of Vm(TP)
    *F,    // Helmholtz energy F(T) (reserved)
    *U,    // Internal energy U(T) (reserved)
    *Cv,   // Heat capacity Cv(T) (reserved)
    *Alp,  // Compressibility (reserved)
    *Bet,  // Expandability (reserved)
    *Wbor, // Born factor relative for aq & sorb (reserved) [0:Ls-1]
    *Wrad, // Eff solvation radius (HKF) (reserved)[0:Ls-1]
    *Fug,  // Fugacity (FGL), reserved [0:Lg-1]
    *dVg   // Work array for CG EoS coeffs (reserved) [Lg*4]
    ;
    char (*DCMod)[6]; // Codes for PT corrections for dependent component data [L]
}
MTPARM;

enum {
   BG_CONST='0', BG_TP_NACL='1', BG_TP_KCL='2', BG_TP_NAOH='3', BG_TP_KOH='4'
};

// Data of MTPARM
class TMTparm :
            public TSubModule
{
    MTPARM tp;

protected:
    void polmod_test();
    void _pr_tab( const char *key );
    void _pr_line( const char *key );

public:

    static TMTparm* sm;

    MTPARM* GetTP()
    {
        return &tp;
    }

    TMTparm( int nrt );

    const char* GetName() const
    {
        return "MTparm";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    const char* GetHtml();

    void LoadMtparm( double cT, double cP );
    double b_gamma_TP( double tk, double pb, double eps, double gsf, int mode );
    //Relative dielectric constant of pure water, eps as a function of (P, T)
    //       Bradley and Pitzer, 1979, JPC 83, 1599.
    double wat_dielectric (double cT, double cP);
    double wat_sat_pressure (double cT);
    double wat_sat_liquid_density (double cT);
    void MTparmAlloc( );
    void LoadDataToLookup( QWidget* par, DATACH* CSD );
    void LoadDataToPair( QWidget* par, DATACH* CSD );

};

#endif //_ms_mtparm_h
