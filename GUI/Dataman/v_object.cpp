//-------------------------------------------------------------------
// $Id: v_object.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TObject and TObjList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cctype>
#include <QJsonObject>
#include <QJsonArray>
#include "v_vals_impl.h"
#include "v_object.h"
#include "m_param.h"
#include "graph.h"
#include "GEMS3K/jsonconfig.h"

// Workaround for ilink unresolved external errors in BCBuilderX
#ifdef __win32_borland
extern template<> const short TVal<short>::EMPTY();
extern template<> const short TVal<short>::ANY();
extern template<> const char* TVal<short>::PATTERN_GET();
extern template<> const char* TVal<short>::PATTERN_SET();
extern template<> const unsigned short TVal<unsigned short>::EMPTY();
extern template<> const unsigned short TVal<unsigned short>::ANY();
extern template<> const char* TVal<unsigned short>::PATTERN_GET();
extern template<> const char* TVal<unsigned short>::PATTERN_SET();
extern template<> const long TVal<long>::EMPTY();
extern template<> const long TVal<long>::ANY();
extern template<> const char* TVal<long>::PATTERN_GET();
extern template<> const char* TVal<long>::PATTERN_SET();
extern template<> const unsigned long TVal<unsigned long>::EMPTY();
extern template<> const unsigned long TVal<unsigned long>::ANY();
extern template<> const char* TVal<unsigned long>::PATTERN_GET();
extern template<> const char* TVal<unsigned long>::PATTERN_SET();
extern template<> const float TVal<float>::EMPTY();
extern template<> const float TVal<float>::ANY();
extern template<> const char* TVal<float>::PATTERN_GET();
extern template<> const char* TVal<float>::PATTERN_SET();
extern template<> const double TVal<double>::EMPTY();
extern template<> const double TVal<double>::ANY();
extern template<> const char* TVal<double>::PATTERN_GET();
extern template<> const char* TVal<double>::PATTERN_SET();
extern template<> const unsigned char TVal<unsigned char>::EMPTY();
extern template<> const unsigned char TVal<unsigned char>::ANY();
extern template<> const char* TVal<unsigned char>::PATTERN_GET();
extern template<> const char* TVal<unsigned char>::PATTERN_SET();
extern template<> const signed char TVal<signed char>::EMPTY();
extern template<> const signed char TVal<signed char>::ANY();
extern template<> const char* TVal<signed char>::PATTERN_GET();
extern template<> const char* TVal<signed char>::PATTERN_SET();
extern template<> const char TVal<char>::EMPTY();
extern template<> const char TVal<char>::ANY();
extern template<> const char* TVal<char>::PATTERN_GET();
extern template<> const char* TVal<char>::PATTERN_SET();
#endif

// Global array of Objects

TObjList aObj;

//=======================================
// class TObject
//=======================================

// DOD list element explicit ctor
TObject::TObject( const char* name, ObjType t, int n,
                  int m, bool dyn, char indexCode, const string desc):
        Dynamic(dyn),   // Flag of dynamic/static data
        N(n), M(m),     // Dimensions N,M
        Descr(desc),    // Description (tooltip) line
        pV(nullptr),          // Pointer to data
        Type(t),        // Data type code
	IndexationCode(indexCode)
{
    strncpy( Keywd, name, MAXKEYWD-1 );
    Keywd[ MAXKEYWD-1 ] = 0;
}

// Ctor by readind from configurator file

TObject::TObject(istream& f):
        pV(nullptr)
{
    read(f);
}

TObject::~TObject()
{
    Keywd[ MAXKEYWD-1 ] = 0;
    if(pV)
      delete pV;
}

// Writes configuration to file f
void
TObject::ToCFG(ostream& f)
{
    write(f);
}

string
TObject::GetFullName(int aN, int aM)
{
  char v[15];
  string item = GetKeywd();
  if( N > 1 )
  {
    sprintf(v, "[%u]", aN );
    item += v;
  }
  if( M > 1 && Type != S_)
  {
    sprintf(v, "[%u]", aM );
    item += v;
  }
  return item;
}

string TObject::GetHelpLink(int aN, int aM)
{
  char v[15];
  string item = GetKeywd();
  if( !(N<=1 || Descr[0] == '|') )
  {
    sprintf(v, "_%u", aN );
    item += v;
  }
  else
    if( M > 1 && Type != S_ )
    {
      sprintf(v, "_%u", aM );
      item += v;
    }
  return item;
}

// Gets description line from Ni line of DOD list
//    e.g., for displaying a tooltip
const string
TObject::GetDescription(int Ni, int Mi)
{
    size_t prev_pos = 0;
    size_t pos = Descr.find('\n');

    if( pos == string::npos )
        return Descr;

    int NMi = Ni;   // Description by lines
    if( N <= 1 || Descr[0] == '|' )     // Decriptions by colums
           NMi = Mi;

    for(int ii=0;  ii < NMi /*&& pos != string::npos*/; ii++ )
    {
        prev_pos = pos + 1;

        if( prev_pos >= Descr.length() )
            return string("TObject:E01 -bad description line in DOD-");

        pos = Descr.find("\n", prev_pos);
        if( pos == string::npos )
            return Descr.substr(prev_pos, string::npos);
    }
    return Descr.substr(prev_pos, pos-prev_pos-1);
}

