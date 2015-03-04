//-------------------------------------------------------------------
// $Id: page_s.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CWinInfo, PageInfo, FieldInfo classes
// ( Provides stream input/output for  visual elements )
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

#ifndef __page_s_h
#define __page_s_h

#include <iostream>
#include "v_object.h"
#include "config.h"

class TCPage;
class TField;
class TCWindow;

struct CWinInfo;
struct PageInfo;
struct FieldInfo;

enum eShowType { stResult='r', stIO='i', stWork='o', stAux='w', stHelp='h' };

enum eFieldType { ftNumeric=1, ftFloat, ftString, ftText,
                  ftCheckBox, ftRef, ftRecord, ftUndefined=0 };
const int nFieldTypes=7;

enum eEdit { eYes='+', eNo='-', eParam='?' };

enum ePlaceMode { First='*', Fixed='+',
                  Right='r', Down='d',
                  NextP='n', SkipLine='b',
                  Tied = 't', Sticked = 's',
                  Under='u',  UndeTabl = 'l',
                  MutableB = 'm' };



//===========================================
// TField class
//===========================================

const int DEF_M_BROWSE = 6;
const int DEF_N_BROWSE = 6;


struct FieldInfo
{
//    const PageInfo& rPageInfo;
//    TField* pField;
    TObject* pObj;
    int nO;
    eFieldType fType;
    int npos;
    bool label;
    ePlaceMode place;
    eEdit edit;
    eShowType showType;
    int maxN, maxM;		// max size of grid w/out scrolls

    FieldInfo( istream& is);

    FieldInfo( TObject& rO, int anO,
              eFieldType fT, int np, bool lb,
              ePlaceMode pl, eEdit e, eShowType sT,
              int w = DEF_M_BROWSE,
              int h = DEF_N_BROWSE);
    FieldInfo( int anO, eFieldType fT, int np, bool lb,
               ePlaceMode pl, eEdit e, eShowType sT, int w, int h);

    void toDAT(ostream& os);
    void fromDAT(istream& os);	// must be protected

    FieldInfo(const FieldInfo& d):
        pObj(d.pObj), nO(d.nO),
        fType(d.fType), npos(d.npos), label(d.label),
        place(d.place), edit(d.edit), showType(d.showType),
        maxN(d.maxN), maxM(d.maxM)
    {}

    const FieldInfo& operator=(const FieldInfo& d)
    {
        pObj = d.pObj;
        nO = d.nO;
        fType = d.fType;
        npos = d.npos;
        label = d.label;
        place = d.place;
        edit = d.edit;
        showType = d.showType;
        maxN = d.maxN;
        maxM = d.maxM;

    }

};

//===========================================
// TPage class
//===========================================
struct PageInfo
{
    /*const */CWinInfo* rWinInfo;

    vector<FieldInfo> aFieldInfo;
    string name;

    static eFieldType GetType(const string& s);

    TCWindow& GetWin();

    PageInfo(/*const */CWinInfo* wi, istream& is);
    PageInfo(/*const */CWinInfo* wi, TConfig& cnf, string name);
    void load(TConfig& c);

    void toDAT(ostream& os);
    void fromDAT(istream& os);	// must be protected

private:

    //PageInfo(const PageInfo& d);
   // const PageInfo& operator=(const PageInfo& d);
};



//===========================================
// CWinInfo class
//===========================================

class TSubModule;

struct CWinInfo
{
    TCWindow* pWin;

    TSubModule& rM;
    vector<PageInfo> aPageInfo;

    int init_width;
    int init_height;

    CWinInfo(TSubModule& r, istream& is);
    CWinInfo(TSubModule& r, TConfig& cnf);
    void load(TConfig& c);

    void toDAT(ostream& os);
    void fromDAT(istream& is);	// must be protected

    void toWinCFG(ostream& os);
    void fromWinCFG(istream& is);

private:
    //CWinInfo(const CWinInfo&);
    //const CWinInfo& operator=(const CWinInfo&);
};

inline TCWindow& PageInfo::GetWin()
{
    return *(rWinInfo->pWin);
}


extern vector<CWinInfo *> aWinInfo;

#endif  // _page_s_h
