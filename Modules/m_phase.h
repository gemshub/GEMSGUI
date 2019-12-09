//-------------------------------------------------------------------
// $Id: m_phase.h 2115 2012-05-10 20:54:59Z kulik $
//
// Declaration of TPhase class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2013 S.Dmytriyeva, D.Kulik, T. Wagner
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
#ifndef _m_phase_h_
#define _m_phase_h_

#include "v_mod.h"
#include "v_module.h"

const int  MAXPHSYMB =      8,
           MAXPHGROUP =     16,
           MAXPNCOEF =  8192,
           QPSIZE = 180, // earlier 20, 40 SD oct 2005
           QDSIZE = 60;

typedef struct
{// Description of PHASE record (old part)
  char pst_[MAXSYMB],       // Phase aggregate state
    symb_[MAXPHSYMB],    // Symbol of phase definition
    nam_[MAXPHNAME],     // Name of phase
    cls_[MAXSYMB],       // Class of phase
    grp_[MAXPHGROUP],    // Record key comment to phase definition

//  old:  sol_t[6],
   sol_t[6], // Phase (solution) type { IGMTVKRLWFPED123HYSQZAUOX } or { N }
             //         Mode of calculation of DC non-ideality parameters { NTX }
             //         Mode of calculation of phase non-ideality parameters { NTX }
             //         Mode of calculation of DC activity coefficients { NSI }
             //	  One group of equations for all DC {U},one group per DC {P},no {N}
             //         Type of mixing rule in EoS models { WCT } or of sorption model {}

    PphC, // Default class of phase in the system {agpmlsxiyzdh} layered {}
    Ppnc, // Flag for 'ph_cf' array for phase-related input parameters {+*-}
    Psco, // Flag for 'dc_cf' array for DC-related parameters {+*-}
    PpEq, // Flag for text field for phase-related equations 'PhEq' {+*-}
    PdEq, // Flag for text field for DC-related equations 'DcEq' {+*-}
    PFsiT,// Flag for vector of surface type fractions 'PFsiT' {+*-}
    Pinternal1,  // Extract parameters from DComp/ReacDC records and refresh DC class codes?
                 // not saved to DB
Pdqf, // new: flag for dqfp array for DC-related DQF parameter coefficients {+*-}
Prcp, // new: flag for rcpp array for DC-related reciprocal parameter coefficients {+*-}
PlPhl,// new: flag for list of record keys of linked phases, cf. lPh, lPhC {+*-}
Psol, /// TW new: flag for solvent interaction coefficients
Pdiel, /// TW new: flag for dielectric constant coefficients
Pdh,  /// TW new: flag for generic DH coefficients
Pres1,// new: reserved

// TSorpMod stuff
PEIpc,  // new: flag for EIpc array of EIL model parameter coefficients {+*-}
PCDc,   // new: flag for CDc array of CD model parameter coefficients {+*-}
PIsoC,  // new: flag for IsoC array of isotherm parameter coefficients per DC {+*-}
PIsoS,  // new: flag for IsoS array of isotherm parameter coefficients per surface site {+*-}
PsDiS,  // new: flag for sDiS array of denticity and surface site indexes {+*-}
Pres2,  // new: reserved

    // TKinMet stuff
    PrpCon, /// flag for rpCon array of kinetic rate constants, feSAr, ocPRk arrays {+*-}
    PumpCon,/// flag for umpCon array of uptake model parameters {+*-}
    PapCon,  /// new: flag for lDCr and apCon arrays for parameters of species involved in activity product terms

    kin_t[8],    // was 6 now 8!
// new:    Type of sorption/ionex/polyelectrolyte isotherm model { N ... }
// new:    Type of sorption EIL model { N ...  }
  /// Type code of the kinetics/metastability model  { N T W ... TBD }
  /// Code of direction of the kinetic process to the rate model applies { N D P G B ... TBD }
  /// Type code of the uptake kinetics model { N E M ... TBD }
  /// Type of metastability links of this phase to other phases { N S P M ... TBD }
  /// Type of particle/pore size distribution and specific surface area correction { N U B ... }
  /// Code of specific MWR kinetic rate model equation { N ... }

    name[MAXFORMULA],   // Full name of phase
    notes[MAXFORMULA]  // Comments
    ;
 short nDC,      // N of DC in phase definition >= 1
    Nsd,        //  N of references to Data Sources  <= 4
    ncpN,    // Number of interaction parameters (rows in pnc and ipx tables)
    ncpM,    // Number of coefficients per IP (cols in pnc table)
    npxM,    // Maximum order of interaction parameters (cols in ipx, to set up on remake)
    nscM,    // Number of parameters per solution phase species (cols in scoef table)
    NsuT,     // N of (surface) types (to set up on remake)
    nMoi,     // Number of different substituent moieties in multi-site mixing model (0: simple mixing)
//
    nlPh,  /// TKinMet, TSorpMod: number of linked phases (cf. PlPhl, lPh), default 0.
    nlPc,  /// TKinMet, TSorpMod: number of parameters per linked phase, default 0.
ndqf,  // TSolMod new: number of DQF parameter coefficients per end member, default 3.
nrcp,  // not used TSolMod new: number of reciprocal parameter coefficients per end member, default 3.
ncsolv, /// TW new: number of solvent parameter coefficients (columns in solvc array)
nsolv,  /// TW new: number of solvent interaction parameters (rows in solvc array)
ncdiel, /// TW new: number of dielectric constant coefficients (colums in dielc array)
ndiel,  /// TW new: number of dielectric constant parameters (rows in dielc array)
ndh,    /// TW new: number of generic DH coefficients (rows in dhc array)

    // TKinMet stuff
    nPRk,  /// number of parallel reactions that affect amount constraints for k-th phase (1, 2, 3, ...), 1 by default
    nSkr,  /// number of (aqueous or gaseous or surface) species from other reacting phases involved, 0 by default
    nrpC,  /// number of parameter (coefficients) involved in 'parallel reaction' terms (0 or 14)
    naptC, /// number of parameter (coefficients) per species involved in 'activity product' terms (0 or 1)
    nAscC, /// number of parameter coefficients in specific surface area correction equation ( 0 or 4 )

    numpC, /// number of uptake model parameter coefficients (per end member)

    nFaces, /// number of (separately considered) crystal faces or surface patches ( 1 to 4 )

// TSorpMod stuff EIL model
nEIl,  // new: number of electrostatic model layers (default: 0, maximum 4)
nEIp,  // new: number of electrostatic model parameters (per layer, default 1, max 4)
nCDc,  // new: number of charge distribution coefficients per surface species (default 0 or nEIl)
iRes3, // new: reserved

// TSorpMod stuff non-EIL model
nIsoC, // new: number of isotherm parameter coefficients per surface species DC (default 0)
nIsoS, // new: number of isotherm parameter coefficients per surface site (default 0)
mDe,   // new: maximum denticity number for surface species (default 1)
nSiT,  /// new: number of site types (to set up on remake)
reserv
 ;

short *ipxt,  // Table of indexation for interaction parameters [ncpN][npxM]
              // takes over from PXres
*xSmD, // new: denticity of surface species per surface site (site allocation) [nDC][nSiT]
       // (default 0, -1 means no binding) [nDC][mDe+1]

    *ocPRk,  /// KinMet: Operation codes for kinetic 'parallel reaction' affinity terms [nPRk*2], default 0
    *ixsolv, /// new TW: array of indexes of solvent interaction parameters [nsolv*2]
    *ixdiel  /// new TW: array of indexes of dielectric interaction parameters [ndiel*2]
    ;

float Asur,  // Specific surface area of (carrier) phase, m2/g (new: of this tile) default: 0.
    Sigma0, // Standard mean surface energy of solid-aqueous interface, J/m2
    SigmaG, // Standard mean surface energy of gas-aqueous interface, J/m2
    R0p,    // Mean radius r0 for (spherical or cylindrical) particles, nm
    h0p,    // Mean shape factor for (spherical or cylindrical) particles or pores, nm
    Eps,    // Dielectric constant for solid carrier at Tr (reserved)
    Cond,   // Specific conductivity at Tr, Sm/m/cm2 (reserved)
    Rsp1,   // Default maximum surface density, 1/nm2 (reserved)

Vpor,  // new: Specific pore volume of (carrier) phase, m3/g (default: 0)
fSAs,  // new: fraction of surface area of the sorbent (ref. in lPh) occupied by this surface tile (def. 1)
fPV,   // new: fraction of phase pore volume occupied by this Donnan electrolyte (default 1)
fRes1, // new: reserved

psdC,  // new: permanent surface charge density (eq/m2), default: 0
pvdC, // new: permanent Donnan volume charge density (eq/m3), default: 0
IEC,  // new: ion exchange capacity (eg/g), default: 0
fRes2, // new: reserved

    // old stuff for sorption models
    *FsiT,    //Fraction of surface type relative to carrier (components)[NsuT]
    *XfIEC,   // Constant surface charge density or IEC, mkeq/g   [NsuT]
    // TSolMod stuff
    *pnc,   //Array of phase-related coefficients of non-ideality model [ncpN][ncpM]
    *scoef, //Array of DC-related coefficients of non-ideality model[nDC][nscM]

    *lPhc,  /// TsolMod, TKinMet, TSorpMod: array of phase link parameters [nlPh*nlPc]
// new: TSolMod stuff
*DQFc,  // new: array of DQF parameters for DCs in phases [nDC*ndqf]
*rcpc,  // new: array of reciprocal parameters for DCs in phases [nDC*nrcp]
*solvc, /// TW new: array of solvent interaction parameters [ncsolv*nsolv]
*dielc, /// TW new: array of dielectric constant parameters [ncdiel*ndiel]
*dhc,   /// TW new: array of generic DH parameters [ndh]

//new: TSorpMod stuff
*EIpc, // new: Array of electrostatic model parameter coefficients per EI layer [nEIl][nEIp]
*CDc,  // new: Array of electrost. model coefficients per surface species [nDC][nCDc]
*IsoP, // new: array of isotherm parameter coefficients per surface species [nDC][nIsoC]
*IsoS, // new: array of isotherm parameter coefficients per surface site [nSiT][nIsoS]
       // here site density etc.

    // TKinMet stuff
    *feSAr,  /// fractions of surface area of the solid related to different parallel reactions [nPRk]
    *rpCon,  /// array of kinetic rate constants for particle faces or 'parallel reactions' [nPRk][nrpC]
    *apCon,  /// array of parameters per species involved in 'activity product' terms [nSkr] [nPRk][naptC]
    *Ascp,   /// parameter coefficients of equation for corrections of specific surface area [nAscC]

    *umpCon  /// array of uptake model parameters [nDC][numpC] (only SS phases)
// new:new: array of nucleation model parameters (A.Testino?)
;

// Old sorption model stuff
  float (*MSDT)[2]; // SAT: Max & min density of reacted species, 1/nm2 [NsuT]
  float (*CapT)[2]; // Inner EDL capacitance density, F/m2 (TLM, CCM)
  float (*MaSdj)[DFCN]; // Max. density, CD-music and isotherm params [nDC][NR1]
                // Outer EDL capacitance density, F/m2 (TLM)  [NsuT]
  char (*SATC)[MCAS]; // SACT method codes & allocations of surface species [nDC][DFCN]
// DC list
  char (*SM)[DC_RKLEN]; // List of DC record keys included into this phase[nDC]
// new stuff (TKinMet, TSorpMod)
    char (*lPh)[PH_RKLEN];    /// TKinMet, TSorpMod: list of record keys of linked phases [nlPh]
                          /// in MULTI vector of phase indexes  long int *xlPh
    char (*lDCr)[DC_RKLEN];   /// TKinMet: list of record keys (names?) of aq, gas or surface catalyzing
      /// or inhibiting species for parallel reactions [nSkr] to take into MULTI vector of DC indexes  long int *xSkr
char (*lICu)[MAXICNAME];   /// TKinMet: list of IC (elements) names [nDC] each representing SS end member
            /// in fractionation coeffs. to take into MULTI vector of IC indexes  long int *xICu
//
char (*dcpcl)[MAXDCNAME]; // new: DC parameter coefficients comment list [nscM]
char (*ipicl)[MAXDCNAME]; // new: interaction parameter indexes comment list [ncpN]
char (*ipccl)[MAXDCNAME]; // new: interaction parameter coefficients comment list [ncpM]
//
    char (*rpkcl)[MAXDCNAME]; /// kinetic rate constants comment list [nrpC]
    char (*rprcl)[MAXDCNAME]; /// kinetic parallel reactions comment list [nPRk]
    char (*umpcl)[MAXDCNAME]; /// uptake kinetic model parameters comment list [numpC]
//
char (*smcDl)[MAXDCNAME]; // new: sorption model parameters comment list per DC [nIsoC]
char (*smcSl)[MAXDCNAME]; // new: sorption model parameters comment list per site [nIsoS]
char (*eimPl)[MAXDCNAME]; // new: EIL model CD parameters comment list per DC [nCDc]
char (*eimLl)[MAXDCNAME]; // new: EIL model parameters comment list per layer [nEIp]
//
  char
    *DCC,   // DC classes { TESKWL GVCHNI JMFD QPR <0-9>  AB  XYZ O } [nDC]
    *DCS,   // Source of input data for DC { r d } d-DCOMP r-REACT    [nDC]
    *lDCd,   /// TKinMet: Source of input data for DC { r d } d-DCOMP r-REACT in lDCr [nSkr]
    *SCMC,  // Class.of EIL models for surface types (old sorption phases) [NsuT]
    *pEq,   // Text with script of IPN equations related to the whole phase (IIPN syntax)
    *dEq,   // Text with script of IPN equations related to phase components (IIPN syntax)
    *tprn,               // internal
// TSorpMod, TKinMet stuff
    *lPhC, /// TSolMod, TKinMet: Phase linkage type codes [nlPh] { 0 1 2 3 ... }
*IsoC // new: isotherm/SATC code for this surface site type 2*[nSiT] { L Q M F B ... }
  ;
  char (*sdref)[V_SD_RKLEN]; // List of Data Sources SDref keys
  char (*sdval)[V_SD_VALEN];  // Comments to Data Sources
// ---------------------------------------------------------------------------------------
// Work data (SIT, Pitzer, EUNIQUAC, ...)
  char (*lsCat)[MAXDCNAME];       // work object - vector of names of cations
  char (*lsAn)[MAXDCNAME];        // work object - vector of names of anions
  char (*lsNs)[MAXDCNAME];        // work object - names of neutral species except H2O
  short
    nCat,   // number of cations
    nAn,    // number of anions
    nNs,    // number of neutral species with  water // BugFix SD 26/11/2010
   *nxCat,  // Vector of indexes for cations
   *nxAn,   // vector of indexes for anions
   *nxNs;   // Vector of indexes of neutral species

// Work data  // Multi-site (sublattice) SS models
  char (*lsMoi)[MAXDCNAME];  // work object - names of moieties (moiety list)
  char (*lsForm)[MAXFORMULA];  // work object - list of chemical formula of DC [nDC]
  short
  //  nMoi,   // number of moieties
    nSub,   // number of sublattices (site types)
   *nxSub;  // Table of sublattice site indexes [nDC][nMoi]
  float
   *OcpN;  // Table of multiplicity numbers [nDC][nMoi]

}
PHASE;


