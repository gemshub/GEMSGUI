//-------------------------------------------------------------------
// $Id: page_f.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CWinInfo, PageInfo, FieldInfo classes
// ( Provides stream input/output for  visual elements )
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
#include <fstream>

#include "page_f.h"
#include "units.h"
#include "v_module.h"

std::vector<std::shared_ptr<CWinInfo>> aWinInfo;

CWinInfo::CWinInfo(TSubModule & m, istream & visor_dat):
        pWin(0), rM(m)
{
    init_width = 600; //400
    init_height = 500; //300
    fromDAT(visor_dat);
}

CWinInfo::CWinInfo(TSubModule & m, const TJsonConfig& cnf):
        pWin(0), rM(m)
{
    init_width = 600; //400;
    init_height = 500; //300;
    load(cnf);

}

void CWinInfo::load(const TJsonConfig& cnf)
{
    string sname = rM.GetName();	// section name

    auto module_section = cnf.section(sname);
    if( !module_section ) {
        Error(sname, "Window for module not defined.");
    }

    auto size_section = module_section->section("size");
    if(size_section) {
        init_width = size_section->value_or_default<int>("width", init_width);
        init_height = size_section->value_or_default<int>("height", init_height);
    }

    auto pages_section = module_section->section("pages");
    if(pages_section) {
        auto pages_array = pages_section->to_vector();
        for(const auto& page_cfg: pages_array) {
            aPageInfo.push_back( std::make_shared<PageInfo>(*this, page_cfg));
        }
    }
}

const int lnWINSIG = 2;
const char SigBEG[lnWINSIG + 1] = "Wd";
const char SigEND[lnWINSIG + 1] = "dw";

const char *SigERROR = "Error in visor description file (visor.cfg)";
const char *SigTITLE = "Configurator";