// Writes DOD data to ostream file
void
TObject::write( ostream& os )
{
    os.write( Keywd, MAXKEYWD );
    os.write( reinterpret_cast<const char*>(&Type), sizeof(ObjType) );
    os.write( reinterpret_cast<const char*>(&Dynamic), sizeof(bool) );
    os.write( reinterpret_cast<const char*>(&N), sizeof(int) );
    os.write( reinterpret_cast<const char*>(&M), sizeof(int) );
    os.write( &IndexationCode, sizeof(char) );
    os.write( Descr.c_str(), Descr.length() );
    os << '@';
}

// Reads DOD data from istream file
void
TObject::read( istream& is )
{
    is.read( Keywd, MAXKEYWD );
    is.read( reinterpret_cast<char*>(&Type), sizeof(ObjType) );
    is.read( reinterpret_cast<char*>(&Dynamic), sizeof(bool) );
    is.read( reinterpret_cast<char*>(&N), sizeof(int) );
    is.read( reinterpret_cast<char*>(&M), sizeof(int) );
    is.read( &IndexationCode, sizeof(char) );
    u_getline( is, Descr, '@' );
}

// Determines size of one cell (17/10/2012 using only from GemDataStream )
int
TObject::GetCellSize() const
{
    if( IsNull() )
        return 0;

    return pV->cSize();
}

// Determines size of the whole data object (17/10/2012 using only from GemDataStream )
int
TObject::GetSize() const
{
    if( Type == S_ )
        return GetCellSize();
    return GetCellSize()*N*M;
}

// Reallocation of object data types
TValBase*
TObject::BuildVal(ObjType type, int m)
{
    TValBase* p = nullptr;
    switch ( type )
    {
    case I_:
        p = new TVal<short>(Dynamic); //short
        break;
    case U_:
        p = new TVal<unsigned short>(Dynamic);
        break;
    case L_:
        p = new TVal<long>(Dynamic);
        //p = new TVal<int>(Dynamic);
        break;
    case X_:
        p = new TVal<unsigned long>(Dynamic);
        break;
    case F_:
        p = new TVal<float>(Dynamic);
        break;
    case D_:
        p = new TVal<double>(Dynamic);
        break;
    case A_:
        p = new TVal<char>(Dynamic);
        break;
    case B_:
        p = new TVal<unsigned char>(Dynamic);
        break;
    case H_:
        p = new TVal<signed char>(Dynamic);
        break;
    case S_:
        p = new TValString(m, Dynamic);
        break;
    default:
        if( type > 0 /*&& type <= 127*/)
            p = new TValFixString(type, Dynamic);
        else
            throw TError(GetKeywd(), "TObject:E02 Invalid data object type");
    }

    if( !p )
        throw TError(GetKeywd(), "TObject:E03 Data object reallocation failure");

    return p;
}

// sets the pointer to data and (maybe) constructs TVal object
void
TObject::SetPtr(void* newPtr)
{
    //  if( !newPtr )
    //    if( !IsDynamic() )
    //      throw TError("TObject:E04 SetPtr for static object");
    /*    else
        {
          if( !IsNull() )
            Free();
          return;
        }
    */
    //  if( !Dynamic )
    //  {
    // static object
    // TVal is constructed only when SetPtr() is called for the 1st time
    if( !pV )
        pV = BuildVal(Type, M);
    //  }
    //  else
    //  {
    // dynamic object should be initialized earlier with Alloc()
    //    if( IsNull() )
    //      throw DynamicError();
    //    delete[] pV->Get;
    //  }

    // error protection
    if( IsDynamic() && Type == S_ && newPtr )
        SetM( strlen(static_cast<const char*>(newPtr))+1 );

    pV->SetPtr(newPtr);
}

// Useful for the 1st call to allocate a new dynamic data object
void*
TObject::Alloc()
{
    // No need if object is allocated - type & dimensions are the same
    if( IsNull() )
        return Alloc(N, M, Type);
    return pV->GetPtr();
}

