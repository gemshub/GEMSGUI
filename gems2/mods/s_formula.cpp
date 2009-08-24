//-------------------------------------------------------------------
// $Id$
//
// Implementation of TFormula - gems chemical formulas parser
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
#include <stdio.h>
#include <math.h>

#include "s_formula.h"
#include "m_icomp.h"
#include "service.h"

enum scan_type{ SCAN_END  = -2,
                SCAN_NO   = -1,
                SCAN_YES  =  0,
                SCAN_1    =  1,
                SCAN_2    =  2,
                SCAN_3    =  3
              };

const char* BLANK_SYMBOLS =" \t\n";
const char* FLOAT_DIGITS  ="0123456789.";
const char* ISOTOPE_N     ="n";
const char* PSTATE_TOK    ="#:";   /*  +1; # -1 */
const char* CHARGE_TOK    ="+-@";
const char* CHARGE_NAME   ="Zz";
const char* CHARGE_TYPE   ="z";


const char LBRACKET1     ='(';
const char LBRACKET2     ='[';
const char RBRACKET1     =')';
const char RBRACKET2     =']';
const char B_ISOTOPE     ='/';
const char B_VALENT      ='|';

const char CHARGE_NUL    ='@';
const char CHARGE_PLUS   ='+';
const char CHARGE_MINUS  ='-';

const char PSUR_L_PLUS  =':';
const char PSUR_L_MINUS  ='#';

/*More examples of chemical formulae:

    Na+, Cl-, NaCl@        cation, anion and neutral aqueous species.
    SiO2, Fe3Si4O10(OH)2   quarts, minnesotaite.
    Fe|3|2O3               hematite.
    /3/H2/18/O             isotopic form of water.
*/

Formuan::Formuan( const char *formula ):
        form_buf(0), lev(0)
{
    int len = strlen(formula);
    form_buf = new char[len+5];
    memcpy( form_buf, formula, len );
    form_buf[len] = '\0';
    cur = form_buf;
    next = cur;

}

Formuan::~Formuan()
{
    delete[] form_buf;
}

// insert charge
double Formuan::charge( const char *chan )
{
//    int cha=1;
double cha = 1.0;
    int sign = 1;
    double aZ = 0.0;
    switch( *chan )
    {
    case CHARGE_NUL:
        return aZ;
    case CHARGE_MINUS:
        sign = -1;
    case CHARGE_PLUS:
        if( strlen( chan ) == 1 )
        {
            aZ += sign;
            return aZ;
        }
        else
//            if( sscanf( chan+1, "%d", &cha ) )
            if( sscanf( chan+1, "%lg", &cha ) )
            {
//                aZ += (float)(cha*sign);
                aZ += cha * sign;
                return aZ;
            }
    default:
        break;
    }
    Error( chan, "Error in formula charge");
    return aZ;
}


bool Formuan::iscapl( char ch )  // is cap letter
{
    if( (ch>='A' && ch<='Z') || ch=='$' )
        return true;
    return false;
}

bool Formuan::islowl( char ch )  // is lcase letter
{
    if( (ch>='a' && ch<='z') ||  ch == '_' )
        return true;
    return false;
}

char *Formuan::xblanc( char *cur_ )
{
    char *ncur;
    if( !cur_ || !*cur_ ) return( 0 );
    ncur = cur_ + strspn( cur_, BLANK_SYMBOLS );
    if( *ncur )
        return( ncur );
    else return( 0 );
}

