//-------------------------------------------------------------------
// $Id: s_formula.cpp 1375 2009-07-24 11:41:49Z gems $
//
// Implementation of TFormula - gems chemical formulas parser
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
#include <cstdio>
#include <cmath>

#include "s_formula.h"
#include "m_icomp.h"
#include "service.h"

const char* BLANK_SYMBOLS =" \t\n";

const char LBRACKET1     ='(';
const char LBRACKET2     ='[';
const char LBRACKET3     ='{';
const char RBRACKET1     =')';
const char RBRACKET2     =']';
const char RBRACKET3     ='}';

const char B_ISOTOPE     ='/';
const char B_VALENT      ='|';
const char PSUR_L_PLUS  =':';
const char* FLOAT_DIGITS  ="0123456789.e";

const char* CHARGE_NAME   ="Zz";
const char* CHARGE_TYPE   ="z";
const char* CHARGE_TOK    ="+-@";
const char CHARGE_NUL    ='@';
const char CHARGE_PLUS   ='+';
const char CHARGE_MINUS  ='-';

const char* ISOTOPE_N     ="n";
const char* VACANCY       ="Va";


//---------------------------------------------------------------------------
// class Formuan - description of disassembled formula token
//---------------------------------------------------------------------------


/*More examples of chemical formulae:

    Na+, Cl-, NaCl@        cation, anion and neutral aqueous species.
    SiO2, Fe3Si4O10(OH)2   quarts, minnesotaite.
    Fe|3|2O3               hematite.
    /3/H2/18/O             isotopic form of water.
*/
Formuan::Formuan( const char *formula )
{
   form_str = string(formula);
   charge_str = "";
}

Formuan::~Formuan()
{ }

//get <formula>  ::= <fterm> | <fterm><charge>
void Formuan::scanFormula( std::vector<ICTERM>& tt )
{
  scanCharge();
  string term = form_str;
  scanFterm( tt, term, '\0');

  // added charge item
  if( !charge_str.empty() )
       charge(tt);

  // TEST OUTPUT
  if( gui_logger->should_log(spdlog::level::trace)) {
      std::ostringstream logs;
      for( uint ii=0; ii<tt.size(); ii++)
          logs << tt[ii].ick << " " << tt[ii].val << "  " << tt[ii].stoc << "\n";
      gui_logger->trace("Formula {} ZZ {} \n {} ", form_str, charge_str, logs.str());
  }
}

//add component to sorted list
void Formuan::icadd(  std::vector<ICTERM>& itt_, ICTERM it )
{
    int iRet;
    size_t ii=0;

    // test for setting element before
    while( ii < itt_.size() )
    {
        iRet = ictcomp( itt_, ii, it.ick, it.val );
        if( iRet == 0 )// ic find
        {
            itt_[ii].stoc += it.stoc;
            return;
        }
        if( iRet > 0 )
            break;
        ii++;
    }
    itt_.insert( itt_.begin()+ii, ICTERM( it ) );
}

//add component to sorted list
void Formuan::icadd(  std::vector<ICTERM>& itt_, const char *icn,
                      const char *iso, int val, double csto )
{
    ICTERM term( icn, iso, val, csto );
    icadd( itt_, term );
}

int Formuan::ictcomp( std::vector<ICTERM>& itt_, size_t ii, string& ick, int val )
{
    //int iRet = memcmp( itt_[ii].ick.c_str(), ick, MAXICNAME+MAXSYMB );
    //if( iRet ) return( iRet );
    if( itt_[ii].ick < ick ) return(-1);
    if( itt_[ii].ick > ick ) return(1);
    if( itt_[ii].val < val ) return(-1);
    if( itt_[ii].val > val ) return(1);
    return(0);
}


void Formuan::xblanc( string& str )
{
    if(str.empty())
      return;
    size_t ti = str.find_first_not_of(BLANK_SYMBOLS);
    if( ti == string::npos ) // no charge token
       str = "";
    else
       str = str.substr(ti);
    /*char *ncur;
    if( !cur_ || !*cur_ ) return( 0 );
    ncur = cur_ + strspn( cur_, BLANK_SYMBOLS );
    if( *ncur )
        return( ncur );
    else return( 0 );*/
}