// Reallocating memory for holding values of object
void*
TObject::Alloc(int newN, int newM, ObjType newType)
{
    if( !IsDynamic() )
        throw TError(GetKeywd(), "TObject:E04 Attempt to reallocate static data object");
    /*
      if( newType == S_ && abs(newN) != 1 )
        throw TError("For type S_  N should be 1(-1)");
    */
    TValBase* pV1 = BuildVal(newType, newM);
    pV1->Alloc( newN*newM );

    /* Filling cells if new block is larger */

    if( newType < 0 && newType != S_ )
    {
        for( int ii=0; ii<newN; ii++ )
            for( int jj=0; jj<newM; jj++ )
                pV1->Put(0., ii*newM+jj /*newndx(ii, jj)*/ );
    }
    else if( newType > 0  )  // Added by Sveta 16/09/99
    {
        string str( newType, ' ' );
        for( int ii=0; ii<newN; ii++ )
            for( int jj=0; jj<newM; jj++ )
                pV1->SetString( str.c_str(), ii*newM+jj );
    }

    // Testing data type for debugging purposes
    gui_logger->trace("Alloc Object: {} newType= {} newN={} newM={}", Keywd, newType, newN, newM);
    if( newType != Type ) {
       gui_logger->trace("Different type! Object: {} Type= {} newType= {}", Keywd, Type, newType);
    }

    /* Saving old values */
    int n = min(N,newN);
    int m = min(M,newM);

    if( !IsNull() )
    {
        if( Type == newType || ( Type > S_ && newType > S_ ) || ( Type < S_ && newType < S_ ) )
        {
            if( Type < S_ && Type != A_ && Type != B_ && Type != H_ ) //S_==0, no
            {
                for( int ii=0; ii<n; ii++ )
                    for( int jj=0; jj<m; jj++ )
                    {
                        double v = pV->Get( ndx(ii, jj) );
                        pV1->Put(v, ii*newM + jj);
                    }
            }
            else // strings copying
            {
                if( Type == S_ )
                    pV1->SetString( GetString(0).c_str(), 0 );
                else // FixString or A_, B_, H_
                    for( int ii=0; ii<n; ii++ )
                        for( int jj=0; jj<m; jj++ )
                        {
                            string str = pV->GetString(ndx(ii,jj));
                            pV1->SetString( str.c_str(), ii*newM+jj );
                        }
            }
        }
    }

    delete pV;
    pV = pV1;

    //  Type = newType;
    N = newN;
    M = newM;
    Type = newType;

    return pV->GetPtr();
}

// freeing memory for holding values of object
void*
TObject::Free()
{
    if( !IsDynamic() )
        throw TError(GetKeywd(), "TObject:E05 Attempt to free static data object");
//    if( !pV )
//        throw TError(GetKeywd(), "TObject:E055 Attempt to free not allocated data object");

    M = 0;
    if( pV )
      delete pV;   // deleting Val
    return pV=nullptr;
}


// puts string into data object cell
bool TObject::SetString(const char *vbuf, int aN, int aM)
{
    check_dim( aN, aM);
    ErrorIf( !vbuf, GetKeywd(),"TObject:W06 Cannot set empty string to object");
    //???

    if( Type >= 0 && vbuf == string_empty_value)
    {
        if( !pV->SetString(S_EMPTY, ndx(aN,aM)) )
            return false;
    }
    else
    {
        if( !pV->SetString(vbuf, ndx(aN,aM)) )
            return false;
    }

    if( Type == S_ && IsDynamic() )  // oct 2005
        M = GetCellSize();

    return true;
}


// get cell of data object as double
double TObject::Get(int n, int m) const
{
    check_dim(n,m);
    return pV->Get(ndx(n,m));
}

// get cell of data object as double
double TObject::GetEmpty(int n, int m)
{
    if( !GetPtr() || n >= N || m >= M || pV->IsEmpty( ndx(n, m) ) )
        return DOUBLE_EMPTY;
    return pV->Get(ndx(n,m));
}

// put value to cell of data object as double
void TObject::Put(double Value, int n, int m)
{                    // truncated if overflowed !!!
    check_dim(n,m);
    pV->Put(Value, ndx(n,m));
}

/*
// Get label of object. Return size of object.
size_t  TObject::olab_new( OLABEL& self )
{  size_t size;

   self.csize = (unsigned char)GetCellSize();
   if( pV )
   {  if( Type != S_ )
	   {  size = (size_t)self.csize*(size_t)N*(size_t)M;
	      self.Odim_N = N;
	      self.Odim_M = M;
	   } else {
	       if( pV && pV->GetPtr() )
	         size = strlen( (char *)pV->GetPtr() )+1;
	       else
	         size = 0;
	       self.Odim_N = size? 1: 0;
	       self.Odim_M = M;
              }
    }
    else  size = self.Odim_N = self.Odim_M = 0;
    self.Lbegin[0] = TOKENOLABEL;
    self.Lbegin[1] = TOKENOLABEL;
    self.Otype = Type;
    self.Obegin[0] = TOKENOBJBEGIN;
    self.Obegin[1] = TOKENOBJBEGIN;
    return size;
}

// Compares label and object.
// Return 0, if unification;
//       -1, size of object < size in label;
//        1, size of object > size in label;
//       -2, type of object != type in label;
//        2, size = 0 in label.
int TObject::olab_comp( OLABEL& self, size_t& ssize )
{  size_t psize;

   psize = GetSize();
   if( self.Otype != S_ )
      ssize = (size_t)(self.csize)*self.Odim_N*self.Odim_M;
   else ssize = self.Odim_M;
   if( !ssize )
       return 2;
  if( Type != self.Otype )
       return -2;
  if( psize == ssize )
       return 0;
   else if( psize < ssize )
	     return -1;
        else return 1;
}
*/
// get object len in DB file

// Size of DOD label image in database file format
const int szOLABEL = sizeof(char[2]) + sizeof(char)  + sizeof(char)
                        + sizeof(int) + sizeof(int) + sizeof(char[2]);

