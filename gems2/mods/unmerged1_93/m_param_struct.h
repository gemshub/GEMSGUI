#ifndef _m_param_struct_h_
#define _m_param_struct_h_

#include <iostream.h>


struct BASE_PARAM
{// Настройки режимов счета (задаются пользователем))
    short PC,    // "Mode of Selekt2() subroutine operation"
    PD,// Mode of DHH():0-invoke,1-at FIA only,2-last IPM it. 3-every IPM it.
    PRD,//Mode GammaCalc(): 0-inactive, 1-FIA only, 2-SELEKT2 only 3-every IPM it.
    PSM,//  Mode of setting FIA Gamma in phases: 0-off; 1 - set activ.coeff. GAN
    DP, //Flag for pressure calculation { 0 1 } reserved
    DW, // Flag to save MULTI data structure to PDB record { 0-no 1-wrk 2-act }
    DT, //IPM view debug time on EQCALC screen from (sec) or 0
    PLLG, // IT # at OFIA  { 0 1 } (temporary)
    PE, // Include electroneutrality condition? { 0 1 }
    IIM // Maximum number of iterations in IPM task { 400 }
    ;
    double DG,   // Precision of LEM IPM solver (Jordan) { 1e-12 }
    DHB, // Precision of mass balance deviations at EFD { 1e-6 }
    DS,  // Cutoff number of moles of a phase { 1e-8 }
    DK,  // Threshold of Dikin criterion of IPM convergence { 1e-4 }
    DF,  // Threshold of Karpov' criteria for phase stability { 0.01 }
    DFM, //Threshold for Karpov' criteria for insertion of phases { -0.1 }
    DFYw,// N of moles of H2O-solvent for phase insertion { 2e-5 }
    DFYaq,// N of moles of aqueous DC for phase insertion { 1e-7 }
    DFYid,// N of moles of DC in ideal solution for phase insertion { 1e-6 }
    DFYr, // Cutoff moles of major DC in non-ideal solution { 1e-5 }
    DFYh, // Cutoff moles of junior DC in non-ideal solution { 1e-8 }
    DFYc, // N of moles for insertion of single-component phase { 1e-6 }
    DFYs, // Moles of DC to boost phases-solutions (SELEKT2) { 1e-12 }
    DB,   // Cutoff number of moles of IC in the system 1e-9
    AG,   //Smoothing parameter for free energy increments on iterations {0.7}
    DGC,  // Power to raise Pa_AG on steps of tinkle-supressor { 0.1 }
    GAR,  // Initial activity coefficient of major DC in a phase (FIA) { 1 }
    GAH,  // Initial activity coefficient of minor DC in phase (FIA) { 1000 }
    GAS,  // reserved
    DNS,  // Default standard surface site density 1/nm2 (12.2)
    XwMin, // Cutoff N of moles of water in aqueous phase { 1e-5 }
    ScMin, // Cutoff N of moles of main DC in solid carrier for sorption {1e-7}
    DcMin, //  Cutoff N of moles of solute (sorbate) { 1e-16 }
    PhMin, // Cutoff N of moles of a phase { 1e-10 }
    ICmin, // Cutoff value of ionic strength to run DH equation { 1e-5 }
    EPS,   // Precision of FIA by SIMPLEX-method { 1e-6 to 1e-9 }
    IEPS,  // Accuracy of cutoff limits 1+eps for surface activity terms {1e-8}
    DKIN; // Range of lower to upper kinetic restrictions for DC { 0.001 }
    char *tprn;       // internal

    void write(ostream& oss);
};

struct SPP_SETTING
{   // Base Parametres of SP
    char ver[TDBVERSION]; // Version & Copyright 64
    BASE_PARAM p; // Настройки режимов счета (задаются пользователем))
    char           // default codes of values
    DCpct[7],      // Default DCOMP flags and codes
    DCpdc[10],     // Default DCOMP class and units
    BCpc[7],       // Default COMPOS configuration
    REpct[7],      // Default REACDC flags and codes 
    REpdc[7],      // Default REACDC class and units
    REpvc[9],      // Default REACDC configuration
    RPpdc[7],      // Default RTPARM flags and codes
    RPpvc[37],     // Default RTPARM configuration
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
    Reserv[50]    // (reserved) objects later
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

    void write(ostream& oss);
};


#endif