struct elmWindowData;
struct phSetupData;

// Current Phase
class TPhase : public TCModule
{
    PHASE ph[1];

protected:

    void CalcPhaseRecord( /*bool getDCC*/  );
    void moiety_new( int nDC, int nMoi, bool setDefault );
    void Set_SolMod_Phase_coef();
    void Set_KinMet_Phase_coef();   // new
    void Set_SorpMod_Phase_coef();  // new
    void makeReacDCompList( const char *caption, TCStringArray& aDclist,
                            short& nDC,  char (*SM)[DC_RKLEN], char * DCS, bool bNsuT);
    void makePhaseList( const char *caption, TCStringArray& aPhlist );
    void makeICompList( const char *caption, TCStringArray& aIclist );
    void DetNumbCatAn(TCStringArray& aDclist);
    void LoadDCC();
    void set_def_comments( bool clearall,
              const char* old_sol, const char *old_kin );

public:

    static TPhase* pm;

    PHASE *php;

    TPhase( uint nrt );

    const char* GetName() const override
    {
        return "Phase";
    }

    void ods_link( int i=0) override;
    void dyn_set( int i=0) override;
    void dyn_kill( int i=0) override;
    void dyn_new( int i=0) override;
    void set_def( int i=0) override;

    //  bool check_input( const char *key, int level=1 );