// Determines size of a binary image of data object in database file
int
TObject::lenDB() const
{
    int size;
    int Odim_N;
    int Odim_M;
    int csize = GetCellSize();

    if( pV )
    {
        if( Type != S_  )
        {
            size = csize*N*M;
            Odim_N = N;
            Odim_M = M;
        }
        else
        {
          if( IsDynamic() )
          {
            if( pV && pV->GetPtr() )
                size = strlen( static_cast<char *>(pV->GetPtr()) ) + 1;
            else
                size = 0;
            Odim_N = size? 1: 0;
            Odim_M = M;
          }
          else                      // oct 2005 Sveta
          {
            size = csize;
            Odim_N = N;
            Odim_M = M;
          }
        }
    }
    else  size = Odim_N = Odim_M = 0;

    /*        if( strcmp(Keywd, "SPPpar") == 0 && size != 0 )
    	    size = 758;
    */
    return (size==0) ? szOLABEL - 2 : szOLABEL + size ;
}

// Puts binary image of data object into database file
// Contains GEMS-specific workarounds for SPP_SETTING alignments
// for different architectures and optimizations
// with debug messages. Needs to be improved !!!!!!!!!!!!!!
int  TObject::toDB( GemDataStream& f )
{
    int size;

    char Lbegin[2];
    unsigned char csize = static_cast<unsigned char>(GetCellSize());
    int Odim_N;
    int Odim_M;
    char Obegin[2];

    if( pV )
    {
        if( Type != S_  )
        {
            size = csize * N * M;
            Odim_N = N;
            Odim_M = M;
        }
        else
        {
          if( IsDynamic() )
          {
            if( pV && pV->GetPtr() )
                size = strlen( static_cast<char *>(pV->GetPtr()) ) + 1;
            else
                size = 0;
            Odim_N = size ? 1 : 0;
            Odim_M = M;
         }
         else                   // oct 2005 Sveta
          {
            size = csize;
            Odim_N = N;
            Odim_M = M;
          }
        }
    }
    else
	size = Odim_N = Odim_M = 0;

    Lbegin[0] = TOKENOLABEL;
    Lbegin[1] = TOKENOLABEL;
    Obegin[0] = TOKENOBJBEGIN;
    Obegin[1] = TOKENOBJBEGIN;


    if( Type == S_ && IsDynamic() )
        Odim_M = size;
    f.writeArray( Lbegin, sizeof Lbegin );
    f << static_cast<signed char>(Type);
    f << csize;
    f << Odim_N;
    f << Odim_M;

    //  size_t Osize = szOLABEL - 2;//(size_t)sizeof(OLABEL)-2;
    if( size )
    {
        f.writeArray( Obegin, 2 );
        // temp workaround of alingment
        if( strcmp(Keywd, "SPPpar") == 0 )
        {
            gui_logger->debug("writing SPP_SETTING, size = {}", size);
            static_cast<SPP_SETTING*>(GetPtr())->write(f);
            //padding
            for( int ii=0; ii<size-758; ii++)
                f.put('%');
            //	    size = 758;
        }
	else	// all object with Keyword "**plt" considered PlotLine object !!!
	if( strcmp(Keywd+2, "plt") == 0 )
	    for(int ii=0; ii<GetN(); ii++)
            (static_cast<TPlotLine*>(GetPtr()) + ii)->write(f);
        else
            pV->write( f, size );

        return szOLABEL + size;
    }
    return szOLABEL - 2;
}

