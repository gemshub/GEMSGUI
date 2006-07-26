//-------------------------------------------------------------------
// $Id$
//
// Declaration of TProfil class, config and calculation functions
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

#ifndef _m_param_h_
#define _m_param_h_

#include <math.h>
#include "v_mod.h"
#include "submod/ms_rmults.h"
#include "submod/ms_mtparm.h"
#include "submod/ms_system.h"
#include "submod/ms_multi.h"
#include "submod/ms_calc.h"

class GemDataStream;

// Physical constants - see m_param.cpp
extern const double R_CONSTANT, NA_CONSTANT, F_CONSTANT,
    e_CONSTANT,k_CONSTANT, cal_to_J, C_to_K, lg_to_ln, ln_to_lg;
//

struct BASE_PARAM
{ // Flags and thersholds for numeric modules
    short PC,    // "Mode of Selekt2() subroutine operation"
    PD,// Mode of DHH():0-invoke,1-at FIA only,2-last IPM it. 3-every IPM it.
    PRD,//Positive: mode GammaCalc(): 0-inactive, 1-FIA only, 2-SELEKT2 only 3-every IPM iteration
         // Negative: number of additional EFD-IPM improvement loops (-1 to -30 )
    PSM,//  Mode of setting FIA Gamma in phases: 0-off; 1 - set activ.coeff. GAN
    DP, //  Max number of EnterFeasibleDomain() iterations { default 144 }
    DW, // IPM-2 precision mode: 0- disable >=1  - enable (number of loops, default 15)
    DT, // Exponent for restoring x_j from dual solution {-5 to +2, default -3}
    PLLG, // IPM view debug time on Progress Dialog screen from (sec) or 0
    PE, // Include electroneutrality condition? { 0 1 }
    IIM // Maximum number of iterations in the main IPM algorithm { 500 }
    ;
    double DG,   // Precision of LEM IPM solver (Jordan) { 1e-15 }
    DHB, // Precision of mass balance deviations at EFD { 1e-8 }
    DS,  // Cutoff number of moles of a phase { 1e-10 }
    DK,  // Threshold of Dikin criterion of IPM convergence { 1e-5 }
    DF,   // Threshold of Karpov' criteria for phase stability { 0.01 }
    DFM,  // Threshold for Karpov' criteria for insertion of phases { -0.1 }
    DFYw, // N of moles of H2O-solvent for phase insertion { 2e-5 }
    DFYaq,// N of moles of aqueous DC for phase insertion { 1e-6 }
    DFYid,// N of moles of DC in ideal solution for phase insertion { 1e-6 }
    DFYr, // Cutoff moles of major DC in non-ideal solution { 1e-6 }
    DFYh, // Cutoff moles of junior DC in non-ideal solution { 1e-6 }
    DFYc, // N of moles for insertion of single-component phase { 1e-6 }
    DFYs, // Moles of DC to boost phases-solutions (SELEKT2) { 1e-6 }
    DB,   // Cutoff number of moles of IC in the system 1e-9
    AG,   //Smoothing parameter for free energy increments on iterations {0.7}
    DGC,  // Power to raise Pa_AG on steps of smoothing { 0.07 }
    GAR,  // Initial activity coefficient of major DC in a phase (FIA) { 1 }
    GAH,  // Initial activity coefficient of minor DC in phase (FIA) { 1000 }
    GAS,  // IPM-2 precision factor (DHB[i] / b[i]) { 1e-3 }
    DNS,  // Default standard surface site density 1/nm2 (12.05)
    XwMin, // Cutoff N of moles of water in aqueous phase { 1e-5 }
    ScMin, // Cutoff N of moles of main DC in solid carrier for sorption {1e-7}
    DcMin, //  Cutoff N of moles of solute (sorbate) { 1e-19 }
    PhMin, // Cutoff N of moles of a phase { 1e-12 }
    ICmin, // Cutoff value of ionic strength to run DH equation { 1e-5 }
    EPS,   // Precision of FIA by SIMPLEX-method { 1e-15 }
    IEPS,  // Accuracy of cutoff limits 1+eps for surface activity terms {1e-8}
    DKIN; // Range of lower to upper kinetic restrictions for DC { 0.001 }
    char *tprn;       // internal

