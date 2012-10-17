//-------------------------------------------------------------------
// $Id: v_vals_impl.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TVal class and its child classes
//
// Copyright (C) 1996-2001 A.Rysin
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _v_vals_impl_h_
#define _v_vals_impl_h_

#include <cstdio>
#include "v_vals.h"
#include "v_user.h"
#include "gdatastream.h"


/* TVal template is defining implementation
    of TValBase interface for manipulation of
    calculation values

    All cleanups and pointer checking
    are performed by TObject class
    for better performance
*/

template<class T>
struct TVal:
            TValBase
{
    // values and string representations for special values: EMPTY and ANY
    static const T EMPTY();
    static const T ANY();
    static const char* PATTERN_GET();
    static const char* PATTERN_SET();

    // basic constructor and descructor
    TVal(bool d):
            TValBase(d)
    { }

    ~TVal()
    {
        if( dynamic && ptr ) delete[] (T*)ptr;
    }

    // returns size of the cell
    size_t cSize() const;

    /* allocates memory for given object
	Note: IsDynamic() verified in TObject
    */
    void* Alloc(size_t sz)
    {
        delete[] (T*)ptr;
        return ptr = new T[sz];
    }

    /* returns double representations of the value
    */
    double Get(size_t ndx) const
    {
       return (double)((T*)ptr)[ndx];
    }

    /* converts double parameter and assigns it to the cell
    */
    void Put(double v, size_t i);

    /* returns true if values equals to ANY
    */
    bool IsAny(size_t ndx) const
    {
        if( ((T*)ptr)[ndx] == ANY() )
            return true;
        return false;
    }

    /* returns true if values equals to EMPTY
    */
    bool IsEmpty(size_t ndx) const
    {
        if( ((T*)ptr)[ndx] == EMPTY() )
            return true;
        return false;
    }

    /* returns string representation of the cell value
    */
    gstring GetString(size_t ndx) const;
    //  bool VerifyString(const char* s);
    /* converts string to the type and puts it into cell
	returns false on failure
    */
    bool SetString(const char* s, size_t ndx);

    void write(GemDataStream& s, size_t size) {
    s.writeArray((T*)ptr, size/cSize());
    }
    void read(GemDataStream& s, size_t size) {
    s.readArray((T*)ptr, size/cSize());
    }
};


/*
    Type-specific implemetations of the functions and classes
*/

// case when 0 < Type < 127

struct TValFixString:
            TValBase
{
    size_t len;

    TValFixString(size_t l, bool d):
            TValBase(d), len(l)
    { }

    ~TValFixString()
    {
        if( dynamic && ptr) delete[] (char*)ptr;
    }

    size_t cSize() const
    {
        return len;
    }

    // see Alloc() description for TVal<T>
    void* Alloc(size_t sz)
    {
        delete[] (char*)ptr;
        ptr = new char[len*sz + 1];
        // +1 for simple maintenance w/'\0' at the end
        *(char*)ptr='\0';
        return ptr;
    }

    double Get(size_t ) const
    {
        return 0.;
    }
    void Put(double , size_t)
    {}


    bool IsAny(size_t ndx) const
    {
        return ((char*)ptr)[ndx*len]=='`';
    }
    bool IsEmpty(size_t ndx) const
    {
        return ((char*)ptr)[ndx*len]=='*';
    }
    gstring GetString(size_t ndx) const
    {
        vstr ss(len);
        strncpy(ss, (const char*)ptr+(ndx*len), len);
        ss[len]='\0';
        return ss.p;
    }
    //  bool VerifyString(const char* s);
    bool SetString(const char* s, size_t ndx);

    void write(GemDataStream& s, size_t size) {
	s.writeArray((char*)ptr, size);
    }
    void read(GemDataStream& s, size_t size) {
	s.readArray((char*)ptr, size);
    }
};


// Type = S_

