//-------------------------------------------------------------------
// Id: gems/vizor/core/v_user.h  version 2.0.0     2001
//
// Declaration of miscellaneous utility functions and classes
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _v_user_h_
#define _v_user_h_

#include <iostream>
using namespace std;
#include "gstring.h"
#include "array.h"
#include "verror.h"

const int MAXKEYWD = 6+1;

typedef TArrayF<gstring> TCStringArray;

#ifdef __unix
#include <stl_algobase.h>
#else

#define min(x,y) ( (x<y) ? (x) : (y) )
#define max(x,y) ( (x>y) ? (x) : (y) )

typedef unsigned int uint;

#endif // __unix


inline
int ROUND(double x)
{
    return int((x)+.5);
}

// Combines path, directory, name and extension to full pathname
void
u_makepath(gstring& Path, const gstring& dir,
           const gstring& name, const gstring& ext);

// Splits full pathname to path, directory, name and extension
void
u_splitpath(const gstring& Path, gstring& dir,
            gstring& name, gstring& ext);

inline
bool
IsSpace(char ch)
{
    return ( (ch == ' ') || (ch == '\t') );
}

void
StripLine(gstring& line);

// Added Sveta 22/12/2001
// Change string on templates
void
ChangeforTempl( gstring& data_str,  const gstring& from_templ1,
                const gstring& to_templ1, uint len_ );



// Returns string representation of current date in HH:MM format
gstring curDate();

// Returns string representation of current time in dd/mm/yyyy format
gstring curTime();

// Returns string representation of current date and time
inline
gstring curDateTime()
{
    return curDate() + curTime();
}

// dynamically allocated temporary 'char*'
// for simple string manipulations
// (used instead of stack char[] allocation to avoid stack problems)
struct vstr
{
    char* p;
    vstr(int ln): p(new char[ln+1])
    { }

    vstr(int ln, const char* s): p(new char[ln+1])
    {
        strncpy(p, s, ln);
        p[ln]='\0';
    }
    vstr(const char* s): p(new char[strlen(s)+1])
    {
        strcpy(p, s);
    }
    ~vstr()
    {
        delete[] p;
    }
    operator char* ()
    {
        return p;
    }

private:
    vstr (const vstr&);
    const vstr& operator= (const vstr&);
};

// read line to gstring class from istream with delimiter
istream& u_getline(istream& instream, gstring& dst_string, char delimit = '\n');


/*! return pointer after spaces in gstring 's'
*/
/*
inline
const char* fastLeftStrip(const char* s)
{ while(*s==' ') s++; 
  return s;
}  
*/

/*! returns length of fgstring without right blanks
*/
/*
inline
unsigned int lenWithRightStrip(const char* s)
{
  const char* pp = s+strlen(s)-1;
  while(*pp==' ') pp--; 
  return pp-s+1;
}  
*/
#ifdef __FreeBSD

// replacement for missing function in FreeBSD

inline char* gcvt(double num, size_t digit, char* buf)
{
    sprintf(buf, "%*g", digit, num);
    return buf;
}

#endif  // __FreedBSD

#include <math.h>
/*!
    Class FPoint represents point with floating point coordinates as in calculations
*/
struct FPoint
{
    float x,y;

    FPoint()
    {
        x=y=0;
    }
    FPoint(float xx, float yy)
    {
        x = xx, y = yy;
    }
    FPoint(const FPoint& p)
    {
        x = p.x, y = p.y;
    }

    const FPoint& operator=(const FPoint& p)
    {
        x = p.x, y = p.y;
        return *this;
    }
    bool operator==(const FPoint& p)
    {
        return (x == p.x && y == p.y);
    }
};


#endif // _v_user_h_
