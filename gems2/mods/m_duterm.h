//-------------------------------------------------------------------
// Id: gems/mods/m_duterm.h  version 2.0.0   2001
// To be finalized in Version 4.0 (2005)
// Declaration of TDuterm class, config and calculation functions
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

#ifndef _m_duterm_h_
#define _m_duterm_h_

#include "v_ipnc.h"
#include "v_mod.h"
#include "v_module.h"

const int DU_RKLEN = 62;

typedef struct
{ // Description  DUTERM
    char // stkey[EQ_RKLEN],    // "Parent EQSTAT record key"
    // NUV[MAXNV], nchz,   // "Variant # of DUTERM task "

    PvDC,  // "Use DC included into RMULTS (+ - *)?
    PvAC,  // Enter additional DC (compounds) (+ - *)?
    PvSol, // Are there DC - solutes (+ -)?
    PvDis, // Are there DC of dispersed phases (+ -)?
    PvSur, // Are there DC of sorption phases (+ -)?
    PvKin, // Are there kinetic DC (+ -)?
    PvYmS, // Calculate deviations to measured data (+ -)?
    PvSd,  // Include references to data sources (+ -)?
    PvEqn,   // Process user-defined IIPN equations (+ -)?
    PvPhas,  //  Use data from existing PHASE description (+ -)? (reserved)"
    name[MAXFORMULA],  // "Name of DUTERM task"
    notes[MAXFORMULA], // "Comments"
    phkey[PH_RKLEN]    // "Key of description of a PHASE prototype"
    ;
    short Ld, // "Number of DC from RMULTS (Ld)
    La,   // Number of new (additional) DC (La)
    L,    // Total number of compounds (L=Ld+La)
    Nsd,  // Number of refs to data sources
    NV_,  // Variant number of MTPARM record to update
    Nr,   // Number of IC used in this task
    Ncq,  // Number of elements in the IIPN work array
    phf;  // Index of a phase-prototype from RMULTS (reserved)"
    float
    IS[2],  // "Ionic strength: I experimental, I calculated"
    pH[2],  // "pH measured,  pH calculated"
    Eh[2],  // "Eh measured,  Eh calculated"
    YmS[2], // "Functional of deviations for DC, Functional of deviations for IC"
    T, P, V,  // "State factors T,P,V "
    Mwat,     // "Concentration parameters: Mwat - H2O-solvent, kg
    Msys,     // Msys - total mass of the system, kg
    Maq,      // Maq - mass of aqueous phase, kg
    R1,       // MOL - total number of moles
    Vsys,     // Vsys - total volume of the system, L
    Vaq,      // Vaq - volume of aqueous phase, L
    Res;      // WmC - molar mass of carrier (for sorption), g"

    short *llf, // RMULTS-indices of selected DC [0:Ld-1]
    *nnf  // RMULTS-indices of selected IC [0:Nr-1]
    ;
    char (*SDM)[DC_RKLEN], //"Keys (names) of DC included into DUTERM" [0:L-1]
    *DDF,       //"Chemical formulae of DC-compounds (,comma-delimited)(La+Ld)
    *DCC,       // "Classifier of DC (compounds)" [0:L-1]
    *DCS, //"Source of DC formulae: d-DCOMP r-REACDC n-newly added f-fictive"[L]
    *UtRes,//"Codes of I/O DUTERM params for each DC { GZCMKDUEASNPYF }"[L]
    *UnE,       // "Units of measurement for energy values" [0:L-1]
    *UnWx,      // "Units of measurement for DC concentrations"  [0:L-1]
    *UnICm,     // "Units of measurement for IC concentrations" [0:Nr-1]
    (*SBM)[MAXICNAME+MAXSYMB],// "Keys (names) of IC" [0:Nr-1]
    *Expr       // "Text with IPN expressions for additional calculations"
    ;
    double *MU, //mu[j] - DC chemical potentials calculated from u[i] and a[j,i][L]
    *gT0,// "Initial part.mol. g0(TP) values for DC" (->UnE)   [0:L-1]
    *gTP,// "Output part.mol. g0(TP) or difference values for DC" [0:L-1]
    *gEx,// "Output excess part.mol.Gibbs energy for DC"(UnE) [0:L-1]
    *gExK,// "Apparent part.mol. Gibbs energy of DC metastability" (UnE)[L]
    *UIC, // "u[i] - IC chemical potentials, normalized" (->UnE) [0:Nr-1]
    *qp   // "Work array for IPN calculations" (->Ncq)
    ;
    float  *WX, // "DC concentrations calculated (according to DC class)" [0:L-1]
    *CX, // "DC concentrations measured (that of aq counterpart for sorbates)"[0:L-1]
    *Gam,// "DC (surface) activity coefficients, input or output" [0:L-1]
    *Psi,  // "Surface Galvani potential, mV (for sorption)" [0:L-1]
    *Nsph, // "Total surface site density, 1/nm2" [0:L-1]
    *Nsig, // "Fraction of surface site types, >0 <1" [0:L-1]
    *Area, // "Specific surface area, m2/g (dispersed phases)" [0:L-1]
    *Sigm, //"Stand.spec.surface energy in water, J/m2 (dispersed phases)"(->UnE)[0:L-1]
    *WArm, // "Weight multipliers for Wx-Cx deviations" [0:L-1]
    *YArm, // "Calculated deviations (^2)  Wx-Cx"  [0:L-1]
    *ErrEa,// "Absolute experimental errors for IC"(->UnICm) [0:Nr-1]
    *ICmE, // "Total dissolved IC concentrations, empirical"(->UnICm)[0:Nr-1]
    *ICmC, // "Total dissolved IC concentrations, calculated"(->UnICm)[0:Nr-1]
    *ImE,  // "Concentration of free ion for dissolved IC"(->UnICm)[0:Nr-1]
    *ImA,  // "Degree of complexation for dissolved IC, %" [0:Nr-1]
    *Wrm,  // "Weight multipliers for ImE-ImC deviations"  [0:Nr-1]
    *Yrm   // "Calculated deviations (^2) ImE-ImC" [0:Nr-1]
    ;
    char (*sdref)[V_SD_RKLEN], // "List of bibl. refs to data sources" [0:Nsd-1]
    (*sdval)[V_SD_VALEN]  // "Parameters taken from the respective data sources"[0:Nsd-1]
    ;
    /* Work arrays */
    float *A;   // "Work stoichiometry matrix A"
    char tpkey[TP_RKLEN],    // "Key of MTPARM record to be modified"
    *tprn;              // "internal"
}

