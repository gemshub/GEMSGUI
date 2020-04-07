//-------------------------------------------------------------------
// $Id: v_vals.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TValBase class and value constants
//
// Copyright (C) 1996-2001 A.Rysin
// Uses  gstring class (C) A.Rysin 1999
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

#ifndef _v_vals_h_
#define _v_vals_h_

#include "gstring.h"

class GemDataStream;

/* TValBase class represents interface for values use in calculating modules
    this interface used by DB, GUI and calculating modules
    TValBase-inherited classes are used only by TObject

    All cleanups and pointer checking
    are performed by TObject class
    for better performance
*/

struct TValBase
{
    static int doublePrecision;	// precision of double, set by VisorImp

    bool dynamic;	// object is dynamic
    void* ptr;		// pointer to the value 

    TValBase(bool d) : dynamic(d), ptr(nullptr)
    {}

    virtual ~TValBase();

    void SetPtr(void* p)
    {
        ptr = p;
    }
    void* GetPtr() const
    {
        return ptr;
    }

    virtual void* Alloc(int sz) = 0;
    virtual int cSize() const = 0;

    virtual double Get(int i=0) const = 0;
    virtual void Put(double v, int i) = 0;

    virtual bool IsAny(int ndx) const = 0;
    virtual bool IsEmpty(int ndx) const = 0;

    virtual gstring GetString(int ndx) const = 0;
    //  virtual bool VerifyString(const char* s)=0;
    virtual bool SetString(const char* s, int ndx) = 0;

    virtual void write(GemDataStream& s, int size) = 0;
    virtual void read(GemDataStream& s, int size) = 0;

private:
// forbidding copying and assigning - it's dangerous!
    TValBase (const TValBase&);
    const TValBase& operator= (const TValBase&);
};

extern const char* S_EMPTY;
extern const char* S_ANY;

/*
    This constants should be 'defined' to satisfy all compilers
*/

#define SHORT_EMPTY 	   -32768
#define SHORT_ANY   	    32767
#define USHORT_EMPTY         0
#define USHORT_ANY           65535
#define LONG_EMPTY             -2147483648L //Int 32 -2,147,483,648 / 2,147,483,647
#define LONG_ANY               2147483647L
#define ULONG_EMPTY   0UL
#define ULONG_ANY     4294967295UL
#define FLOAT_EMPTY	          1.17549435e-38F
#define FLOAT_ANY             3.40282347e+38F
#define DOUBLE_EMPTY         2.2250738585072014e-308
#define DOUBLE_ANY           1.7976931348623157e+308
#define UCHAR_EMPTY   0
#define UCHAR_ANY     0xFF
#define SCHAR_EMPTY     -127
#define SCHAR_ANY       127
#define CHAR_EMPTY   	     '`'
#define CHAR_ANY       	     '*'

inline bool IsFloatEmpty( const float v )
{
    return ( v>0.f && v <= FLOAT_EMPTY);
}
inline bool IsDoubleEmpty( const double v )
{
    return ( v>0. && v <= DOUBLE_EMPTY);
}

#endif //_v_vals_h_