    void RecInput( const char *key ) override;
    void MakeQuery() override;
    int RecBuild( const char *key, int mode = VF_UNDEF ) override;
    void RecCalc( const char *key ) override;
    void RecordPrint( const char *key=nullptr ) override; //test  phase-sublattice-compress

    //void CmHelp();
    const char* GetHtml() override;
    // added by KD on 21.11.04 for SIT
    void MakeCatAnLists( bool WorkCount, bool WorkAlloc, bool FillOut );
    // make new aq and/or gas phases (re-written by KD 30.07.03)
    void newAqGasPhase( const char *akey, const char *gkey, int file,
        const char amod, const char gmod, float apar[4], /*float gpar[4],*/
            bool useLst = false, TCStringArray lst = 0 );
    // Added by KD on 31.07.03
    void AssemblePhase( const char* key, const char* part, float *param,
           int file, bool useLst = false, TCStringArray lst = 0, int Npar = 4 );

    void CopyRecords( const char * prfName, TCStringArray& aPHnoused,
                      TCStringArray& aPHtmp, elmWindowData el_data,
                      phSetupData st_data,  TCStringArray& SDlist);
    bool CompressRecord( int nDCused, TCIntArray& DCused, bool onlyIPX=false );
    void MakeSublatticeLists( TCStringArray& form_array  );

protected:

