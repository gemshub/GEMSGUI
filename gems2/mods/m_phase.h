//-------------------------------------------------------------------
// $Id$
//
// Declaration of TPhase class, config and calculation functions
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
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//
#ifndef _m_phase_h_
#define _m_phase_h_

#include "v_mod.h"
#include "v_module.h"

const int MAXPHSYMB =      8,
                MAXPHGROUP =     16,
                           MAXPNCOEF =  8192,
                                      QPSIZE = 60, // earlier 20, 40 SD oct 2005
                                               QDSIZE = 60;

typedef struct
{// Description  PHASE
  char pst[MAXSYMB],       // Phase aggregate state
    symb[MAXPHSYMB],    // Symbol of phase definition
    nam[MAXPHNAME],     // Name of phase
    cls[MAXSYMB],       // Class of phase
    grp[MAXPHGROUP],    // Record key comment to phase definition

    sol_t[6], /* Phase type { IRV123HPEAU } or { N }
                       Mode of calculation of DC non-ideality parameters { NTX }
                       Mode of calculation of phase non-ideality parameters { NTX }
                       Mode of calculation of DC activity coefficients { NSI }
                   One group of equations for all DC {U},one group per DC {P},no {N}
                       Default type of adsorption/ion exchange model { NCLDTE }  */
    PphC, // Default class of phase in the system {agpmlsxdh}
    Ppnc, // Flag for 'ph_cf' array for phase-related input parameters {+*-}
    Psco, // Flag for 'dc_cf' array for DC-related parameters {+*-}
    PpEq, // Flag for text field for phase-related equations 'PhEq' {+*-}
    PdEq, // Flag for text field for DC-related equations 'DcEq' {+*-}
    PFsiT,// Flag for vector of surface type fractions 'PFsiT' {+*-}
    name[MAXFORMULA],   // Full name of phase
    notes[MAXFORMULA]   // Comments
    ;
 short nDC,      // N of DC in phase definition >= 1
    Nsd,        //  N of references to Data Sources  <= 4
// changed 07.12.2006 KD
ncpN,    // Number of interaction parameters (rows in pnc and ipx tables)
ncpM,    // Number of coefficients per IP (cols in pnc table)
npxM,    // Maximum order of interaction parameters (cols in ipx, to set up on remake)
nscM,    // Number of parameters per solution phase species (cols in scoef table)
    NsiT,     // N of surface site types (to set up on remake)
    NR1;      // Number of elements per species in MaSdj array (1: old 6: new)
short *ipxt;  // Table of indexation for interaction parameters ncpN x npxM
              // takes over from PXres
 float Asur,  // Specific surface area of major component (carrier), m2/g
    Sigma0,// Standard mean surface energy of solid-aqueous interface, J/m2
    SigmaG,// Standard mean surface energy of gas-aqueous interface, J/m2
    R0p,   // Mean radius r0 for (spherical or cylindrical) particles, nm (reserved)
    h0p,   // Mean thickness h0 for cylindrical or 0 for spherical particles, nm (reserved)
    Eps,   // Dielectric constant for solid carrier at Tr (reserved)
    Cond,  // Specific conductivity at Tr, Sm/m/cm2 (reserved)
    Rsp1,  // Default maximum surface density, 1/nm2"
    *FsiT,    //Fraction of surface type relative to carrier (components)[0:NsiT-1]
    *XfIEC,   // Constant surface charge density or IEC, mkeq/g   [NsiT]
// *PXres, // Reserved
    *pnc, //Array of phase-related coefficients of non-ideality model [ncpN][ncpM]
    *scoef;//Array of DC-related coefficients of non-ideality model[nDC][nscM]

  float (*MSDT)[2]; // SAT: Max & min density of reacted species, 1/nm2 [NsiT]
  float (*CapT)[2]; // Inner EDL capacitance density, F/m2 (TLM, CCM)
  float (*MaSdj)[DFCN]; // Max. density, CD-music and isotherm params [nDC][NR1]
                // Outer EDL capacitance density, F/m2 (TLM)  [NsiT]
  char (*SATC)[MCAS]; // SACT method codes & allocations of surface species [nDC][DFCN]
  char (*SM)[DC_RKLEN]; // List of DC record keys included into phase[0:nDC-1]
  char
	*DCC,   // DC classes { TESWGVCHNIJM<digit>XYZABPQRO}[0:nDC-1]
    *DCS,   // Source of input data for DC { r d }.d-DCOMP r-REACT[0:nDC-1]
    *SCMC,  /* Class.of EDL models for surf types [0:NsiT-1]*/
    *pEq, // Text of IPN equations related to the whole phase (IIPN syntax)
    *dEq, // Text of IPN equations related to phase components (IIPN syntax)
    *tprn;               // internal

  char (*sdref)[V_SD_RKLEN]; // List of Data Sources SDref keys
  char (*sdval)[V_SD_VALEN];  // Comments to Data Sources

  char (*lsCat)[MAXDCNAME];       // work object - vector of names of cations
  char (*lsAn)[MAXDCNAME];        // work object - vector of names of anions
char (*lsNs)[MAXDCNAME];        // work object - names of neutral species except H2O

  short
    nCat,   // Work data - number of cations
    nAn,    // Work data - number of anions
nNs,      // Work data - number of neutral species except water
   *nxCat,            // Vector of indexes for cations in SIT or Pitzer coeff table
   *nxAn,             // vector of indexes for anions  in SIT or Pitzer coeff table
*nxNs;    // Vector of indexes of neutral species in Pitzer coeff table
}