struct TValString:
            TValBase
{
    size_t size;

    TValString(size_t M, bool d):
            TValBase(d), size(M)
    { }

    ~TValString()
    {
        if( dynamic && ptr ) delete[] (char*)ptr;
    }
    // if *ptr is dynamic Free should be called

    size_t cSize() const
    {
        return size;/*strlen(ptr);*/
    }

    // see Alloc() description for TVal<T>
    void* Alloc(size_t sz)
    {
        delete[] (char*)ptr;
        ptr = new char[(size=sz)+1];
        *(char*)ptr='\0';
        return ptr;
    }

    double Get(size_t /*ndx*/) const
    {
        return 0.;
    }
    void Put(double /*val*/, size_t /*ndx*/)
    {}

    bool IsAny(size_t /*ndx*/) const
    {
        return (strcmp((char*)ptr,S_ANY)==0);
    }
    bool IsEmpty(size_t /*ndx*/) const
    {
        return (strcmp((char*)ptr,S_EMPTY)==0);
    }
    gstring GetString(size_t /*ndx*/) const
    {
        return (!*(char*)ptr) ? S_EMPTY: (char*)ptr;
    }
    //  bool VerifyString(const char* s);
    bool SetString(const char* s, size_t ndx);

    void write(GemDataStream& s, size_t size) {
	s.writeArray((char*)ptr, size);
    }
    void read(GemDataStream& s, size_t size) {
	s.readArray((char*)ptr, size);
    }
};

//
//  TVal<T> functions definitions
//

template<class T>
inline
gstring
TVal<T>::GetString(size_t ndx) const
{
    if( IsEmpty(ndx) )
        return S_EMPTY;
    if( IsAny(ndx) )
        return S_ANY;

    vstr vbuf(30);	// double is ~15 digit
    sprintf(vbuf, PATTERN_GET(), ((T*)ptr)[ndx]);

    return vbuf.p;
}

template<>
inline
gstring
TVal<double>::GetString(size_t ndx) const
{
    if( IsEmpty(ndx) )
        return S_EMPTY;
    if( IsAny(ndx) )
        return S_ANY;

    vstr vbuf(30);	// double is ~15 digit   PATTERN_GET()
    sprintf(vbuf, "%.*lg" , doublePrecision, ((double*)ptr)[ndx]);

    return vbuf.p;
}

template<class T>
//inline
bool
TVal<T>::SetString(const char* s, size_t ndx)
{
    gstring ss = s;
    ss.strip();
    if( /*ss.empty() ||*/ ss==S_EMPTY )
    {
        ((T*)ptr)[ndx] = EMPTY();
        return true;
    }

    if( ss == S_ANY )
    {
        ((T*)ptr)[ndx] = ANY();
        return true;
    }

    T v;
    vstr sv(strlen(s)+3);
    if( sscanf(ss.c_str(), PATTERN_SET(), &v, sv.p ) != 1 )
        return false;

    ((T*)ptr)[ndx] = v;
    return true;
}


// Set/GetString() for <char> need special handling
template<>
inline
bool
TVal<char>::SetString(const char* s, size_t ndx)
{
    ((char*)ptr)[ndx] = *s;
    return true;
}

// Set/GetString() for <char> need special handling
template<>
inline
bool
TVal<unsigned char>::SetString(const char* s, size_t ndx)
{
    ((unsigned char*)ptr)[ndx] = (unsigned char)*s;
    return true;
}

template<>
inline
gstring
TVal<char>::GetString(size_t ndx) const
{
    return gstring(1, ((char*)ptr)[ndx]);
}

template<>
inline
gstring
TVal<unsigned char>::GetString(size_t ndx) const
{
    return gstring(1, ((char*)ptr)[ndx]);
}


template<class T>
inline
void
TVal<T>::Put(double v, size_t ndx)
{
   ((T*)ptr)[ndx] = ( fabs(v) <= ANY() ) ? T(v/*+.5*/) : EMPTY();	// truncate
}

// Put() for <double> need special handling for efficency

template<>
inline
void
TVal<double>::Put(double v, size_t ndx)
{
    ((double*)ptr)[ndx] = v;
}

// returns size of the cell
template<class T>
inline
size_t TVal<T>::cSize() const
{
    return sizeof(T);
}

// returns size of the cell (17/10/2012 using only from GemDataStream )
template<>
inline
size_t TVal<long>::cSize() const
{
    return sizeof(int32_t);
}

/*
template<class T>
extern
bool
TVal<unsigned char>::SetString(const char* s, size_t ndx);

template<class T>
extern
bool
TVal<signed char>::SetString(const char* s, size_t ndx);
*/

/*
template<class T>
bool
TVal<T>::VerifyString(const char* s, T& v)
{
  gstring ss = s;
  ss.strip();
  if( ss==S_EMPTY )
    return true;
  if( ss == S_ANY )
    return true;
  T v;
  vstr sv(strlen(s)+1);
  gstring PAT = PATTERN;
  PAT += "%s";

  if( sscanf(ss.c_str(), PAT.c_str(), &v, sv.p ) != 1 )
    return false;

  return true;
}
*/

#endif // _v_vals_impl_h_

