//-------------------------------------------------------------------
// $Id$
//
// Declaration of TProfil class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2007 S.Dmytriyeva, D.Kulik
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
// Uses: GEM-Vizor GUI DBMS library, gems2/lib/gems_qt.lib
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
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
class QWidget;

// Physical constants - see m_param.cpp
extern const double R_CONSTANT, NA_CONSTANT, F_CONSTANT,
    e_CONSTANT,k_CONSTANT, cal_to_J, C_to_K, lg_to_ln, ln_to_lg;
extern long int showMss;

//

struct BASE_PARAM
{ // Flags and thresholds for numeric modules
  short
    PC,   // Mode of PhaseSelect() operation ( 0 1 2 ... ) { 1 }
    PD,   // Mode of execution of the built-in DebyeHueckel()/Davies() functions { 3 }
          // Mode of DHH():0-invoke,1-at FIA only,2-last IPM it. 3-every IPM it.
    PRD,  //  Negative number (from -1 to -50): the number |PRD| of additional full IPM-2 loops 
          //  to improve the GEM final solution, else  no additional loops, { 3 }
    PSM,  // Level of diagnostic messages: 0- disabled (no ipmlog file); 1- normal; 2-including warnings { 1 }
    DP,   // Maximum allowed number of iterations in the EnterFeasibleDomain() procedure {  150 }
    DW,   // Maximum number of allowed IPM-2 mass balance accuracy improvement loops { 0- disable >=1  - enable ; default 15}
    DT,   // IPM-2 cutoff exponent to recover small elements of the primal x vector using the dual solution vector u { -3 }
    PLLG, // TIPM-2 tolerance for checking change in dual solution among GEM refinement loops { 200 }
          //      { 0 to 1000 mol/mol, default 0 or 32000 means no check }
    PE,   // Flag for using electroneutrality condition in GEM IPM calculations { 0 1 }
    IIM   // Maximum allowed number of iterations in the MainIPM_Descent() procedure { 500 }
    ;
  double DG,   // Threshold for minimum descent step size Lambda in EntryFeasibleDomain() { 1e-5 }
    DHB,  // Maximum allowed mass balance residual (moles) for major Independent Components { 1e-8 }
    DS,   // Cutoff minimum mole amount of stable Phase present in the IPM primal solution { 1e-12 }
    DK,   // IPM-2 convergence threshold for the Dikin criterion (may be set in the interval 1e-6 < DK < 1e-3) { 1e-4 }
    DF,   // Threshold for the application of the Karpov phase stability criterion: (Fa > DF) for a lost stable phase { 0.01 }
    DFM,  // Threshold for Karpov stability criterion f_a for insertion of a phase (Fa < -DFM) for a present unstable phase { 0.1 }
    DFYw, // Insertion mole amount for water-solvent { 1e-6 }
    DFYaq,// Insertion mole amount for aqueous species { 1e-6 }
    DFYid,// Insertion mole amount for ideal solution components { 1e-6 }
    DFYr, // Insertion mole amount for major solution components { 1e-6 }
    DFYh, // Insertion mole amount for minor solution components { 1e-6 }
    DFYc, // Insertion mole amount for single-component phase { 1e-6 }
    DFYs, // Insertion mole amount used in PhaseSelect() for a condensed phase component  { 1e-7 }
    DB,   // Minimum amount of Independent Component in the bulk system composition (except charge "Zz") (moles) (1e-17)
    AG,   // Smoothing parameter for non-ideal increments to primal chemical potentials between IPM descent iterations {0.7}
    DGC,  // Exponent in the sigmoidal smoothing function, or minimal smoothing factor in new functions { 0.07 }
    GAR,  // Initial activity coefficient value for major (M) species in a solution phase before Simplex() approximation { 1 }
    GAH,  // Initial activity coefficient value for minor (J) species in a solution phase before Simplex() approximation { 1000 }
    GAS,  // IPM-2 balance accuracy control ratio DHBM[i]/b[i], determines the maximum allowed mass balance residual for minor IC { 1e-3 }
    DNS,  // Standard surface density (nm-2) for calculating activity of surface species (12.05)
    XwMin,// Cutoff mole amount for elimination of water-solvent { 1e-9 }
    ScMin,// Cutoff mole amount for elimination of solid sorbent {1e-7}
    DcMin,// Cutoff mole amount for elimination of solution- or surface species { 1e-20 }
    PhMin,// Cutoff mole amount for elimination of  non-electrolyte solution phase with all its components { 1e-10 }
    ICmin,// Minimal effective ionic strength (molal), below which the activity coefficients for aqueous species are set to 1. { 3e-5 }
    EPS,  // Precision criterion of the simplex() procedure to obtain the automatic initial approximation { 1e-6 to 1e-14, default 1e-7 }
    IEPS, // Convergence parameter of SACT calculation in sorption/surface complexation models { 0.01 to 0.000001, default 0.001 }
    DKIN; // Tolerance on the amount of DC with two-side metastability constraints  { 1e-7 }
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
    aqPar[5];  // b_gamma, a0, NeutPolicy, GamH2O, b_gam_T_dep for auto aq phase model
//    ResFloat;   // one parameter for auto gas/fluid phase