/*
  <formula>   ::= <fterm> \ <fterm> <charge>
  <fterm>     ::= <icterm> \ <icterm><charge>
  <icterm>    ::= <elem> \ <elem> <elem_st_coef>
  <elem>      ::= (<fterm>) \ [<fterm>] \
                 <isotope_mass><icsymb><valence> \
                 <isotope_mass><icsymb> \ <icsymb><valence> \ <icsymb>
*/
int Formuan::bterm()
{
    int lev_ = lev;
    ;
    char *pp1, *pp2, rbc, lbc;

    if( (cur = xblanc( cur )) == 0 )
        return SCAN_END;
    if( *cur != LBRACKET1 && *cur != LBRACKET2 )
        return SCAN_NO;
    lbc = *cur;
    if( lbc == LBRACKET1 ) rbc = RBRACKET1;
    else rbc = RBRACKET2;
    pp1 = pp2 = cur+1;
LOOP1: // search ) or }
    pp1 = strchr( pp1, lbc );
LOOP2:
    pp2 = strchr( pp2, rbc );
    ErrorIf( !pp2, "TFormula","No ) or ].");
    if( !pp1 || pp1 > pp2 )  // no ( /] before pp2
    {
        if( lev_ == lev )
        {
            next = pp2+1;
            return SCAN_YES;
        }
        lev_--;
        pp2++;
        goto LOOP2;
    }
    else
    {
        lev_++;
        pp2 = ++pp1;
        goto LOOP1;
    }
}

//load <fterm>
//<icterm>    ::= <elem> \ <elem> <elem_st_coef>
void Formuan::push_sto()
{
    int len = strspn( next, FLOAT_DIGITS );
    if( !len )
    {
        mult[lev] = 1.;
        cur++;
        //return 0;
    }

    else   // read double number
    {
        gstring buf( next, 0, len);
        double sto;
        sscanf( buf.c_str(), "%lg", &sto );
        mult[lev] = sto;
        cur++;
        //return -1;
    }

}

//test ) or ]
int Formuan::rterm()
{
    int len;
    if( (cur = xblanc( cur )) == 0 )
        return SCAN_END;
    if( *cur != RBRACKET1 && *cur != RBRACKET2 )
        return SCAN_NO;
    len = strspn( ++cur, FLOAT_DIGITS );
    next = cur + len;
    return SCAN_YES;
}

//delete level ( or [ from stack
void Formuan::pop_sto()
{
    if( lev )
    {
        lev--;
        mult[lev] = 0.;
        cur = next; // return next - cur;
    }

}


// /3/H2/18/O             isotopic form of water.
//  <isotope_mass>  ::= /<integer>/
int Formuan::isotop()
{
    if( (cur = xblanc( cur )) == 0 )
        return SCAN_END;
    if( *cur != B_ISOTOPE )
        return SCAN_NO;
    next = strchr( cur+1, B_ISOTOPE );
    if( !next || ((next - cur -1) >= MAXICNAME) )
        Error( cur,  "No isotope");
    next++;
    return SCAN_YES;
}

//get isotop
void Formuan::iso_get( gstring& isotop )
{
    int len = next - cur;
    if( len <= 2 )
        Error( cur, "Must be <isotope_mass>  ::= /<integer>/" );
    isotop = gstring( cur+1, 0, len-2 );
    cur += len;
    //strncpy( isotop, aFa.cur+1, len-2 );
}


// <icsymb>    ::= <Capital_letter> \ <icsymb><lcase_letter> \ <icsymb>_

int Formuan::icsymb()
{
    uint i;
    if( (cur = xblanc( cur )) == 0 )
        return SCAN_END;
    if( !iscapl( *cur ))
        return SCAN_NO;
    for( i=1; i<=MAXICNAME; i++ )
        if( !islowl( *(cur+i)))
            goto STOP_S;
    Error(  cur,  "IC Symbol scan error");
STOP_S:
    next = cur+i;
    return SCAN_YES;
}

//get icsymb
void Formuan::symb_get( gstring& ic )
{
    int len = next - cur;
    ErrorIf( !len, cur, "Must be <icsymb>" );
    ic = gstring( cur, 0, len ); //  strncpy( ic, aFa.cur, len );
    cur += len;
}

//  <valence>   ::= |-<integer>| \ |+<integer>| \ |<integer>|
int Formuan::icval()
{
    if( (cur = xblanc( cur )) == 0 )
        return SCAN_END;
    if( *cur != B_VALENT )
        return SCAN_NO;
    next = strchr( cur+1, B_VALENT );
    if( !next || next-cur-1 >= 3 )
        Error( cur,  "Term valence scan error");
    next++;
    return SCAN_YES;
}