    void write(GemDataStream& oss);
    void read(GemDataStream& oss);
};

struct SPP_SETTING
{   // Base Parametres of SP
    char ver[TDBVERSION]; // Version & Copyright 64
    BASE_PARAM p; // Flags and thresholds for numeric modules
    char           // default codes of values
    DCpct[7],      // Default DCOMP flags and codes
    DCpdc[10],     // Default DCOMP class and units
    BCpc[7],       // Default COMPOS configuration
    REpct[7],      // Default REACDC flags and codes

    REpdc[7],      // Default REACDC class and units
    REpvc[9],      // Default REACDC configuration
    RPpdc[11],      // Default RTPARM flags and codes
    RPpvc[33],     // Default RTPARM configuration  reserved
    PHsol_t[7],    // Default PHASE model codes
    PHpvc[7],      // Default PHASE configuration
    MUpmv[11],     // Default RMULTS configuration
    TPpdc[9],      // Default class and units for MTPARM
    TPpvc[21],     // Default MTPARM configuration
    SYppc[11],     // Default class and flags for SYSTEM
    SYpvc[29],     // Default SYSTEM confifuration
    UTppc[11],     // Default DUTERM class and flags
    PEpsc[13],     // Default PROCES class and flags
    PEpvc[13],     // Default PROCES configuration
    GDcode[2][20], // Default names of screen and graphs in GTDEMO ????
    GDpsc[7],      // Default names of lines on GTDEMO graphs
    GDpcc[2][9],   // Default axis names for GTDEMO
    GDptc[7],      // Default GTDEMO configuration
    GDpgw[7],      // Default setup of graphs in GTDEMO
    SDrefKey[32],  // sdref key
    Reserv[50-32]    // (reserved) objects later
    ;
    // for RTPARM
    short NP,NT,  // Default N of points (RTPARM): P, T
    NV,       // reserved
    Mode,     // Default indexation mode RTPARM
    ResShort[5];
    float        // RTPARM
    Pi[3],    // Default interval for pressure
    Ti[3],    // Default interval for temperature, C
    Vi[3],    // Default interval for volume, cm3
    DRpst, DRtcst,   // Default Pr, Tr for DCOMP & REACDC
    lowPosNum, // MULTI Cutoff moles of DC (Ls set) { 1e-19 };
    logXw,     // log(1e-16)
    logYFk,    // log(1e-9)
    aqPar[4],  // b_gamma, a0, NeutPolicy, I_max for auto aq phase model
    ResFloat   // one parameter for auto gas/fluid phase
    ;

    void write(GemDataStream& oss);
    void read(GemDataStream& oss);
};


struct CompItem
{
    int line;
    short delta; // RpnItemType
    CompItem(int nLine, short nDelta):
            line(nLine), delta(nDelta)
    {}

};

// Module TParam (RMULTS+MTPARM+SYSTEM+MULTY see SubModules)
class TProfil : public TCModule
{
    TRMults* rmults;
    TMTparm* mtparm;
    TSyst* syst;
    TMulti* multi;

    bool newRecord;
//    int pll;
//    double FXold;

    // to compare with old Project
    bool comp_change_all;
    char (*SFold)[PH_RKLEN];// List of PHASE definition keys [0:Fi-1]             DB
    char (*SMold)[DC_RKLEN];// List of DC definition keys (DCOMP, REACDC) [0:L-1] DB
    char (*SAold)[BC_RKLEN];// List of COMPOS definition keys [0:La-1]            DB
    char (*SBold)[IC_RKLEN];// List of ICOMP record keys (stoichiometry basis)[0:N-1] DB
    short *Llold;// L1 vector, shows a number of DC included to each phase [0:Fi-1] DB
    short Nold,     // N of IC, incl. Zz (charge) and Vol (volume)
    Lold,       // L   - of DC - total for all phases
    Fiold,      // FI  - total number of phases
    Fisold,     // FIs - total number of multi-component phases
    Laold,      // La  - of references to COMPOS records
    Lsold;      // Ls  - total number of DC in multi-component phases