// Gets data object from binary database file
// Contains GEMS-specific workarounds for SPP_SETTING alignments
// with debug messages. Needs to be improved !!!!!!!!!!!!!!
int  TObject::ofDB( GemDataStream& f )
{
    int ssize;
    bool error_S_ = false;
    int cmp;

    char Lbegin[2];
    signed char Otype;
    unsigned char csize;
    int Odim_N;
    int Odim_M;
    char Obegin[2];


    f.readArray( Lbegin, sizeof Lbegin );
    f >> Otype;
    f >> csize;
    f >> Odim_N;
    f >> Odim_M;

    gui_logger->trace("ofDB: begin for {} Type={} Otype={} csize={} Odim_N={} Odim_M={}", GetKeywd(), Type, Otype, csize, Odim_N, Odim_M);
    //  f.read( (char *)&OlbufRead, sizeof( OLABEL )-2 );
    //  cmp = olab_comp( OlbufRead, OlenRead );


    auto psize = GetSize();
    if( Otype != S_  )
        ssize = csize * Odim_N * Odim_M;
    else
        ssize = Odim_M; // csize

    if( !ssize )
        cmp = 2;
    else
        if( Type != Otype )
            cmp = -2;
        else
            if( psize == ssize
                    /* tmp workaround for alignment*/ || strcmp(Keywd, "SPPpar") == 0 )
                cmp = 0;
            else
                if( psize < ssize )
                    cmp = -1;
                else cmp = 1;


    switch( cmp )
    {
    case  1: // old size > new size
        if( !IsDynamic() )
            break;
        [[fallthrough]];
    case -1:  // old size < new size
    case -2: // old Type != new Type
        if( IsDynamic() )
        {
            Alloc( Odim_N,
                   Odim_M, Otype );
            check();
        }
        else
        {
            if( Otype == S_ && cmp == -1 )
                error_S_ = true;
            else
                ErrorIf( cmp!=1, GetKeywd(),
                         "TObject:E07 Invalid type/size on getting static data object");
        }
        break;
    case  2: //size of new object is 0
        if( IsDynamic() )
            Free();
        break;
    case 0: // old size = new size
        break;
    }

    if( !ssize )
    {
        if( GetPtr() != nullptr && Type == S_ )
            *(static_cast<char *>(GetPtr())) = '\0';

        return szOLABEL - 2;///(size_t)(sizeof( OLABEL )-2);
    }

    f.readArray( Obegin, 2);
    if( Obegin[0] != TOKENOBJBEGIN ||
            Obegin[1] != TOKENOBJBEGIN)
        Error(GetKeywd(),
              "TObject:E08 Wrong format on getting data object from database");

    // temporary workaround of different alignment in SPP_SETTING
    if( strcmp(Keywd, "SPPpar") == 0 )
    {
        size_t off = 0;
        size_t r_pos = f.tellg();

        f.seekg( 758+r_pos, ios::beg );
        f.sync();

        char ch;
        do
        {
            f.get( ch );
            if( ch == '^' )
            {
                f.get( ch );
                if( ch == '^' )
                    break;
            }
            //                padding = ch;
            off++;
        }
        while(1);

        f.seekg( r_pos, ios::beg );
        f.sync();

        static_cast<SPP_SETTING*>(GetPtr())->read(f);
        // ios::cur - dosen't work in BCB4 :-(
        f.seekg( off + f.tellg(), ios::beg );
        //ssize = 768;
    }
    else	// all object with Keyword "**plt" considered PlotLine object !!!
    if( strcmp(Keywd+2, "plt") == 0 ) {
	for(int ii=0; ii<GetN(); ii++)
            (static_cast<TPlotLine*>(GetPtr()) + ii)->read(f);
    }
    else
       if( error_S_ )
       {
         pV->read( f, psize );
         char ch;
         for( int ii=psize; ii<ssize; ii++)
             f.get( ch );

       }  else
           pV->read( f, ssize );

    return szOLABEL + ssize;
}


//Puts data object to backup format TXT file
void TObject::toTXT( fstream& to )
{
    int dimM, i, j;

    dimM = M;
    if( IsDynamic() )
    {
        if( IsNull() )
            dimM = 0;
        else
            if( Type == S_  )
                dimM = strlen( static_cast<char *>(GetPtr()) )+2;
        to << "^^" << N << " " << dimM << " " << static_cast<int>(Type) << "~~\n";
        if( N==0 || dimM==0 )
            return;
    }
    else
        if( IsNull() || N==0 || dimM==0 )
        {
            to << "~\n";
            return;
        }

    if( strcmp(Keywd+2, "plt") == 0 )
    {
        for(int ii=0; ii<GetN(); ii++)
            (static_cast<TPlotLine*>(GetPtr()) + ii)->write(to);
         return;
    }

    if( Type == S_ ) // text
    {  //*((char*)(char*)GetPtr()+M-1) = 0;

        to << "\'" << static_cast<char *>(GetPtr()) << "\'\n";
    }
    else
        for( i=0; i < N; i++ )
        {
            for( j=0; j < M; j++ )
            {
                auto sbuf = GetString(i,j);
                if( Type > 0 )
                    to << "\"";
                if( sbuf.empty() )
                {
                    sbuf = "`";
                }
                else if( sbuf[0] == ' ' )
                {
                    sbuf = " ";
                }
                 to << sbuf;
                  if( Type > 0 )
                    to << "\" ";
                  else
                     to << " ";
             }
              to << "\n";
        }
 }