    void write(GemDataStream& oss);
    void read(GemDataStream& oss);
};

extern SPP_SETTING pa_;

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
        const char* key, bool changeAqGas, bool addFile,  bool remakeRec );
    bool NewProfileMode( bool remakeRec, gstring& key_templ );
    bool NewProfileModeElements( bool remakeRec, gstring& key_templ );
    void CalcAllSystems(int makeDump);
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
TMulti *pmulti;

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
    void MakeQuery();
    void CmHelp();    // 05.01.01

    // Setup one of 5 default IPM numerical settings
    void ChangeSettings(int nSettings);

    // work with Project
    bool initCalcMode();
    void loadSystat( const char *key=0 );
    void newSystat( int mode );
    void deriveSystat();
    void PackSystat();
    double CalcEqstat( bool show_progress=true );
    int  indDC( int );
    int  indPH( int );
    void  deleteAutoGenerated();

    // Multi make functions
    void PMtest( const char *key );
    void LoadFromMtparm(double T, double P,double *G0,  double *V0, 
    		double *H0, double *S0, double *Cp0, double *A0, double *U0, 
    		double denW[5], double epsW[5], double denWg[5], double epsWg[5], int* tp_mark );
    void CalcBcc(); // Calc bulk composition
    void ShowPhaseWindow();
    void ShowEqPhaseWindow();
    void ShowDBWindow( const char *objName, int nLine=0 );

    // Proces make functions
    void ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je,
     tget_ndx *get_ndx = 0 )
     { multi->ET_translate( nOet, nOpex, JB, JE, jb, je, get_ndx); }
    double MolWeight( int N, double *ICaw, double *Smline )
     { 	return syst->MolWeight( N, ICaw, Smline ); }
    void SyTestSizes()
     { 	syst->SyTestSizes(); }

   //test
   void outMulti( GemDataStream& ff, gstring& path  );
   // brief_mode - Do not write data items that contain only default values
   // with_comments -Write files with comments for all data entries ( in text mode)
   // addMui - Print internal indices in RMULTS to IPM file for reading into Gems back 
   void outMulti( gstring& path, bool addMui, bool with_comments = true, bool brief_mode = false );
   void makeGEM2MTFiles(QWidget* par);
   void outMultiTxt( const char *path, bool append=false  );
   void readMulti( GemDataStream& ff );
   void readMulti( const char* path );
   void CmReadMulti( const char* path );
   double calcMulti( long int& NumPrecLoops, long int& NumIterFIA, long int& NumIterIPM );
   long int testMulti();

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


