//-------------------------------------------------------------------
// $Id: v_vals_impl.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TVal class and its child classes
//
// Copyright (C) 1996-2021 A.Rysin, S.Dmytriyeva
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

#ifndef _v_vals_impl_h_
#define _v_vals_impl_h_

#include <memory>
#include <stdint.h>  // To be replaced with cstdint after switching to C++ 11 standard
#include <cstdio>
#include "v_vals.h"
#include "v_user.h"
#include "GEMS3K/gdatastream.h"


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
    static T EMPTY();
    static T ANY();

    // basic constructor and descructor
    TVal(bool d):
        TValBase(d)
    { }

    ~TVal()
    {
        if( dynamic && ptr )
            delete[] static_cast<T*>(ptr);
    }

    // returns size of the cell
    int cSize() const;

    /* allocates memory for given object
    Note: IsDynamic() verified in TObject
    */
    void* Alloc(int sz)
    {
        delete[] static_cast<T*>(ptr);
        return ptr = new T[sz];
    }

    /* returns double representations of the value
    */
    double Get(int ndx) const
    {
        return static_cast<double>(static_cast<T*>(ptr)[ndx]);
    }

    /* converts double parameter and assigns it to the cell
    */
    void Put(double v, int i);

    /* returns true if values equals to ANY
    */
    bool IsAny(int ndx) const;

    /* returns true if values equals to EMPTY */
    bool IsEmpty(int ndx) const;

    /* returns string representation of the cell value */
    string GetString(int ndx) const;
    /* converts string to the type and puts it into cell
    returns false on failure  */
    bool SetString(const char* s, int ndx);

    void write(GemDataStream& s, int size) {
        s.writeArray(static_cast<T*>(ptr), size/cSize());
    }
    void read(GemDataStream& s, int size) {
        s.readArray(static_cast<T*>(ptr), size/cSize());
    }
};


/*
    Type-specific implemetations of the functions and classes
*/

// case when 0 < Type < 127
struct TValFixString:
        TValBase
{
    int len;

    TValFixString(int l, bool d):
        TValBase(d), len(l)
    { }

    ~TValFixString()
    {
        if( dynamic && ptr) delete[] static_cast<char*>(ptr);
    }

    int cSize() const
    {
        return len;
    }

    // see Alloc() description for TVal<T>
    void* Alloc(int sz)
    {
        delete[] static_cast<char*>(ptr);
        ptr = new char[len*sz + 1];
        // +1 for simple maintenance w/'\0' at the end
        /// *static_cast<char*>(ptr)='\0';
        memset(ptr, '\0', len*sz + 1);
        return ptr;
    }

    double Get(int ) const
    {
        return 0.;
    }
    void Put(double , int)
    {}

    bool IsAny(int ndx) const
    {
        return static_cast<char*>(ptr)[ndx*len]=='*';
    }
    bool IsEmpty(int ndx) const
    {
        return static_cast<char*>(ptr)[ndx*len]=='`';
    }
    string GetString(int ndx) const
    {
        if(!ptr)
            return "";
        auto ss = char_array_to_string( static_cast<char*>(ptr)+(ndx*len), len);
        return ss;
    }
    bool SetString(const char* s, int ndx);

    void write(GemDataStream& s, int size) {
        s.writeArray(static_cast<char*>(ptr), size);
    }
    void read(GemDataStream& s, int size) {
        s.readArray(static_cast<char*>(ptr), size);
    }
};


// Type = S_