//get <valence>
void Formuan::val_get( short& val )
{
    int len = next - cur;
    ErrorIf(len <= 2, cur,
            "Must be |-<integer>| or |+<integer>| or |<integer>|");
    gstring buf( cur+1, 0, len-2 );
    int vv;
    if( sscanf( buf.c_str(), " %d", &vv ))
    {
        val=(short)vv;
        cur+=len;
    }
    else Error( cur, "Integer number scan error");
}

//  <elem_st_coef>  ::= <real>
//   <real>      ::= <num>.<num> \ <num>. \ .<num> \ <num>
int Formuan::icstoc()
{
    int len;
    if( ( cur = xblanc( cur )) == 0 )
        return SCAN_END;
    if( strcspn( cur, FLOAT_DIGITS ))
        return SCAN_NO;
    len = strspn( cur, FLOAT_DIGITS );
    next = cur + len;
    return SCAN_YES;
}

//get <elem_st_coef>  ::= <real>
void Formuan::stoc_get( double& bstoc )
{
    int len = next - cur;
    gstring buf( cur, 0, len );
    if( sscanf( buf.c_str(), " %lg", &bstoc )) //%lg - double
    {
        cur += len;
    }
    else Error( cur, "Real number scan error");
}

int Formuan::ictcomp( int ii, const char *ick, short val )
{
    int iRet = memcmp( itt_[ii].ick.c_str(), ick, MAXICNAME+MAXSYMB );
    if( iRet ) return( iRet );
    if( itt_[ii].val < val ) return(-1);
    if( itt_[ii].val > val ) return(1);
    return(0);
}

//add component to sorted list
void Formuan::icadd(  const char *icn,
                      const char *iso,short val,double csto)
{
    char ICkey[MAXICNAME+MAXSYMB+1];
    int iRet;
    uint ii=0;

    memset( ICkey, ' ',MAXICNAME+MAXSYMB );
    strncpy( ICkey, icn, min( strlen(icn),(size_t)MAXICNAME) );
    strncpy( ICkey+MAXICNAME, iso, min(strlen(iso),(size_t)MAXSYMB) );
    ICkey[MAXICNAME+MAXSYMB] = '\0';
    if( lev )
        for(int i=0; i<lev; i++ )
            csto *= mult[i];
    // test for setting element before
    while( ii < itt_.GetCount() )
    {
        iRet = ictcomp( ii, ICkey, val );
        if( iRet == 0 )// ic find
        {
            itt_[ii].stoc += csto;
            return;
        }
        if( iRet > 0 )
            break;
        ii++;
    }
    itt_.AddAt( new ICTERM( ICkey, val, csto ), ii );
}

