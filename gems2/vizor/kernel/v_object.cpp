//-------------------------------------------------------------------
// $Id$
//
// Implementation of TObject and TObjList classes
//
// Copyright (C) 1996-2001 S.Dmytriyeva, A.Rysin
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include "v_vals_impl.h"
#include "v_object.h"
#include "m_param.h"
#include "graph.h"
#include "config.h"

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
                  int m, bool dyn, char indexCode, const gstring desc):
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
void
TObject::ToCFG(ostream& f)
{
    write(f);
}

gstring
TObject::GetFullName(int aN, int aM)
{
  vstr v(15);
  gstring item = GetKeywd();
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

// Gets description line from Ni line of DOD list
//    e.g., for displaying a tooltip
const gstring
TObject::GetDescription(int Ni, int Mi)
{
    size_t prev_pos = 0;
    size_t pos = Descr.find('\n');

    if( pos == gstring::npos )
        return Descr;

    int NMi = Ni;   // Description by lines
    if( N <= 1 || Descr[0] == '|' )     // Decriptions by colums
           NMi = Mi;

    for(int ii=0;  ii < NMi /*&& pos != gstring::npos*/; ii++ )
    {
        prev_pos = pos + 1;

        if( prev_pos >= Descr.length() )
            return gstring("TObject:E01 -bad description line in DOD-");

        pos = Descr.find("\n", prev_pos);
        if( pos == gstring::npos )
            return Descr.substr(prev_pos, gstring::npos);
    }
    return Descr.substr(prev_pos, pos-prev_pos-1);
}

// Writes DOD data to ostream file
void
TObject::write( ostream& os )
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
void
TObject::read( istream& is )
{
    is.read( Keywd, MAXKEYWD );
    is.read( (char*)&Type, sizeof(ObjType) );
    is.read( (char*)&Dynamic, sizeof(bool) );
    is.read( (char*)&N, sizeof(int) );
    is.read( (char*)&M, sizeof(int) );
    is.read( &IndexationCode, sizeof(char) );
    u_getline( is, Descr, '@' );
}

// Determines size of one cell
size_t
TObject::GetCellSize() const
{
    if( IsNull() )
        return 0;

    return pV->cSize();
}

// Determines size of the whole data object
size_t
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
        SetM( strlen((const char*)newPtr)+1 );

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
        gstring str( newType, ' ' );
        // cout << (int)newType << ' ' << Keywd << " T" << str.c_str() << "T" << endl;
        for( int ii=0; ii<newN; ii++ )
            for( int jj=0; jj<newM; jj++ )
                pV1->SetString( str.c_str(), ii*newM+jj );
    }

    // Testing data type for debugging purposes
#ifdef __unix
    if( newType != Type )
        cout << "Different type! Object:" << Keywd
        << " Type= " << (int)Type << " newType= " << (int)newType << endl;
#endif
    /* Saving old values */
    int n = min(N,newN);
    int m = min(M,newM);

    if( !IsNull() )
    {
        if( Type == newType || ( Type > S_ && newType > S_ )  )
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
                            gstring str = pV->GetString(ndx(ii,jj));
                            pV1->SetString( str.c_str(), ii*newM+jj );
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
    return pV=0;
}


// puts gstring into data object cell
bool
TObject::SetString(const char *vbuf, int aN, int aM)
{
    check_dim( aN, aM);
    ErrorIf( !vbuf, GetKeywd(),"TObject:W06 Cannot set empty gstring to object");
    //???

    if( !pV->SetString(vbuf, ndx(aN,aM)) )
        return false;

    if( Type == S_ && IsDynamic() )  // oct 2005
        M = GetCellSize();

    return true;
}


// get cell of data object as double
double
TObject::Get(int n, int m)
{
    check_dim(n,m);
    return pV->Get(ndx(n,m));
}

// put value to cell of data object as double
void
TObject::Put(double Value, int n, int m)
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
const size_t szOLABEL = sizeof(char[2]) + sizeof(char)  + sizeof(char)
                        + sizeof(int) + sizeof(int) + sizeof(char[2]);

