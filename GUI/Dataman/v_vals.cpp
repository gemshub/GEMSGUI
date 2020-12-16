//-------------------------------------------------------------------
// $Id: v_vals.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TValBase, TVal and its templates
//
// Copyright (C) 1996-2001 A.Rysin
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


#include <iostream>
#include <cstdio>
#include <cmath>

#include "v_vals_impl.h"

int TValBase::doublePrecision = 8;

const char* S_EMPTY	=    "`";
const char* S_ANY  	=    "*";


TValBase::~TValBase()
{}

/* defining values EMPTY and ANY for different types
    Note: for some reason static constants in template classes
    did not go well for Borland C++, we must use functions members
*/

template<> short TVal<short>::EMPTY()
{
    return  SHORT_EMPTY;
}
template<> short TVal<short>::ANY()
{
    return  SHORT_ANY;
}
template<> const char* TVal<short>::PATTERN_GET()
{
    return "%hd";
}
template<> const char* TVal<short>::PATTERN_SET()
{
    return  "%hd%s";
}

template<> unsigned short TVal<unsigned short>::EMPTY()
{
    return USHORT_EMPTY;
}
template<> unsigned short TVal<unsigned short>::ANY()
{
    return USHORT_ANY;
}
template<> const char* TVal<unsigned short>::PATTERN_GET()
{
    return "%hu";
}
template<> const char* TVal<unsigned short>::PATTERN_SET()
{
    return "%hu%s";
}

template<> long TVal<long>::EMPTY()
{
    return LONG_EMPTY;
}
template<> long TVal<long>::ANY()
{
    return LONG_ANY;
}
template<> const char* TVal<long>::PATTERN_GET()
{
    return  "%ld";
}
template<> const char* TVal<long>::PATTERN_SET()
{
    return  "%ld%s";
}

template<> unsigned long TVal<unsigned long>::EMPTY()
{
    return ULONG_EMPTY;
}
template<> unsigned long TVal<unsigned long>::ANY()
{
    return ULONG_ANY;
}
template<> const char* TVal<unsigned long>::PATTERN_GET()
{
    return  "%lu";
}
template<> const char* TVal<unsigned long>::PATTERN_SET()
{
    return  "%lu%s";
}

template<> float TVal<float>::EMPTY()
{
    return FLOAT_EMPTY;
}
template<> float TVal<float>::ANY()
{
    return FLOAT_ANY;
}
template<> const char* TVal<float>::PATTERN_GET()
{
    return  "%g";
}
template<> const char* TVal<float>::PATTERN_SET()
{
    return  "%g%s";
}

template<> double TVal<double>::EMPTY()
{
    return DOUBLE_EMPTY;
}
template<> double TVal<double>::ANY()
{
    return DOUBLE_ANY;
}
template<> const char* TVal<double>::PATTERN_GET()
{
    return  "%.10lg";
}
template<> const char* TVal<double>::PATTERN_SET()
{
    return  "%lg%s";
}

template<> unsigned char TVal<unsigned char>::EMPTY()
{
    return UCHAR_EMPTY;
}
template<> unsigned char TVal<unsigned char>::ANY()
{
    return UCHAR_ANY;
}
template<> const char* TVal<unsigned char>::PATTERN_GET()
{
    return  "%hu";
}
template<> const char* TVal<unsigned char>::PATTERN_SET()
{
    return  "%hu%s";
}

template<> signed char TVal<signed char>::EMPTY()
{
    return SCHAR_EMPTY;
}
template<> signed char TVal<signed char>::ANY()
{
    return SCHAR_ANY;
}
template<> const char* TVal<signed char>::PATTERN_GET()
{
    return  "%hd";
}
template<> const char* TVal<signed char>::PATTERN_SET()
{
    return  "%hd%s";
}

template<> char TVal<char>::EMPTY()
{
    return CHAR_EMPTY;
}
template<> char TVal<char>::ANY()
{
    return CHAR_ANY;
}
template<> const char* TVal<char>::PATTERN_GET()
{
    return  "%c";
}
template<> const char* TVal<char>::PATTERN_SET()
{
    return  "%c%s";
}

