//-------------------------------------------------------------------
// $Id: v_object.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TObject and TObjList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cctype>

//#include <QJsonValue>
//#include <QJsonArray>


#include "v_vals_impl.h"
#include "v_object.h"
#include "v_ejdb.h"
#include "m_param.h"
#include "graph.h"
#include "config.h"
#include "v_json.h"
#include "encoding.h"
#include "service.h"

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

string TValBson::GetString(size_t ) const
{
    ParserJson pars;
    string jsonstr;
    pars.printBsonObjectToJson( jsonstr, bcobj.data );
    return jsonstr;
}

bool TValBson::SetString(const char* s, size_t )
{
    string objStr = s;
    ParserJson parserJson;
    parserJson.setJsonText( objStr.substr( objStr.find_first_of('{')+1 ));

    Alloc(0);
    bson_init( &bcobj );
    parserJson.parseObject( &bcobj );
    bson_finish( &bcobj );

    return true;
}

void TValBson::write(GemDataStream& s, size_t size)
{
  Error( "Bson object", "Invalid object type for old DataBase");
  s.writeArray(bcobj.data, size );
}

void TValBson::read(GemDataStream& s, size_t size )
{
   Error( "Bson object", "Invalid object type for old DataBase");
   if( bcobj.data )
     s.readArray( bcobj.data,  size  );
}



//=======================================
// class TObject
//=======================================

// DOD list element explicit ctor
TObject::TObject( const char* name, ObjType t, int n,
                  int m, bool dyn, char indexCode, const string desc):
        Dynamic(dyn),   // Flag of dynamic/static data
        N(n), M(m),     // Dimensions N,M
        Descr(desc),    // Description (tooltip) line
        pV(0),          // Pointer to data
        Type(t),        // Data type code
	IndexationCode(indexCode)
{
    strncpy( Keywd, name, MAXKEYWD-1 );
    Keywd[ MAXKEYWD-1 ] = 0;
}

// Ctor by readind from configurator file

TObject::TObject(istream& f):
        pV(0)
{
    read(f);
}

TObject::~TObject()
{
    Keywd[ MAXKEYWD-1 ] = 0;
    if(Dynamic)
        delete pV;
}

// Writes configuration to file f
void TObject::ToCFG(ostream& f)
{
    write(f);
}

string TObject::GetFullName(int aN, int aM)
{
  char v[25];
  string item = GetKeywd();
  if( N > 1 )
  {
    sprintf(v, "[%u]", aN );
    item += v;
  }
  if( M > 1 && ( Type != S_ && Type != J_) )
  {
    sprintf(v, "[%u]", aM );
    item += v;
  }
  return item;
}

string TObject::GetHelpLink(int aN, int aM)
{
  char v[25];
  string item = GetKeywd();
  if( !(N<=1 || Descr[0] == '|') )
  {
    sprintf(v, "_%u", aN );
    item += v;
  }
  else
    if( M > 1 && Type != S_ && Type != J_)
    {
      sprintf(v, "_%u", aM );
      item += v;
    }
  return item;
}

// Gets description line from Ni line of DOD list
//    e.g., for displaying a tooltip
const string TObject::GetDescription(int Ni, int Mi)
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
void TObject::write( ostream& os )
{
    os.write( Keywd, MAXKEYWD );
    os.write( (char*)&Type, sizeof(ObjType) );
    os.write( (char*)&Dynamic, sizeof(bool) );
    os.write( (char*)&N, sizeof(int) );
    os.write( (char*)&M, sizeof(int) );
    os.write( &IndexationCode, sizeof(char) );
    os.write( Descr.c_str(),Descr.length() );
    os << '@';
}

// Reads DOD data from istream file
void TObject::read( istream& is )
{
    is.read( Keywd, MAXKEYWD );
    is.read( (char*)&Type, sizeof(ObjType) );
    is.read( (char*)&Dynamic, sizeof(bool) );
    is.read( (char*)&N, sizeof(int) );
    is.read( (char*)&M, sizeof(int) );
    is.read( &IndexationCode, sizeof(char) );
    u_getline( is, Descr, '@' );
}

// Determines size of one cell (17/10/2012 using only from GemDataStream )
size_t TObject::GetCellSize() const
{
    if( IsNull() )
        return 0;

    return pV->cSize();
}

// Determines size of the whole data object (17/10/2012 using only from GemDataStream )
size_t TObject::GetSize() const
{
    if( Type == S_ || Type == J_ )
        return GetCellSize();
    return GetCellSize()*N*M;
}