    int CompressDecomp( int nDCused, const TCIntArray& DCused );
    int CompressSublattice( int nDCused, const TCIntArray&  DCused );
    TCStringArray getSavedLsMoi() const;
    TCStringArray readFormulaes(int nDCused, const TCIntArray&  DCused) const;
};

enum solmod_switches { // indexes of keys of phase (solution, sorption, kinetic) models
    SPHAS_TYP,
    DCOMP_DEP,
    SPHAS_DEP,
    SGM_MODE,
    DCE_LINK,
    MIX_TYP,

    // Link state of CalculateActivityCoefficients()
    LINK_UX_MODE,
    LINK_TP_MODE,
    LINK_PP_MODE,  // LINK_PHP_MODE,
LINK_IN_MODE, // Initialization mode for kinetics and other time-dependent processes
SORP_MOD,  // new, see also enum sorption_control
KINR_MOD,  /// see also enum kinmet_controls
    // Posible modes of calculation of activity coefficients (private, public)
    SM_UNDEF = 'N',
    SM_TPDEP = 'T',
    SM_UXDEP = 'X',
    SM_PRIVATE_ = 'P',
    SM_PUBLIC = 'U',

    // Posible modes of calculation of activity coefficients (built-in or scripted models)
    SM_STNGAM = 'S',        // Built-in function for activity coefficients
    SM_NOSTGAM = 'N',