template<>
bool TVal<double>::IsAny(int ndx) const
{
    if( approximatelyEqual( static_cast<double*>(ptr)[ndx], ANY() ) )
        return true;
    return false;
}

template<>
bool TVal<double>::IsEmpty(int ndx) const
{
    if( approximatelyEqual( static_cast<double*>(ptr)[ndx], EMPTY()) )
        return true;
    return false;
}

template<>
bool TVal<float>::IsAny(int ndx) const
{
    if( approximatelyEqual( static_cast<float*>(ptr)[ndx], ANY() ) )
        return true;
    return false;
}

template<>
bool TVal<float>::IsEmpty(int ndx) const
{
    if( approximatelyEqual( static_cast<float*>(ptr)[ndx], EMPTY()) )
        return true;
    return false;
}

template<>
string
TVal<double>::GetString(int ndx) const
{
    if( IsEmpty(ndx) )
        return S_EMPTY;
    if( IsAny(ndx) )
        return S_ANY;

    char vbuf[30];	// double is ~15 digit   PATTERN_GET()
    sprintf(vbuf, "%.*lg" , doublePrecision, static_cast<double*>(ptr)[ndx]);

    return vbuf;
}

// explicit instantiation of the templates

//template struct TVal<short>;
//template struct TVal<long>;
//template struct TVal<float>;


/* specific functions definitions
*/

bool
TValFixString::SetString(const char* s, int ndx)
{
    //  if( strlen(s) >= len )
    //  { // cutting extra symbols
    strncpy(&(static_cast<char*>(ptr))[ndx*len], s, len/*-1*/);
    // ptr[ndx*len + len-1] = '\0';
    //  }
    //  else
    //    strcpy(&ptr[ndx*len], s);

    return true;
}

//-----------------------


bool
TValString::SetString(const char* s, int )
{
    int l = static_cast<int>( strlen(s) );
    if( l > size && dynamic )
        Alloc(l);

    strncpy(static_cast<char*>(ptr), s, size);
    if(dynamic)
    static_cast<char*>(ptr)[size] = '\0';
//    else
//    ((char*)ptr)[size-1] = '\0';

    return true;
}

// transfered from v_vals.h

#ifndef _WIN32

/*
template<>
//inline
bool
TVal<unsigned char>::SetString(const char* s, int ndx)
{
    string ss = s;
    ss.strip();
    if( *ss.empty() ||* ss==S_EMPTY )
    {
        ((unsigned char*)ptr)[ndx] = EMPTY();
        return true;
    }

    if( ss == S_ANY )
    {
        ((unsigned char*)ptr)[ndx] = ANY();
        return true;
    }

    vvstr sv(strlen(s)+1);
    unsigned short v;
    if( sscanf(ss.c_str(), "%hu%s", &v, sv.p ) != 1 )
        return false;
    if( v > 255 )
        return false;

    ((unsigned char*)ptr)[ndx] = (unsigned char)v;
    return true;
}
*/

template<>
//inline
bool
TVal<signed char>::SetString(const char* s, int ndx)
{
    string ss = s;
    strip( ss );
    if( /*ss.empty() ||*/ ss==S_EMPTY )
    {
        static_cast<signed char*>(ptr)[ndx] = EMPTY();
        return true;
    }

    if( ss == S_ANY )
    {
        static_cast<signed char*>(ptr)[ndx] = ANY();
        return true;
    }

    auto sv = std::make_shared<char>( ss.length()+3 );
    signed short v;
    if( sscanf(ss.c_str(), "%hd%s", &v, sv.get() ) != 1 )
        return false;
    if( v<-127 || v>127 )
        return false;

    static_cast<signed char*>(ptr)[ndx] = static_cast<signed char>(v);
    return true;
}

//#endif
/*
template<>
inline
string
TVal<double>::GetString(int ndx) const
{
    if( IsEmpty(ndx) )
        return S_EMPTY;
    if( IsAny(ndx) )
        return S_ANY;

    char vbuf[30];	// double is ~15 digit   PATTERN_GET()
    sprintf(vbuf, "%.*lg" , doublePrecision, ((double*)ptr)[ndx]);

    return vbuf;
}*/
#endif

//--------------------- End of v_vals.cpp ---------------------------


