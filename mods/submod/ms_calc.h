//-------------------------------------------------------------------
// $Id: ms_calc.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TEQCalc and TEQDemo classes, config  functions
//
// Copyright (C) 1995-2001 S.Dmytriyeva
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
#ifndef _ms_calc_h_
#define _ms_calc_h_

#include "v_mod.h"

// Data of EQUSTAT calc
class TEQCalc :
            public TSubModule
{

protected:

public:

    TEQCalc( int nrt ): TSubModule( nrt )
    {}

    const char* GetName() const
    {
        return "Calc";
    }

    void ods_link( int /*i*/=0)
    {}

    void dyn_set( int /*i*/=0)
    {}

    void dyn_kill( int /*i*/=0)
    {}

    void dyn_new( int /*i*/=0)
    {}

    void set_def( int /*i*/=0)
    {}


};

// Data of EQUSTAT demo
class TEQDemo :
            public TSubModule
{
      gstring titler;

protected:

public:

    TEQDemo( int nrt ): TSubModule( nrt )
    {}

    const char* GetName() const
    {
        return "EqDemo";
    }

    void ods_link( int /*i*/=0)
    {}

    void dyn_set( int /*i*/=0)
    {}

    void dyn_kill( int /*i*/=0)
    {}

    void dyn_new( int /*i*/=0)
    {}

    void set_def( int /*i*/=0)
    {}

    const gstring& GetString()
    {
     titler = rt[RT_SYSEQ].PackKey();
     //titler += " : ";
     //titler += TSubModule::GetString();
     return titler;
    }


};

#endif  // _ms_calc_h
