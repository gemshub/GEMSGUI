//-------------------------------------------------------------------
// Id: gems/mods/m_icomp.h  version 2.0.0   2001
//
// Declaration of TIComp class, config and calculation functions
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

#ifndef _m_icomp_h_
#define _m_icomp_h_

#include "v_mod.h"
#include "v_module.h"

typedef struct
{ // Description  ICOMP
    char //symb[MAXICNAME], // "Symbol of Independent Component (IC)"
    //type[MAXSYMB],   // "IC class { e a z v <isotope> }"
    //grp[MAXICGROUP], // "Record Key Comment to IC description"
    sst[MAXSYMB],    // "Code of standard aggregate state { s l g a }"
    name[MAXFORMULA],// "Name of Independent Component"
    form[MAXFORMULA],// "Formula of Independent Component"
    dc_ref[DC_RKLEN];// "Reference to DCOMP record (reserved)"
    float awt,   // "Atomic (molar) mass, g/atom (g/mole)"
    aSs,   // "Atomic entropy S0 at standard state, J/mole/K"
    aCp,   // "Atomic heat capacity Cp0 at standard state, J/mole/K"
    aVs,   // "Atomic volume V0 at standard state, cm3/mole"
    arad,  // "Ionic radius,  (reserved)"
    idis;  // "Relative isotope abundance { 1; number }"
    short val,   // "Default valence number (charge) in compounds"
    num,     // "Index in Periodical (Mendeleev's) table"
    coor;    // "Default coordination number (reserved)"
    char *tprn;  // Print bufer
}

ICOMP;


// Current IComp
class TIComp : public TCModule
{
    ICOMP ic[1];

protected:


public:

    static TIComp* pm;

    ICOMP *icp;

    TIComp( int nrt );

    const char* GetName() const
    {
        return "IComp";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    void RecInput( const char *key );
    void RecordPrint( const char *key=0 );
    void CmHelp();                        
    //void RecBild( const char *key );
    //void RecCalc( const char *key );

    void GetElements( bool isotopes, TCStringArray& names,
                      TCStringArray& aIC, TCIntArray& aIndMT );
    void CopyElements( const char * prfName, TCStringArray& aKeys );
};

#endif  // _m_icomp_h