PHASE;

struct elmWindowData;
struct phSetupData;

// Current Phase
class TPhase : public TCModule
{
    PHASE ph[1];

protected:

    void CalcPhaseRecord( bool getDCC  );

public:

    static TPhase* pm;

    PHASE *php;

    TPhase( int nrt );

    const char* GetName() const
    {
        return "Phase";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    //  bool check_input( const char *key, int level=1 );

    void RecInput( const char *key );
    void MakeQuery();
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    void CmHelp();
    // added by KD on 21.11.04 for SIT
    void MakeCatAnLists( bool WorkCount, bool WorkAlloc, bool FillOut );
    // make new aq and/or gas phases (re-written by KD 30.07.03)
    void newAqGasPhase( const char *akey, const char *gkey, int file,
        const char amod, const char gmod, float apar[4], float gpar[4],
            bool useLst = false, TCStringArray lst = 0 );
    // Added by KD on 31.07.03
    void AssemblePhase( const char* key, const char* part, float param[4],
           int file, bool useLst = false, TCStringArray lst = 0, int Npar = 4 );

    void CopyRecords( const char * prfName, TCStringArray& aPHnoused,
          TCStringArray& aPHtmp, elmWindowData el_data, phSetupData st_data);
}
;

enum solmod_switches { /* indexes of keys of model solution*/
    SPHAS_TYP,
    DCOMP_DEP,
    SPHAS_DEP,
    SGM_MODE,
    DCE_LINK,
    SCM_TYPE,

    // Link state of GammaCalc() calculation of activity coefficients
    LINK_UX_MODE,
    LINK_TP_MODE,
    LINK_FIA_MODE,

    // Posible modes of calculation of activity coefficients (private, public)
    SM_UNDEF = 'N',
    SM_TPDEP = 'T',
    SM_UXDEP = 'X',
    SM_PRIVATE_ = 'P',
    SM_PUBLIC = 'U',

    // Posible modes of calculation of activity coefficients (built-in or scripted models)
    SM_STNGAM = 'S',        // Built-in function for activity coefficients
    SM_NOSTGAM = 'N',

