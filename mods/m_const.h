//-------------------------------------------------------------------
// $Id$
//
// Declaration of TConst class, config and calculation functions
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
#ifndef _m_const_h_
#define _m_const_h_

#include "v_module.h"

const int CONST_LEN = 32;

// Module TConst
class TConst : public TCModule
{

    char ConLab[CONST_LEN]; //Label of Array of Constants
    short Dim[4];             //Dimension of array K, I, J

    /*short DimI;
      short DimJ;
      short Type;             // type (I_ F_ D_)
    */

protected:

public:

    static TConst* pm;

    TConst( int nrt );
    const char* GetName() const
    {
        return "Const";
    }

    void ods_link( int i=0);
    void dyn_set( int i=0);
    void dyn_kill( int i=0);
    void dyn_new( int i=0);
    void set_def( int i=0);
    void *CA;

};

#endif   // _m_const_h

