//-------------------------------------------------------------------
// $Id: m_sdata.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TSData class, config and calculation functions
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

#ifndef _r_sdata_h_
#define _r_sdata_h_

#include "v_module.h"
#include "v_mod.h"

const int
MAXSDNAME = 20,
            MAXSDYEAR = 5,
                        MAXSDTYPE = 7,
                                    MAXSDSTRING = 72,
                                                  V_SDKWD_LEN = 32;

// Module TSData
// Screenform & chain SDATA_
class TSData : public TCModule
{
    char auth[MAXSDNAME+1];  //Author's name(s)
    char year[MAXSDYEAR+1];  //Year
    char type[MAXSDTYPE+1];  //Ref Type: pap book diss rep map dset code
    char Nkwd;             // number of key words
    bool Pref;             // exist abstract
    char authors[MAXSDSTRING]; //Author(s), complete
    char editn[MAXSDSTRING];   //Edition (journal), editors, city,...
    char volyr[MAXSDSTRING];   //Year, volume, issue
    char pages[MAXSDSTRING];   //Pages, other data
    char *refs;    //Next record key of Data Source
    char notes[MAXSDSTRING];   //Comments
    char *title;
    char *abstr;               //Abstract
    char *kwd[V_SDKWD_LEN];    //List of keywords [0:Nkwd] (reserved)

    char chs[5];
    short nREf;

protected:

public:

    static TSData* pm;

    TSData( int nrt );
    const char* GetName() const
    {
        return "SDref";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);

    int RecBuild( const char *key, int mode = VF_UNDEF );
    void RecSave( const char *key, bool onOld ); // Added 09.04.03 KD

    void CmHelp();                       // 05.01.01
    char *getAbstr() const
     { return abstr;  }             //Abstract

};


#endif // _m_sdata_h