struct TValString:
        TValBase
{
    int size;

    TValString(int M, bool d):
        TValBase(d), size(M)
    { }

    ~TValString()
    {
        if( dynamic && ptr ) delete[] static_cast<char*>(ptr);
    }
    // if *ptr is dynamic Free should be called

    int cSize() const
    {
        return size;/*strlen(ptr);*/
    }

    // see Alloc() description for TVal<T>
    void* Alloc(int sz)
    {
        delete[] static_cast<char*>(ptr);
        ptr = new char[(size=sz)+1];
        memset(ptr, '\0', size + 1);
        ///*static_cast<char*>(ptr)='\0';
        return ptr;
    }

    double Get(int /*ndx*/) const
    {
        return 0.;
    }
    void Put(double /*val*/, int /*ndx*/)
    {}

    bool IsAny(int /*ndx*/) const
    {
        if(!ptr)
            return false;
        return (strcmp(static_cast<char*>(ptr),S_ANY)==0);
    }
    bool IsEmpty(int /*ndx*/) const
    {
        if(!ptr)
            return true;
        return (strcmp(static_cast<char*>(ptr),S_EMPTY)==0);
    }
    string GetString(int /*ndx*/) const
    {
        if(!ptr)
            return S_EMPTY;
        return (!static_cast<char*>(ptr) ? S_EMPTY: char_array_to_string(static_cast<char*>(ptr), size));
    }
    bool SetString(const char* s, int ndx);

    void write(GemDataStream& s, int size1) {
        s.writeArray(static_cast<char*>(ptr), size1);
    }
    void read(GemDataStream& s, int size1) {
        s.readArray(static_cast<char*>(ptr), size1);
    }
};

//
//  TVal<T> functions definitions
//

// returns true if values equals to ANY
template<class T>
inline bool TVal<T>::IsAny(int ndx) const
{
    if( static_cast<T*>(ptr)[ndx] == ANY() )
        return true;
    return false;
}

template<class T>
inline bool TVal<T>::IsEmpty(int ndx) const
{
    if( static_cast<T*>(ptr)[ndx] == EMPTY() )
        return true;
    return false;
}

template <class T>
inline string TVal<T>::GetString(int ndx) const
{
    if( IsEmpty(ndx) )
        return S_EMPTY;
    if( IsAny(ndx) )
        return S_ANY;

    return value2string(static_cast<T*>(ptr)[ndx], doublePrecision );
}

template<class T>
bool TVal<T>::SetString(const char* s, int ndx)
{
    string ss = s;
    strip( ss );
    if( ss==S_EMPTY )
    {
        static_cast<T*>(ptr)[ndx] = EMPTY();
        return true;
    }
    if( ss == S_ANY )
    {
        static_cast<T*>(ptr)[ndx] = ANY();
        return true;
    }

    T v;
    if( !string2value( v, ss ) )
        return false;

    static_cast<T*>(ptr)[ndx] = v;
    return true;
}


// Set/GetString() for <char> need special handling
template<>
inline bool TVal<char>::SetString(const char* s, int ndx)
{
    static_cast<char*>(ptr)[ndx] = *s;
    return true;
}

// Set/GetString() for <char> need special handling
template<>
inline bool TVal<unsigned char>::SetString(const char* s, int ndx)
{
    static_cast<unsigned char*>(ptr)[ndx] = static_cast<unsigned char>(*s);
    return true;
}

template<>
inline string TVal<char>::GetString(int ndx) const
{
    return string(1, static_cast<char*>(ptr)[ndx]);
}

template<>
inline string TVal<unsigned char>::GetString(int ndx) const
{
    return string(1, static_cast<char*>(ptr)[ndx]);
}


template<class T>
inline void TVal<T>::Put(double v, int ndx)
{
    static_cast<T*>(ptr)[ndx] = ( fabs(v) <= ANY() ) ? T(v/*+.5*/) : EMPTY();	// truncate
}

// Put() for <double> need special handling for efficency
template<>
inline void TVal<double>::Put(double v, int ndx)
{
    static_cast<double*>(ptr)[ndx] = v;
}

// returns size of the cell
template<class T>
inline int TVal<T>::cSize() const
{
    return sizeof(T);
}

// returns size of the cell (17/10/2012 using only from GemDataStream )
template<>
inline int TVal<long>::cSize() const
{
    return sizeof(int32_t);
}

template<> bool TVal<double>::IsAny(int ndx) const;
template<> bool TVal<double>::IsEmpty(int ndx) const;
template<> bool TVal<float>::IsAny(int ndx) const;
template<> bool TVal<float>::IsEmpty(int ndx) const;

#endif // _v_vals_impl_h_

