//-------------------------------------------------------------------
// $Id: s_formula.h 1373 2009-07-22 12:25:22Z gems $
//
// Declaration of TFormula - gems chemical formulas parser
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2001  D.Kulik, S.Dmytriyeva
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

#ifndef _s_formula_h_
#define _s_formula_h_

#include "v_user.h"
#include "v_mod.h"

extern const char * CHARGE_NAME;

struct ICTERM     // description of parsed element
{
    string ick;
    string ick_iso;
    int val;             // valence IC
    double stoc;          // stoich. coef.

    ICTERM( const char* aIck, const char* aIso, int aVal, double aStoc ):
            ick(aIck), ick_iso(aIso), val(aVal), stoc(aStoc)
    {}
};

struct MOITERM    // description of moiety element
{
    string name;
    int  site;        // sublattice site
    double nj;          // moiety-site occupancy.

    MOITERM( const char* aName, int aSite, double aNj ):
            /*name(aName),*/ site(aSite), nj(aNj)
    {
       char val[100];
       sprintf(val, "{%s}%d", aName, site);
       name = string(val);
    }

};

class Formuan  // stack for analyzing formula
{
    string form_str;
    string charge_str;

    //std::vector<ICTERM>  ict_;

    int nSites;             // number of sites in formula
    //std::vector<MOITERM> moit_;

protected:

    void icadd(  std::vector<ICTERM>& itt_, const char *icn,
                 const char *iso, int val, double csto );
    void icadd(  std::vector<ICTERM>& itt_, ICTERM it );
    int ictcomp( std::vector<ICTERM>& itt_, int ii, string& ick, int val );


    inline bool iscapl( char ch )  // is cap letter
    {
        return( (ch>='A' && ch<='Z') || ch=='$' );
    }
    inline bool islowl( char ch )  // is lcase letter
    {
        return( (ch>='a' && ch<='z') ||  ch == '_' );
    }
    //char *xblanc( char *cur );
    void xblanc( string& str );

    void charge(std::vector<ICTERM>& tt);
    void   scanCharge();
    void scanFterm( std::vector<ICTERM>& itt_, string& startPos, char endSimb );
    void scanElem( std::vector<ICTERM>& itt_, string& cur );
    void getReal( double& real, string& cur);
    void scanValence( int& val, string& cur);
    void scanIsotope( string& isotop, string& cur);
    void scanICsymb(  string& icName, string& cur);


public:

    Formuan( const char * formula );
    ~Formuan();

    int scanMoiety( std::vector<MOITERM>& moit_ );
    void scanFormula( std::vector<ICTERM>& tt );

};


class TFormula  // description of disassembled formula token
{
    double aZ;   // calculated charge in Mol

    std::vector<std::string>  aCn;  // list of IC
    std::vector<double> aSC;  // list of stoichiometric coef.
    std::vector<short> aVal;  // list of valence numbers
    //std::vector<MOITERM> moit_;

    std::string aFormula;  // analayzed formula

protected:

    void fo_clear();
    void fo_unpak( std::vector<ICTERM>& itt_ );

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
        return aCn.size();
    }
    const char* GetFormula( ) const
    {
        return aFormula.c_str();
    }

    int BuildMoiety( const char * StrForm, std::vector<MOITERM>& moit_ );

    //--- Value manipulation
    void fixup_ics( char* ICs );
    void SetFormula( const char * StrForm ); // and ce_fscan
    int Fmwtz( double &Z, double &mW, double &eSm, short *lAn );
    int Fmwtz( double &Z, double &mW, double &eSm, short *lAn, double &Nj );
    void TestIC( const char *key, int N, char *ICsym );
    void Stm_line( int N, double *Sml, char *ICsym, short *ICval );
    void Reset();
    std::string form_extr( int nCk, int L, char *Cfor );
};



#endif  // _s_formula_h