// Determines size of a binary image of data object in database file
size_t
TObject::lenDB() const
{
    size_t size;
    int Odim_N;
    unsigned int Odim_M;
    unsigned char csize = (unsigned char)GetCellSize();

    if( pV )
    {
        if( Type != S_  )
        {
            size = (size_t)csize*(size_t)N*(size_t)M;
            Odim_N = N;
            Odim_M = M;
        }
        else
        {
          if( IsDynamic() )
          {
            if( pV && pV->GetPtr() )
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
        if( Type != S_  )
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
#ifdef __unix
            cerr << endl << "writing SPP_SETTING, size = " << size << endl;
#endif
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
    if( Otype != S_  )
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


//Puts data object to backup format TXT file
void TObject::toTXT( fstream& to )
{
    int dimM, i, j;
    vstr sbuf(150);

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
                else switch( *sbuf )
                    {
                    case 0:
                        sbuf[0] = '`';
                        sbuf[1] = 0;
                        break;
                    case ' ':
                        sbuf[0] = ' '/*'�'*/;
                        sbuf[1] = 0;
                        break;
                    default:
                        break;
                    }
                to << sbuf;
                if( Type > 0 )
                    to << "\"";
                else to << " ";
            }
            to << "\n";
        }
}

//Gets data object from backup format TXT file
void TObject::ofTXT( fstream& of )
{
    int rdimN, rdimM, i, j;
    int r_otype;
    vstr sbuf(160);

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
                    if( of.get() != '\"')
                        Error(GetKeywd(),
                              "TObject:E16 Bad format or character when reading text object from TXT");
                }

                else of >> sbuf.p;
                if( Type == A_ )
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



// Implementation of TObjList class

// Finds DOD index from a given name s
// returns -1 if such a name does not exist
int
TObjList::Find(const char* s)
{
    for( int ii=0; ii<(int)GetCount(); ii++ )
        if( strcmp(s, elem(ii).GetKeywd() )==0 )
            return ii;

    return -1;
}

const int N_OBJECTS = 1000;

// default DOD list configuration
TObjList::TObjList():
        TIArrayF<TObject>(N_OBJECTS, 30 )
{
    //  load( "viz_od.ini", MINVISOROBJECTS );
}

// Reads DOD list configuration from file

TObjList::TObjList(istream& f):
        TIArrayF<TObject>(N_OBJECTS, 30 )
{
    fromDAT(f);
}


// Writes DOD list configuration to file
void
TObjList::toDAT(ostream& f)
{
    double Value = 0.0;
    int nObj = GetCount();

    f << '@';
    f.write((char*) &Value, sizeof(double) );
    f.write((char*) &nObj, sizeof(int) );
    for(int ii=0; ii<nObj; ii++)
        elem(ii).ToCFG( f );
}

// Reads DOD configuration from config file
void
TObjList::fromDAT(istream& f)
{
    double Value;	// not used

    Clear();
    int nObj;
    f.get(); // '@' marker
    f.read((char*) &Value, sizeof(double) );
    f.read((char*) &nObj, sizeof(int) );
    for(int ii=0; ii<nObj; ii++)
        Add( new TObject(f) );
}


const int nTypes = 12;
static char OBtype[nTypes][3] =
    { "S_", "I_", "U_", "L_", "X_", "F_",
      "D_", "C_", "N_", "A_", "B_", "H_" };

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
    gstring par;
    int N;
    unsigned M;
    ObjType objectType = 0;
    char indexationCode;
    gstring astr[6];

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
                throw TFatalError(astr[0].c_str(),
                                  "TObject:E17 Unknown object type in ini-file");
        }

        if( Find(astr[0].c_str()) >0 )
        {
           const char* label = astr[0].c_str();
           throw TFatalError(label,
                       "TObject:E18 This data object is already defined");
        }
        Add( new TObject(astr[0].c_str(), objectType, abs(N), M, N<0,
		     indexationCode, astr[5]) );
        par = cnf.getNext();
    }

}

//--------------------- End of v_object.cpp ---------------------------