// Reallocation of object data types
TValBase* TObject::BuildVal(ObjType type, int m)
{
    TValBase* p = 0;
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
    case J_:
        p = new TValBson();
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
void TObject::SetPtr(void* newPtr)
{
    // TVal is constructed only when SetPtr() is called for the 1st time
    if( !pV )
        pV = BuildVal(Type, M);

    // error protection
    if( IsDynamic() && Type == S_ && newPtr )
        SetM( strlen((const char*)newPtr)+1 );

    if( Type != J_ )
      pV->SetPtr(newPtr);
}

// Useful for the 1st call to allocate a new dynamic data object
void* TObject::Alloc()
{
    // No need if object is allocated - type & dimensions are the same
    if( IsNull() )
        return Alloc(N, M, Type);
    return pV->GetPtr();
}

// Reallocating memory for holding values of object
void* TObject::Alloc(int newN, int newM, ObjType newType)
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

    if( newType < 0 && newType != S_ && newType != J_ )
    {
        for( int ii=0; ii<newN; ii++ )
            for( int jj=0; jj<newM; jj++ )
                pV1->Put(0., ii*newM+jj /*newndx(ii, jj)*/ );
    }
    else if( newType > 0  )  // Added by Sveta 16/09/99
    {
        string str( newType, ' ' );
        // cout << (int)newType << ' ' << Keywd << " T" << str.c_str() << "T" << endl;
        for( int ii=0; ii<newN; ii++ )
            for( int jj=0; jj<newM; jj++ )
                pV1->SetString( str.c_str(), ii*newM+jj );
    }

    /* Testing data type for debugging purposes
#ifdef __unix
    if( newType != Type )
        cout << "Different type! Object:" << Keywd
        << " Type= " << (int)Type << " newType= " << (int)newType << endl;
#endif
    */
    /* Saving old values */
    int n = min(N,newN);
    int m = min(M,newM);

    if( !IsNull() &&  newType != J_ )
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
            else // gstrings copying
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
    N = newN, M = newM;
    Type = newType;

    return pV->GetPtr();
}

// freeing memory for holding values of object
void* TObject::Free()
{
    if( !IsDynamic() )
        throw TError(GetKeywd(), "TObject:E05 Attempt to free static data object");
//    if( !pV )
//        throw TError(GetKeywd(), "TObject:E055 Attempt to free not allocated data object");

    M = 0;
    if( pV )
      delete pV;   // deleting Val
    return pV=0;
}


// puts string into data object cell
bool TObject::SetString(const char *vbuf, int aN, int aM)
{
    check_dim( aN, aM);
    ErrorIf( !vbuf, GetKeywd(),"TObject:W06 Cannot set empty string to object");
    //???

    if( !pV->SetString(vbuf, ndx(aN,aM)) )
        return false;

    if( ( Type == S_ || Type == J_) && IsDynamic() )  // oct 2005
        M = GetCellSize();

    return true;
}


// get cell of data object as double
double TObject::Get(int n, int m)
{
    check_dim(n,m);
    return pV->Get(ndx(n,m));
}

// get cell of data object as double
double TObject::GetEmpty(int n, int m)
{
    check_dim(n,m);
    if( pV->IsEmpty( ndx(n, m) ) )
        return DOUBLE_EMPTY;
    return pV->Get(ndx(n,m));
}

// put value to cell of data object as double
void TObject::Put(double Value, int n, int m)
{                    // truncated if overflowed !!!
    check_dim(n,m);
    pV->Put(Value, ndx(n,m));
}

// get object len in DB file

// Size of DOD label image in database file format
const size_t szOLABEL = sizeof(char[2]) + sizeof(char)  + sizeof(char)
                        + sizeof(int) + sizeof(int) + sizeof(char[2]);

