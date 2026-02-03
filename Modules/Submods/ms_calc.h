//-------------------------------------------------------------------
// $Id: ms_calc.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TEQCalc and TEQDemo classes, config  functions
//
// Copyright (C) 1995-2001 S.Dmytriyeva
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
#ifndef _ms_calc_h_
#define _ms_calc_h_

#include "v_module.h"
#include "service.h"

// Data of EQUSTAT calc
class TEQCalc :
            public TCModule
{

protected:

public:

    TEQCalc( int nrt ): TCModule( nrt )
    {}

    const char* GetName() const override
    {
        return "Calc";
    }

    void ods_link( int /*i*/=0) override
    {}

    void dyn_set( int /*i*/=0) override
    {}

    void dyn_kill( int /*i*/=0) override
    {}

    void dyn_new( int /*i*/=0) override
    {}

    void set_def( int /*i*/=0) override
    {}

    const char* GetHtml() override
    {
       return GSM_EQDEMO_HTML;
    }

};

// Data of EQUSTAT demo
class TEQDemo :
            public TCModule
{
      std::string titler;

protected:

public:

    TEQDemo( int nrt ): TCModule( nrt )
    {}

    const char* GetName() const override
    {
        return "EqDemo";
    }

    void ods_link( int /*i*/=0) override
    {}

    void dyn_set( int /*i*/=0) override
    {}

    void dyn_kill( int /*i*/=0) override
    {}

    void dyn_new( int /*i*/=0) override
    {}

    void set_def( int /*i*/=0) override
    {}

    const std::string& GetString() override
    {
     titler = rt[RT_SYSEQ]->PackKey();
     return titler;
    }

    const char* GetHtml() override
    {
       return GSM_EQDEMO_HTML;
    }

};

#endif  // _ms_calc_h
