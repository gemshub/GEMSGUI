//-------------------------------------------------------------------
// $Id$
//
// Declaration of TMulti class, config functions
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
// See http://les.web.psi.ch/Software/GEMS-PSI for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#ifndef _ms_multi_h_
#define _ms_multi_h_

#include "datach.h"
#include "databr.h"

class GemDataStream;

#ifndef IPMGEMPLUGIN

#include "m_param.h"
#include "v_ipnc.h"

#else

#include "m_const.h"

#endif

// const double LOWESTPHASE_=1e-18;

typedef struct
{  // MULTI is base to Project (local values)
    char
    stkey[EQ_RKLEN+1],   // Record key identifying IPM minimization problem
    // NV_[MAXNV], nulch, nulch1, // Variant Nr for fixed b,P,T,V; index in a megasystem
    PunE,         // Units of energy  { j;  J c C N reserved }
    PunV,         // Units of volume  { j;  c L a reserved }
    PunP,         // Units of pressure  { b;  B p P A reserved }
    PunT;         // Units of temperature  { C; K F reserved }
    short N,        	// N - number of IC in IPM problem
    NR,       	// NR - dimensions of R matrix
    L,        	// L -   number of DC in IPM problem
    Ls,       	// Ls -   total number of DC in multi-component phases
    LO,       	// LO -   index of water-solvent in IPM DC list
    PG,       	// PG -   number of DC in gas phase
    PSOL,     	// PSOL - number of DC in liquid hydrocarbon phase
    Lads,     	// Lads - number of DC in sorption phases
    FI,       	// FI -   number of phases in IPM problem
    FIs,      	// FIs -   number of multicomponent phases
    FIa,      	// FIa -   number of sorption phases
    FI1,     // FI1 -   number of phases present in eqstate
    FI1s,    // FI1s -   number of multicomponent phases present in eqstate
    FI1a,    // FI1a -   number of sorption phases present in eqstate
    IT,      // It - number of completed IPM iterations
    E,       // PE - flag of electroneutrality constraint { 0 1 }
    PD,      // PD - mode of calling GammaCalc() { 0 1 2 3 4 }
    PV,      // PV - flag of system volume constraint { 0 1 }
    PLIM,    // PU - flag of activation of DC/phase restrictions { 0 1 }
    Ec,    // GammaCalc() return code: 0 (OK) or 1 (error)
    K2,    // Number of Selekt2() loops
    PZ,    // Indicator of IPM-2 precision algorithm activation
    /* funT, sysT, */
    pNP, //Mode of FIA selection: 0-auto-SIMPLEX,1-old eqstate,-1-user's choice
    pESU,  // Unpack old eqstate from EQSTAT record?  0-no 1-yes
    pIPN,  // State of IPN-arrays:  0-create; 1-available; -1 remake
    pBAL,  // State of reloading CSD:  1- BAL only; 0-whole CSD
    pFAG,  //State of initial lnGam load: 0-no, 1-on Mbel, 2-lnGmf, -1-SurEta
    pTPD,  // State of reloading thermod data: 0- all  1 - G0 only  2 - no
    pULR,  // Start recalc kin ogranich (0-do not, 1-do )internal
    pRR1,  // internal
    FIat,   // max. number of surface site types  2/28/97 DAK */
    MK,     // PM return code: 0 - continue;  1 - converged
    W1,     // internal IPM-2 indicator
    is,     // is - index of IC for IPN equations ( GammaCalc() )
    js,     // js - index of DC for IPN equations ( GammaCalc() )
    next;  // Next - continue variable for  IPN equations ( GammaCalc() )
    double TC,TCc, 	// Temperature T, min.-max. (0,2000 C)
    T,Tc,   	// T, min.-max. K
    P,Pc,   	// Pressure P, min.-max.(0,10000 bar)
    VX_,VXc,   // V(X) - volume of the system, min.-max., cm3
    GX_,GXc,   // Gibbs potential of the system G(X), min.-max. (J)
    AX_,AXc,   // Helmholtz potential of the system F(X), reserved
    UX_,UXc,  	// Internal energy of the system U(X), reserved
    HX_,HXc, 	// Total enthalpy of the system H(X), reserved
    SX_,SXc, 	// Total entropy of the system S(X), reserved
    CpX_,CpXc,  // reserved
    CvX_,CvXc,  // reserved
    T0,       	 // reserved
    VE,     	 // reserved
    MBX,    // Total mass of the system, kg
    FX,    	// Current Gibbs potential of the system in IPM, moles
    IC,     // Effective molal ionic strength of aqueous electrolyte
    pH,     // pH of aqueous solution
    pe,     // pe of aqueous solution
    Eh,     // Eh of aqueous solution, V
    DHBM,   // Adjusted balance precision criterion (IPM-2 )
    DSM,    // min value phase DS (IPM-2)
    GWAT,   // reserved
    YMET,   // reserved
    denW,denWg,  // Density of H2O(l) and steam for Tc,Pc
    epsW,epsWg,  // Diel. constant of H2O(l) and steam for Tc,Pc
    PCI,    // Current value of Dikin criterion of IPM convergence DK>=DX
    DX,     // IPM convergence criterion threshold DX (1e-5)
    lnP,    // log Ptotal
    RT,     // RT: 8.31451*T (J/mole/K)
    FRT,    // F/RT, F - Faraday constant = 96485.309 C/mol
    Yw,     // Current number of moles of solvent in aqueous phase
    ln5551, // ln(55.508373) = 4.0165339
    aqsTail,  // v_j asymmetry correction factor for aqueous species
    lowPosNum, // Minimum DC quantity defining Ls set (1e-19)
    logXw,     /*log(1e-16)*/
    logYFk,    /*log(1e-9)*/
    YFk,       // Current number of moles in a multicomponent phase
    FitVar[5]; // internal
    short
    *L1,    // l_a vector - number of DC included into each phase [Fi]
    *LsMod, // Number of non-ideality coeffs per multicomponent phase [FIs]
    *LsMdc, // Number of non-ideality coeffs per one DC in multicomponent phase[FIs]
    *mui,   // IC indices in RMULTS IC list [N]
    *muk,   // Phase indices in RMULTS phase list [FI]
    *muj,   // DC indices in RMULTS DC list [L]
    (*SATNdx)[2];  /* surface site type index, 0,1,...,Fiat-1 [0:Ls-1][2] */
    /* and assign. sur.DC to carrier end-member indices */
    float
    *PMc,   // Non-ideality coefficients f(TP) -> LsMod
    *DMc,   // Non-ideality coefficients f(TPX) for DC -> LsMdc
    *A,    // DC stoichiometry matrix A composed of a_ji [0:N-1][0:L-1]
    *Awt,   // IC atomic (molar) mass, g/mole [0:N-1]
    *Wb,    //Relative Born factors (HKF, reserved) [0:Ls-1]
    *Wabs,  // Absolute Born factors (HKF, reserved) [0:Ls-1]
    *Rion,  // Ionic or solvation radii, A (reserved) [0:Ls-1]
    *HYM,   // reserved
    *ENT,   // reserved no object
    *H0,    // DC p-molar enthalpies, reserved [L]
    *A0,    // DC p-molar Helmholtz energies, reserved [L]
    *U0,    // DC p-molar internal energies, reserved [L]
    *S0,    // DC p-molar entropies, reserved [L]
    *Cp0,   // DC p-molar entropies, reserved [L]
    *Cv0,   // DC p-molar Cv, reserved [L]
    *VL,    // ln mole fraction
    *Xcond, 	/* conductivity of phase carrier, sm/m2   [0:FI-1] */
    *Xeps,  	/* diel.permeability of phase carriers    [0:FI-1] */
    *Aalp,  	/* phase specific surface area m2/g       [0:FI-1] */
    *Sigw,  	/* st.surface free energy in water,J/m2   [0:FI-1] */
    *Sigg,  	/* st.surface free energy in air,J/m2     [0:FI-1] */
    (*Xr0h0)[2],  /* mean r & h of particles (- pores) nm  [0:FI-1][2] */
    (*Nfsp)[MST], /* area fraction of surface types At/A  [FIs][FIat] */
    (*MASDT)[MST], /* Max.reactive species sur.density, mkmol/g [FIs][FIat] */
    (*XcapF)[MST], /* eff.capacitance of Ba EDL layer in FLM [FIs][FIat] */
    (*XcapA)[MST], /* eff.capacitance of A EDL layer, F/m2  [FIs][FIat] */
    (*XcapB)[MST], /* eff.capacitance of B EDL layer, F/m2  [FIs][FIat] */
    (*XcapD)[MST], /* eff.capacitance of diffuse layer,F/m2 [FIs][FIat] */
    (*XdlA)[MST],  /* eff.thickness of A EDL layer, nm      [FIs][FIat] */
    (*XdlB)[MST],  /* eff.thickness of B EDL layer, nm      [FIs][FIat] */
    (*XdlD)[MST],  /* eff.thickness of diffuse layer, nm    [FIs][FIat] */
    (*XlamA)[MST], /* Factor of EDL discretness  A < 1      [FIs][FIat] */
    (*Xetaf)[MST], /* Fixed charge density at surface type, C/m2 [FIs][FIat] */
    *XFs,  // Current quantities of phases X_a at IPM iterations [0:FI-1]
    *Falps, // Current Karpov criteria of phase stability  F_a [0:FI-1]
    *Fug,   // Partial fugacities of gases [0:PG-1]
    *Fug_l, // log  partial fugacities of gases [0:PG-1]
    *Ppg_l, // log  partial pressures of gases [0:PG-1]
    *MASDJ;  /* Max sur dens for non-compet.species mkmol/g [Ls] */
    double
    *DUL,  // VG Vector of upper restrictions to x_j (reserved) [L]
    *DLL,  // NG Vector of lower restrictions to x_j, moles [L]
    *GEX,  // Excess free energy of (metastable) DC, moles [L]
    *PUL,  // Vector of upper restrictions to X_a (reserved)[FIs]
    *PLL,  // Vector of lower restrictions to X_a (reserved)[FIs]
    *YOF,  // Phase metastability parameter [FI !!!!]
    *Vol,  // DC molar volumes, cm3/mole [L]
    *MM,   // DC molar mass, g/mole [L]
    *Pparc, // DC partial pressures, bar (Pc by default) [0:L-1]
    *Y_m,   // Molalities of aqueous species and sorbates [0:Ls-1]
    *Y_la,  // log activity of DC in multi-component phases[0:Ls-1]
    *Y_w,   // Mass concentrations of DC in multi-component phases,%(ppm)[Ls]
    *Gamma, // DC activity coefficients [0:L-1]
    *lnGmf, // ln of initial DC activity coefficients [0:L-1]
    *lnGmM, // ln of DC metastability coefficients (reserved)[0:L-1]
    *EZ,    // Formula charge of DC in multi-component phases [0:Ls-1]
    *FVOL,  // phase volume, cm3/mol                  [0:FI-1]
    *FWGT,  // phase (carrier) mass, g                [0:FI-1]

    *G,    // Normalized DC energy function c(j), mole/mole [0:L-1]
    *G0,   // Input normalized g0_j(T,P) for DC at unified standard scale[L]
    *lnGam, // ln of DC activity coefficients [0:L-1]
    *lnGmo, // Copy of lnGam from previous IPM iteration (reserved)
    *lnSAT, // Ln of surface activity terms           [0:Ls-1]
    *B,  // Input bulk chem. compos. of the system-b vector, moles of IC[N]
    *U,  // IC chemical potentials u_i (mole/mole) - dual IPM solution [N]
    *U_r, // IC chemical potentials u_i (J/mole) [0:N-1]
    *C,   // Calculated IC mass-balance deviations (moles) [0:N-1]
    *IC_m, // Total IC molalities in aqueous phase (excl.solvent) [0:N-1]
    *IC_lm,	// log total IC molalities in aqueous phase [0:N-1]
    *IC_wm,	// Total dissolved IC concentrations in g/kg_soln [0:N-1]
    *BF, //Output bulk compositions of multicomponent phases bf_ai[FIs][N]
    *XF,    // Output total number of moles of phases Xa[0:FI-1]
    *YF,    // Copy of X_a from previous IPM iteration [0:FI-1]
    *XFA,   // Quantity of carrier in asymmetric phases Xwa, moles [FIs]
    *YFA,   // Copy of Xwa from previous IPM iteration [0:FIs-1]
    *Falp,  // Karpov phase stability criteria F_a [0:FI-1]
    (*XetaA)[MST], // Total EDL charge on A EDL phase surface, moles [FIs][FIat]
    (*XetaB)[MST], // Total charge of surface species on B EDL layer, moles[FIs][FIat]
    (*XpsiA)[MST], /* Galvani potential at A(0) EDL plane,V [FIs][FIat] */
    (*XpsiB)[MST], /* Galvani potential at B EDL plane,V    [FIs][FIat] */
    (*XpsiD)[MST], /* Galvani potential at diffuse plane,V  [FIs][FIat] */
    (*XFTS)[MST],  // Total number of moles of surface DC at surtype [FIs][FIat]
    *X,  // DC quantities at eqstate x_j, moles - prime IPM solution [L]
    *Y,  // Copy of x_j from previous IPM iteration [0:L-1]
    *XY, // Copy of x_j from previous loop of Selekt2() [0:L-1]
    *Qp, // Work IIPN variables related to non-ideal phases FIs*(20)
    *Qd, // Work IIPN variables related to DC in non-ideal phases (20)
    *MU, // mu_j values of differences between dual DC chem.potentials[L]
    *EMU, // Exponents of DC increment to F_a criterion for phase [0:L-1]
    *NMU, // DC increments to F_a criterion for phase [0:L-1]
    *W,   // Weight multipliers for DC (incl restrictions) in IPM [0:L-1]
    *Fx,  // Dual DC chemical potentials defined via u_i and a_ji [0:L-1]
    *Wx,  // Mole fractions Wx of DC in multi-component phases [0:L-1]
    *F, //Prime DC chemical potentials defined via g0_j, Wx_j and lnGam_j[L]
    *F0,  // Excess Gibbs energies for (metastable) DC, mole/mole [0:L-1]
    *D,    /* Reserved */
    *R,    // R matrix of IPM linear equations [0:NR][0:NR+1]
    *R1;   // Copy of R for Jordan()
    char
    (*sMod)[6], 	 // Codes of models of multicomponent phases [0:FIs-1]
    (*SB)[MAXICNAME+MAXSYMB], // List of IC names in the system [0:N-1]
    (*SB1)[MAXICNAME], // List of IC names in the system [0:N-1]
    (*SM)[MAXDCNAME],  // List of DC names in the system [0:L-1]
    (*SF)[MAXPHNAME+MAXSYMB],  // List of phase names in the system [0:FI-1]
    (*SM2)[MAXDCNAME],  // List of multicomp. DC names in the system [Ls]
    (*SF2)[MAXPHNAME+MAXSYMB], // List of multicomp. phase names in the syst [FIs]
    (*SFs)[MAXPHNAME+MAXSYMB],
    // List of phase currently present in non-zero quantities [0:FI-1]
    *pbuf, 	// Text buffer for EQSTATe table printout
    *RLC,   // Classifier of restriction types for x_j 0:L-1
    *RSC,   // Classifier of restriction scales for x_j 0:L-1
    *RFLC,  // Classifier of restriction types for XF_a 0:FIs-1
    *RFSC,  // Classifier of restriction scales for XF_a 0:FIs-1
    *ICC,   // Classifier of IC { e o h a z v i <int> } 0:N-1
    *DCC,   // Classifier of DC { TESWGVCHNIJMDRAB0123XYZPQO } 0:L-1
    *PHC,   // Classifier of phases { a g p m l x d h } 0:FI-1
    (*SCM)[MST], //classifier of adsorption models for sur types [FIs][FIat]{DCTMIN}
    *SATT,  /* classifier of methods of SAT calculation [0:Ls] { C N S I }*/
    *DCCW;  // reserved                0:L-1
    // codes see in file S_CLASS.H

    /* add functions to calc sum potentuals *
    double (*GX)(double LM, int q );    * calc G(X) *
    double (*LMD)(double LM, int q );   * calc delta *
    double (*HX)(double Par, int q );     *  *
    double (*UX)(double Par, int q );     *  *
    double (*SX)(double Par, int q );     *  *
    double (*VX)(double Par, int q );     *  *
    double (*CpX)(double Par, int q );    *  *
    double (*CvX)(double Par, int q );    *  */
}
MULTI;