//Gets data object from backup format TXT file
void TObject::ofTXT( fstream& of )
{
    int rdimN, rdimM, i, j;
    int r_otype;
    char sbuf[160];

    do
    {
        of.get(*sbuf);
        if( !of.good() )
            Error(GetKeywd(),
                  "TObject:E09 Error reading data object header from TXT");
    }
    while( *sbuf == ' ' || *sbuf=='\n' );

    if( IsDynamic() )
    {
        of.putback( *sbuf );
        of.get( sbuf, 60, '\n');
        if ( sscanf( sbuf , "^^%d%d%d~~", &rdimN, &rdimM, &r_otype /*rOtype*/ ) < 3 )
            Error(GetKeywd(),
                  "TObject:E10 Wrong format on getting data object header from TXT");
        if( rdimN==0 || rdimM==0 )
        {
            Free();
            return;
        }
        //if( rOtype == 9 ) rOtype = 18;// Sveta
        ObjType  rOtype = r_otype;
        Alloc( rdimN, rdimM, rOtype );
        check();
     }
    else
    {
        if( *sbuf == '~' )
            return;
        check();
        of.putback( *sbuf );
    }

    if( strcmp(Keywd+2, "plt") == 0 )
    { for(int ii=0; ii<GetN(); ii++)
            (static_cast<TPlotLine*>(GetPtr()) + ii)->read(of);
      return;
    }

    if( Type == S_ )
    {
        do
        {
            of.get(*sbuf);
            if( !of.good() )
                Error(GetKeywd(),
                      "TObject:E11 Error reading data object contents from TXT");
        }
        while( *sbuf == ' ' || *sbuf=='\n' );
        if( *sbuf != '\'')
            Error(GetKeywd(),
                  "TObject:E12 Wrong format on getting data object contents from TXT ");

        if( IsDynamic() )
        { //if(M<=16)
            of.get( static_cast<char*>(GetPtr()), M, '\'');
            //else
            //{ of.get( (char*)GetPtr(), M-16, '\0');
            //  of.get( (char*)GetPtr()+M-17, 17, '\'');
            //}
            if( of.get() != '\'')
                Error(GetKeywd(),
                      "TObject:E13 Missing trailing ' when reading text object from TXT");
        }
        else
        {
            char *sp = static_cast<char *>(GetPtr());
            of.get( sp, M, '\n');
            if( sp[strlen(sp)-1] == '\'')
                sp[strlen(sp)-1] = '\0';
            else
                if( of.get() != '\'')
                    Error(GetKeywd(),
                          "TObject:E14 Missing trailing ' when reading text object from TXT");
        }
        return;
    }
    else
    {
        for( i = 0; i < N; i++ )
            for( j = 0; j < M; j++ )
            {
                if( Type > 0 )
                {
                    do of.get(*sbuf);
                    while( *sbuf == ' ' || *sbuf=='\n' );
                    if( *sbuf != '\"' )
                        Error(GetKeywd(),
                              "TObject:E15 Bad format or character when reading text object from TXT");
                    of.get( sbuf, 130, '\"');
                    if(  of.get() != '\"')
                        Error(GetKeywd(),
                              "TObject:E16 Bad format or character when reading text object from TXT");
                }
                else
                   of >> sbuf;

                if( Type == A_ || Type == B_ )
                    switch( *sbuf )
                    {
                    case '`':
                        sbuf[0] = '0';
                        break;
//                        case '�':
                        case ' ':
                        sbuf[0] = ' ';
                        sbuf[1] = 0;
                        break;
                    default:
                        break;
                    }

                SetString( sbuf, i, j );
            }
    }
}

void TObject::toJsonObject( QJsonObject& obj ) const
{
    obj[ "label"] = Keywd;
    obj[ "dot"] = Type;
    if( IsNull() && Dynamic && M != 0)
        obj[ "dN"] = 0;
    else
        obj[ "dN"] =  N;
    obj[ "dM"] =  M;
    obj[ "dyn"] = Dynamic;

    if( IsNull() )
    {
        //no value will be written as null in the json file
        obj[ "val" ];
        return;
    }

    if( strcmp(Keywd, "SPPpar") == 0 )
    {
        //obj.insert( "val", ((SPP_SETTING*)GetPtr())->toJsonObject());
        return;
    }

    if( strcmp(Keywd+2, "plt") == 0 )
    {
        QJsonArray pltArray;
        for(int ii=0; ii<GetN(); ii++)
        {
            QJsonObject pltObject;
            ((TPlotLine*)GetPtr() + ii)->toJsonObject( pltObject );
            pltArray.append(pltObject);
        }
        obj["val"] = pltArray;
        return;
    }

    if( Type == S_ ) // text
    {
        obj["val"] = (const char*)GetPtr();
    }
    else
    {
        QJsonArray valArray;
        for(int i=0; i < N; i++ )
            for(int j=0; j < M; j++ )
            {
                if( IsEmpty(i,j))
                    valArray.append( QJsonValue(QJsonValue::Null) ); // null
                else
                {
                    if( Type>= D_ && Type<= I_)
                        valArray.append( Get(i,j) );
                    else {
                        auto str_val = GetString(i,j);
                        //strip(str_val);
                        valArray.append( str_val.c_str() );
                    }
                }
            }
        obj["val"] = valArray;
    }
}