    // Codes to identify the mixing models used (during IPM iterations)
    SM_IDEAL =  'I',	// ideal solution or single-component phase
    SM_BERMAN = 'B',    // built-in multicomponent multisite (a)symmetric solid-solution model
    SM_CEF = '$',    //     built-in multicomponent multisite solid-solution model (CALPHAD)
    SM_MBW = '#',    //     built-in Modified Bragg-Williams model, [Vinograd et al. 2018]
    SM_REDKIS = 'G', 	// built-in binary Guggenheim (Redlich-Kister) solid-solution model
    SM_MARGB = 'M',	// built-in binary Margules solid-solutions (subregular)
    SM_MARGT = 'T',	// built-in ternary Margules solid-solution (regular)
    SM_VANLAAR = 'V',	// built-in multi-component Van Laar solid-solution model
    SM_GUGGENM = 'K',	// built-in multi-component Guggenheim solid-solution model
    SM_REGULAR = 'R',	// built-in multi-component Regular solid-solution model
    SM_NRTLLIQ = 'L',	// built-in multi-component NRTL model for liquid solutions
    SM_WILSLIQ = 'W',	// built-in multi-component Wilson model for liquid solutions
    SM_CGFLUID = 'F',	// built-in multi-component Churakov-Gottschalk (CG) fluid EoS model
    SM_PRFLUID = 'P',	// built-in Peng-Robinson-Stryjek-Vera (PRSV) fluid EoS model
    SM_PCFLUID = '5',   // built-in perturbed-chain statistical-association (PCSAFT) fluid EoS model (reserved)
    SM_STFLUID = '6',   // built-in Sterner-Pitzer (STP) fluid EoS model
    SM_PR78FL = '7',	// built-in Peng-Robinson (PR78) fluid EoS model
    SM_CORKFL = '8',    // built-in compensated Redlich-Kwong (CORK) fluid EoS model
    SM_REFLUID = '9',   // built-in reference EoS fluid model (reserved)
    SM_SRFLUID = 'E',	// built-in Soave-Redlich-Kwong (SRK) fluid EoS model
    SM_AQDAV = 'D',	// built-in Davies model (with 0.3) for aqueous electrolytes
    SM_AQDH1 = '1',	// built-in Debye-Hueckel limiting law for aqueous electrolytes
    SM_AQDH2 = '2',	// built-in 2-term Debye-Hueckel model for aqueous electrolytes
    SM_AQDH3 = '3',	// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Karpov version)
    SM_AQDHH = 'H',	// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Helgeson version)
    SM_AQDHS = 'Y',	// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Shvarov version)
    SM_AQSIT = 'S',	// built-in SIT model for aqueous electrolytes
    SM_AQEXUQ = 'Q',    // built-in extended UNIQUAC model for aqueous electrolytes
    SM_AQPITZ = 'Z',    // built-in Pitzer HMW model for aqueous electrolytes
    SM_AQMIX = 'C',     // built-in mixed-solvent aqueous Debye-Hueckel model (reserved)
    SM_AQELVIS = 'J',   // built-in modified extended UNIQUAC model (ELVIS) for aqueous electrolyte
    SM_DONNAN = 'X',    // ion exchange (Donnan volume model) (reserved)
    SM_SURCOM = 'A',	// models of surface complexation at solid-aqueous interface
    SM_USERDEF = 'U',	// user-defined mixing model (scripts in Phase record)
    SM_OTHER = 'O',	// other built-in phase-specific models of non-ideal solutions (selected by phase name)

