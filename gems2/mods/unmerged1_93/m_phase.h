//-------------------------------------------------------------------
// Id: gems/mods/m_phase.h  version 2.0.0   2001
//
// Declaration of TPhase class, config and calculation functions
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
//
#ifndef _m_phase_h_
#define _m_phase_h_

#include "v_mod.h"
#include "v_module.h"

const int MAXPHSYMB =      8,
          MAXPHGROUP =     16,
          MAXPNCOEF =      10;

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
    PFsiT,// Flag for vector of surface type fractions 'PFsiT'? {+*-}
    name[MAXFORMULA],   // Full name of phase
    notes[MAXFORMULA]   // Comments
    ;
    short nDC,      // N of DC in phase definition >= 1
    Nsd,        //  N of references to Data Sources  <= 4
    ncpN, ncpM, // Dimensions of 'ph_cf' array (to set up on remake)
    nscN, nscM, // Dimensions of 'dc_cf' array (to set up on remake)
    NsiT,       // N of surface site types (to set up on remake)
    NR1;        // reserved
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
    (*MSDT)[2], // SAT: Max & min density of reacted species, 1/nm2 [NsiT]
    (*CapT)[2]; // Inner EDL capacitance density, F/m2 (TLM, CCM)
    // Outer EDL capacitance density, F/m2 (TLM)  [NsiT]
    char (*SATC)[2]; /* Classifier of methods of SAT calculation
                        & alloc.to carrier DC, [nDC][2] */
    float *MaSdj, // Max. density of non-competitive species, 1/nm2 [nDC]
    *PXres, // Reserved
    *pnc, //Array of phase-related coefficients of non-ideality model [ncpN][ncpM]
    *scoef;//Array of DC-related coefficients of non-ideality model[nDC][nscN][nscM]
    char (*SM)[DC_RKLEN], // List of DC record keys included into phase[0:nDC-1]
    *DCC,   // DC classes { TESWGVCHNIJM<digit>XYZABPQRO}[0:nDC-1]
    *DCS,   // Source of input data for DC { r d }.d-DCOMP r-REACT[0:nDC-1]
    *SCMC,  /* Class.of EDL models for surf types [0:NsiT-1]*/
    *pEq, // Text of IPN equations related to the whole phase (IIPN syntax)
    *dEq, // Text of IPN equations related to phase components (IIPN syntax)

    (*sdref)[V_SD_RKLEN], // List of Data Sources SDref keys
    (*sdval)[V_SD_VALEN],  // Comments to Data Sources

    *tprn;               // internal
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
    int RecBuild( const char *key );
    void RecCalc( const char *key );
    void RecordPrint( const char *key=0 );
    void CmHelp();

    // make new phases (emod added KD 25.01.02)
    void newAqGasPhase( const char *key, int file, const char emod = '3',
                        bool useLst = false, TCStringArray lst = 0 );

    void CopyRecords( const char * prfName, TCStringArray& aPHnoused,
            elmWindowData el_data, phSetupData st_data);

}
;

enum solmod_switches { /* indexes of keys of model solution*/
    SPHAS_TYP, DCOMP_DEP, SPHAS_DEP, SGM_MODE, DCE_LINK, SCM_TYPE,
    /* link state */
    LINK_UX_MODE, LINK_TP_MODE, LINK_FIA_MODE,
    /* Posible values of  of keys of model solution - DCOMP_DEP, SPHAS_DEP */
    SM_UNDEF = 'N', SM_TPDEP = 'T', SM_UXDEP = 'X', SM_PRIVATE = 'P',
    SM_PUBLIC = 'U',
    // Posible modes calculating of activity coefficients SGM_MODE
    SM_STNGAM = 'S', SM_NOSTGAM = 'N',
    /* This code (one upper-case letter or digit) defines type of mixing
    and default method of calculation of mixing properties before and at
    IPM iterations.   Possible values: (SPHAS_TYP) */
    SM_IDEAL = 'I',  // ideal solution or single-component phase;
    SM_REGULAR = 'R',// regular solution, also with limited miscibility
    SM_RECIP = 'V',  // reciprocal solution (reserved)
    SM_AQDAV = 'D',  // built-in Davies equation (with 0.3) added KD 25.01.02
    SM_AQDH1 = '1',  // limiting Debye-Hueckel law for aqueous species (reserved)
    SM_AQDH2 = '2',  // 2-d approximation of Debye-Hueckel (reserved)
    SM_AQDH3 = '3',  // built-in 3-d approximation of Debye-Hueckel
    SM_AQDHH = 'H',  // built-in 3-d approximation of Debye-Hueckel (Helgeson, reserved)
    SM_AQPITZ = 'P', // Pitzer's model for aqueous brines (reserved)
    SM_IONEX = 'E',  // ion exchange (Donnan, Nikolskii) (reserved)
    SM_SURCOM = 'A', // models of surface complexation at solid-aqueous interface
    SM_SCIEM = 'U'  // combined ion exchange / adsorption on clay particles,
               // oligoelectrolytes
               //   O  -  other models of non-ideal solutions.
};

//    This code defines standard state and reference scale of concentra-
// tions for components of this phase. It is used by many subroutines
// during calculations of equilibrium states
enum PH_CLASSES{  /* Possible values */
    PH_AQUEL    = 'a',  // aqueous electrolyte
    PH_GASMIX   = 'g',  // mixture of gases
    PH_PLASMA   = 'p',  // plasma
    PH_SOLUTION = 'l',  // non-electrolyte liquid (melt)
    PH_SIMELT   = 'm',  // silicate (magmatic) melt or non-aqueous electrolyte
    PH_SORPTION = 'x',  // dilspersed solid with adsorption (ion exchange) in aqueous
    PH_POLYEL = 'y',    // colloidal poly- (oligo)electrolyte
    PH_SINCOND  = 's',  // condenced solid phase, also multicomponent
    PH_SINDIS   = 'd',  // dispersed solid phase, also multicomponent
    PH_HCARBL   = 'h'   // mixture of condensed hydrocarbons
};

enum sorption_control {
    /* EDL interface models - separate for site types in v. 3.1 */
    SC_DDLM = 'D',  SC_CCM = 'C',     SC_TLM = 'T',   SC_MTL = 'M',
    SC_MXC = 'E',   SC_NNE = 'X',     SC_IEV  = 'I',  SC_BSM = 'S',
    SC_NOT_USED = 'N',
    /* Methods of Surface Activity Terms calculation */
    SAT_COMP = 'C', SAT_NCOMP = 'N', SAT_SITE = 'S', SAT_INDEF = 'I',
    /* Assignment of surtype to carrier (end-member) */
    CCA_VOL = 'V', CCA_0 = '0', CCA_1, CCA_2, CCA_3, CCA_4, CCA_5,
    CCA_6, CCA_7, CCA_8, CCA_9
};

enum volume_code {  /* Codes of volume parameter ??? */
    VOL_UNDEF, VOL_CALC, VOL_CONSTR
};


#endif //  _m_phase_h