    // Code to identify the mixing models used (during IPM iterations)
    SM_IDEAL =  'I',	// ideal solution or single-component phase;
    SM_REDKIS = 'G', 	// built-in binary Guggenheim (Redlich-Kister) solid-solution model
    SM_MARGB = 'M',		// built-in binary Margules solid-solutions (subregular)
    SM_MARGT = 'T',		// built-in ternary Margules solid-solution (regular)
    SM_VANLAAR = 'V',	// built-in multi-component Van Laar solid-solution model
    SM_GUGGENM = 'K',	// built-in multi-component Guggenheim solid-solution model
    SM_REGULAR = 'R',	// built-in multi-component Regular solid-solution model
    SM_NRTLLIQ = 'L',	// built-in multi-component NRTL model for liquid solutions
    SM_WILSLIQ = 'W',	// built-in multi-component Wilson model for liquid solutions
    SM_CGFLUID = 'F',	// built-in multi-component Churakov-Gottschalk (CG) fluid EOS model
    SM_PRFLUID = 'P',	// built-in Peng-Robinson-Stryjek-Vera (PRSV) fluid EOS model
    SM_SRFLUID = 'E',	// built-in Soave-Redlich-Kwong (SRK) fluid EOS model
    SM_AQDAV = 'D',		// built-in Davies model (with 0.3) for aqueous electrolytes
    SM_AQDH1 = '1',		// built-in Debye-Hueckel limiting law for aqueous electrolytes
    SM_AQDH2 = '2',		// built-in 2-term Debye-Hueckel model for aqueous electrolytes
    SM_AQDH3 = '3',		// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Karpov version)
    SM_AQDHH = 'H',		// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Helgeson version)
    SM_AQDHS = 'Y',		// built-in 3-term Debye-Hueckel model for aqueous electrolytes (Shvarov version), reserved
    SM_AQSIT = 'S',		// built-in SIT model for aqueous electrolytes
    SM_AQEXUQ = 'Q',    // built-in EUNIQUAC model for aqueous electrolytes
    SM_AQPITZ = 'Z',    // built-in Pitzer HMW model for aqueous electrolytes
// SM_IONEX = 'E',		// ion exchange (Donnan, Nikolskii) (reserved)
    SM_SURCOM = 'A',	// models of surface complexation at solid-aqueous interface
    SM_USERDEF = 'U',	// user-defined mixing model (scripts in Phase record)
    SM_OTHER = 'O'		// other built-in phase-specific models of non-ideal solutions
    	                //    (selected through phase name)
};

//    This code defines standard state and reference scale of concentra-
// tions for components of this phase. It is used by many subroutines
// during calculations of equilibrium states
enum PH_CLASSES{  /* Possible values */
    PH_AQUEL    = 'a',  // aqueous electrolyte
    PH_GASMIX   = 'g',  // mixture of gases
    PH_FLUID    = 'f',  // fluid phase
    PH_PLASMA   = 'p',  // plasma
    PH_LIQUID   = 'l',  // non-electrolyte liquid (melt)
    PH_SIMELT   = 'm',  // silicate (magmatic) melt or non-aqueous electrolyte
    PH_SORPTION = 'x',  // dilspersed solid with adsorption (ion exchange) in aqueous
    PH_POLYEL   = 'y',  // colloidal poly- (oligo)electrolyte
    PH_SINCOND  = 's',  // condenced solid phase, also multicomponent
    PH_SINDIS   = 'd',  // dispersed solid phase, also multicomponent
    PH_HCARBL   = 'h'   // mixture of condensed hydrocarbons
};

enum sorption_control {
    /* EDL interface models - separate for site types in v. 3.1 */
    SC_DDLM = 'D',  SC_CCM = 'C',     SC_TLM = 'T',   SC_MTL = 'M',
    SC_MXC = 'E',   SC_NNE = 'X',     SC_IEV  = 'I',  SC_BSM = 'S',
SC_3LM = '3', SC_NOT_USED = 'N',
    /* Methods of Surface Activity Terms calculation */
    SAT_COMP = 'C', SAT_NCOMP = 'N', SAT_SOLV = 'S', SAT_INDEF = 'I',
/* New methods for surface activity coefficient terms (2004) */
 SAT_L_COMP = 'L', SAT_QCA_NCOMP = 'Q', SAT_QCA1_NCOMP = '1',
 SAT_QCA2_NCOMP = '2', SAT_QCA3_NCOMP = '3', SAT_FREU_NCOMP = 'f',
 SAT_QCA4_NCOMP = '4', SAT_BET_NCOMP = 'B', SAT_VIR_NCOMP = 'W',
 SAT_FRUM_NCOMP = 'F', SAT_FRUM_COMP = 'R', SAT_PIVO_NCOMP = 'P',
    /* Assignment of surtype to carrier (end-member) */
    CCA_VOL = 'V', CCA_0 = '0', CCA_1, CCA_2, CCA_3, CCA_4, CCA_5,
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

enum volume_code {  /* Codes of volume parameter ??? */
    VOL_UNDEF, VOL_CALC, VOL_CONSTR
};


#endif //  _m_phase_h