void
CWinInfo::toDAT(ostream & os)
{
    // start signature
    os.write(SigBEG, 2);

    int n = aPageInfo.size();
    os.write((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aPageInfo[ii]->toDAT(os);

    os.write((char *) &init_width, sizeof init_width);
    os.write((char *) &init_height, sizeof init_height);

    // end signature
    os.write(SigEND, 2);
}

void
CWinInfo::fromDAT(istream & is)
{
    // start signature
    char sg[2];
    is.read(sg, sizeof sg);
    if (sg[0] != SigBEG[0] || sg[1] != SigBEG[1])
        throw TError(SigERROR, SigTITLE);

    int n;
    is.read((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aPageInfo.push_back( std::make_shared<PageInfo>(*this, is));

    is.read((char *) &init_width, sizeof init_width);
    is.read((char *) &init_height, sizeof init_height);

    // end signature
    is.read(sg, sizeof sg);
    if (sg[0] != SigEND[0] || sg[1] != SigEND[1])
        throw TError(SigERROR, SigTITLE);
}

void
CWinInfo::toWinCFG(ostream & win_cfg)
{
    win_cfg << "[" << rM.GetName() << "]" << endl; 
//    win_cfg << "init_wdith\t=\t" << init_width << endl;
//    win_cfg << "init_height\t=\t" << init_height << endl;
    win_cfg << init_width << endl;
    win_cfg << init_height << endl;
}

void
CWinInfo::fromWinCFG(istream & win_cfg)
{
//    win_cfg.read((char *) &init_width, sizeof init_width);
//    win_cfg.read((char *) &init_height, sizeof init_height);
    char name[100];
    win_cfg >> name;	// Don't compile in BCB without .p
    win_cfg >> init_width;
    win_cfg >> init_height;
}

//----------------------------------------------------------------
// TCPage
//----------------------------------------------------------------

PageInfo::PageInfo( CWinInfo & wi, istream & is):
        rWinInfo(wi)/*, pPage(0)*/
{
    fromDAT(is);
}


PageInfo::PageInfo( CWinInfo & wi,const TJsonConfig& cnf):
        rWinInfo(wi)/*, pPage(0), name(s) */
{
    load(cnf);
}

void PageInfo::load(const TJsonConfig& cnf )
{
    name = cnf.value_or_default<std::string>("page", "page");

    string obj;
    string mode;
    eFieldType type;
    int npos;
    int maxM, maxN;

    auto tables_section = cnf.section("fields");
    if(tables_section) {
        auto objects_array = tables_section->to_vector();
        for(const auto& obj_line: objects_array) {

            obj = obj_line.value_must_exist<std::string>("label");

            type = GetType(obj_line.value_must_exist<std::string>("type"));
            if (type == ftUndefined)
                throw TError(obj.c_str(), "Bad field type");

            if (type != ftCheckBox) {
                npos = obj_line.value_must_exist<int>("lenth");
            }
            else {
                auto units_name = obj_line.value_must_exist<std::string>("lenth");
                if( (npos = aUnits.Find(units_name)) == -1)
                    throw TError(units_name, "Bad checkbox description");
            }

            maxM = max( obj_line.value_or_default<int>("maxM", DEF_M_BROWSE), DEF_M_BROWSE );
            maxN = max( obj_line.value_or_default<int>("maxN", DEF_N_BROWSE), DEF_N_BROWSE );
            mode = obj_line.value_must_exist<std::string>("locus");

            int ind = aObj.Find(obj.c_str());
            if (ind == -1)
                throw TError(obj.c_str(), "Object name not found");
            else  {
                TObject& rO = *aObj[ind];

                bool label = mode[0] == '+';
                eEdit edit = eEdit(mode[2]);
                if (edit != '+' && edit != '-' && edit != '?')
                    throw TError(obj.c_str(), "Bad edit type");

                ePlaceMode place = ePlaceMode(mode[1]);
                eShowType showT = eShowType(mode[3]);
                aFieldInfo.push_back( std::make_shared<FieldInfo>( rO, ind, type, npos,
                                                                   label, place, edit, showT, maxM, maxN));
            }
        }
    }
}

const char PSigBEG[lnWINSIG + 1] = "Pd";
const char PSigEND[lnWINSIG + 1] = "dp";

void
PageInfo::toDAT(ostream & os)
{
    // begin signature
    os.write(PSigBEG, 2);
    int l = name.length() + 1;	// writing ending '\0'
    os.write((char *) &l, sizeof l);
    os.write(name.c_str(), l);
    int n = aFieldInfo.size();
    os.write((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aFieldInfo[ii]->toDAT(os);
    // end signature
    os.write(PSigEND, 2);
}


void
PageInfo::fromDAT(istream & is)
{
    char sg[2];
    is.read(sg, sizeof sg);
    if (sg[0] != PSigBEG[0] || sg[1] != PSigBEG[1])
        throw TError(SigERROR, "BegPage");

    int l;
    is.read((char *) &l, sizeof l);
    char *s = new char[l];
    is.read(s, l);
    name = s;
    delete[]s;

    int n;
    is.read((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aFieldInfo.push_back( std::make_shared<FieldInfo>(is));

    is.read(sg, sizeof sg);
    if (sg[0] != PSigEND[0] || sg[1] != PSigEND[1])
        throw TError(SigERROR, "EndPage");
}

eFieldType
PageInfo::GetType(const string & s)
{
    const char *snames[nFieldTypes] =
        { "F_NUM", "F_FLOAT", "F_STRING", "F_TEXT",
          "F_CHECKBOX", "F_REF", "F_RECORD"
        };

    if (s == "F_VALUE")
        return ftFloat;

    for (int ii = 0; ii < nFieldTypes; ii++)
        if (s == snames[ii])
            return eFieldType(ii + 1);

    return ftUndefined;
}



FieldInfo::FieldInfo( TObject & rO, int anO,
                     eFieldType fT, int np, bool lb,
                     ePlaceMode pl, eEdit e, eShowType sT, int w, int h):
        pObj(&rO), nO(anO), fType(fT), npos(np),
        label(lb), place(pl), edit(e), showType(sT), maxN(h), maxM(w)
{}

FieldInfo::FieldInfo( int anO, eFieldType fT, int np, bool lb,
           ePlaceMode pl, eEdit e, eShowType sT, int w, int h):
        pObj( anO>=0 ?  aObj[anO].get(): nullptr ), nO(anO),
        fType(fT), npos(np),
        label(lb), place(pl), edit(e), showType(sT), maxN(h), maxM(w)
{ }


FieldInfo::FieldInfo( istream & is )
{
    fromDAT(is);
}

const char FSigBEG[lnWINSIG + 1] = "Fd";
const char FSigEND[lnWINSIG + 1] = "df";

void
FieldInfo::toDAT(ostream & os)
{
    //  int n = aFieldInfo.GetCount();
    // begin signature
    os.write(FSigBEG, 2);

/*qt3to4    unsigned ii = 0;
    for (; ii < aObj.GetCount(); ii++)
        if (pObj == &aObj[ii])
            break;
    if (ii == aObj.GetCount())
        throw TError("Bad object pointer", "TField::toDAT()");
    os.write((char *) &ii, sizeof ii);
*/    os.write((char *) &nO, sizeof nO);
    os.write((char *) &fType, sizeof fType);
    os.write((char *) &npos, sizeof npos);
    os.write((char *) &label, sizeof label);
    os.write((char *) &place, sizeof place);
    os.write((char *) &edit, sizeof edit);
    os.write((char *) &showType, sizeof showType);
    os.write((char *) &maxM, sizeof maxM);
    os.write((char *) &maxN, sizeof maxN);

    // end signature
    os.write(FSigEND, 2);
}


void
FieldInfo::fromDAT(istream & is)
{
    char sg[2];
    is.read(sg, sizeof sg);
    if (sg[0] != FSigBEG[0] || sg[1] != FSigBEG[1])
        throw TError(SigERROR, "BegField");

//    int n;
    is.read((char *) &nO, sizeof nO);
    is.read((char *) &fType, sizeof fType);
    is.read((char *) &npos, sizeof npos);
    is.read((char *) &label, sizeof label);
    is.read((char *) &place, sizeof place);
    is.read((char *) &edit, sizeof edit);
    is.read((char *) &showType, sizeof showType);
    is.read((char *) &maxM, sizeof maxM);
    is.read((char *) &maxN, sizeof maxN);

    is.read(sg, sizeof sg);
    if (sg[0] != FSigEND[0] || sg[1] != FSigEND[1])
        throw TError(SigERROR, "EndField");

    pObj = aObj[nO].get();
}
//--------------------- End of page_f.cpp ---------------------------