void TObject::fromJsonObject(const QJsonObject &obj)
{
    signed char Otype;
    int Odim_N;
    int Odim_M;
    bool ODynamic;
    std::string valStr;

    Otype = obj[ "dot" ].toInt( N_TYPE_ );
    Odim_N = obj[ "dN" ].toInt( 0 );
    Odim_M = obj[ "dM" ].toInt( 0 );
    ODynamic = obj[ "dyn" ].toBool( false );
    // IndexationCode = (int)obj.value("ndxs").toDouble();

    // now it is an error
    ErrorIf( ODynamic != Dynamic, GetKeywd(), "TObject:E19 Try to change allocation of object");

    if( IsDynamic() )
    {
        if( Odim_M==0 || Odim_N==0 )
        {
            Type = Otype;
            N = Odim_N;
            M = Odim_M;
            Free();
            return;
        }

        if( Otype != Type || Odim_N != N || Odim_M != M || IsNull() )
        {
            Alloc( Odim_N, Odim_M, Otype );
            check();
        }
    }
    else
    {
        if( Otype != Type || Odim_N != N || Odim_M != M )
            Error( GetKeywd(), "TObject:E07 Invalid type/size on getting static data object");
    }

    if( strcmp(Keywd, "SPPpar") == 0 )
    {
        //((SPP_SETTING*)GetPtr())->fromJsonObject( obj.value("val").toObject() );
        return;
    }

    if( strcmp(Keywd+2, "plt") == 0 )
    {
        QJsonArray pltArray = obj["val"].toArray();
        for(int ii=0; ii< min<int>( N, pltArray.size()); ii++)
        {
            QJsonObject pltObject = pltArray[ii].toObject();
            ((TPlotLine*)GetPtr() + ii)->fromJsonObject( pltObject );
        }
        return;
    }

    else if( Type == S_ ) // text
    {
        valStr = obj["val"].toString("").toStdString();
        SetString( valStr.c_str(), 0, 0 );
    }
    else
    {
        QJsonArray valArray = obj["val"].toArray();
        for(int i=0; i < N; i++ )
            for(int j=0; j < M; j++ )
            {
                int ndx_ = ndx(i, j);
                if( valArray.size() <= ndx_ )
                    return;

                if( Type>= D_ && Type<= I_ )
                {
                    double valDouble =  valArray[ndx_].toDouble(DOUBLE_EMPTY);
                    if(  IsDoubleEmpty( valDouble) )
                        SetString( S_EMPTY, i, j );
                    else
                        Put( valDouble, i, j );
                }
                else
                {
                    valStr = valArray[ndx_].toString("").toStdString();
                    SetString( valStr.c_str(), i, j );
                }
            }
    }
}

QJsonValue TObject::toJsonValue() const
{
    if( IsNull() ) {
        return QJsonValue(QJsonValue::Null);
    }
    if( strcmp(Keywd, "SPPpar") == 0 )  {
        //obj.insert( "val", ((SPP_SETTING*)GetPtr())->toJsonObject());
        return QJsonValue();
    }
    if( strcmp(Keywd+2, "plt") == 0 ) {
        QJsonArray pltArray;
        for(int ii=0; ii<GetN(); ii++) {
            QJsonObject pltObject;
            ((TPlotLine*)GetPtr() + ii)->toJsonObject( pltObject );
            pltArray.append(pltObject);
        }
        return pltArray;
    }

    if( Type == S_ ) { // text
        return QJsonValue((const char*)GetPtr());
    }
    else if( !IsDynamic() && N==1 && M==1 ) {
        if( IsEmpty(0,0)) {
            return QJsonValue(QJsonValue::Null);
        }
        else {
            if( Type>= D_ && Type<= I_) {
                return QJsonValue(Get(0,0));
            }
            else {
                return QJsonValue(GetString(0,0).c_str());
            }
        }
    }
    else {
        QJsonArray fullArray;
        for(int i=0; i < N; i++ ) {
            QJsonArray valArray;
            for(int j=0; j < M; j++ )  {
                if( IsEmpty(i,j)) {
                    valArray.append( QJsonValue(QJsonValue::Null) );
                }
                else {
                    if( Type>= D_ && Type<= I_) {
                        valArray.append(Get(i,j));
                    }
                    else {
                        auto str_val = GetString(i,j);
                        //strip(str_val);
                        valArray.append(str_val.c_str());
                    }
                }
            }
            fullArray.push_back(valArray);
        }
        return fullArray;
    }
}

void TObject::fromJsonValue(const QJsonValue &obj)
{
    std::string valStr;
    if( obj.isNull() ) {
        if( IsDynamic() ) {
            Free();
        }
        else if( !IsNull() ) { // clear
            SetString(S_EMPTY, 0, 0);
        }
        return;
    }

    if( strcmp(Keywd, "SPPpar") == 0 )  {
        //((SPP_SETTING*)GetPtr())->fromJsonObject( obj.value("val").toObject() );
        return;
    }

    if( strcmp(Keywd+2, "plt") == 0 )  {
        QJsonArray pltArray = obj.toArray();
        auto plot = static_cast<TPlotLine * >(Alloc( pltArray.size(), sizeof(TPlotLine) ));
        for(int ii=0; ii< min<int>( N, pltArray.size()); ii++) {
            QJsonObject pltObject = pltArray[ii].toObject();
            plot[ii].fromJsonObject( pltObject );
        }
        return;
    }

    if( Type == S_ ) { // text
        valStr = obj.toString("`").toStdString();
        auto Odim_M = valStr.length() + 1;
        if(IsDynamic()) {
            Alloc( 1, Odim_M, Type );
        }
        if(!IsNull()) {
            SetString( valStr.c_str(), 0, 0 );
        }
    }
    else if( obj.isArray() )  {
        int Odim_M = 0;
        QJsonArray fullArray = obj.toArray();
        int Odim_N = fullArray.size();
        if(!fullArray.isEmpty()) {
            Odim_M = fullArray[0].toArray().size();
        }

        if( IsDynamic() ) {
            if( Odim_M==0 || Odim_N==0 )  {
                Free();
                return;
            }
            if( Odim_N != N || Odim_M != M || IsNull() ) {
                Alloc( Odim_N, Odim_M, Type );
                check();
            }
        }
        else {
            if( Odim_N != N || Odim_M != M )
                Error( GetKeywd(), "TObject:E07 Invalid size on getting static data object");
        }

        for(int i=0; i < N; i++ ) {
            auto valArray = fullArray[i].toArray();
            for(int j=0; j < M; j++ ) {
                if( valArray.size() <= j ) {
                    SetString( S_EMPTY, i, j );
                    continue;
                }
                if( Type>= D_ && Type<= I_ ) {
                    double valDouble =  valArray[j].toDouble(DOUBLE_EMPTY);
                    if(IsDoubleEmpty(valDouble)) {
                        SetString( S_EMPTY, i, j );
                    }
                    else {
                        Put( valDouble, i, j );
                    }
                }
                else   {
                    valStr = valArray[j].toString(string_empty_value.c_str()).toStdString();
                    SetString( valStr.c_str(), i, j );
                }
            }
        }
    }

    else if( !IsDynamic() && N==1 && M==1 ) {
        if( Type>= D_ && Type<= I_ ) {
            double valDouble =  obj.toDouble(DOUBLE_EMPTY);
            if(IsDoubleEmpty(valDouble)) {
                SetString( S_EMPTY, 0, 0 );
            }
            else {
                Put( valDouble, 0, 0 );
            }
        }
        else   {
            valStr = obj.toString(string_empty_value.c_str()).toStdString();
            SetString( valStr.c_str(), 0, 0 );
        }
    }
}

