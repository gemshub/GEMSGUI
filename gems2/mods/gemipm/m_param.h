
#ifndef _m_param_h_
#define _m_param_h_

#ifndef IPMGEMPLUGIN
 #define IPMGEMPLUGIN
#endif

#include "ms_multi.h"
#include "verror.h"

// Physical constants - see m_param.cpp
extern const double R_CONSTANT, NA_CONSTANT, F_CONSTANT,
    e_CONSTANT,k_CONSTANT, cal_to_J, C_to_K, lg_to_ln, ln_to_lg;
//

struct BASE_PARAM
{ // Flags and thersholds for numeric modules
    short PC,    // "Mode of Selekt2() subroutine operation"
    PD,// Mode of DHH():0-invoke,1-at FIA only,2-last IPM it. 3-every IPM it.
    PRD,//Mode GammaCalc(): 0-inactive, 1-FIA only, 2-SELEKT2 only 3-every IPM it.
    PSM,//  Mode of setting FIA Gamma in phases: 0-off; 1 - set activ.coeff. GAN
    DP, //  Max number of EnterFeasibleDomain() iterations { 100 }
    DW, // IPM-2 precision mode: 0- disable 1-enable
    DT, // Restore x from dual solution {0 1}( temporary instead FGL )
    PLLG, // IPM view debug time on Progress Dialog screen from (sec) or 0
    PE, // Include electroneutrality condition? { 0 1 }
    IIM // Maximum number of iterations in main IPM { 200 }
    ;
    double DG,   // Precision of LEM IPM solver (Jordan) { 1e-15 }
    DHB, // Precision of mass balance deviations at EFD { 1e-8 }
    DS,  // Cutoff number of moles of a phase { 1e-10 }
    DK,  // Threshold of Dikin criterion of IPM convergence { 1e-5 }
    DF,   // Threshold of Karpov' criteria for phase stability { 0.01 }
    DFM,  // Threshold for Karpov' criteria for insertion of phases { -0.1 }
    DFYw, // N of moles of H2O-solvent for phase insertion { 2e-5 }
    DFYaq,// N of moles of aqueous DC for phase insertion { 1e-7 }
    DFYid,// N of moles of DC in ideal solution for phase insertion { 1e-6 }
    DFYr, // Cutoff moles of major DC in non-ideal solution { 1e-5 }
    DFYh, // Cutoff moles of junior DC in non-ideal solution { 1e-8 }
    DFYc, // N of moles for insertion of single-component phase { 1e-6 }
    DFYs, // Moles of DC to boost phases-solutions (SELEKT2) { 1e-12 }
    DB,   // Cutoff number of moles of IC in the system 1e-9
    AG,   //Smoothing parameter for free energy increments on iterations {0.7}
    DGC,  // Power to raise Pa_AG on steps of tinkle-supressor { 0.07 }
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

    void write(ostream& oss);
};

struct SPP_SETTING
{   // Base Parametres of SP
    char ver[TDBVERSION]; // Version & Copyright 64
    BASE_PARAM p; // Настройки режимов счета (задаются пользователем))
/*
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
    ResFloat[5]
    ;
*/
    void write(ostream& oss);
};


// Module TParam (RMULTS+MTPARM+SYSTEM+MULTY see SubModules)
class TProfil //: public TCModule
{

    int pll;
    double FXold;

protected:

    // multi load
    short BAL_compare();


    void multi_sys_dc();
    void multi_sys_ph();
    void ph_sur_param( int k, int kk );
    void ph_surtype_assign( int k, int kk, int jb, int je,
                            short car_l[], int car_c, short Cjs );
    void ConvertDCC();
    double Cj_init_calc( double g0, int j, int k );
    void sm_text_analyze( int nph, int Type, int JB, int JE, int jb, int je );
    gstring PressSolMod( int nP );
    char *ExtractEG( char *Etext, int jp, int *EGlen, int Nes );
    int find_icnum( char *name, int LNmode );
    int find_dcnum( char *name, int jb, int je, int LNmode );
    int find_phnum( char *name, int LNmode );
    int find_acnum( char *name, int LNmode );
    //   void ET_translate( int nOet, int nOpex, int JB, int JE, int jb, int je );
    void CompG0Load();
    int Set_DC_limits( int Mode );
    void ConCalc( double X[], double XF[], double XFA[]);
    void Mol_u( double Y[], double X[], double XF[], double XFA[] );
    double DualChemPot( double U[], float AL[], int N );
    void phase_bcs( int N, int M, float *A, double X[], double BF[] );
    void ConCalcDC( double X[], double XF[], double XFA[],
                    double Factor, double MMC, double Dsur, int jb, int je, int k );
    double pH_via_hydroxyl( double x[], double Factor, int j);
    void TotalPhases( double X[], double XF[], double XFA[] );
    //   void eDmb( int N, int L, float *A, double *Y, double *B, double *C );
    void IS_EtaCalc();
    void GouyChapman(  int jb, int je, int k );
    void GasParcP();

