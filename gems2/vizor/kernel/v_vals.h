//-------------------------------------------------------------------
// $Id$
//
// Declaration of TValBase class and value constants
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

    TValBase(bool d) : dynamic(d), ptr(0)
    {}

    virtual ~TValBase()
    {}

    void SetPtr(void* p)
    {
        ptr = p;
    }
    void* GetPtr() const
    {
        return ptr;
    }

    virtual void* Alloc(size_t sz) = 0;
    virtual size_t cSize() const = 0;

    virtual double Get(size_t i=0) const = 0;
    virtual void Put(double v, size_t i) = 0;

    virtual bool IsAny(size_t ndx) const = 0;
    virtual bool IsEmpty(size_t ndx) const = 0;

    virtual gstring GetString(size_t ndx) const = 0;
    //  virtual bool VerifyString(const char* s)=0;
    virtual bool SetString(const char* s, size_t ndx) = 0;

    virtual void write(GemDataStream& s, size_t size) = 0;
    virtual void read(GemDataStream& s, size_t size) = 0;

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
#define LONG_EMPTY             -2147483648L
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

#endif //_v_vals_h_