    // data to load SysEq <project>:G:z_cp_config:0:0:1:25:0
    bool  isSysEq;
    TCIntArray DCon;
    TCIntArray PHon;
    TCIntArray DCoff;
    TCIntArray PHoff;

protected:

    void InitFN( const char * prfName, const char* prfTemplate );
    void RenameFN( const char * prfName, const char* prfTemplate );
    bool GetFN( const char * prfName, bool show_dlg=true );
    void SetFN();
    bool rCopyFilterProfile( const char * prfName );

    void OpenProfileMode(
        const char* key, bool changeAqGas, bool addFile );
    bool NewProfileMode( bool remakeRec, gstring& key_templ );
    bool NewProfileModeElements( bool remakeRec, gstring& key_templ );
    void SaveOldList();
    void DeleteOldList();
    void TestChangeProfile();
    void Push( TIArray<CompItem>& aList, int aLine,
               short aDelta, const char* dbKeywd, gstring aKey );
    void ICcompare( TIArray<CompItem>& aIComp);
    void COMPcompare( TIArray<CompItem>& aCompos);
    void DCcompare( TIArray<CompItem>& aList, int& i,int& j, int nI, int nJ);
    void PHcompare( TIArray<CompItem>& aPhase, TIArray<CompItem>& aDComp);

    // multi load
    short BAL_compare();

public:

    static TProfil* pm;

   bool userCancel;
   bool stepWise;
   bool calcFinished;
   const char * status;

    bool useAqPhase;
    bool useGasPhase;

// temporary !Use_mt_mode
    bool fStopCalc;

    RMULTS* mup;
    MTPARM *tpp;
    SYSTEM *syp;
    MULTI *pmp;

    SPP_SETTING pa;

    TProfil( int nrt );
    void InitSubModules();

    const char* GetName() const
    {
        return "Project";
    }

    void ods_link(int i=0);
    void dyn_set(int i=0);
    void dyn_kill(int i=0);
    void dyn_new(int i=0);
    void set_def(int i=0);
    void DeleteRecord( const char *key, bool errinNo=true );
    void CmHelp();    // 05.01.01

    // work with Project
    bool initCalcMode();
    void loadSystat( const char *key=0 );
    void newSystat( int mode );
    void deriveSystat();
    void PackSystat();
    void CalcEqstat( bool show_progress=true );
    int  indDC( int );
    int  indPH( int );

    // Multi make functions
    void PMtest( const char *key );
    void LoadFromMtparm(double T, double P,double *G0,
        double *V0, double *H0, double *S0, double *Cp0, double &roW, double &epsW );
    void CalcBcc(); // Calc bulk composition
    void ShowPhaseWindow();
    void ShowEqPhaseWindow();
    void ShowDBWindow( const char *objName, int nLine=0 );

    // Proces make functions
    void ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
     tget_ndx *get_ndx = 0 )
     { multi->ET_translate( nOet, nOpex, JB, JE, jb, je, get_ndx); }
    double MolWeight( int N, float *ICaw, float *Smline )
     { return syst->MolWeight( N, ICaw, Smline ); }

   //test
   void outMulti( GemDataStream& ff, gstring& path  );
   void outMulti( gstring& path  );
   void outMulti();
   void readMulti( GemDataStream& ff );
   void calcMulti();
};

/* Work codes of surface site types in pm->AtNdx vector (compatibility with old-style SCMs *
enum SurTypeNdx {
    AT_SA0=0, AT_SB0=0, AT_SA1=1, AT_SB1=1, AT_SA2=2, AT_SB2=2, AT_SA3=3,
    AT_SB3=3, AT_SA4=4, AT_SB4=4, AT_SA5=5, AT_SB5=5,
    MSPN = 2   * Max number of EDL planes considered in old-style SCMs *
}; */

/* Work DC classifier codes  pm->DCCW */
enum SolDCodes {
    DC_SINGLE = 'U',        /* This DC is a single-component phase */
    DC_SYMMETRIC = 'I',     /* This DC is in symmetric solution phase */
    DC_ASYM_SPECIES = 'S', /*This is DC-solute(sorbate) in asymmetric phase */
    DC_ASYM_CARRIER = 'W'  /*This is carrier(solvent) DC in asymmetric phase*/
};

#endif  // _m_param_h