// scan <fterm> s
double Formuan::scan_formulae( TIArray<ICTERM>& tt )
{
//    char savc=' ';
    char *pp, *fs;
    int ti, sur, len;

    int State = SCAN_YES;
    short val;
    double stoc, aZ =0.0;
    gstring ic_symb, ic_isotop;

    cur = form_buf;
    next = cur;
    lev = 0;
    memset( mult, 0, MAXLEVELS*sizeof(double) );

    itt_.Clear();

    fs = form_buf;
    ti = strcspn( fs, CHARGE_TOK );
    pp = strrchr( fs, B_VALENT );
    sur = strcspn( fs, PSTATE_TOK );
    len = strlen(fs);

LOOPZ:
    if( ti < len )
    {
        if( fs+ti > pp ) // charge is set, get it
        {
            if( fs[ti+1] == ' ' )
                len = ti+1;
            gstring charge_s( fs, ti, len-ti+1 );
            double aZZ = charge( charge_s.c_str() );
            icadd( CHARGE_NAME, CHARGE_TYPE, 1, aZZ );
            aZ = 0.0;
//            savc = *(fs+ti);
            *(fs+ti) = 0;
            len = ti;
SUR_AGAIN:
            if( sur<len )
                switch( fs[sur] )// reserved for surface species
                {
                case PSUR_L_PLUS:
                    aZ += 1;
                    fs[sur++] = ' ';
                    goto SUR_AGAIN;
                case PSUR_L_MINUS:
                    aZ -= 1;
                    fs[sur++] = ' ';
                    goto SUR_AGAIN;
                default:
                    break;
                }
        }
        else //valence
        {
            ti = pp-fs + strcspn( pp, CHARGE_TOK );
            goto LOOPZ;
        }
    }

    while( State != SCAN_END )
    {
        State = bterm();
        switch( State )
        {
        case SCAN_NO:
            break;
        case SCAN_END:
            continue;
        case SCAN_YES:
            push_sto();
            lev++;
            continue;

        }
        State = rterm();
        switch( State )
        {
        case SCAN_NO:
            break;
        case SCAN_END:
            continue;
        case SCAN_YES:
            pop_sto();
            continue;
        }
        State = isotop();
        switch( State )
        {
        case SCAN_NO:
            ic_isotop = gstring(ISOTOPE_N);
            break;
        case SCAN_YES:
            iso_get( ic_isotop );
            break;
        default:
            Error( cur, " E30FPrun: A symbol of element expected here!");
        }
        State = icsymb(); // symbol of element
        switch( State )
        {
        case SCAN_YES:
            symb_get( ic_symb );
            break;
        default:
            Error( cur, " E30FPrun: A symbol of element expected here!");
        }
        State = icval(); // valence
        switch( State )
        {
        case SCAN_END:
        case SCAN_NO:
            val = SHORT_EMPTY;
            break;
        case SCAN_YES:
            val_get( val );
            break;
        }
        State = icstoc(); // stoich. coef.
        switch( State )
        {
        case SCAN_END:
        case SCAN_NO:
            stoc = 1.;
            break;
        case SCAN_YES:
            stoc_get( stoc );
            break;
        }
        //  set term
        icadd( ic_symb.c_str(), ic_isotop.c_str(), val, stoc );
    }
    // if( savc != ' ') *(fs+ti) = savc;
    // all terms scanned
    ErrorIf( !itt_.GetCount() || lev, form_buf,
             "E31FPrun: Errors in scanning chemical formula" );
    tt.Clear();
    for(uint ii=0; ii<itt_.GetCount(); ii++)
        tt.Add( new ICTERM(itt_[ii]) );
    return aZ;
}


//----------------------------------------------------------
// TFormula
//----------------------------------------------------------

void TFormula::fo_clear()
{
    aCn.Clear();
    aSC.Clear();
    aVal.Clear();
}

void TFormula::Reset()
{
    fo_clear();
    aFormula = "";
    aZ = 0;
}


TFormula::TFormula( ):
        aZ(0), aFormula("")
{}


TFormula::~TFormula()
{
    Reset();
}

// unpack list of terms to data
void TFormula::fo_unpak( TIArray<ICTERM>& itt_ )
{
    fo_clear();
    vstr ICS(MAXICNAME), ICI(MAXICNAME), ICbuf(MAXICNAME*2);

    for(uint i=0; i<itt_.GetCount(); i++ )
    {
        memset( ICbuf, ' ', MAXICNAME+MAXSYMB );
        sscanf(  itt_[i].ick.c_str(), "%s %s", ICS.p, ICI.p );
        strncpy( ICbuf, ICS, strlen(ICS));
        if( !strcmp( ICI, CHARGE_TYPE ) ||
                !strcmp( ICI, ISOTOPE_N ) ||
                *ICI == 'v' )
            ICbuf[MAXICNAME] = '*';
        else
            strncpy( ICbuf+MAXICNAME, ICI, strlen(ICI));
        ICbuf[MAXICNAME+MAXSYMB]=0;

        aCn.Add( ICbuf.p );
        aSC.Add( itt_[i].stoc );
        aVal.Add( itt_[i].val  );

        if( itt_[i].val != SHORT_EMPTY/*I_EMPTY*/ && *ICI != 'z' )
            aZ += itt_[i].stoc * itt_[i].val;
    }
}

const char* EQDEL_CHARS   ="$&,=<>";

