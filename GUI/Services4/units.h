//-------------------------------------------------------------------
// $Id: units.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TUnitsList class
//
// Copyright (C) 1996-2001  A.Rysin
// Uses  string class (C) A.Rysin 1999
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
#ifndef _units_h_
#define _units_h_

#include <fstream>
#include <vector>
#include "verror.h"
//---------------------------------
// units for TCellCheck
//---------------------------------

struct sunits
{
    string name;
    string vals;

    sunits()
    {}

    sunits(const string & n, const string & v):
            name(n), vals(v)
    {}


    string getVals(int m) const;
};

class TUnitsList : public vector<sunits>
{

public:
    TUnitsList();
    int Find(const char* s)
    {
        return Find(string(s));
    }
    int Find(const string & s);

    void toDAT(ostream & visor_dat);
    void fromDAT(istream & visor_dat);
    void load(const char* units_ini);
};

extern TUnitsList aUnits;

#endif	// _units_h_
