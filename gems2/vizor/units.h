//-------------------------------------------------------------------
// $Id$
//
// Declaration of TUnitsList class
//
// Copyright (C) 1996-2001  A.Rysin
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
#ifndef _units_h_
#define _units_h_

#include <fstream>
using namespace std;

#include "array.h"
#include "gstring.h"


//---------------------------------
// units for TCellCheck
//---------------------------------

struct sunits
{
    gstring name;
    gstring vals;

    sunits()
    {}

    sunits(const gstring & n, const gstring & v):
            name(n), vals(v)
    {}


    gstring getVals(int m) const;
};

class TUnitsList : public TOArray<sunits>
{

public:
    TUnitsList();
    int Find(const char* s)
    {
        return Find(gstring(s));
    }
    int Find(const gstring & s);

    void toDAT(ostream & visor_dat);
    void fromDAT(istream & visor_dat);
    void load(const char* units_ini);
};

extern TUnitsList aUnits;

#endif	// _units_h_