DUTERM;

/* Описание рабочей структуры пересчета DUTERM */
typedef struct
{
    double RT,      /* множитель RT */
    F,       /* константа Фарадея */
    Nu,      /* Двойственный хим.потенциал ЗК (через u, A )*/
    Mu,      /* Прямой хим.потенциал ЗК (через Mu0 и активность) */
    G0,      /* Стандартный part.mol. хим.потенциал для TP == Mu0 */
    lnAx,    /* ln активности ЗК */
    Ax,    /* Активность ЗК (в соотв. системе сравнения) */
    dGex,    /* парц. мольн. энергия метастабильности (кинет.) */
    Gdis,    /* парц. мольн. своб. энергия дисперсности носителя */
    Asig,  /* удельная своб. энергия поверхности носителя Дж/м2 */
    Asur,  /* удельная площадь поверхности носителя м2/г */
    Amw,   /* мольная масса носителя г/моль */
    Gid,     /* парц. мольн. своб.энергия идеального смешения */
    lnCx,  /* log( Cx ) */
    Cx,    /* мольная доля солюта (сорбата) в фазе */
    Mx,  /* моляльность солюта (сорбата) */
    lnMx, /* ln отношения xj/Xw (для солютов и сорбатов) */
    lnWx,  /* log( Wx ) */
    Wx,    /* мольная доля носителя (сольвента) в фазе */
    Gcas,  /* Член коррекции асимметрии для носителя (сольвента) */
    Gsas,  /* Член коррекции асимметрии для солюта (сорбата) */
    Gsm,   /* Член перевода в моляльную шкалу log(1000/WH2O) 4.01653 */
    Gsd,   /* Корректирующий член для сорбата на поз. р-го типа */
    Tetp, /* мольная доля позиций р-го типа от их общего числа */
    Nx,   /* мольная доля всех сорбатов (позиций) ко всей фазе */
    Agmx, /* плотность активных позиций моль/м2 */
    Agx, /* плотность активных позиций, 1/нм2 */
    Gex,     /* парц.мольн.избыточная своб.энергия неидеальности */
    Gamma, /* коэф. активности (фугитивности) */
    lnGam, /* ln коэф. активности */
    Gpsi,    /* парц. мольн. электрохимическая своб. энергия */
    Ze,    /* заряд ЗК (сорбата, солюта) */
    Psi,   /* Вольта-потенциал ЗК */
    lnPg;    /* ln P общ для газов */
    /* 32 double - числа */
}
DUTERMSET;

