//-------------------------------------------------------------------
// $Id$
// To be finalized in Version 2.1 (2004)
// Declaration of TDualTh class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2004 S.Dmytriyeva, D.Kulik
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

#ifndef _m_dualth_h_
#define _m_dualth_h_

#include "m_param.h"
#include "v_ipnc.h"
#include "v_module.h"

const int DT_RKLEN = 80;
const int  MAXIDNAME = 12;
const   int MAXFORMUNITDT=     40;


typedef struct
{ // Description of DualTh data structure (revised in July 2004)
  // Record key format: the same as Proces
    char
PunE,          // Units of energy   { j;  J c C N reserved }
PunV,          // Units of volume   { j;  c L a reserved }
PunP,          //  Units of pressure  { b;  B p P A reserved }
PunT,          // Units of temperature  { C; K F reserved }

// Allocation flags
PvCoul,   // Use coulombic factors for non-basis DCs (+ -)
PvICb,    // Use IC quantities for basis sub-system compositions? { + * - }
PvICn,    // Use IC quantities for non-basis sub-system compositions? { + * - }
PvAUb,    // Use formula units for basis sub-system compositions? { + * - }
PvSd,     // Include references to data sources (+ -)?
PvChi,    // Use math script for mole fractions of non-basis DCs (+ -)?
PvGam,    // Use math script for activity coeffs of non-basis DCs (+ -)?
PvRes,  // Reserved

     // Controls on operation
PsMode,  // DualTh mode of operation { M G A X }
PsSt,    // State of non-basis part saturation { E P S }

//  Status and control flags (+-)
gStat,  // DualTh generation status: 0 -indefinite; 1 on-going generation run;
        //  2 - done generation run; 3 - generation run error (+ 5: the same in stepwise mode)
aStat,  // DualTh analysis status:  0 - indefinite; 1 ready for analysis;
        //  2 - analysis run; 3 - analysis done; ( + 5: the same using stepwise mode)
PsSYd,  // Save generated SysEq records to data base (+ -)
PsRes1,    // reserved
PsRes2,    // reserved
PsRes3,    // reserved

name[MAXFORMULA],  //  "Name of DualTh task"
notes[MAXFORMULA] //  "Comments"
    ;
    short
// input I
nQ,   // Q - number of experiments (equilibria) in basis sub-system
La_b, // Lb - number of formula units to set compositions in basis sub-system
nK,   // K - number of DC (end-member) candidates in non-basis sub-system
Nsd,  // N of references to data sources
Nqpn, // Number of elements in the math script work arrays qpn per non-basis DC
Nqpg, // Number of elements in the math script work arrays qpg per non-basis DC

Nb,    // N - number of independent components (in basis sub-system, taken
        // from project system dimensions automatically)
Asiz,    //reserved
// for generating syseq record keys
    tmd[3],  // SYSTEM CSD definition #: start, end, step (initial)
    NVd[3]  // Restrictions variant #: start, end, step
 ; // to discuss !!!!!!!!!!!!!!
    float
    Pd[3],    // Pressure P, bar: start, end, increment
    Td[3],    // Temperature T, C: start, end, increment
    Vd[3],    // Volume of the system (L): start, end, increment

  // Input I
Msysb, // Masses (kg) and volumes (L) for basis sub-system: Ms (total mass, normalize)
Vsysb, // Vs (total volume of the object, for volume concentrations)
Mwatb, // M(H2O) (mass of water-solvent for molalities)
Maqb,  // Maq (mass of aqueous solution for ppm etc.)
Vaqb,  // Vaq (volume of aqueous solution for molarities)
Pgb,   // Pg (pressure in gas, for partial pressures)
Tmolb, // MOL total mole amount for basis sub-system composition calculations
WmCb,  // molar mass of carrier DC (e.g. sorbent or solvent), reserved
    cT, cP, cV,  // "State factors T,P,V "
Flres    //reserved
    ;
    double
*Bb,   //  [Q][N] Table of bulk compositions of basis sub-systems
*Bn,   //  [Q][N] Table of bulk compositions of non-basis sub-systems
*Ub,   //  [Q][N] Table of dual solution values for basis sub-systems
*chi,   //  [Q][K] Table of mole fractions of DC (end-member) candidates
*mu_n,  //  [Q][K] Table of DualTh chemical potentials of K DC (end-member)
           // candidates in Q experiments
*Coul,  //  [Q][K] Table of Coulombic terms for DC candidates (optional)
*gam_n, //  [Q][K] Table of activity coefficients for DC candidates
*avg_g, //  [K] mean over gam_n columns (experiments) for DC candidates
*sd_g,  //  [K] st.deviation over gam_n columns (experiments) for DC candidates
*muo_n, //  [Q][K] Table of standard Gibbs energies for DC candidates
*avg_m, //  [K] mean over muo_n columns (experiments) for DC candidates
*sd_m,  //  [K] st.deviation over muo_n columns (experiments) for DC candidates
// *act_n,   // [Q][K] table of DualTh-calculated activities
*qpn,   //  [Nqpn] Work array for chi calculation math script
*qpg    //  [Nqpg] Work array for gamma calculation math script
    ;
    float
*CIb, // [Q][N] Table of quantity/concentration of IC in basis sub-systems
*CIn, // [K][N] Table of quantity/concentration of IC in non-basis sub-systems
*CAb, // [Q][Lb] Table of quantity/concentration of formulae for basis sub-systems
*CAn; // [Q][K] Table of quantity/concentration of DC formulae for non-basis sub-systems

char
*cExpr,  // Math script text for calculation of mole fractions of DC in non-basis
*gExpr,  // Math script text for calculation of activity coeffs of DC in non-basis
(*sdref)[V_SD_RKLEN], // "List of bibl. refs to data sources" [0:Nsd-1]
(*sdval)[V_SD_VALEN],  // "Parameters taken from the respective data sources"[0:Nsd-1]
(*nam_b)[MAXIDNAME], // [Q][16] id names of experiments
(*nam_n)[MAXIDNAME], // [K][16] id names of DC (end-member) stoichiometry candidates
(*for_n)[MAXFORMUNITDT], // [K][40] formulae of DC (end-member) stoichiometry candidates
(*for_b)[MAXFORMUNITDT], // [Lb][40] formulae for setting basis system compositions
(*stld)[EQ_RKLEN], // List of SysEq record keys [Q]

*typ_n, // [K] type code of DC stoichiometry candidates {O M J I S ... }
*CIclb, // [N] Units of IC quantity/concentration for basis compositions
*CIcln, // [N] Units of IC quantity/concentration for non-basis compositions
*AUclb, // [Lb] Units of setting quantities of formula units for basis system composition
*AUcln, // [K] Units of setting quantities of DC formulae for non-basis system composition

    (*SBM)[MAXICNAME+MAXSYMB] // "Keys (names) of IC" [0:Nr-1] ??????????

    ;
    /* Work arrays */
float
 *An;  // [K][N] stoich matrix for DC (end-member) stoichiometry candidates
char sykey[EQ_RKLEN+10],    // "Key of currently processed SysEq record"
    *tprn;              // "internal"
//work
short
 q,      // index of experiment
 i,      // index of IC
 jm,     // index of non-basis sub-system component
 c_tm,         // Current Tm - SYSTEM CSD number
 c_NV;         // Current Nv - MTPARM variant number
 char timep[16], TCp[16], Pp[16], NVp[16], Bnamep[16];
}

