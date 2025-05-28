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

#include "v_user.h"
#include "units.h"
#include "GEMS3K/jsonconfig.h"


//=============================================
// Units List ( for TCellCheck )
//=============================================

const char *defVALS = "+-* `";

std::string sunits::getVals(int m) const
{
    if(vals_list.size()<1)	// empty
        return  defVALS;
    if(vals_list.size()==1)
        return vals_list[0];
    size_t pos = std::min<size_t>(m, vals_list.size()-1);
    return vals_list[pos]+" `";
}

TUnitsList::TUnitsList()
{}

const int lnUSIG = 2;
const char USigBEG[lnUSIG + 1] = "Us";
const char USigEND[lnUSIG + 1] = "uX";

const char *USigERROR = "Error in visor description (units)";
const char *USigTITLE = "Configurator";
//,"Config error"


void TUnitsList::toDAT(std::ostream& visor_dat)
{
    // begin signature
    visor_dat << USigBEG;

    int n1 = size();
    visor_dat.write((char *) &n1, sizeof n1);
    for (int ii = 0; ii < n1; ii++)
    {
        std::string vals;
        std::for_each(at(ii).vals_list.begin(), at(ii).vals_list.end(),
                      [&](const std::string &el){ vals += "/"+el; });
        vals = vals.substr(1);

        int n = at(ii).name.length() + 1;
        visor_dat.write((char *) &n, sizeof n);
        visor_dat.write(at(ii).name.c_str(), n);
        n = vals.length() + 1;
        visor_dat.write((char *) &n, sizeof n);
        visor_dat.write(vals.c_str(), n);
    }

    // end signature
    visor_dat << USigEND;
}

void TUnitsList::fromDAT(std::istream& visor_dat)
{
    char sg[2];
    visor_dat.read(sg, sizeof sg);
    if (sg[0] != USigBEG[0] || sg[1] != USigBEG[1])
        throw TError(USigERROR, USigTITLE);

    char nm[100];
    char vl[100];
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
        auto units_list = split(vl, "/");
        push_back(sunits(std::string (nm), units_list));
    }

    visor_dat.read(sg, sizeof sg);
    if (sg[0] != USigEND[0] || sg[1] != USigEND[1])
        throw TError(USigERROR, USigTITLE);
}


void TUnitsList::load(const std::string& f_units)
{
    std::string label;
    std::vector<std::string> vals_list;

    TJsonConfig cnf1(f_units+".json");
    auto units_array = cnf1.to_vector();

    for(const auto& units_ini: units_array)
    {
        label = units_ini.value_must_exist<std::string>("label");
//        auto vals_section = units_ini.section("values");
//        vals_list.clear();
//        if(vals_section)
//        {
//            auto sections = vals_section->to_vector();
//            for(const auto& block: sections)
//                vals_list.push_back( block.get_as<std::string>() );
//        }
        vals_list = units_ini.value_or_default<std::vector<std::string>>("values", {"+-"});
        push_back(sunits(label, vals_list));
    }
}

int TUnitsList::Find(const std::string &s)
{
    for (size_t ii = 0; ii < size(); ii++)
        if (at(ii).name == s)
            return ii;
    return -1;
}

TUnitsList aUnits;

//--------------------- End of units.cpp ---------------------------

