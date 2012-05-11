//-------------------------------------------------------------------
// $Id: page_f.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CWinInfo, PageInfo, FieldInfo classes
// ( Provides stream input/output for  visual elements )
//
// Copyright (C) 1996-2001  A.Rysin
// Uses  gstring class (C) A.Rysin 1999
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

#include <cstdio>
#include <fstream>

#include "page_f.h"
#include "units.h"
#include "v_module.h"

TIArray < CWinInfo > aWinInfo;

CWinInfo::CWinInfo(TSubModule & m, istream & visor_dat):
        pWin(0), rM(m)
{
    init_width = 600; //400
    init_height = 500; //300
    fromDAT(visor_dat);
}

CWinInfo::CWinInfo(TSubModule & m, TConfig & cnf):
        pWin(0), rM(m)
{
    init_width = 600; //400;
    init_height = 500; //300;
    load(cnf);

}

void
CWinInfo::load(TConfig & cnf)
{
    gstring sname = rM.GetName();	// section name

    cnf.SetSection(sname);

    gstring size_params[2];
    if (cnf.getFirst() == "size")
    {
        cnf.getcStrings(2, size_params);

        if (sscanf(size_params[0].c_str(), "%u", &init_width) != 1
                || sscanf(size_params[1].c_str(), "%u", &init_height) != 1)
            Error("Visor configuration", "Window size is bad!");
    }

    gstring ss = cnf.GetFirstSubSection();
    while (!ss.empty())
    {
        aPageInfo.Add(new PageInfo(*this, cnf, ss));
        ss = cnf.GetNextSubSection();
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

    int n = aPageInfo.GetCount();
    os.write((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aPageInfo[ii].toDAT(os);

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
        aPageInfo.Add(new PageInfo(*this, is));

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
    vstr name(100);
    win_cfg >> name.p;	// Don't compile in BCB without .p 
    win_cfg >> init_width;
    win_cfg >> init_height;
}

//----------------------------------------------------------------
// TCPage
//----------------------------------------------------------------

PageInfo::PageInfo(const CWinInfo & wi, istream & is):
        rWinInfo(wi)/*, pPage(0)*/
{
    fromDAT(is);
}


PageInfo::PageInfo(const CWinInfo & wi, TConfig & cnf, gstring s):
        rWinInfo(wi), /*pPage(0),*/ name(s)
{
    load(cnf);
}

void
PageInfo::load(TConfig & cnf)
{
    // object type len p1 p2 abcd
    gstring obj;
    gstring mode;
    int npos;

    gstring astr[5];

    obj = cnf.getFirst();
    while (!obj.empty())
    {
        cnf.getcStrings(5, astr);

        gstring field = astr[0];
        eFieldType type = GetType(field);

        if (type == ftUndefined)
            throw TError(obj.c_str(), "Bad field type");

        if (type != ftCheckBox)
        {
            if ( /*type!=ftText && */ sscanf(astr[1].c_str(), "%u", &npos) != 1)
                throw TError(obj.c_str(), "Bad field lenth");
        }
        else if ((npos = aUnits.Find(astr[1])) == -1)
            throw TError(astr[1].c_str(), "Bad checkbox description");

        int maxm, maxn;
        int maxM = DEF_M_BROWSE, maxN = DEF_N_BROWSE;
        if (sscanf(astr[2].c_str(), "%u", &maxm) != 1)
            throw TError(obj.c_str(), "Bad field width");
        if (maxm > DEF_M_BROWSE)
            maxM = maxm;
        if (sscanf(astr[3].c_str(), "%d", &maxn) != 1)
            throw TError(obj.c_str(), "Bad field heigth");
        if (maxn > DEF_N_BROWSE)
            maxN = maxn;


        mode = astr[4];

        //    int p1, p2;
        //    sscanf( astr[2].c_str(), "%d", &p1);
        //    sscanf( astr[3].c_str(), "%u", &p2);

        int ind = aObj.Find(obj.c_str());
        if (ind == -1)
            throw TError(obj.c_str(), "Object name not found");
        else
        {
            TObject & rO = aObj[ind];

            bool label = mode[0] == '+';
            eEdit edit = eEdit(mode[2]);
            if (edit != '+' && edit != '-' && edit != '?')
                throw TError(obj.c_str(), "Bad edit type");

            ePlaceMode place = ePlaceMode(mode[1]);
            eShowType showT = eShowType(mode[3]);
            aFieldInfo.Add(new FieldInfo(*this, rO, ind, type, npos,
                                         label, place, edit, showT, maxM,
                                         maxN));
        }
        obj = cnf.getNext();
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
    int n = aFieldInfo.GetCount();
    os.write((char *) &n, sizeof n);
    for (int ii = 0; ii < n; ii++)
        aFieldInfo[ii].toDAT(os);
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
        aFieldInfo.Add(new FieldInfo(*this, is));

    is.read(sg, sizeof sg);
    if (sg[0] != PSigEND[0] || sg[1] != PSigEND[1])
        throw TError(SigERROR, "EndPage");
}

eFieldType
PageInfo::GetType(const gstring & s)
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



FieldInfo::FieldInfo(const PageInfo & pi, TObject & rO, int anO,
                     eFieldType fT, int np, bool lb,
                     ePlaceMode pl, eEdit e, eShowType sT, int w, int h):
        rPageInfo(pi),
        /*pField(0),*/ pObj(&rO), nO(anO),
        fType(fT), npos(np),
        label(lb), place(pl), edit(e), showType(sT), maxN(h), maxM(w)
{
}

FieldInfo::FieldInfo( int anO, eFieldType fT, int np, bool lb,
           ePlaceMode pl, eEdit e, eShowType sT, int w, int h):
        rPageInfo(aWinInfo[0].aPageInfo[0]), // for internal using in TreeList 
        pObj(&aObj[anO]), nO(anO),
        fType(fT), npos(np),
        label(lb), place(pl), edit(e), showType(sT), maxN(h), maxM(w)
{
}


FieldInfo::FieldInfo(const PageInfo & pi, istream & is):
        rPageInfo(pi)/*, pField(0)*/
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

    pObj = &aObj[nO];
}
//--------------------- End of page_f.cpp ---------------------------