#ifndef IPMGEMPLUGIN

// Data of MULTI
class TMulti :
            public TSubModule
{
    MULTI pm;

    SYSTEM *syp;
    //MTPARM *tpp;

    // transport
    DATACH  data_CH;
    DATABR  data_BR;
public:

    TIArray<IPNCalc> qEp;
    TIArray<IPNCalc> qEd;


    MULTI* GetPM()
    {
        return &pm;
    }

    TMulti( int nrt, SYSTEM* sy_ );

    const char* GetName() const
    {
        return "Multi";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    // EQUSTAT
    void packData();
    void packData( TCIntArray PHon, TCIntArray DCon );
    void setSizes();
    void loadData( bool newRec );
    void unpackData();

    //mass transport
    void to_file( GemDataStream& ff );
    void to_text_file();
    void from_file( GemDataStream& ff );
    void inArray( fstream& ff, char *name, short* arr, int size );
    void outArray( fstream& ff, char *name, short* arr, int size );
    void outArray( fstream& ff, char *name,  float* arr,
                             int size, int l_size=-1 );
    void outArray( fstream& ff, char *name, double* arr, int size );
    void outArray( fstream& ff, char *name, char* arr,
                              int size, int arr_siz );

    void datach_to_file( GemDataStream& ff );
    void datach_from_file( GemDataStream& ff );
    void databr_to_file( GemDataStream& ff );
    void databr_from_file( GemDataStream& ff );
    void datach_realloc();
    void datach_free();
    void databr_realloc();
    void databr_free();
    void datach_to_text_file();
    void datach_from_text_file();
    void databr_to_text_file();

    void makeStartDataChBR(
       TCIntArray& selIC, TCIntArray& selDC, TCIntArray& selPH );
    void packDataBr();
    void unpackDataBr();
//    void multi_realloc( char PAalp, char PSigm );
//    void multi_free();


    // test
    void dyn_kill_test(MULTI& tes);
    void dyn_new_test(MULTI& tes);
    void Test_Eq( int size, double *p1, double *p2, const char *key );
    void Test_Eq( int size, float *p1, float *p2, const char *key );
    void Test_Eq( int size, short *p1, short *p2, const char *key );
    void dyn__test(MULTI& tes);
    MULTI copy1;
    bool flCopy;
    MULTI& GetPMcopy1()
    {
        return copy1;
    }
 };

#else

// Data of MULTI
class TMulti
//:public TSubModule
{
   MULTI pm;

    // transport
    DATACH  data_CH;
    DATABR  data_BR;

   char PAalp_;
   char PSigm_;
public:

   float EpsW_;
   float RoW_;

//    TIArray<IPNCalc> qEp;
//    TIArray<IPNCalc> qEd;


    MULTI* GetPM()
    {
        return &pm;
    }

    TMulti( /*int nrt, SYSTEM* sy_*/ )
    {}

    const char* GetName() const
    {
        return "Multi";
    }

    //mass transport
    void to_file( GemDataStream& ff );
    void to_text_file();
    void from_file( GemDataStream& ff );
    void multi_realloc( char PAalp, char PSigm );
    void multi_free();
    void outArray( fstream& ff, char *name, short* arr,
                             int size, int l_size=-1  );
    void outArray( fstream& ff, char *name,  float* arr, int size );
    void outArray( fstream& ff, char *name, double* arr, int size );
    void outArray( fstream& ff, char *name, char* arr,
                              int size, int arr_siz );

    void datach_to_file( GemDataStream& ff );
    void datach_from_file( GemDataStream& ff );
    void databr_to_file( GemDataStream& ff );
    void databr_from_file( GemDataStream& ff );
    void datach_realloc();
    void datach_free();
    void databr_realloc();
    void databr_free();
    void datach_to_text_file();
    void databr_to_text_file();

    void packDataBr();
    void unpackDataBr();
    // test
    void dyn_kill_test(MULTI& tes);
    void dyn_new_test(MULTI& tes);
    void Test_Eq( int size, double *p1, double *p2, const char *key );
    void Test_Eq( int size, float *p1, float *p2, const char *key );
    void Test_Eq( int size, short *p1, short *p2, const char *key );
    void dyn__test(MULTI& tes);
    MULTI copy1;
    bool flCopy;
    MULTI& GetPMcopy1()
    {
        return copy1;
    }

};

#endif

#endif   //_ms_multi_h

