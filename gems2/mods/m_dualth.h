//-------------------------------------------------------------------
// $Id$
// To be finalized in Version 2.1 (2005)
// Declaration of TDualTh class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2005 S.Dmytriyeva, D.Kulik
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

typedef struct
{ // Description of DualTh data structure (revised in October 2005 to bring in
//   accordance with paper: Kulik, 2005, Chem.Geol. in press)
  // Record key format: the same as Proces
  char
   PunE,          // Units of energy   { j;  J c C N reserved }
   PunV,          // Units of volume   { j;  c L a reserved }
   PunP,          //  Units of pressure  { b;  B p P A reserved }
   PunT,          // Units of temperature  { C; K F reserved }

// Allocation flags
   PvCoul,   // Use coulombic factors for non-basis DCs (+ -)
   PvICb,    // Use IC amounts for defining basis sub-system compositions? { + * - }
   PvICn,    // Use IC amounts for defining non-basis sub-system compositions? { + * - }
   PvAUb,    // Use formula units for basis and non-basis sub-system compositions? { + * - }
   PvSd,     // Include references to data sources (+ -)?
   PvChi,    // Use math script for mole fractions of non-basis DCs (+ -)?
   PvGam,    // Use math script for Gex or activity coeffs in non-basis (DCs) (+ -)?
   PvTPI,    // Use vectors for T, P and IS of nQ experiments (+ -)?

     // Controls on operation
   PsMode,  // DualTh mode of operation { M G A X }
   PsSt,    // State of non-basis part saturation { E P S } equil, prim., stoich. sat.

//  Status and control flags ( + - )
   gStat,  // DualTh generation status: 0 -indefinite; 1 on-going generation run;
        //  2 - done generation run; 3 - generation run error (+ 5: the same in stepwise mode)
   aStat,  // DualTh analysis status:  0 - indefinite; 1 ready for analysis;
        //  2 - analysis run; 3 - analysis done; ( + 5: the same using stepwise mode)
   PsSYd,  // Save generated SysEq records to data base ( + - )
PsIPf,    // interaction parameter formalism code (I G R T M V B O)
PsIPu,    // interaction parameter units code (J K N)
PsLSF, //   Control flag for least-square param. fitting  { ( N L S B C )}
       // to be extended !
   name[MAXFORMULA],  //  "Name of DualTh task"
   notes[MAXFORMULA] //  "Comments"
    ;
  short
// input I
   nQ,   // n(Q) - number of experiments (equilibria) in basis sub-system
   La_b, // Lb - number of formula units to set compositions in basis sub-system
nM,   // n(M) - number of DC (end-member) candidates in non-basis sub-system (default 2)
   Nsd,  // N of references to data sources
   Nqpn, // reserved
   Nqpg, // Number of elements in the math script work arrays qpg per non-basis DC
   Nb,    // N - number of independent components (in basis sub-system, taken
        // from project system dimensions automatically)
nP,  // Number of interaction parameters in the non-basis mixture (default 1 - regular)
// for generating syseq record keys
   tmd[3],  // SYSTEM CSD definition #: start, end, step (initial)
   NVd[3],  // Restrictions variant #: start, end, step
*wa_cp  //  [nP] mixing model parameter classifier (for Bayesian LSF)
   ;
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
   WmCb,  // mole fraction of the carrier DC (e.g. sorbent or solvent)
   Asur,  // Specific surface area of the sorbent (for adsorbed species)
   cT,
   cP,
   cV  // State factors T,P,V
    ;
 double        // in n(Q) experiments:
   *Bb,    //  [Q][N] Table of bulk compositions of basis sub-systems  B(BS)
   *Bn,    //  [Q][N] Table of bulk compositions of non-basis sub-systems B(NS)
   *Ub,    //  [Q][N] Table of dual solution values for basis sub-systems U(BS)

   *chi,   //  [Q][M] Table of mole fractions of DC (end-member) candidates X(BS)
*eta_b, //  [Q][M] Table of DualTh chemical potentials of n(M) end-members, d-less H(BS)
*act_n, //  [Q][M] table of DualTh-calculated activities
*gam_n, //  [Q][M] Table of activity coefficients for DC candidates
*Coul,  //  [Q][M] Input Coulombic terms for surface complexes (optional)

*mu_b,   // [Q][M] Table of DualTh chemical potentials of M end-members (J/mol) M(BS)

*mu_o,   // [Q][M]  st. state chem. potentials for candidates (or EMPTY if unknown) GoNS
*avsd_o, // [2][M] mean and st.dev. over mu_o columns (experiments) for DC candidates
*mu_a,   // [Q][M] Table of estimated st. Gibbs energy function for trace DC candidates G*NS
*avsd_a, // [2][M] mean and st.dev. over mu_a cols (experiments) for DC candidates

(*gmx_n)[5],  // [Q][5] in J/mol
         //        Excess Gibbs energy of mixing for Q experiments (col. 0)   G-Ex
         //        Gibbs energy of ideal mixing for Q experiments (col. 1)    G-ID
         //        total Gibbs energy of mixing for Q experiments (col. 2)    G-MIX
         //   Gibbs energy of mechanical mixture for Q experiments ( col. 3)  G-SS-0
         //   integral Gibbs energy of mixture for Q experiments ( col. 4) 0  G-SS-BS
*Wa,     // [Q][P] interaction parameters for the mixing model
*avsd_w, // [2][P] mean and st.dev. over Wa cols (experiments) for DC candidates
// *chisq,  // [P] chisquare values from LSM fits ( to be extended )

   *yconst,   //  [Q] Array for constant for LM&SVD regression
   *qpg    //  [Nqpg] Array for gamma calculation math script (interaction params?)
    ;
 float
   *CIb,  // [Q][N] Table of quantity/concentration of IC in basis sub-systems
   *CIn,  // [Q][N] Table of quantity/concentration of IC in non-basis sub-systems
   *CAb,  // [Q][Lb] Table of quantity/concentration of formulae for basis sub-systems
   *CAn,  // [Q][Lb] Table of quantity/concentration of DC formulae for non-basis sub-systems

*An,  // [M][N] stoich matrix for DC (end-member) stoichiometry candidates

  *Tdq,   //  [Q]  Temperatures of experiment (allocation depends on PsTPI)
  *Pdq,   //  [Q]  Pressures of experiment
  *ISq    //  [Q]  Effective ionic strength in experimental aq solutions
    ;
 char
   *cExpr,  // Math script text for calculation of mole fractions of DC in non-basis
   *gExpr,  // Math script text for calculation of activity coeffs of DC in non-basis
   (*sdref)[V_SD_RKLEN], // "List of bibl. refs to data sources" [0:Nsd-1]
   (*sdval)[V_SD_VALEN],  // "Parameters taken from the respective data sources"[0:Nsd-1]
   (*nam_b)[MAXIDNAME], // [Q][12] id names of experiments
   (*nam_n)[MAXIDNAME], // [K][12] id names of DC (end-member) stoichiometry candidates
(*par_n)[MAXIDNAME], // [P][12] id names of non-ideal parameters
   (*for_n)[MAXFORMUNITDT], // [K][40] formulae of DC (end-member) stoichiometry candidates
   (*for_b)[MAXFORMUNITDT], // [Lb][40] formulae for setting basis and non-basis system compositions
   (*stld)[EQ_RKLEN], // List of SysEq record keys [Q]
//
   *typ_n, // [K] type code of DC stoichiometry candidates {O M J I S ... }
   *CIclb, // [N] Units of IC quantity/concentration for basis compositions
   *CIcln, // [N] Units of IC quantity/concentration for non-basis compositions
   *AUclb, // [Lb] Units of setting quantities of formula units for basis system composition
   *AUcln, // [Lb] Units of setting quantities of DC formulae for non-basis system composition
//
   (*SBM)[MAXICNAME+MAXSYMB] // Keys (names) of IC
   ;

//work data
 short
   q,      // index of experiment
   i,      // index of IC
   jm,     // index of non-basis sub-system component
kp,    // index of the interaction parameter
   c_tm,         // Current Tm - SYSTEM CSD number
   c_NV,         // Current Nv - MTPARM variant number
Asiz,      // Current number of rows in the An matrix
// LM fitter data
   n_par,  // number of parameters to fit  (<-nP)
   m_dat,  // number of data points to use in fitting (<-nQ)
   tm_d,   // number of arguments in tdat array per one data point  (<-nM(-1))
   resILM; // reserved
/* Work arrays */
 double
   *tdat,  //  to LM fitter: array of data arguments [nQ][nM]
   *ydat,  //  to LM fitter: array of data functions [nQ]
   *wdat,  // array of weight factors for data  [nQ], optional
   *par,   // [nP] to LM fitter: guess parameters; from LM fitter - fitted parameters
   *wpar,  // [nP] to LM fitter: weight factors for parameters (optional)
   *sdpar, // [nP] from LM fitter: errors of fitted parameter values (optional)
   *Wa_ap, // [nP] a prior parametres valuies
   xi2,  // final value of chi2 (quality of fit)
   resLM; // reserved
   char sykey[EQ_RKLEN+10],    // Key of currently processed SysEq record
   *tprn;              // internal
   char timep[16], TCp[16], Pp[16], NVp[16], Bnamep[16];
}
DUALTH;