    // Codes to identify specific mixing rules and temperature functions in EoS and activity models
    MR_UNDEF = 'N', // Default mixing rule or form of interaction parameter coefficients; NEM for adsorption 'A'
    MR_WAAL = 'W',	// Basic Van der Waals mixing rules in cubic EoS models
    MR_CONST = 'C',	// Constant one-term interaction parameter kij; CCM for sorption 'A'
    MR_TEMP = 'T',	// Temperature-dependent one-term interaction parameter kij (Jaubert et al. 2005); TLM for adsorption 'A'
    MR_LJ = 'J',        // Lemmon-Jacobsen mixing rule (Lemmon and Jacobsen, 1999)
    MR_KW1 = 'K',       // Kunz-Wagner mixing rule (Kunz and Wagner, 2007)
    MR_PITZ5 = '5',     // 5-term Pitzer model temperature dependence (TOUGHREACT variant)
    MR_PITZ6 = '6',     // 6-term Pitzer model temperature dependence (FREZCHEM variant)
    MR_PITZ8 = '8',     // 8-term Pitzer model temperature dependence
    MR_B_RCPT = 'R',    // Use CEF reciprocal non-ideality terms in Berman multi-site ss model
    MR_A_DLM  = 'D',    // Diffuse-layer electrostatic model (DLM) for adsorption 'A'
    MR_A_BSM  = 'B',    // Basic Stern electrostatic model (BSM) for adsorption 'A'
    MR_A_CDLM  = 'M',    // CD-MUSIC (3-layer) electrostatic model (DLM) for adsorption 'A'
    MR_A_ETLM  = 'E'     // Extended TLM electrostatic model (ETLM) for adsorption 'A'
};