void Formuan::scanCharge( )
{
  size_t ti = form_str.find_last_of(CHARGE_TOK );
  if( ti == string::npos ) // no charge token
     return;

  size_t pp = form_str.find( B_VALENT, ti);
  if( pp != string::npos )   // no charge (this is valence)
      return;

  // get charge string
  charge_str = form_str.substr(ti);
  form_str   = form_str.substr(0,ti);
}


// read charge
void Formuan::charge(std::vector<ICTERM>& tt)
{
 double cha = 1.0;
 int sign = 1;
 double aZ = 0.0;
 string chan = charge_str;
 //char *chan = (char *)charge_str.c_str();

 switch( chan[0] )
 {
    case CHARGE_NUL:    break;
    case CHARGE_MINUS:  sign = -1;
                        [[fallthrough]];
    case CHARGE_PLUS:
                        chan = chan.substr(1);
                        getReal( cha, chan );
                        aZ = cha * sign;
    default:            break;
 }
 icadd( tt, CHARGE_NAME, CHARGE_TYPE, 1, aZ );
}

//get <fterm>  ::= <icterm> | <icterm><icterm>
//    <icterm> ::= <elem>   | <elem>< elem_st_coef>
void Formuan::scanFterm( std::vector<ICTERM>& itt_, string& startPos, char endSimb )
{
  //char *cur_ = startPos;
  size_t ii;
  double st_coef;
  std::vector<ICTERM> elt_;

  while( startPos[0] != endSimb && !startPos.empty())  // list of <elem>< elem_st_coef>
  {
      // get element
      elt_.clear();

      scanElem( elt_, startPos );

      if( !startPos.empty() )
      {
        // get elem_st_coef
        st_coef = 1.;
        getReal( st_coef, startPos );
        for(size_t ii1=0; ii1<elt_.size(); ii1++)
          elt_[ii1].stoc *= st_coef;
      }

      // added elements list to top level elements
      for( ii=0; ii<elt_.size(); ii++ )
        icadd(  itt_, elt_[ii] );

      xblanc( startPos );
      if( startPos.empty() )
          return;
   }
}

//get <elem>    ::= (<fterm>) | [<fterm>] |
//                   <isotope_mass><icsymb><valence> |
//                   <isotope_mass><icsymb> |
//                   <icsymb><valence> | <icsymb>
void Formuan::scanElem( std::vector<ICTERM>& itt_, string& startPos )
{
  //char *cur = startPos;

  xblanc( startPos );
  if( startPos.empty() )
        return;

  switch( startPos[0] )
  {

    case   LBRACKET1: startPos = startPos.substr(1);
                      scanFterm( itt_, startPos, RBRACKET1 );
                      ErrorIf( startPos[0]!=RBRACKET1, "TFormula","Must be )");
                      startPos = startPos.substr(1);
                      break;
    case   LBRACKET2: startPos = startPos.substr(1);
                      scanFterm( itt_, startPos, RBRACKET2 );
                      ErrorIf( startPos[0]!=RBRACKET2, "TFormula","Must be ]");
                      startPos = startPos.substr(1);
                      break;
    case   LBRACKET3: startPos = startPos.substr(1);
                      scanFterm( itt_, startPos, RBRACKET3 );
                      ErrorIf( startPos[0]!=RBRACKET3, "TFormula","Must be }");
                      startPos = startPos.substr(1);
                      break;
    case   PSUR_L_PLUS: startPos = startPos.substr(1);
                      break;
    case   'V':      if( startPos[1] == 'a' )  // Va - ignore vacancy
                      {   startPos = startPos.substr(2);
                          break;
                      } // else goto default - other <icsymb>
                    [[fallthrough]];
    default: // <isotope_mass><icsymb><valence>
        {
          string isotop = string(ISOTOPE_N);
          string icName = "";
          int val = SHORT_EMPTY;;

          scanIsotope( isotop, startPos);
          scanICsymb( icName, startPos);
          scanValence( val, startPos);
          icadd( itt_, icName.c_str(), isotop.c_str(), val, 1. );
          break;
       }
  }
}


// test and get
//  <elem_st_coef>  ::= <real>
//  <real>    ::= <num>.<num> | <num>. | .<num> | <num>
//  <num>     ::= <digit> | <num><digit>
//  <digit>   ::= 0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9
void Formuan::getReal( double& valReal, string& cur)
{
    xblanc( cur );
    if( cur.empty() )
        return;

    size_t ti = cur.find_first_not_of(FLOAT_DIGITS);
    if(ti == 0)
      return;  // next token no real

    string val = cur.substr(0,ti);
    if( ti== string::npos )
       cur="";
    else
       cur = cur.substr(ti);

    if( !sscanf( val.c_str(), " %lg", &valReal )) //%lg - double
        Error( cur, "Real number scan error");
}