/* Work objects for DualTh scripts - to add??? */
typedef struct   // not used yet
{
    double RT,
    F,
    Nu,
    Mu,
    G0,
    lnAx,
    Ax,
    dGex,
    Gdis,
    Asig,
    Asur,
    Amw,
    Gid,
    lnCx,
    Cx,
    Mx,
    lnMx,
    lnWx,
    Wx,
    Gcas,
    Gsas,
    Gsm,
    Gsd,
    Tetp,
    Nx,
    Agmx,
    Agx,
    Gex,
    Gamma,
    lnGam,
    Gpsi,
    Ze,
    Psi,
    lnPg;
    /* 32 double */
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
    void get_RT_P( int ii, double& RT, double& P);
    void lmfit_new();
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
    void Init_Generation(); // Start generation of BS SysEq records
    void build_Ub();        // generate systems and calculate new Ub
    void build_mu_n();      // calculate mu_n matrix
    void Init_Analyse();    // init analyse the results (change DK)
    void Analyse();         // analyse the results (change DK)
    int CalcMoleFractNS();  // built-in calculation of mole fractions in NS
    void CalcActivCoeffNS( char ModP, char ModIPu ); // built-in calculation of
                            // activity coeffs in NS end-members
    void Calc_gmix_n( char Mod, char eState ); // calculation of mixing energies
    void Calc_muo_n( char eState ); // calculate mu_o DualTh
    void Calc_gam_n( char eState ); // calculate gamma DualTh
    void Calc_act_n( char eState ); // calculate activity DualTh
    int Calc_alp_n( char IpfCode, char ModIPu ); // retrieve interaction parameters DualTh
    void Calc_muo_n_stat( char eState ); // statistics for EM candidates
    void Calc_alp_n_stat( char eState ); // statistics for interaction params 
    double ColumnAverage( double *DataTable, short N, short M, short ColInd  );
    double ColumnStdev( double *DataTable, double ColAvg,
                  short N, short M, short ColInd  ); //

     // forward calculation of j-th activity coefficient in various models
    double Guggenheim( double chi[], double alp[], short j, short nK, short nM,
                       double cFactor );
    double TWMargules( double chi[], double alp[], short j, short nK, short nM,
                       double cFactor  );
    double MargulesO( double chi[], double alp[], short j, short nK, short nM,
                     double cFactor  );
    double VanLaar( double chi[], double alp[], short j, short nK, short nM,
                    double cFactor  );
    double BalePelton( double chi[], double alp[], short j, short nK, short nM,
                       double cFactor, bool solvent );

       // retrieval from excess Gibbs energy of mixing for nQ points
    int GuggenheimR( double gex[], double alp[], short nQ, short nK, short nM,
                    char ModIPu );
    int TWMargulesR( double gex[], double alp[], short nQ, short nK, short nM,
                    char ModIPu );
    int MargulesOR( double gex[], double alp[], short nQ, short nK, short nM,
                    char ModIPu );
    int VanLaarR( double gex[], double alp[], short nQ, short nK, short nM,
                    char ModIPu );
    int BalePeltonR( double gex[], double alp[], short nQ, short nK, short nM,
                    char ModIPu, short solventNdx );

    // LSM regression subroutine
    int RegressionLSM( int );

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
    void MakeQuery();
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void CmHelp();

    void InsertChanges( TIArray<CompItem>& aIComp );
};

