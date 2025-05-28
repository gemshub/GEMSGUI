//-------------------------------------------------------------------
// $Id: page_s.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CWinInfo, PageInfo, FieldInfo classes
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

#ifndef __page_s_h
#define __page_s_h

#include "v_object.h"
#include "GEMS3K/jsonconfig.h"

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
    TObject* pObj;
    int nO;
    eFieldType fType;
    int npos;
    bool label;
    ePlaceMode place;
    eEdit edit;
    eShowType showType;
    int maxN, maxM;		// max size of grid w/out scrolls

    explicit FieldInfo( std::istream& is);

    explicit FieldInfo( TObject& rO, int anO,
              eFieldType fT, int np, bool lb,
              ePlaceMode pl, eEdit e, eShowType sT,
              int w = DEF_M_BROWSE,
              int h = DEF_N_BROWSE);
    explicit FieldInfo( int anO, eFieldType fT, int np, bool lb,
               ePlaceMode pl, eEdit e, eShowType sT, int w, int h);

    void toDAT(std::ostream& os);
    void fromDAT(std::istream& os);	// must be protected
};


//===========================================
// TPage class
//===========================================
struct PageInfo
{
    /*const */CWinInfo& rWinInfo;
//    TCPage* pPage;

    std::vector<std::shared_ptr<FieldInfo>> aFieldInfo;
    std::string name;

    static eFieldType GetType(const std::string& s);

    TCWindow& GetWin();

    PageInfo( CWinInfo& wi, std::istream& is);
    PageInfo( CWinInfo& wi, const TJsonConfig& cnf);
    void load(const TJsonConfig& cnf);

    void toDAT(std::ostream& os);
    void fromDAT(std::istream& os);	// must be protected

private:
    PageInfo(const PageInfo&);
    const PageInfo& operator=(const PageInfo&);
};


//===========================================
// CWinInfo class
//===========================================

class TSubModule;

struct CWinInfo
{
    TCWindow* pWin;

    TSubModule& rM;
   std::vector<std::shared_ptr<PageInfo>> aPageInfo;

    int init_width;
    int init_height;

    CWinInfo(TSubModule& r, std::istream& is);
    CWinInfo(TSubModule& r, const TJsonConfig& cnf);
    void load(const TJsonConfig& cnf);

    void toDAT(std::ostream& os);
    void fromDAT(std::istream& is);	// must be protected

    void toWinCFG(std::ostream& os);
    void fromWinCFG(std::istream& is);

private:
    //CWinInfo(const CWinInfo&);
    //const CWinInfo& operator=(const CWinInfo&);
};

inline TCWindow&
PageInfo::GetWin()
{
    return *rWinInfo.pWin;
}


extern std::vector<std::shared_ptr<CWinInfo>> aWinInfo;

#endif  // _page_s_h