// Set new formula and analyze it  (ce_fscan)
void TFormula::SetFormula( const char * StrForm )
{
    int len, ti;

    aZ = 0.;
    fo_clear();
    aFormula = gstring(StrForm);
    if( aFormula.empty() )
        return;

    len = strcspn( StrForm, EQDEL_CHARS );
    ti = strlen( StrForm );
    len = ( len < ti )? len: ti;
    ErrorIf( !len, "TFormula", "E32FPrun: Null length of the formula string");

    gstring fbuf = gstring( StrForm, 0, len );

    TIArray<ICTERM> itt_;

    Formuan aFa( fbuf.c_str() );
    aZ = aFa.scan_formulae( itt_ );

    fo_unpak( itt_ );
}

// add key IC
void TFormula::fixup_ics( char* ICs )
{
    if( !isdigit( ICs[MAXICNAME] ))
        switch( *ICs )
        {
        case 'Z':
            if( *(ICs+1) == 'z' )
                ICs[MAXICNAME] = IC_CHARGE;
            else ICs[MAXICNAME] = IC_ELEMENT;
            break;
        case 'V':
            if( *(ICs+1) == 'o' && *(ICs+2) == 'l' )
                ICs[MAXICNAME] = IC_VOLUME;
            else ICs[MAXICNAME] = IC_ELEMENT;
            break;
        case 'O':
            if( *(ICs+1) == ' ' )
                ICs[MAXICNAME] = IC_OXYGEN;
            else ICs[MAXICNAME] = IC_ELEMENT;
            break;
        case 'H':
            if( *(ICs+1) == ' ' )
                ICs[MAXICNAME] = IC_HYDROGEN;
             else ICs[MAXICNAME] = IC_ELEMENT;
             break;
        default:
            ICs[MAXICNAME] = IC_ELEMENT;
//            if( strcspn( ICs, " *" ) > 2 )
//                ICs[MAXICNAME] = IC_ADDIT;
            // symbols must be >2 characters and not start with Z V O - obsolete!
        }
// IC_TEST: // Changed by DK on 5.09.2007  New rule: if IC symbol has >= 3 chars
            // then it is declared as IC_ADDIT, except Vol (IC_VOLUME)
               // Caution! Anything starting with Vol is recognized as IC_VOLUME!
            // Zz is recognized as IC_CHARGE; O as IC_OXYGEN; H as IC_HYDROGEN
    if( ICs[MAXICNAME] != IC_VOLUME )
         if( strcspn( ICs, " *" ) > 2 )
                ICs[MAXICNAME] = IC_ADDIT; // >2 and <6 characters: IC_ADDIT is set!
    ICs[MAXICNAME+MAXSYMB] = '*';  // This key field is actually not checked!
}

// calculate charge, molar mass and elemental entropy from chemical formulae
// Modified by adding extraction of atomic numbers lAn on 28.03.2008  DK TW
// returns number of elements (ICs) used in the formula
int TFormula::Fmwtz( double &Z, double &mW, double &eS, short *lAn )
{
    time_t icrtim;
    TIComp* aIC = TIComp::pm;
    vstr ICs(MAXICNAME+MAXSYMB+3);
    double Sc, Zf=0.0;

    Z =  mW =  eS = 0.0;
    for(uint i=0; i<aCn.GetCount(); i++ ) // TERMS
    {
        memset( ICs, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICs, aCn[i].c_str(), MAXICNAME+MAXSYMB );
        ICs[MAXICNAME+MAXSYMB]=0;
        fixup_ics( ICs );
        // read and if necessary recalc IComp
        aIC->TryRecInp( ICs, icrtim, 0 );
        Sc = aSC[i];
        mW += Sc * aIC->icp->awt;
        eS += Sc * aIC->icp->aSs;
        lAn[i] = aIC->icp->num;    // getting atomic number
        if( aVal[i] == SHORT_EMPTY/*I_EMPTY*/ )
            aVal[i] = aIC->icp->val;
        Z += Sc * aVal[i];
        if( ICs[MAXICNAME] == IC_CHARGE )
            Zf = Sc;
    }
    if( fabs( Zf ) > 1e-10 && fabs(Z) > 1e-10 )
        Z -= Zf;
    return aCn.GetCount();
}

const char* tc_ecname    ="Zz";