// get <valence>   ::= |-<integer>| \ |+<integer>| \ |<integer>|
//  <integer>    ::= <num>
void Formuan::scanValence( int& val, string& cur)
{
    xblanc( cur );
    if( cur.empty() )
        return;

    if( cur[0] != B_VALENT ) // next token no valence
        return;

    cur = cur.substr(1);
    if(cur.empty())
        Error( cur,  "Term valence scan error");

    size_t ti = cur.find_first_of(B_VALENT);
    if( ti >= 3 || ti==string::npos )
        Error( cur,  "Term valence scan error");

    if( !sscanf( cur.c_str(), " %d", &val ))
        Error( cur, "Integer number scan error");
    cur = cur.substr(ti+1);
}

// /3/H2/18/O             isotopic form of water.
//  get <isotope_mass>  ::= /<integer>/
void Formuan::scanIsotope( string& isotop, string& cur)
{
    xblanc( cur );
    if( cur.empty() )
        return;

    if( cur[0] != B_ISOTOPE ) // next token no isotop
        return;

    cur = cur.substr(1);
    if(cur.empty())
        Error( cur,  "Term isotope scan error");

    size_t ti = cur.find_first_of(B_ISOTOPE);
    if( ti >= MAXICNAME || ti==string::npos )
        Error( cur,  "Term isotope scan error");

    isotop = string( cur, 0, ti );  // test please
    cur = cur.substr(ti+1);
}

// <icsymb>    ::= <Capital_letter> \ <icsymb><lcase_letter> \ <icsymb>_
void Formuan::scanICsymb( string& icName, string& cur)
{
    uint i=1;

    xblanc( cur );
    if( cur.empty() )
        return;

    if( !iscapl( cur[0] ))
        Error( cur, " E30FPrun: A symbol of element expected here!");

    for( i=1; i<=MAXICNAME+2; i++ )
       if( !islowl( cur[i]))
           break;
    ErrorIf(  i>=MAXICNAME, cur.c_str(),  "IC Symbol scan error");

    icName = string( cur, 0, i ); //  strncpy( ic, aFa.cur, len );
    cur = cur.substr(i);
}

// <fterm>  ::= <site_term> : | <fterm> <site_term>:
// <site_term> ::= <moiety>   | <moiety><moiety>
// <moiety>    ::= {<elem>}   | {<elem>} <elem_st_coef> | Va
int Formuan::scanMoiety( std::vector<MOITERM>& moit_ )
{
  string cur_ = form_str;
  size_t endmoi;
  string moiName;
  double nj;          // moiety-site occupancy.
  moit_.clear();

  nSites = 0;
  while(  !cur_.empty() )  // list of {<elem>}< elem_st_coef>
  {
     switch( cur_[0])
     { case PSUR_L_PLUS:  nSites++;
                          cur_ = cur_.substr(1);
                          continue;
       case LBRACKET3: {
                      cur_ = cur_.substr(1);
                      if( cur_.empty() )
                          Error( "scanMoiety","Must be }");
                      endmoi =  cur_.find_first_of( RBRACKET3 );
                      if( endmoi == string::npos )
                          Error( "scanMoiety","Must be }");
                      moiName = string( cur_, 0, endmoi );
                      //                                            moiName = string( cur_, 0, endmoi-1 );
                      cur_ = cur_.substr(endmoi+1);
                      nj = 1.;
                      getReal( nj, cur_ );
                      moit_.push_back( MOITERM(moiName.c_str(), nSites, nj ));
                      }
                     break;
        case 'V':  if( cur_[1] == 'a' ) // Va vacancy
                   {
                      cur_ = cur_.substr(2); //Va
                      nj = 1.;
                      getReal( nj, cur_ );
                      moit_.push_back( MOITERM("Va", nSites, nj ));
                      break;
                   }
                  [[fallthrough]];
                  // else other symbol
        default:   cur_ = cur_.substr(1);
                    break;
     }
   }
  return nSites;
 }



//----------------------------------------------------------
// TFormula
//----------------------------------------------------------

TFormula::TFormula( ):
  aZ(0), aFormula("")
{}


TFormula::~TFormula()
{
    Reset();
}