enum dualth_inernal {
              A_CHI = 1,  A_GAM =2,

              DT_MODE_M = 'M',   // Estimation of G0 for end-members
              DT_MODE_G = 'G',   // Estimation of activity coeffs and inter.param.
              DT_MODE_A = 'A',   // Estimation of activities of end-members
              DT_MODE_X = 'X',   // Estimation of mole fractions of end-members

              DT_STATE_E = 'E',  // equilibrium
              DT_STATE_P = 'P',  // primary saturation
              DT_STATE_S = 'S',  // stoichiometric saturation

              DT_LSF_N = 'N',  // not use
              DT_LSF_L = 'L',  // Levenberg usual
              DT_LSF_S = 'S',  // SVD usual
              DT_LSF_B = 'B',  // SVD Bayesian
              DT_LSF_C = 'C',  // Levenberg constrained

                                // interaction parameter codes
              DT_IPF_I = 'I',   // Ideal Raoult 
              DT_IPF_R = 'R',   // Redlich-Kister
              DT_IPF_G = 'G',   // Guggenheim
              DT_IPF_T = 'T',   // Thompson-Waldbaum
              DT_IPF_M = 'M',   // Margules
              DT_IPF_V = 'V',   // Van Laar
              DT_IPF_B = 'B',   // Bale-Pelton dilute formalism
              DT_IPF_O = 'O',   // Other
                                // interaction parameter units of measurement
              DT_IPU_J = 'J',   // J/mol
              DT_IPU_K = 'K',   // kJ/mol
              DT_IPU_N = 'N',   // normalized
                    };

#endif //_m_dualth_h_
