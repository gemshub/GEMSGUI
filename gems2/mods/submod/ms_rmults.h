//-------------------------------------------------------------------
// $Id$
//
// Declaration of TRMults class, config and  load functions
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
#ifndef _ms_rmults_h_
#define _ms_rmults_h_

#include "v_mod.h"
#include "v_module.h"

const int MAXMUGROUP =     38;
const int MAXNUMCHAINS =   20;  /* max. number of chains */


typedef struct
{ // RMULTS is base to Project
    char symb[MAXMUNAME],    // Identifier of root multisystem definition
    grp[MAXMUGROUP],    // Record Key Comment to RMULTS definition
    // Switches DB
    PmvSA,  // List of COMPOS: + include - exclude * rebuild
    PmvPH,  // List of PHASE: { + * }
    PmvDC,  // List of DCOMP/REACDC: { + * }
    PmvIC,  // List of ICOMP:  { + * }
    PmvID,  // List of IC->DC references { + * - }? (reserved)
    PmvIP,  // Arrays of IC properties  { + * }
    PmvIF,  // List of IC formulae { + * - }? (reserved)
    PmvDF,  // Lisr of DC formulae (for stoichiometry matrix A) { + * }
    PmvAq,  // Default aqueous phase { 3 D + - }
    PmvGas, // Default gaseous phase { + - }

    name[MAXFORMULA],   // Full name of root multisystem DB
    notes[MAXFORMULA];  // Comments DB
    // Sizes - calc from data
    short N,     // N of IC, incl. Zz (charge) and Vol (volume)
    L,       // L   - of DC - total for all phases
    Laq,     // LO  - of DC in aqueous phase
    Pg,      // PG  - of DC in gaseous phase
    Lhc,     // LH  - of DC in mixture of liquid hydrocarbons (reserved)
    Fi,      // FI  - total number of phases
    Fis,     // FIs - total number of multi-component phases
    La,      // La  - of references to COMPOS records
    Ls,      // Ls  - total number of DC in multi-component phases
Lads,      // former Lx: total number of surface species
    FiE,     // FIe - number of phases with sorption (and EDL)
    NfT,     // Total number of linked files sigma(Nfl)

    nAq,     // Index select aqueous phase
    nGas;    // Index select gaseous phase

    short
    *Ll,// L1 vector, shows a number of DC included to each phase [0:Fi-1] DB
    Nfl[MAXNUMCHAINS]; // Number of open files for each modules DB

    char
    (*SF2)[PH_RKLEN],// List of multicomponent PHASE definition keys [Fis]             DB
    (*SM2)[DC_RKLEN],// List of multicomp DC definition keys  [Ls]
(*SM3)[DC_RKLEN],// List of adsorption DC definition keys  [Lads]  new 

    (*SF)[PH_RKLEN],// List of PHASE definition keys [0:Fi-1]             DB
    (*SM)[DC_RKLEN],// List of DC definition keys (DCOMP, REACDC) [0:L-1] DB
    (*SA)[BC_RKLEN],// List of COMPOS definition keys [0:La-1]            DB
    (*SB)[IC_RKLEN],// List of ICOMP record keys (stoichiometry basis)[0:N-1] DB
    (*FN)[MAX_FILENAME_LEN]; // list file names for each modules [0:NfT-1] DB

    char // build from PHASE  (calc from cfg)
    *PHC, // Classifier of phases { agpmslxdh } [0:Fi-1]
    *DCC, // DC classes { TESWGVCHNIJM<digit>XYZABPQRO }[0:L-1]
*DCC3, // DC classes for adsorption [Lads]
    *DCS; // Code of data source for DC: d-DCOMP r-REACDC { dr }[0:L-1]
    short   *Pl;   // indexes of DC in PHASE [0:Ls-1]
    char // build from  ICOMP records
    *ICC, // Classifier of IC { eohazvi<integer> } [0:N-1]
    *ICF,  // List of IC chemical formulae (comma separated) text
    (*IDCref)[DC_RKLEN]; // List of IC-to-DC references [0:N-1]
    short *Val;  // Vector of default valencies of IC [0:N-1]
    float *BC,   // Vector of IC atomic (molar) masses, g/mole [0:N-1]
    *AS,   // Vector of IC atomic entropies, J/mole/K [0:N-1]
    *AV;   // Vector of IC molar volumes, cm3/mole [0:N-1]
    char // build from DCOMP/REACDC records
    *DCF;  // List of DC chemical formulae (comma separated) text
}

RMULTS;


// Data of RMULTS
class TRMults :
            public TSubModule
{
    RMULTS mu;

    // List records to test

protected:
    void MakeRecordLists( gstring& AqKey, gstring& GasKey );

public:

    RMULTS* GetMU()
    {
        return &mu;
    }

    TRMults( int nrt );

    const char* GetName() const
    {
        return "RMults";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    void DCListLoad( gstring& AqKey, gstring& GasKey,
                     bool useLst = false, TCStringArray lst = 0 );
    void ICmake();
    void PHmake();
    void TestIComp();
    void LoadRmults( bool NewRec, bool changePhases );
    gstring SelectAqPhase(const char * dfKey );
    gstring SelectGasPhase(const char * dfKey );
    void SetAqGas( const char* AqKey, const char* GasKey );
};

// extern const char * defaultAqKey;
// extern const char * defaultGasKey;

#endif      //_ms_rmults_h