DUALTH;

/* Work objects for DualTh scripts */
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
DualThSet;

// Current DualTh
class TDualTh : public TCModule
{
    DUALTH dt[1];
    DualThSet dts;

    IPNCalc rpn[2];      // IPN of DualTh

protected:

    void keyTest( const char *key );
    // internal
    bool test_sizes();
    void dt_initiate( bool mode = true );   // must be changed with DK
    void dt_next();
    void make_A( int siz_, char (*for_)[MAXFORMUNITDT] );
    void calc_eqstat();
    void dt_text_analyze();                 // translate &
    void CalcEquat( int type_ );            // calculate RPN
    void Bb_Calc();
    void Bn_Calc();

    // last level
    void Init_Generation();
    void build_Ub();         // generate systems and calculate new Ub
    void build_mu_n();       // calculate mu_n matrix
    void Init_Analyse();     // init analyse the results (change DK)
    void Analyse();          // analyse the results (change DK)
    void Calc_muo_n( char eState ); // calculate mu_o DualTh
    void Calc_gam_n( char eState ); // calculate gamma DualTh
//  void Calc_act_n( char eState ); // calculate activity DualTh


public:

    static TDualTh* pm;

    DUALTH *dtp;

    TDualTh( int nrt );

    const char* GetName() const
    {
        return "DualTh";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    bool check_input( const char *key, int level=1 );
    gstring   GetKeyofRecord( const char *oldKey, const char *strTitle,
                              int keyType );

    void RecInput( const char *key );
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void CmHelp();

    void InsertChanges( TIArray<CompItem>& aIComp,
    TIArray<CompItem>& aPhase,  TIArray<CompItem>&aDComp );
};

enum dualth_inernal {
              A_CHI = 1,  A_GAM =2,

              DT_MODE_M = 'M',
              DT_MODE_G = 'G',
              DT_MODE_A = 'A',
//            DT_MODE_X = 'X',

              DT_STATE_E = 'E',
              DT_STATE_P = 'P',
              DT_STATE_S = 'S',
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

#endif //_m_dualth_h_
