//-------------------------------------------------------------------
// $Id$
//
// Declaration of IPNCalc class
//
// Copyright (C) 1996-2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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

#ifndef __v_ipnc_h_
#define __v_ipnc_h_

#include <ctype.h>
#include <fstream>
using namespace std;

#include "array.h"
#include "v_user.h"

struct RpnItem
{ // IPN item
    char code; // RpnItemType
    unsigned num;
    RpnItem(char nCode, unsigned nNum):
            code(nCode), num(nNum)
    {}

};

struct Equation
{  // IPN equation
    int nItems;    // count of items
    int first;     // position of first item
    Equation(int Item, int First):
            nItems(Item), first(First)
    {}

};

const int RPN_BLOCK = 200, // 1024,
                      EQUATION_BLOCK = 10, //256,
                                       CONST_BLOCK = 50, //256,
                                                     MAXSTACK  = 150;

const char CK_ = ';';		// end of equat

class IPNCalc
{
    enum RpnItemType { // element code
        IT_X,   // undefined
        IT_C,   // numerical constant
        IT_V,   // variable (TObject ob)
        IT_O,   // binary operation
        IT_U,   // unary operation
        IT_A,   // assigment =:
        IT_F,   // function
        IT_I,   // interval
        IT_T,   // conditional jump
        IT_G,   // unconditional jump
        IT_W,   // conditional jump to equation
        IT_B     // unconditional jump to equation
    };

    enum func_numbers {
        abs_f, sign_f, exp_f, sqrt_f, ln_f,
        lg_f, sin_f, sinh_f, cos_f, cosh_f, tan_f, tanh_f,
        ceil_f, floor_f, round_f, asin_f, acos_f, atan_f,
        erf_f, erfc_f,
        mod_f, sum_f, prod_f, max_f, min_f, null_f, one_f
    };

    TIArray<RpnItem> aItm;  // list of items
    TIArray<Equation> aEq;  // list of equations
    TOArray<double> aCon;  // list of constant
    char *input;

protected:
    void Push( char ch, unsigned n );
    void eq_add( int Item, int First=-1 );
    int con_add( double val );
    char *xblanc( char *cur );
    bool IsLetter( char c )
    {
        return(isalpha(c)||c=='_');
    }
    bool IsDigit( char c )
    {
        return(isdigit(c)||c=='.');
    }
    int INDEX( char *s, char ch );
    int SPRTY( char c );
    void InDigit();
    void IsAscii();
    void Ident( char *s );
    void Ffun( char * str);
    void Variab( char * str);
    void I_Variab( char * str);
    void bildEquat();
    void bildWhile();
    void bildIf();
    void RPN_expr( char ck );

public:
    IPNCalc();
    ~IPNCalc();
    void GetEquat( char *txt );
    void CalcEquat();
    void PrintEquat( char *s, fstream& f);
    int nEquat() const
    {
        return aEq.GetCount();
    }

};

#endif  // _v_ipnc_h