void TFormula::fo_clear()
{
    aCn.clear();
    aSC.clear();
    aVal.clear();
    aZ = 0.;
}

void TFormula::Reset()
{
    fo_clear();
    aFormula = "";
}

// unpack list of terms to data and calculate charge
void TFormula::fo_unpak( std::vector<ICTERM>& itt_ )
{
    char ICbuf[MAXICNAME+MAXSYMB+2];
    string specSim = string(CHARGE_TYPE) + string(ISOTOPE_N) + "v";

    fo_clear();

    for(size_t i=0; i<itt_.size(); i++ )
    {
        memset( ICbuf, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICbuf, itt_[i].ick.c_str(), min(itt_[i].ick.length(), (size_t)MAXICNAME));

        if( itt_[i].ick_iso.find_first_not_of(specSim) != string::npos )
            strncpy( ICbuf+MAXICNAME, itt_[i].ick_iso.c_str(), min(itt_[i].ick_iso.length(), (size_t)MAXSYMB));
        else
            ICbuf[MAXICNAME] = '*';
        ICbuf[MAXICNAME+MAXSYMB]=0;

        aCn.push_back( ICbuf );
        aSC.push_back( itt_[i].stoc );
        aVal.push_back( itt_[i].val  );

        if( itt_[i].val != SHORT_EMPTY/*I_EMPTY*/ && itt_[i].ick_iso[0] != 'z' )
            aZ += itt_[i].stoc * itt_[i].val;
    }
}

const char* EQDEL_CHARS   ="$&,=<>";

int TFormula::BuildMoiety( const char * StrForm, std::vector<MOITERM>& moit_ )
{
    int nSites = 0;
    Formuan aFa( StrForm );

    nSites = aFa.scanMoiety( moit_ );

    //only for test
    if( gui_logger->should_log(spdlog::level::trace)) {
        std::ostringstream logs;
        for( uint ii=0; ii<moit_.size(); ii++)
            logs << moit_[ii].name << " " << moit_[ii].site << "  " << moit_[ii].nj << "\n";
        gui_logger->trace("Formula {}\n {} ", StrForm, logs.str());
    }
    return nSites;
}

// Set new formula and analyze it , calculate charge, get moiety
void TFormula::SetFormula( const char * StrForm )
{
    size_t len, ti;

    fo_clear();

    aFormula = std::string(StrForm);
    if( aFormula.empty() )
        return;

    len = strcspn( StrForm, EQDEL_CHARS );
    ti = strlen( StrForm );
    len = ( len < ti )? len: ti;
    ErrorIf( !len, "TFormula", "E32FPrun: Null length of the formula string");

    std::string fbuf = std::string( StrForm, 0, len );
    std::vector<ICTERM> itt_;

    Formuan aFa( fbuf.c_str() );

    aFa.scanFormula( itt_  );
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
        }

    // IC_TEST:// Changed by DK on 5.09.2007  New rule: if IC symbol has >= 3 chars
               // then it is declared as IC_ADDIT, except Vol (IC_VOLUME)
               // Caution! Anything starting with Vol is recognized as IC_VOLUME!
               // Zz is recognized as IC_CHARGE; O as IC_OXYGEN; H as IC_HYDROGEN
    if( ICs[MAXICNAME] != IC_VOLUME )
         if( strcspn( ICs, " *" ) > 2 )
                ICs[MAXICNAME] = IC_ADDIT; // >2 and <6 characters: IC_ADDIT is set!
    ICs[MAXICNAME+MAXSYMB] = '*';  // This key field is actually not checked!
}

// test all IComps in DB, before unpacking the formula
void TFormula::TestIC( const char* key, int N, char *ICsym )
{
    uint i;
    int jj=-1;
    char ICS[MAXICNAME+MAXSYMB+10];

    for( i=0; i<aCn.size(); i++ )
    {
        memset( ICS, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICS, aCn[i].c_str(), MAXICNAME+MAXSYMB );
        ICS[MAXICNAME+MAXSYMB] = 0;
        ICS[MAXICNAME+MAXSYMB+1] = 0;
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
            std::string msg = "IComp: ";
            msg += std::string(ICS, 0, MAXICNAME+MAXSYMB );
            msg += "\n in formula in DComp/ReacDC record: \n";
            msg += std::string( key, 0, DC_RKLEN);

            Error( "E37FPrun: Invalid symbol ", msg );
        }
    }
}