// This code defines standard state and reference scale of concentrations
// for components of this phase. It is used by many subroutines
// during calculations of equilibrium states
enum PH_CLASSES {  // Possible values
    PH_AQUEL = 'a',  	// aqueous electrolyte
    PH_GASMIX = 'g',  	// mixture of gases
    PH_FLUID = 'f',  	// fluid phase
    PH_PLASMA = 'p',  	// plasma
    PH_LIQUID = 'l',  	// non-electrolyte liquid (melt)
    PH_SIMELT = 'm',  	// silicate (magmatic) melt or non-aqueous electrolyte
    PH_SORPTION = 'x',  // Sorption phase (sorbent+sorbates) in aqueous electrolyte
    PH_POLYEL = 'y',  	// colloidal poly- (oligo)electrolyte e.g. Donnan volume phase
 PH_IONEX = 'i',     // ion exchange on permanent charge ligand e.g. B&B Clay
 PH_ADSORPT = 'z',   // surface complexation (adsorption) on hydrated amphoteric surface
    PH_SINCOND = 's',  	// condenced solid phase, also multicomponent (solid solution)
    PH_SINDIS = 'd',  	// dispersed solid phase, also multicomponent
    PH_HCARBL = 'h'   	// mixture of condensed hydrocarbons
};

enum sorption_control {
    // EDL interface models - separate for site types in v. 3.1
    SC_DDLM = 'D',  SC_CCM = 'C',     SC_TLM = 'T',   SC_MTL = 'M',
    SC_MXC = 'E',   SC_NNE = 'X',     SC_IEV  = 'I',  SC_BSM = 'S',
    SC_3LM = '3', SC_NOT_USED = 'N',

    // Methods of Surface Activity Terms calculation
    SAT_COMP = 'C', SAT_NCOMP = 'N', SAT_SOLV = 'S', SAT_INDEF = 'I',

    // New methods for surface activity coefficient terms (2004)
    SAT_L_COMP = 'L', SAT_QCA_NCOMP = 'Q', SAT_QCA1_NCOMP = '1',
    SAT_QCA2_NCOMP = '2', SAT_QCA3_NCOMP = '3', SAT_FREU_NCOMP = 'f',
    SAT_QCA4_NCOMP = '4', SAT_BET_NCOMP = 'B', SAT_VIR_NCOMP = 'W',
    SAT_FRUM_NCOMP = 'F', SAT_FRUM_COMP = 'R', SAT_PIVO_NCOMP = 'P',

    // Assignment of surtype to carrier (end-member) */
    CCA_VOL = 'V',
    CCA_0 = '0', CCA_1, CCA_2, CCA_3, CCA_4, CCA_5,
    CCA_6, CCA_7, CCA_8, CCA_9, SPL_0='0', SPL_1, SPL_2, SPL_3,
    SPL_B = 'b', SPL_D = 'd', SPL_C = 'c',
    SDU_N = 'n', SDU_m = 'm', SDU_M = 'M', SDU_g = 'g',
    CST_0 = '0', CST_1, CST_2, CST_3, CST_4, CST_5, // surface type index
    CSI_0 = '0', CSI_1, CSI_2, CSI_3, CSI_4, CSI_5, // surface site index

    // Number of parameters per surface species in the MaSdj array
    // MCAS = 6 = DFCN ; position index    added by KD 25.10.2004
    // Column indices of surface species allocation table MCAS
    SA_MCA=0, SA_EMX, SA_STX, SA_PLAX, SA_SITX, SA_UNIT,

    // Column indices of MaSdj table of adsorption parameters
    PI_DEN=0, PI_CD0, PI_CDB, PI_P1, PI_P2, PI_P3
};

enum ph_kinmet_controls { /// TKinMet: codes to control kinetic rate models

