//-------------------------------------------------------------------
// $Id: units.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TUnitsList class
//
// Copyright (C) 1996-2001  A.Rysin
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
#ifndef _units_h_
#define _units_h_

#include <fstream>
#include <vector>
#include <string>

//---------------------------------
// units for TCellCheck
//---------------------------------

struct sunits
{
    std::string name;
    std::vector<std::string> vals_list;

    sunits()
    {}

    sunits(const std::string& n, const std::vector<std::string>& v):
            name(n), vals_list(v)
    {}

    std::string getVals(int m) const;
};

class TUnitsList : public std::vector<sunits>
{

public:
    TUnitsList();
    int Find(const std::string& s);
    void toDAT(std::ostream& visor_dat);
    void fromDAT(std::istream& visor_dat);
    void load(const std::string& units_ini);
};

extern TUnitsList aUnits;

#endif	// _units_h_