    // ipm_gamma
    void SurfaceActivityTerm( int jb, int je, int k );
    double Ej_init_calc( double YOF, int j, int k);
    void PrimeChemicalPotentials( double F[], double Y[], double YF[], double YFA[] );
    double  PrimeChemPot(  double G,  double logY,  double logYF,
                           double asTail,  double logYw,  char DCCW );
    void f_alpha();
    double KarpovCriterionDC( double *dNuG, double logYF, double asTail,
                              double logYw, double Wx,  char DCCW );
    double FreeEnergyIncr(   double G,  double x,  double logXF,
                             double logXw,  char DCCW );
    double GX( double LM  );
    void pm_GC_ods_link( int k, int jb, int jpb, int jdb );
    double TinkleSupressFactor( double ag, int ir);
    void DebyeHueckel3HelKarp( int jb, int je, int jpb, int jdb, int k );
    void Davies03temp( int jb, int je, int k ); // added by KD 25.01.02
    void GammaCalc( int LinkMode );
    // ipm_fia_bc
    void MassBalanceDeviations( int N, int L, float *A, double *Y, double *B, double *C );
    void SimplexInitialApproximation( );
    void Simplex(int M, int N, int T, double GZ, double EPS,
                 double *UND, double *UP, double *B, double *U,
                 double *AA, int *STR, int *NMB );
    void SPOS( double *P, int STR[],int NMB[],int J,int M,double AA[]);
    void START( int T,int *ITER,int M,int N,int NMB[],
                double GZ,double EPS,int STR[],int *BASE,
                double B[],double UND[],double UP[],double AA[],double *A,
                double *Q );
    void NEW(int *OPT,int N,int M,double EPS,double *LEVEL,int *J0,
             int *Z,int STR[], int NMB[], double UP[],
             double AA[], double *A);
    void WORK(double GZ,double EPS,int *I0, int *J0,int *Z,int *ITER,
              int M, int STR[],int NMB[],double AA[],
              int BASE[],int *UNO,double UP[],double *A,double Q[]);
    void FIN(double EPS,int M,int N,int STR[],int NMB[],
             int BASE[],double UND[],double UP[],double U[],
             double AA[],double *A,double Q[],int *ITER);
    // ipm_sel_bc
    int Gordan( int N, double DG, double *A, double *X );
    int SquareRoots( int N, double *R, double *X, double *B );
    // int RISLUR( int N, double Eps, double *A, double *X, double *F );
    // void NBM( int N, int L, float *A, double *Y, double *B, double *C);
    void PhaseSelect( );
    int EnterFeasibleDomain( );
    //   void PhaseListPress( );
    double LMD( double LM );
    int InteriorPointsIteration( );
    int InteriorPointsMethod( );
    void Set_z_sp_config( const char *profil );
    void CopyF( const char * fName, const char* fTempl );

public:


    TMulti* multi;
    MULTI *pmp;

    SPP_SETTING pa;

    TProfil();

    const char* GetName() const
    {
        return "Project";
    }


    // Multi make functions
    void PMtest( const char *key );
    void SolModLoad();
    void XmaxSAT_IPM2();
    void XmaxSAT_IPM2_reset();
    void MultiRemake( const char *key );
    void EqstatExpand( const char *key );
    void CalcBcc(); // Calc bulk composition
    // MultiCalc
    void MultiCalcInit( const char *key );
    bool AutoInitialApprox();
    void MultiCalcIterations();
    bool MultiCalcMain( int &pll, double &FXold );
    double pb_GX( double *Gxx  );

   //test
   void outMulti( fstream& ff )
   {
      ff.write ((char*)&pa.p, sizeof(BASE_PARAM));
      multi->to_file( ff );
   }

   void readMulti( fstream& ff )
   {
      ff.read ((char*)&pa.p, sizeof(BASE_PARAM));
      multi->from_file( ff );
   }

   void calcMulti();

};

/* Work codes of surface site type indices in pm->AtNdx vector */
enum SurTypeNdx {
    AT_SA0 = 0, AT_SB0, AT_SA1, AT_SB1, AT_SA2, AT_SB2, AT_SA3, AT_SB3,
    AT_SA4, AT_SB4, AT_SA5, AT_SB5, MSPN = 2
                    /* Max number of EDL planes considered */
};

/* Work DC classifier codes  pm->DCCW */
enum SolDCodes {
    DC_SINGLE = 'U',        /* This DC is a single-component phase */
    DC_SYMMETRIC = 'I',     /* This DC is in symmetric solution phase */
    DC_ASYM_SPECIES = 'S', /*This is DC-solute(sorbate) in asymmetric phase */
    DC_ASYM_CARRIER = 'W'  /*This is carrier(solvent) DC in asymmetric phase*/
};

#endif  // _m_param_h


