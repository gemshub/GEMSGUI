//-------------------------------------------------------------------
// $Id$
//
// Implementation of TValBase, TVal and its templates
//
// Copyright (C) 1996-2001 A.Rysin
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


#include <iostream>
#include <stdio.h>
#include <math.h>

#include "v_user.h"
#include "v_vals.h"
#include "v_vals_impl.h"

int TValBase::doublePrecision = 8;

const char* S_EMPTY	=    "`";
const char* S_ANY  	=    "*";

/* defining values EMPTY and ANY for different types
    Note: for some reason static constants in template classes
    did not go well for Borland C++, we must use functions members
*/

template<> const short TVal<short>::EMPTY()
{
    return  SHORT_EMPTY;
}
template<> const short TVal<short>::ANY()
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

template<> const unsigned short TVal<unsigned short>::EMPTY()
{
    return USHORT_EMPTY;
}
template<> const unsigned short TVal<unsigned short>::ANY()
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

template<> const long TVal<long>::EMPTY()
{
    return LONG_EMPTY;
}
template<> const long TVal<long>::ANY()
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

template<> const unsigned long TVal<unsigned long>::EMPTY()
{
    return ULONG_EMPTY;
}
template<> const unsigned long TVal<unsigned long>::ANY()
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

template<> const float TVal<float>::EMPTY()
{
    return FLOAT_EMPTY;
}
template<> const float TVal<float>::ANY()
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

template<> const double TVal<double>::EMPTY()
{
    return DOUBLE_EMPTY;
}
template<> const double TVal<double>::ANY()
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

template<> const unsigned char TVal<unsigned char>::EMPTY()
{
    return UCHAR_EMPTY;
}
template<> const unsigned char TVal<unsigned char>::ANY()
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

template<> const signed char TVal<signed char>::EMPTY()
{
    return SCHAR_EMPTY;
}
template<> const signed char TVal<signed char>::ANY()
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

template<> const char TVal<char>::EMPTY()
{
    return CHAR_EMPTY;
}
template<> const char TVal<char>::ANY()
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

// explicit instantiation of the templates

template struct TVal<short>;
//template struct TVal<unsigned short>;
template struct TVal<long>;
//template struct TVal<unsigned long>;
template struct TVal<float>;
//template class TVal<double>;


/* specific functions definitions
*/

bool
TValFixString::SetString(const char* s, size_t ndx)
{
    //  if( strlen(s) >= len )
    //  { // cutting extra symbols
    strncpy(&((char*)ptr)[ndx*len], s, len/*-1*/);
    // ptr[ndx*len + len-1] = '\0';
    //  }
    //  else
    //    strcpy(&ptr[ndx*len], s);

    return true;
}

//-----------------------


bool
TValString::SetString(const char* s, size_t )
{
    size_t l = strlen(s);
    if( l > size && dynamic )
        Alloc(l);

    strncpy((char*)ptr, s, size);
    if(dynamic)
    ((char*)ptr)[size] = '\0';
//    else
//    ((char*)ptr)[size-1] = '\0';

    return true;
}

// transfered from v_vals.h

#ifdef __unix


template<>
//inline
bool
TVal<unsigned char>::SetString(const char* s, size_t ndx)
{
    gstring ss = s;
    ss.strip();
    if( /*ss.empty() ||*/ ss==S_EMPTY )
    {
        ((unsigned char*)ptr)[ndx] = EMPTY();
        return true;
    }

    if( ss == S_ANY )
    {
        ((unsigned char*)ptr)[ndx] = ANY();
        return true;
    }

    vstr sv(strlen(s)+1);
    unsigned short v;
    if( sscanf(ss.c_str(), "%hu%s", &v, sv.p ) != 1 )
        return false;
    if( v > 255 )
        return false;

    ((unsigned char*)ptr)[ndx] = (unsigned char)v;
    return true;
}


template<>
//inline
bool
TVal<signed char>::SetString(const char* s, size_t ndx)
{
    gstring ss = s;
    ss.strip();
    if( /*ss.empty() ||*/ ss==S_EMPTY )
    {
        ((signed char*)ptr)[ndx] = EMPTY();
        return true;
    }

    if( ss == S_ANY )
    {
        ((signed char*)ptr)[ndx] = ANY();
        return true;
    }

    vstr sv(strlen(s)+1);
    signed short v;
    if( sscanf(ss.c_str(), "%hd%s", &v, sv.p ) != 1 )
        return false;
    if( v<-127 || v>127 )
        return false;

    ((signed char*)ptr)[ndx] = (signed char)v;
    return true;
}

//#endif
/*
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
}*/
#endif

//--------------------- End of v_vals.cpp ---------------------------


