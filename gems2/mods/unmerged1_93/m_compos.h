//-------------------------------------------------------------------
// Id: gems/mods/m_compos.h  version 2.0.0   2001
//
// Declaration of TCompos class, config and calculation functions
//
// Rewritten from C to C++ by S.Dmytriyeva  970207 modified 010804
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

#ifndef _m_compos_h_
#define _m_compos_h_

#include "v_mod.h"
#include "v_module.h"

const int MAXCMPFORM =    255;

typedef struct
{ // Description  COMPOS
    char //cname[MAXCMPNAME],//Identifier of Geochemical Composition Object (GCO)
    //type[MAXSYMB],      // Code of GCO type
    //group[MAXCMPGROUP], // Record Key Comment to GCO description
    PcIC,  // Use IC quantities for GCO definition? { + * - }
    PcDC,  // Use DC quantities for GCO definition? { + * - }
    PcAU,  // Create formula units for GCO definition? { + * - }
    PcFO,  // Create large formula text for GCO definition? { + * - }
    PcdC,  // Create delC vector of uncertainties for GCO definition? {+ * - }
    PcRes, // Reserved
    name[MAXFORMULA],    // Name of geochemical predefined composition object
    notes[MAXFORMULA]    // Comments
    ;
    short N,   // Numbers: of IC to define GCO
    Ld,   // N of DC (DCOMP+REACDC) stoichiometies to define GCO
    La,   // N of formula units created in this GCO definition
    Nsd,  // N of references to data sources
    Nmax, //Nmax total current number of IC in ICOMP chain
    Nc;   // Nc   final number of IC in GCO definition
    float
    Msys, // Masses (kg) and volumes (L) for GCO: Ms (total mass, normalize)
    Vsys, // Vs (total volume of the object, for volume concentrations)
    Mwat, // M(H2O) (mass of water-solvent for molalities)
    Maq,  // Maq (mass of aqueous solution for ppm etc.)
    Vaq,  // Vaq (volume of aqueous solution for molarities)
    Pg,   // Pg (pressure in gas, for partial pressures)
    R1,   // total number of IC moles in GCO, normalization
    R2;   //mas % of matter specified by Ctext, relative to the whole GCO mass
    double *C;  // Resulting GCO bulk: numbers of moles of IC [0:N-1]
    char *CFOR,  // Text-formula of GCO increment
    (*SB)[MAXICNAME+MAXSYMB],// List of IC symbols
    (*SA)[MAXFORMUNIT],// List of formula units
    (*SM)[DC_RKLEN], // List of DC record keys      [0:Ld-1]
    *CIcl, // Units of IC quantity/concentration  [0:N-1]
    *CDcl, //Units of DC quantity/concentration    [0:Ld-1]
    *AUcl, // Units of formulae quantity/concentration  [0:La-1]
    *DCS   // Source of DC data: d-DCOMP r-REACDC { d r } [0:Ld-1]
    ;
    float *CI, // Quantity/concentration of IC C(IC)   [0:Nc-1]
    *CD,  // Quantity/concentration of DC C(DC     [0:Ld-1]
    *CA,  //Quantity/concentration of formulae C(AC)   [0:La-1]
    *delC; //Uncertainty of Cm elements in moles       [0:N-1]
    char
    (*sdref)[V_SD_RKLEN], // List of SDref keys to data sources  [0:Nsd-1]
    (*sdval)[V_SD_VALEN]; // List of comments to data sources [0:Nsd-1]
    // COMPOS Work arrays
    float *A,   // stoichiometry matrix formul. ed and DC  [0:Nmax-1]
    *ICw  //IC atomic (molar) masses [0:Nmax-1]
    ;
    char (*SB1)[IC_RKLEN], //Complete work list of IC keys in ICOMP chain [0:Nmax-1]
    *tprn;             // internal
}

COMPOS;

// Current Compos
class TCompos : public TCModule
{

    COMPOS bc[1];

    // work data for calc
    double  *C;
    float *CI;
    char *CIcl;


protected:
    void bc_work_dyn_new();
    void bc_work_dyn_kill();

public:

    static TCompos* pm;
    COMPOS *bcp;

    TCompos( int nrt );
    ~TCompos();

    const char* GetName() const
    {
        return "Compos";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    void RecInput( const char *key );
    int RecBuild( const char *key );
    void RecCalc( const char *key );
    double Reduce_Conc( char UNITP, double Xe, double DCmw, double Vm,
     double R1, double Msys, double Mwat, double Vaq, double Maq, double Vsys);

    void RecordPrint( const char *key=0 );
    void CmHelp();

    void CopyRecords( const char * prfName,
            TCStringArray& aIC, TCStringArray& names,
            bool aAqueous, bool aGaseous );
}
;

#endif   // _m_compos.h