// Calculate charge, molar mass, elemental entropy, atomic numbers
// from chemical formulae
// returns number of elements (ICs) used in the formula
size_t TFormula::Fmwtz( double &Z, double &mW, double &eS, short *lAn )
{
    time_t icrtim;
    TIComp* aIC = TIComp::pm;
    char ICs[MAXICNAME+MAXSYMB+3];
    double Sc, Zf=0.0;

    Z =  mW =  eS = 0.0;
    for(size_t i=0; i<aCn.size(); i++ ) // TERMS
    {
        memset( ICs, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICs, aCn[i].c_str(), MAXICNAME+MAXSYMB );
        ICs[MAXICNAME+MAXSYMB]=0;
        ICs[MAXICNAME+MAXSYMB+1]=0;
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
    return aCn.size();
}


// Overload variant for calculation in Dcomp
// calculate charge, molar mass, elemental entropy, atomic number, atoms per formula unit
// from chemical formulae
// returns number of elements (ICs) used in the formula
size_t TFormula::Fmwtz( double &Z, double &mW, double &eS, short *lAn, double &Nj )
{
    time_t icrtim;
    TIComp* aIC = TIComp::pm;
    char ICs[MAXICNAME+MAXSYMB+3];
    double Sc, Zf=0.0;

    Z = mW = eS = Nj = 0.0;
    for(uint i=0; i<aCn.size(); i++ ) // TERMS
    {
        memset( ICs, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICs, aCn[i].c_str(), MAXICNAME+MAXSYMB );
        ICs[MAXICNAME+MAXSYMB]=0;
        ICs[MAXICNAME+MAXSYMB+1]=0;
        fixup_ics( ICs );
        // read and if necessary recalc IComp
        aIC->TryRecInp( ICs, icrtim, 0 );
        Sc = aSC[i];
        Nj += Sc;
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
    return aCn.size();
}


// Get a row of stoichiometry matrix (Sml) from the unpacked formula,
// ICsym - list of IC names, ICval is the vector of IC default valences
// if ICval is NULL, valences and formula charge balance are not checked
//
void TFormula::Stm_line( int N, double *Sml, char *ICsym, short *ICval )
{
    size_t i, ii;
    int jj=-1;
    char ICS[MAXICNAME+MAXSYMB+10];
    char *icsp = ICS;

    fillValue( Sml, 0., N);
    for( i=0; i<aCn.size(); i++ )
    {
        memset( ICS, ' ', MAXICNAME+MAXSYMB );
        strncpy( ICS, aCn[i].c_str(), MAXICNAME+MAXSYMB );
        ICS[MAXICNAME+MAXSYMB] = 0;
        ICS[MAXICNAME+MAXSYMB+1]=0;
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
        if( aVal[i] == SHORT_EMPTY && ICval )
            aVal[i] = ICval[jj];
    }
    ii = aCn.size();
    if( ii < 1 || !ICval )
        return;
    aZ = 0;    // calculate charge balance
    if( !memcmp( aCn[ii-1].c_str(), CHARGE_NAME, 2 ))
        ii--;
    for( i=0; i<ii; i++ )
        aZ += aVal[i] * aSC[i];

    // Sveta 11/09/2020 Error 2
    if( ii < aCn.size() )
    {
        double tt = aSC[ii];
        if( fabs( (aZ - tt) ) > 1e-6 )
        {
            std::string str = " in the formula: ";
            str +=  aFormula;
            str += "\n calculated charge: ";
            char   buf[40];
            sprintf( buf, "%lg != %lg", aZ, tt );
            str += buf;
 aSC[ii] = aZ;  // KD 03.01.04  - temporary workaround (adsorption)
            gui_logger->error("W34FPrun: Charge imbalance: {}", str.c_str());
           // vfMessage( 0,  "W34FPrun: Charge imbalance ", str.c_str() );
         }
   }
}

// Get a formula with index nCk from the formula list Cfor (L is total number of formulae in the list)
std::string TFormula::form_extr( int nCk, int L, char *Cfor )
{
    size_t  len;
    char  *Fbg;

    ErrorIf( nCk >= L || nCk < 0, "Formula", "E35FPrun: Error in the formula list!" );
    Fbg = Cfor;
    if( nCk )
        for(int i=0; i<nCk; i++ )
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

    std::string rez( Fbg, 0, len );
    return rez;
}


//--------------------- End of s_formula.cpp ---------------------------

