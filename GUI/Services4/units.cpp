//-------------------------------------------------------------------
// $Id: units.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TUnitsList class
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

#include <cstdio>
#include <iostream>

#include "v_user.h"
#include "units.h"
#include "config.h"


//=============================================
// Units List ( for TCellCheck )
//=============================================

const char *defVALS = "+-* `";


string
sunits::getVals(int m) const
{
    if (vals.find('/') == string::npos)	// not splitted
        return vals;

    size_t pos = 0, p = 0;
    ;
    for (int ii = 0; ii < m; ii++)
    {
        pos = vals.find('/', pos);
        if (pos == string::npos)
            //      return defVALS;
            return (string (vals, p, string::npos) + " `");	// the last defined vals
        p = ++pos;
    }

    size_t sp = vals.find('/', pos);
    if (sp != string::npos)
        sp = sp - pos;
    // else the rest of the string
    return (string (vals, pos, sp) + " `");
}

TUnitsList::TUnitsList()
//  std::vector<sunits>
{}


const int lnUSIG = 2;
const char USigBEG[lnUSIG + 1] = "Us";
const char USigEND[lnUSIG + 1] = "uX";

const char *USigERROR = "Error in visor description (units)";
const char *USigTITLE = "Configurator";
//,"Config error"


void
TUnitsList::toDAT(ostream& visor_dat)
{
    // begin signature
    visor_dat << USigBEG;

    int n1 = size();
    visor_dat.write((char *) &n1, sizeof n1);
    for (int ii = 0; ii < n1; ii++)
    {
        int n = at(ii).name.length() + 1;
        visor_dat.write((char *) &n, sizeof n);
        visor_dat.write(at(ii).name.c_str(), n);
        n = at(ii).vals.length() + 1;
        visor_dat.write((char *) &n, sizeof n);
        visor_dat.write(at(ii).vals.c_str(), n);
    }

    // end signature
    visor_dat << USigEND;
}

void
TUnitsList::fromDAT(istream& visor_dat)
{
    char sg[2];
    visor_dat.read(sg, sizeof sg);
    if (sg[0] != USigBEG[0] || sg[1] != USigBEG[1])
        throw TError(USigERROR, USigTITLE);

    vstr nm(100);
    vstr vl(100);
    int n1;
    visor_dat.read((char *) &n1, sizeof n1);

    clear();
    for (int ii = 0; ii < n1; ii++)
    {
        int n;
        visor_dat.read((char *) &n, sizeof n);
        visor_dat.read(nm, n);
        visor_dat.read((char *) &n, sizeof n);
        visor_dat.read(vl, n);
        push_back(sunits(string (nm), string (vl)));
        ;
    }

    visor_dat.read(sg, sizeof sg);
    if (sg[0] != USigEND[0] || sg[1] != USigEND[1])
        throw TError(USigERROR, USigTITLE);
}


void
TUnitsList::load(const char *f_units)
{
    TConfig cnf(f_units, ' ');
    string par;
    string str;

    par = cnf.getFirst();

    while (!par.empty())
    {
        //cout << par << "  " << str << endl;
        cnf.getcStr(str);
        //    str += " `";

        push_back(sunits(par, str));
        par = cnf.getNext();
    }
}

int
TUnitsList::Find(const string &s)
{
    for (size_t ii = 0; ii < size(); ii++)
        if (at(ii).name == s)
            return ii;

    return -1;
}

TUnitsList aUnits;

//--------------------- End of units.cpp ---------------------------

