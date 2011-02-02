//-------------------------------------------------------------------
// $Id: m_const.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TConst class, config and calculation functions
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

#include "m_const.h"
#include "v_object.h"
#include "v_mod.h"
#include "service.h"

TConst* TConst::pm;

TConst::TConst( int nrt ):
        TCModule( nrt )
{
    CA=0;
    aFldKeysHelp.Add("Code of function using table of constants");
    aFldKeysHelp.Add("Table of constants ID and dimensions");
    set_def();
    start_title = " Numerical Constants ";
}


// link values to objects
void TConst::ods_link(int)
{
    aObj[o_constlab].SetPtr( ConLab );
    aObj[o_constdim].SetPtr( Dim );
}


// set default data or zero if nessasary
void TConst::set_def(int)
{
    Dim[0] = Dim[1] = Dim[2] = 2;
    Dim[3] = D_;
    *ConLab = '\0';
    dyn_new();
}


// set dynamic objects ptr to values
void TConst::dyn_set(int)
{
    CA = aObj[o_const].GetPtr();
    if( CA==0 )
        dyn_new();
}

// free dynamic memory in objects and values
void TConst::dyn_kill(int)
{
    CA = aObj[o_const].Free();
}

// realloc dynamic memory
void TConst::dyn_new(int)
{
    CA = aObj[o_const].Alloc( Dim[0]*Dim[1], Dim[2], Dim[3] );
}

// Help on Compos module  ( ? button )
const char* TConst::GetHtml()
{
   return GM_CONST_HTML;
}


//--------------------- End of m_const.cpp ---------------------------