// get string of stoichiometric matrix from unpack formulae,
// ICsym - list of IC names, ICval standart valence
void TFormula::Stm_line( int N, double *Sml, char *ICsym, short *ICval )
{
    uint i, ii;
    int jj=-1;
    vstr ICS(MAXICNAME+MAXSYMB+10);
    char *icsp = ICS;

    fillValue( Sml, 0., N);
    for( i=0; i<aCn.GetCount(); i++ )
    {
        memset( ICS, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICS, aCn[i].c_str(), MAXICNAME+MAXSYMB );
        ICS[MAXICNAME+MAXSYMB] = 0;
        fixup_ics( ICS );
        for(int j=0; j<N; j++ )
        {
            if( !memcmp( ICsym+j*IC_RKLEN, ICS, MAXICNAME+MAXSYMB ))
            {
                jj = j;
                break;
            }
        }
        if( jj==-1 )
            Error( icsp, "E33FPrun: This is not a symbol of IComp!");
        Sml[jj] += aSC[i];
        if( aVal[i] == SHORT_EMPTY/*I_EMPTY*/ )
            aVal[i] = ICval[jj];
    }
    ii = aCn.GetCount();
    if( ii < 1 )
        return;
    aZ = 0;    // calc charge balans
    if( !memcmp( aCn[ii-1].c_str(), tc_ecname, 2 ))
        ii--;
    for( i=0; i<ii; i++ )
        aZ += aVal[i] * aSC[i];

    // Sveta
    double tt=aSC[ii];
    if( ii < aCn.GetCount() )
        if( fabs( (aZ - tt) ) > 1e-6 )
        {
            gstring str = " in the formula: ";
            str +=  aFormula;
            str += "\n calculated charge: ";
            vstr   buf(40);
            sprintf( buf, "%lg != %lg", aZ, tt );
            str += buf.p;
 aSC[ii] = aZ;  // KD 03.01.04  - temporary workaround (adsorption)
            vfMessage( 0,  "W34FPrun: Charge disbalance ", str.c_str() );
         }
}

// Get formule number nCK from list Cfor (L - number formules in list)
gstring TFormula::form_extr( int nCk, int L, char *Cfor )
{
    int i, len;
    char  *Fbg;

    ErrorIf( nCk >= L || nCk < 0, "Formula", "E35FPrun: Error in the formula list!" );
    Fbg = Cfor;
    if( nCk )
        for( i=0; i<nCk; i++ )
        {
            len = strcspn( Fbg, EQDEL_CHARS );
            ErrorIf( !len && nCk < L-1, "Formula", "E35FPrun: Error in the formula list" );
            Fbg += len+1;
            Fbg += strspn( Fbg, BLANK_SYMBOLS );
        }
    if( nCk == L-1 )
        len = strlen( Fbg );
    else
        len = strcspn( Fbg, EQDEL_CHARS );

    ErrorIf( !len, "Formula", "E36FPrun: Invalid formula length!" );

    gstring rez( Fbg, 0, len );
    return rez;
}

// test all IComps in DB, for set before formula
void TFormula::TestIC( const char* key, int N, char *ICsym )
{
    uint i;
    int jj=-1;
    vstr ICS(MAXICNAME+MAXSYMB+10);

    for( i=0; i<aCn.GetCount(); i++ )
    {
        memset( ICS, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICS, aCn[i].c_str(), MAXICNAME+MAXSYMB );
        ICS[MAXICNAME+MAXSYMB] = 0;
        fixup_ics( ICS );
        for(int j=0; j<N; j++ )
        {
            if( !memcmp( ICsym+j*IC_RKLEN, ICS, MAXICNAME+MAXSYMB ))
            {
                jj = j;
                break;
            }
        }
        if( jj==-1 )
        {
            gstring msg = "IComp: ";
            msg += gstring(ICS.p, 0, MAXICNAME+MAXSYMB );
            msg += "\n in formula in DComp/ReacDC record: \n";
            msg += gstring( key, 0, DC_RKLEN);

            Error( "E37FPrun: Invalid symbol ", msg.c_str() );
        }
    }
}

//--------------------- End of s_formula.cpp ---------------------------