// Determines size of a binary image of data object in database file
size_t TObject::lenDB() const
{
    size_t size;
    int Odim_N;
    unsigned int Odim_M;
    unsigned char csize = (unsigned char)GetCellSize();

    if( pV )
    {
        if( Type == J_  )
        {
            size = (size_t)csize;
            Odim_N = 1;
            Odim_M = 1;
        }
        else if( Type != S_  )
        {
            size = (size_t)csize*(size_t)N*(size_t)M;
            Odim_N = N;
            Odim_M = M;
        }
        else
        {
          if( IsDynamic() )
          {
            if(  pV->GetPtr() )
                size = strlen( (char *)pV->GetPtr() )+1;
            else
                size = 0;
            Odim_N = size? 1: 0;
            Odim_M = M;
          }
          else                      // oct 2005 Sveta
          {
            size = (size_t)csize;
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
size_t  TObject::toDB( GemDataStream& f )
{
    size_t size;

    char Lbegin[2];
    unsigned char csize = (unsigned char)GetCellSize();
    int Odim_N;
    unsigned int Odim_M;
    char Obegin[2];

    if( pV )
    {
        if( Type == J_  )
        {
            size = (size_t)csize;
            Odim_N = 1;
            Odim_M = size;
        }
        else if( Type != S_  )
        {
            size = (size_t)csize * (size_t)N * (size_t)M;
            Odim_N = N;
            Odim_M = M;
        }
        else
        {
          if( IsDynamic() )
          {
            if( pV && pV->GetPtr() )
                size = strlen( (char *)pV->GetPtr() ) + 1;
            else
                size = 0;
            Odim_N = size ? 1 : 0;
            Odim_M = M;
         }
         else                   // oct 2005 Sveta
          {
            size = (size_t)csize;
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


    if( Type == S_ && IsDynamic() )  Odim_M = size;
    f.writeArray( Lbegin, sizeof Lbegin );
    f << (signed char)Type;
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
//#ifdef __unix
//            cerr << endl << "writing SPP_SETTING, size = " << size << endl;
//#endif
            ((SPP_SETTING*)GetPtr())->write(f);
            //padding
            for( unsigned int ii=0; ii<size-758; ii++)
                f.put('%');
            //	    size = 758;
        }
	else	// all object with Keyword "**plt" considered PlotLine object !!!
	if( strcmp(Keywd+2, "plt") == 0 )
	    for(int ii=0; ii<GetN(); ii++)
        	((TPlotLine*)GetPtr() + ii)->write(f);
        else
            pV->write( f, size );

        return szOLABEL + size;
    }
    return szOLABEL - 2;
}

// Gets data object from binary database file
// Contains GEMS-specific workarounds for SPP_SETTING alignments
// with debug messages. Needs to be improved !!!!!!!!!!!!!!
size_t  TObject::ofDB( GemDataStream& f )
{
    size_t ssize;
    bool error_S_ = false;
    int cmp;

    char Lbegin[2];
    signed char Otype;
    unsigned char csize;
    int Odim_N;
    unsigned int Odim_M;
    char Obegin[2];


    f.readArray( Lbegin, sizeof Lbegin );
    f >> Otype;
    f >> csize;
    f >> Odim_N;
    f >> Odim_M;

//cerr << "ofDB: begin for " << GetKeywd() << endl;
//cerr << "Type " << (int)Type << endl;
//cerr << "Otype " << (int)Otype << " csize " << (int)csize << " Odim_N " << Odim_N << " Odim_M " << Odim_M << endl;

    //  f.read( (char *)&OlbufRead, sizeof( OLABEL )-2 );
    //  cmp = olab_comp( OlbufRead, OlenRead );

    size_t psize;

    psize = GetSize();
    if( Otype != S_ && Otype != J_ )
        ssize = (size_t)(csize) * Odim_N * Odim_M;
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
        if( GetPtr() != NULL && Type == S_ )
            *((char *)GetPtr()) = '\0';

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

        ((SPP_SETTING*)GetPtr())->read(f);
        // ios::cur - dosen't work in BCB4 :-(
        f.seekg( off + f.tellg(), ios::beg );
        //ssize = 768;
    }
    else	// all object with Keyword "**plt" considered PlotLine object !!!
    if( strcmp(Keywd+2, "plt") == 0 ) {
	for(int ii=0; ii<GetN(); ii++)
    	    ((TPlotLine*)GetPtr() + ii)->read(f);
    }
    else
       if( error_S_ )
       {
         pV->read( f, psize );
         char ch;
         for( size_t ii=psize; ii<ssize; ii++)
             f.get( ch );

       }  else
           pV->read( f, ssize );

//cerr << "ofDB: end" << endl;

    return szOLABEL + ssize;
}


/// chang to a UTF-8 representation of the string
void TObject::checkUtf8()
{
  if( strcmp(Keywd, "SPPpar") == 0 ||
      strcmp(Keywd+2, "plt") == 0 ||
      ( Type>= D_ && Type<= I_ ) || IsNull() || Type==J_ )
   return;
  string value;

  if( Type == S_ ) // text
  {
      value = (const char*)GetPtr();
      stringToUtf8( value );
      SetString(value.c_str(), 0, 0 );
  }
  else
  {
      for(int i=0; i < N; i++ )
        for(int j=0; j < M; j++ )
         {
            if( IsEmpty(i,j) )
                  continue;
               else
               {
                 value = GetString(i,j);
                 stringToUtf8( value );
                 SetString( value.c_str(), i, j );
               }
         }
   }
}


/// Puts data object to bson format
void TObject::toBsonObject( bson *obj )
{
  char buf[100];
  bson_append_string( obj,"label", Keywd );
  bson_append_int( obj, "dot", Type );
  if( IsNull() && Dynamic && M != 0)
    bson_append_int( obj, "dN", 0 );
  else
     bson_append_int( obj, "dN", N );
   bson_append_int( obj, "dM", M );

  bson_append_bool( obj, "dyn", Dynamic );
  //obj.insert("ndxs",  QJsonValue(IndexationCode) );

  if( IsNull() )
  {
      bson_append_null(obj, "val");
      return;
  }

  if( strcmp(Keywd, "SPPpar") == 0 )
  {
      //obj.insert( "val", ((SPP_SETTING*)GetPtr())->toBsonObject());
      return;
  }

  if( strcmp(Keywd+2, "plt") == 0 )
  {
      bson_append_start_array(obj, "val");
      for(int ii=0; ii<GetN(); ii++)
      {
          sprintf(buf, "%d", ii);
          bson_append_start_object( obj, buf);
          ((TPlotLine*)GetPtr() + ii)->toBsonObject( obj );
          bson_append_finish_object( obj );
      }
      bson_append_finish_array(obj);
     return;
  }


  if( Type == J_ ) // text
  {
      bson_append_bson( obj, "val", (bson*)GetPtr()  );
  }
  else if( Type == S_ ) // text
  {
      bson_append_string( obj, "val", (const char*)GetPtr()  );
  }
  else
  {
    bson_append_start_array(obj, "val");

      for(int i=0; i < N; i++ )
        for(int j=0; j < M; j++ )
         {
            int ndx_ = ndx(i,j);
            sprintf(buf, "%d", ndx_);

            if( IsEmpty(i,j))
                bson_append_null( obj, buf); // null
            else
            {
               if( Type>= D_ && Type<= I_)
                  bson_append_double( obj, buf, Get(i,j) );
               else
               {
                 int ret =   bson_append_string( obj, buf, GetString(i,j).c_str());
                 if( ret == BSON_ERROR )
                     cout<< Keywd << "___" <<GetString(i,j).c_str() << endl;
               }
            }
         }
    bson_append_finish_array(obj);
  }
}



/// read object from bson structure
/// "label" and "id" must be test before
void TObject::fromBsonObject( const char *obj )
{
    signed char Otype;
    int Odim_N;
    unsigned int Odim_M;
    bool ODynamic;
    double valDouble;
    string valStr;
    char buf[100];

    // get definition of object
    if(!bson_find_value( obj, "dot", Otype ) )
        Error( "E001BSon: ", "Undefined dot.");
    if(!bson_find_value( obj, "dN", Odim_N ) )
        Error( "E002BSon: ", "Undefined dN.");
    if(!bson_find_value( obj, "dM", Odim_M ) )
        Error( "E003BSon: ", "Undefined dM.");
    if(!bson_find_value( obj, "dyn", ODynamic ) )
        Error( "E004BSon: ", "Undefined dyn.");
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
        const char *arr  = bson_find_array(  obj, "val" );
        for(int ii=0; ii<N; ii++)
        {
            sprintf(buf, "%d", ii);
            ( ((TPlotLine*)GetPtr() + ii)->fromBsonObject( bson_find_object(  arr, buf ) ) );
        }
        return;
    }

    if( Type == J_ ) // text
    {
        const char *objdata = bson_find_object_null( obj, "val");
        if( objdata  )
         bson_create_from_buffer2( (bson*)GetPtr(), objdata, bson_size2(objdata) );
     }
    else if( Type == S_ ) // text
    {
        bson_find_string( obj, "val", valStr );
        SetString(valStr.c_str(), 0,0);
     }
    else
    {
      const char *arr  = bson_find_array(  obj, "val" );
      for(int i=0; i < N; i++ )
          for(int j=0; j < M; j++ )
          {
            int ndx_ = ndx(i, j);
            sprintf(buf, "%d", ndx_);

            if( Type>= D_ && Type<= I_)
                {
                    if( bson_find_value( arr, buf, valDouble ) )
                      Put(valDouble, i,j);
                    else
                      SetString(S_EMPTY, i,j);
                }
                else
                {   bson_find_string( arr, buf, valStr );
                    SetString(valStr.c_str(), i,j);
                }
           }
    }
}

//===================================================
// Implementation of TObjList class

// Finds DOD index from a given name s
// returns -1 if such a name does not exist
int
TObjList::Find(const char* s)
{
    for( int ii=0; ii<size(); ii++ )
        if( strcmp(s, at(ii)->GetKeywd() )==0 )
            return ii;

    return -1;
}

const int N_OBJECTS = 1000;

// default DOD list configuration
TObjList::TObjList()
{
    //  load( "viz_od.ini", MINVISOROBJECTS );
}

// Reads DOD list configuration from file

TObjList::TObjList(istream& f)
{
    fromDAT(f);
}


// Writes DOD list configuration to file
void
TObjList::toDAT(ostream& f)
{
    double Value = 0.0;
    int nObj = size();

    f << '@';
    f.write((char*) &Value, sizeof(double) );
    f.write((char*) &nObj, sizeof(int) );
    for(int ii=0; ii<nObj; ii++)
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
    f.read((char*) &Value, sizeof(double) );
    f.read((char*) &nObj, sizeof(int) );
    for(int ii=0; ii<nObj; ii++)
        push_back( new TObject(f) );
}


const int nTypes = 13;
static char OBtype[nTypes][3] =
    { "S_", "I_", "U_", "L_", "X_", "F_",
      "D_", "C_", "N_", "A_", "B_", "H_", "J_" };

// Loads default configurations from a text .ini file
void
TObjList::load(const char* f_obj, int /* maxN */ )
{
#ifndef __unix
    // normal notation somehow causes crash on Win32 - workaround to work with pointer
    // Is it memory leak?
//    TConfig cnf(f_obj,' ');
    TConfig& cnf = *new TConfig(f_obj,' ', 55 );
#else
TConfig cnf(f_obj,' ');
#endif
    string par;
    int N;
    unsigned M;
    ObjType objectType = 0;
    char indexationCode;
    string astr[6];

    par = cnf.getFirst();

    while( !par.empty() )
    {
        cnf.getcStrings(6, astr);

        sscanf( astr[2].c_str(),"%d", &N);
        sscanf( astr[3].c_str(),"%u", &M);
	indexationCode = astr[4][0];

        if( isdigit(astr[1][0]) )
        {
            short i;
            sscanf(astr[1].c_str(), "%hd", &i);
            objectType = i;
        }
        else
        {
            int ii;
            for( ii=0; ii<nTypes; ii++ )
                if( strcmp(astr[1].c_str(),OBtype[ii])==0 )
                {
                    objectType = ObjType(-ii);
                    break;
                }
            if( ii == nTypes )
              {
                 throw TFatalError(astr[0].c_str(),
                                  "TObject:E17 Unknown object type in ini-file");

               }
        }

        if( Find(astr[0].c_str()) >0 )
        {
           const char* label = astr[0].c_str();
           throw TFatalError(label,
                       "TObject:E18 This data object is already defined");
        }
        // cout <<  astr[0].c_str() <<  "  " << objectType<< endl;
        push_back( new TObject(astr[0].c_str(), objectType, abs(N), M, N<0,
		     indexationCode, astr[5]) );
        par = cnf.getNext();
    }
    cnf.close();
}

// deleted functions

/*Puts data object to backup format TXT file
void TObject::toTXT( fstream& to )
{
    int dimM, i, j;
    char sbuf[150];

    dimM = M;
    if( IsDynamic() )
    {
        if( IsNull() )
            dimM = 0;
        else
            if( Type == S_  )
                dimM = strlen((char*)GetPtr())+2;
        to << "^^" << N << " " << dimM << " " << (int)Type << "~~\n";
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
            ((TPlotLine*)GetPtr() + ii)->write(to);
         return;
    }

    if( Type == S_ ) // text
    {  //*((char*)(char*)GetPtr()+M-1) = 0;

        to << "\'" << (char*)GetPtr() << "\'\n";
    }
    else
        for( i=0; i < N; i++ )
        {
            for( j=0; j < M; j++ )
            {
                strcpy(sbuf, GetString(i,j).c_str() );
                if( Type > 0 )
                    to << "\"";
                 switch( *sbuf )
                 {
                    case 0:
                        sbuf[0] = '`';
                        sbuf[1] = 0;
                        break;
                    case ' ':
                        sbuf[0] = ' ';
                        sbuf[1] = 0;
                        break;
                    default:
                        break;
                  }
                  to << sbuf;
                  if( Type > 0 )
                    to << "\"";
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
        if ( sscanf( sbuf , "^^%d%d%d~~", &rdimN, &rdimM, &r_otype  ) < 3 )
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
            ((TPlotLine*)GetPtr() + ii)->read(of);
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
            of.get( (char*)GetPtr(), M, '\'');
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
            char *sp = (char*)GetPtr();
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
*/

/* Puts data object to backup JSON format
QJsonObject TObject::toJsonObject()
{
  QJsonObject obj;
  QJsonArray  valArr;
  QJsonValue  val;

  obj.insert("label", QJsonValue(QString(Keywd)) );
  obj.insert("dot", QJsonValue(Type) );
  obj.insert("dN", QJsonValue(N) );
  obj.insert("dM", QJsonValue(M) );
  obj.insert("dyn",  QJsonValue(Dynamic) );
  //obj.insert("ndxs",  QJsonValue(IndexationCode) );

  if( strcmp(Keywd, "SPPpar") == 0 )
  {
      obj.insert( "val", ((SPP_SETTING*)GetPtr())->toJsonObject());
      return obj;
  }

  if( strcmp(Keywd+2, "plt") == 0 )
  {
      for(int ii=0; ii<GetN(); ii++)
          valArr.append( ((TPlotLine*)GetPtr() + ii)->toJsonObject() );
      obj.insert( "val", valArr );

      return obj;
  }

  if( Type == S_ ) // text
  {
      QString txt( (char*)GetPtr() );
      obj.insert( "val", QJsonValue(txt) );
  }
  else
  {    for(int i=0; i < N; i++ )
        for(int j=0; j < M; j++ )
        {
            if( IsEmpty(i,j))
                val = QJsonValue(); // null
            else
            {
               if( Type>= D_ && Type<= I_)
                  val = QJsonValue( Get(i,j) );
               else
                  val = QJsonValue(QString(GetString(i,j).c_str()));
            }

            valArr.append(val);
         }
      obj.insert( "val", valArr );
  }

  return obj;
}


// read object from json structure
// "label" and "id" must be test before
void TObject::fromJsonObject( QJsonObject obj )
{
    signed char Otype;
    int Odim_N;
    unsigned int Odim_M;
    bool ODynamic;

    // get definition of object

    Otype = (int)obj.value("dot").toDouble();
    Odim_N = (int)obj.value("dN").toDouble();
    Odim_M = (int)obj.value("dM").toDouble();
    ODynamic = obj.value("dyn").toBool();
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

        if( Otype != Type || Odim_N != N || Odim_M != M )
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

    // read values
    QJsonArray arr;

    if( strcmp(Keywd, "SPPpar") == 0 )
    {
        ((SPP_SETTING*)GetPtr())->fromJsonObject( obj.value("val").toObject() );
        return;
    }

    if( strcmp(Keywd+2, "plt") == 0 )
    {
        arr = obj.value("val").toArray();
        for(int ii=0; ii<N; ii++)
            ( ((TPlotLine*)GetPtr() + ii)->fromJsonObject( arr[ii].toObject() ) );
        return;
    }

    if( Type == S_ ) // text
    {
        string txt = obj.value("val").toString().toUtf8().data();
        SetString(txt.c_str(), 0, 0);
    }
    else
    {
       arr = obj.value("val").toArray();
       double valDouble;
       string valStr;

       for(int i=0; i < N; i++ )
          for(int j=0; j < M; j++ )
          {
            int ndx_ = ndx(i, j);

            if( arr[ndx_].isNull() )
            {
               SetString(S_EMPTY, i,j);
            }
            else
                if( Type>= D_ && Type<= I_)
                {   valDouble = arr[ ndx_].toDouble();
                    Put(valDouble, i,j);
                }
                else
                {   valStr = arr[ ndx_].toString().toUtf8().data();
                    SetString(valStr.c_str(), i,j);
                }
           }
    }
}
*/


//--------------------- End of v_object.cpp ---------------------------