// Implementation of TObjList class

// Finds DOD index from a given name s
// returns -1 if such a name does not exist
int
TObjList::Find(const char* s)
{
    for( size_t ii=0; ii< size(); ii++ )
        if( strcmp(s, at(ii)->GetKeywd() )==0 )
            return static_cast<int>(ii);

    return -1;
}

//const int N_OBJECTS = 1000;

// default DOD list configuration
TObjList::TObjList():
        std::vector<std::shared_ptr<TObject>>()
{
    //  load( "viz_od.ini", MINVISOROBJECTS );
}

// Reads DOD list configuration from file

TObjList::TObjList(istream& f):
        std::vector<std::shared_ptr<TObject>>()
{
    fromDAT(f);
}


// Writes DOD list configuration to file
void
TObjList::toDAT(ostream& f)
{
    double Value = 0.0;
    auto nObj = size();

    f << '@';
    f.write(reinterpret_cast<char*>(&Value), sizeof(double) );
    f.write(reinterpret_cast<char*>(&nObj), sizeof(int) );
    for(size_t ii=0; ii<nObj; ii++)
        at(ii)->ToCFG( f );
}

// Reads DOD configuration from config file
void
TObjList::fromDAT(istream& f)
{
    double Value;	// not used

    clear();
    int nObj;
    f.get(); // '@' marker
    f.read(reinterpret_cast<char*>(&Value), sizeof(double) );
    f.read(reinterpret_cast<char*>(&nObj), sizeof(int) );
    for(int ii=0; ii<nObj; ii++)
        push_back( std::make_shared<TObject>(f) );
}


//const int nTypes = 12;
static std::vector<std::string> OBtype =
    { "S_", "I_", "U_", "L_", "X_", "F_",
      "D_", "C_", "N_", "A_", "B_", "H_" };

// Loads default configurations from a text .ini file
void TObjList::load(const char* f_obj, int /* maxN */ )
{
    int objectType = 0;
    std::string objectKeywd;
    TJsonConfig cnf1( std::string(f_obj) + ".json");
    auto obj_array = cnf1.to_vector();

    for(const auto& obj_ini: obj_array)
    {
        objectKeywd = obj_ini.value_must_exist<std::string>("label");
        if( Find(objectKeywd.c_str())>0 )  {
            throw TFatalError(objectKeywd, "TObject:E18 This data object is already defined");
        }

        auto dotipe_section = obj_ini.section("type");
        if( !dotipe_section || !dotipe_section->is_string()) {
            objectType = dotipe_section->get_as<int>();
        }
        else {
            auto type_str =  dotipe_section->get_as<std::string>();
            auto it = find(OBtype.begin(), OBtype.end(), type_str);
            if (it != OBtype.end()) {
                objectType = ObjType(-(it-OBtype.begin()));
            }
            else {
                if( !string2value(objectType, type_str)) {
                    throw TFatalError(type_str, "TObject:E17 Unknown object type in ini-file");
                }
            }
        }
        push_back( std::make_shared<TObject>(objectKeywd.c_str(),
                                             objectType,
                                             abs(obj_ini.value_must_exist<int>("dimN")),
                                             obj_ini.value_must_exist<int>("dimM"),
                                             obj_ini.value_must_exist<bool>("dyna"),
                                             obj_ini.value_or_default<std::string>("ndxs", std::string("N"))[0],
                                             obj_ini.value_or_default<std::string>("hint", "")) );
    }
}

//--------------------- End of v_object.cpp ---------------------------