// Current Duterm
class TDuterm : public TCModule
{
    DUTERM ut[1];
    IPNCalc rpn;       // IPN of equats of process  -- Expr
    DUTERMSET dt;

protected:

    void keyTest( const char *key );
    void ut_text_analyze();
    double dut_lnAx_calc( char DCC );
    double dut_Mu_Nu_calc( char DCC );
    double dut_G0TP_calc( char DCC );
    double dut_Gexk_calc( char DCC );
    double dut_Gdis_calc( char DCC );
    double dut_Asur_calc( char DCC );
    double dut_Asig_calc( char DCC );
    double dut_Gex_calc( char DCC );
    double dut_sGam_calc( char DCC );
    double dut_Cx_calc( char DCC );
    double dut_Nsur_calc( char DCC );
    double dut_Psi_calc( char DCC );
    double DU_Reduce_Conc( char DCC, char UNITP, double Xe, double DCmw,
                           double Vm, double R1, double Msys, double Mwat, double Vaq, double Maq,
                           double Vsys );
    void ut_tp_insert();
    void duterm_calc();
    double PrimeChemPot( double G, double logY, double logYF,
                         double asTail, double logYw,  char DCCW );
public:

    static TDuterm* pm;

    DUTERM *utp;

    TDuterm( int nrt );

    const char* GetName() const
    {
        return "Duterm";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    bool check_input( const char *key, int level=1 );
    gstring   GetKeyofRecord( const char *oldKey, const char *strTitle,
                              int keyType );

    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void CmHelp();
};

enum duterm_rescode {  /* Коды результата расчета DUTERM 11.6.96 DAK
                          Сх - эмпирические, Wx - расчетные концентрации  */
    DUT_MU_NU = 'M',     /* Расчет Mu и разности хим. потенциалов */
    DUT_G0TP = 'G',      /* Расчет g0(T,P) по u, Cx, gamma, dGex       */
    DUT_GETPX = 'Z',     /* Расчет RT*ln(gamma) по g0(T,P), u, Cx, dGex */
    DUT_GEKIN = 'K',     /* Расчет dGex(metast) по g0, u, Cx(x), gamma */
    DUT_CONDC = 'C',     /* Расчет Wx по g0(T,P), dGex, u, gamma       */
    DUT_DEVDC = 'D',     /* Расчет Wx как для CONDC и ^2 невязок Wx-Cx */
    DUT_DEVIC = 'U',     /* Расчет ^2 невязок mE-mC для подмн. НК */
    DUT_DEVAC = 'E',     /* Расчет степени комплексации для НК в aq */
    DUT_DISG = 'R',      /* Расчет св. энергии дисперсности частиц */
    DUT_DISAREA = 'A',   /* Расчет Asur по g0,u,Cx,Sigma,gamma,Nsur,Psi*/
    DUT_DISSIG = 'S',    /* Расчет Sigma по g0,u,Cx,Asur,gamma,Nsur,Psi*/
    DUT_SURNSG = 'N',    /* Расчет Nsur по g0,u,Cx,Sigma,Asur,gamma,Psi*/
    DUT_SURPSI = 'P',    /* Расчет Psi по g0,u,Cx,Sigma,Asur,gamma,Nsur*/
    DUT_SURAC = 'Y',     /* Расчет Sur gamma по g0,u,Cx,Sigma,Asur,Nsur,Psi*/
    DUT_EUSER = 'F'      /* Расчет любых парам. по пользов. формуле IIPN
                                           gamma по Cx можно считать в IIPN всегда */
};

#endif //_m_duterm_h