    KM_UNDEF = 'N',       /// not defined, no account for
KinProCode = 2,
    KM_PRO_MWR = 'M',     /// Kinetics of generic dissolution/precipitation (no uptake, ionex, adsorption)
    KM_PRO_UPT = 'U',     /// Kinetics of uptake/entrapment (of minor/trace element) into solid solution
    KM_PRO_IEX = 'X',     /// Kinetics of ion exchange (clays, C-S-H, zeolites, ...)
    KM_PRO_ADS = 'A',     /// Kinetics of adsorption (on MWI), redox
    KM_PRO_NUPR = 'P',    /// Kinetics of nucleation and precipitation
KinModCode = 3,
    KM_MOD_TST = 'T',     /// Generic TST dissolution/precipitation model following Shott ea 2012
    KM_MOD_PAL = 'P',     /// Dissolution/precipitation model of the form (Palandri 2004)
    KM_MOD_WOL = 'W',     /// Carbonate growth model following (Wolthers 2012)
    KM_MOD_NUGR = 'U',    /// Mineral nucleation and growth model with nuclei/particle size distr. (TBD)
KinSorpCode = 4,
    KM_UPT_ENTRAP = 'E',  ///	Unified entrapment model (Thien,Kulik,Curti 2012)
    KM_UPT_UPDP = 'M',    ///	DePaolo (2011) uptake kinetics model
    KM_UPT_SEMO = 'G',    ///  Growth (surface) entrapment model (Watson 2004)
    KM_IEX_FAST = 'F',    ///  Fast ion exchange kinetics (e.g. montmorillonite, CSH)
    KM_IEX_SLOW = 'L',    ///  Slow ion exchange kinetics (e.g. illite, zeolites)
    KM_ADS_INHIB = 'I',   ///  Adsorption inhibition
    KM_NUCL_SSMP  = 'P',  ///  Solid solution nucleation model (Prieto 2013)
KinLinkCode = 5,
    KM_LNK_SURF = 'S',    ///   Link to (fraction of) solid substrate surface
    KM_LNK_PVOL = 'P',    ///    Link to (fraction of) solid substrate (pore) volume
    KM_LNK_MASS = 'M',    ///	Link to (fraction of) solid substrate mass
KinSizedCode = 6,
    KM_SIZED_ETM = 'T',   ///  Empirical f(time) cubic polynomial f = a + bt +ct^2 + dt^3 (default)
    KM_SIZED_ESI = 'S',   ///  Empirical f(lgSI) cubic polynomial f = a + bt +ct^2 + dt^3
    KM_SIZED_ESA = 'A',   ///  Empirical f(sarea-change) cubic polynomial f = a + bt +ct^2 + dt^3
    KM_SIZED_EVOL = 'V',  ///  Empirical f(volume-change) cubic polynomial f = a + bt +ct^2 + dt^3
    KM_SIZED_MASS = 'M',  ///  Empirical f(mass-change) cubic polynomial f = a + bt +ct^2 + dt^3
    KM_SIZED_MOL = 'X',   ///  Empirical f(amount-change) cubic polynomial f = a + bt +ct^2 + dt^3
    KM_SIZED_UNI = 'U',   /// 	Uniform particle/pore size distribution
    KM_SIZED_BIN = 'B',   /// 	Binodal particle/pore size distribution
    KM_SIZED_FUN = 'F',   ///    Empirical distribution function
KinResCode = 7,
    KM_RES_SURF_N = 'A',   /// surface-scaled rate constant (k in mol/m2/s), default
    KM_RES_SURF_M = 'M',   /// surface-scaled rate constant (k in kg/m2/s)
    KM_RES_PVS_N  = 'V',   /// pore-volume-scaled rate constant (k in mol/m3/s)
    KM_RES_PVS_M  = 'W',   /// pore-volume-scaled rate constant (k in kg/m3/s)
    KM_RES_ABS_N  = 'F',   /// absolute (unscaled) rate constant (k in mol/s)
    KM_RES_ABS_M  = 'G',   /// absolute (unscaled) rate constant (k in kg/s)
    KM_LIN_RATE   = 'L'   /// linear growth/dissolution rate constant (v in m/s)

};

enum ph_affin_term_op_codes { /// TKinMet: codes to control affinity terms
    ATOP_CLASSIC = 0,       /// classic TST affinity term (see .../Doc/Descriptions/KinetParams.pdf)
    ATOP_CLASSIC_REV = 1,   /// classic TST affinity term, reversed
    ATOP_SCHOTT = 2,        /// Schott et al. 2012 fig. 1e
    ATOP_HELLMANN = 3,      /// Hellmann Tisserand 2006 eq 9
    ATOP_TENG1 = 4,         /// Teng et al. 2000, eq 13
    ATOP_TENG2 = 5,         /// Teng et al. 2000, Fig 6
    ATOP_FRITZ = 6          /// Fritz et al. 2009, eq 6 nucleation and growth

};

enum volume_code {  /* Codes of volume parameter ??? */
    VOL_UNDEF, VOL_CALC, VOL_CONSTR
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif //  _m_phase_h

