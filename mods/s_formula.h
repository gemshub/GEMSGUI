//-------------------------------------------------------------------
// $Id$
//
// Declaration of TFormula - gems chemical formulas parser
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001  D.Kulik, S.Dmytriyeva
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

#ifndef _s_formula_h_
#define _s_formula_h_

#include "array.h"
#include "v_user.h"
#include "v_mod.h"
#include "gstring.h"

extern const char * CHARGE_NAME;

struct ICTERM
{ // description of parsed element
    gstring ick;
    short val;             // valence IC
    double stoc;          // stoich. coef.
    ICTERM( const char* aIck, short aVal, double aStoc ):
            ick(aIck), val(aVal), stoc(aStoc)
    {}

    ICTERM( ICTERM& data ):
            val(data.val), stoc(data.stoc)
    {
        ick = data.ick;
    }
};

class Formuan  // stack for analyzing formula
{
    char *form_buf;

    char *cur;   // current position
    char *next;  // position of end of token +1
    int lev;     // level of recursion
    double mult[MAXLEVELS];  // stack stoich. coef.

    TIArray<ICTERM> itt_;

protected:

    double charge( const char *chan );
    bool iscapl( char ch );
    bool islowl( char ch );
    char *xblanc( char *cur );
    int ictcomp( int ii, const char *ick, short val );
    void icadd(  const char *icn,
                 const char *iso, short val, double csto );
    int bterm();
    int rterm();
    int isotop();
    int icsymb();
    int icval();
    int icstoc();
    void push_sto();
    void pop_sto();
    void iso_get( gstring& isotop );
    void symb_get( gstring& ic );
    void val_get( short& val );
    void stoc_get( double& bstoc );

public:

    Formuan( const char * formula );
    ~Formuan();

    double scan_formulae( TIArray<ICTERM>& tt );
};

class TFormula  // description of disassembled formula token
{
    double aZ;   // calculated charge in Mol

    TCStringArray  aCn;  // list of IC
    TOArray<double> aSC;  // list of stoichiometric coef.
    TOArray<short> aVal;  // list of valence numbers

    gstring aFormula;  // analayzed formula

protected:

    void fo_clear();
    void fo_unpak( TIArray<ICTERM>& itt_ );

public:

    TFormula();
    ~TFormula();

    //--- Selectors
    const char* GetCn( int ii ) const
    {
        return aCn[ii].c_str();
    }
    double GetSC( int ii ) const
    {
        return aSC[ii];
    }
    short GetVal( int ii ) const
    {
        return aVal[ii];
    }
    double GetZ() const
    {
        return aZ;
    }
    int GetIn() const
    {
        return aCn.GetCount();
    }
    const char* GetFormula( ) const
    {
        return aFormula.c_str();
    }

    //--- Value manipulation
    void fixup_ics( char* ICs );
    void SetFormula( const char * StrFopm ); // and ce_fscan
    int Fmwtz( double &Z, double &mW, double &eSm, short *lAn );
    void TestIC( const char *key, int N, char *ICsym );
    void Stm_line( int N, double *Sml, char *ICsym, short *ICval );
    void Reset();
    gstring form_extr( int nCk, int L, char *Cfor );

};



#endif  // _s_formula_h


