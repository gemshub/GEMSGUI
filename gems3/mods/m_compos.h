//-------------------------------------------------------------------
// $Id: m_compos.h 1373 2009-07-22 12:25:22Z gems $
//
// Declaration of TCompos class, config and calculation functions
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
    PcRes, // Units of concentration/amount for Ctext formula (default M) 
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
    R2;   // amount/concentration of compound specified by Ctext
    double *C;  // Resulting GCO bulk: numbers of moles of IC [0:N-1]
    char *CFOR,  // Text-formula of GCO increment
    *CIcl, // Units of IC quantity/concentration  [0:N-1]
    *CDcl, //Units of DC quantity/concentration    [0:Ld-1]
    *AUcl, // Units of formulae quantity/concentration  [0:La-1]
    *DCS   // Source of DC data: d-DCOMP r-REACDC { d r } [0:Ld-1]
    ;
    float *CI, // Quantity/concentration of IC C(IC)   [0:Nc-1]
    *CD,  // Quantity/concentration of DC C(DC     [0:Ld-1]
    *CA,  //Quantity/concentration of formulae C(AC)   [0:La-1]
    *delC; //Uncertainty of Cm elements in moles       [0:N-1]
    char (*sdref)[V_SD_RKLEN]; // List of SDref keys to data sources  [0:Nsd-1]
    char (*sdval)[V_SD_VALEN]; // List of comments to data sources [0:Nsd-1]
    char (*SB)[MAXICNAME+MAXSYMB];// List of IC symbols
    char (*SA)[MAXFORMUNIT];// List of formula units
    char (*SM)[DC_RKLEN]; // List of DC record keys      [0:Ld-1]
    // COMPOS Work arrays
    double *A,   // stoichiometry matrix formul. ed and DC  [0:Nmax-1]
    *ICw  //IC atomic (molar) masses [0:Nmax-1]
    ;
    char (*SB1)[IC_RKLEN], //Complete work list of IC keys in ICOMP chain [0:Nmax-1]
    *tprn;             // internal
}

COMPOS;

struct elmWindowData;
struct cmSetupData;


// Current Compos
class TCompos : public TCModule
{

    COMPOS bc[1];

    // work data for calc
    double  *C;
    double *CI;
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
    void MakeQuery();
    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecCalc( const char *key );
    double Reduce_Conc( char UNITP, double Xe, double DCmw, double Vm,
    double R1, double Msys, double Mwat, double Vaq, double Maq, double Vsys);
    double MolWeight( int N, double *ICaw, double *Smline );
    //void CmHelp();
    const char* GetHtml();

    void CopyRecords( const char * prfName, TCStringArray& aCMnoused,
                     elmWindowData el_data, cmSetupData st_data );
}
;

#endif   // _m_compos.h

