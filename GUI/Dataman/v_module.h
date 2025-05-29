//-------------------------------------------------------------------
// $Id: v_module.h 1391 2009-08-10 12:58:16Z gems $
//
// Declaration of TSubModule, TModule and TModList classes
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
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
#ifndef _v_module_h_
#define _v_module_h_

#include <set>
#include "v_mod.h"
#include "v_dbm.h"

class TCModuleImp;
class QWidget;
struct GraphData;
namespace jsonui {
class ChartData;
}

// --- Parametres of function GetKeyofRecord()
const int KEY_OLD = 1;
const int KEY_NEW = 2;
const int KEY_TEMP = 3;
const int KEY_NEW_SAVEAS = 4;

/// --- Parametres of function RecBuild()
const int VF_UNDEF =   0;
const int VF_BYPASS =  6;
const int VF_REMAKE =  7;
const int VF_CLEARALL = 2;

/// Default logger for gems3gui part
extern std::shared_ptr<spdlog::logger> gui_logger;

/*
    TCModule class represents full-featured modules of the program
    and is the derived from TSubModule class
    'aMod' is the list of all the modules and submodules in the system
    TModuleImp class is visual representation (window) for TCModule
*/
class TCModule
{

public:
    TCModule(size_t nrt);
    virtual ~TCModule();

    virtual bool IsSubModule()
    {
        return nRT>=MD_RMULTS;
    }
    size_t rtNum() const
    {
        return nRT;
    }

    /// Mark changes in the module
    virtual void CellChanged(bool val = true)
    {
        contents_changed = val;
    }
    /// Test changes in the module
    virtual bool isCellChanged() const
    {
        return contents_changed;
    }

    void SetIcon(const char* icon_file )
    {
        icon_file_path = icon_file;
    }
    const std::string& GetIcon() const
    {
        return icon_file_path;
    }

    void SetTitle()
    {
        set_string( start_title );
    }

    size_t keyEditField();
    void  setKeyEditField(size_t fld)
    {
        start_key_field_edit = fld;
    }

    const std::string& getFilter()
    {
        if(testKeyFilter()) {
            Filter = makeKeyFilter();
        }
        return Filter;
    }
    void setFilter(const std::string& filt)
    {
        Filter = filt;
    }

    virtual const std::string& GetString()
    {
        return state_line;
    }
    virtual const char* GetName() const = 0;
    virtual const char* GetHtml();

    virtual void ods_link(int i=0) = 0;   // link values to objects
    virtual void dyn_set(int i=0) = 0;    // set dynamic objects ptr to values
    virtual void dyn_kill(int i=0) = 0;    // free dynamic memory in objects and values
    virtual void dyn_new(int i=0) = 0;    // realloc dynamic memory
    virtual void set_def(int /*i*/=0)
    {}  // set default data or zero if nessasary
    /// this function is called when active page of submodule is changed
    virtual void EvPageChanged(int /*nPage*/)
    {}

    //-- Module manipulation

    size_t NumberOpenFils() const
    {
        return (db ? db->GetOpenFiles().size(): 0);
    }
    std::string GetFldHelp(size_t ni) const
    {
        return ( ni<aFldKeysHelp.size() ? aFldKeysHelp[ni] : "");
    }
    const char* GetKey() const
    {
        return (db ? db->PackKey(): nullptr);
    }

    /// Test input data (necessary recalculations)
    virtual bool check_input( const char* /*key*/, int /*level*/=1 )
    {
        return false;
    }
    virtual std::string GetKeyofRecord(const char *oldKey, const char *strTitle, int keyType);
    /// opens window with 'Remake record' parameters
    virtual void MakeQuery(); //!!??
    virtual void RecSave(const char *key, bool onOld= true);
    virtual void RecInput(const char *key);
    virtual int RecBuild(const char *key, int mode = VF_UNDEF);
    virtual void RecCalc(const char *key);
    virtual void TryRecInp(const char *key, time_t& time, int q, bool save = true);
    virtual void AddRecord(const char* key);
    virtual void AddRecord(const char* key, int& fnum);
    virtual int AddRecordTest(const char* key, int& fnum );
    virtual void DeleteRecord(const char *key, bool errifNo = true);
    virtual bool ImplementedPrint() const
    { return false; }
    virtual void RecordPrint(const char *key=nullptr);
    virtual void RecordPlot(const char *key);
    //-- for graphic data set
    virtual bool SaveChartData( jsonui::ChartData* /*graph*/ )
    { return false; }
    virtual void ClearGraphDialog()
    { }

    void CurrentToJSON(const std::string& filename);
    TCIntArray SelectFileList(int mode);

    //-- Database manipulation
    void DelList();
    void Transfer();
    void CopyRecordsList(bool if_rename );
    void KeysToTXT();
    void RecToTXT();
    void RecOfTXT();
    void RecExport();
    void RecImport( );
    void RecListToJSON();
    void RecListFromJSON();

    void SaveM();
    void SaveCurrentKey();
    void SaveAs();
    void DeleteCurrent();
    void RunFilter();
    void RunNext();
    void RunPrevious();
    void RunDerive();
    void RunCalc();
    void RunNew();
    void RunCreate();
    void RecordLoadinProfile(const char *key);
    void NewinProfile();
    void CreateinProfile();
    std::string CurrentKey();
    std::string DBKeywd() const
    {
        return (db ? db->GetKeywd() :"");
    }
    void SetString(const std::string s)
    {
        state_line = s;
    }

    void RebildFile();
    void AddFileToList();
    void ReOpenFileList();
    void AddOpenFile();
    void ReplaceRecordwithQuestion(int Rnum, const char *key, int &quest_reply);

    //--- Manipulation show current record
    bool  CheckEqText( const char *erscan, const char *msg=nullptr );
    bool MessageToSave();  //!!??
    void ModUpdate(const std::string& str);  //!!??
    void Show(QWidget* parent, const char *str=nullptr, bool viewmode=false); //!!??
    void Update(bool force=true); //!!??
    QWidget* window(); //!!??

protected:   // SumModule level
    size_t nRT;
    size_t start_key_field_edit;
    bool contents_changed;     // needs to save
    std::string state_line;
    std::string icon_file_path;

    void set_string(const std::string s)
    {
        state_line = s;
    }

    TCModuleImp* pImp;  //!!??
    void clearEditFocus(); //!!??
    friend class TCModuleImp;

    TCModule(const TCModule&);
    const TCModule& operator=(const TCModule&);

protected:  // Module level
    int nQ;                // number of DB structures
    std::string Filter;
    TCStringArray aFldKeysHelp;      // string help of fields
    std::string start_title;
    TDataBase* db=nullptr;

    void PrintSDref( const char* sd_key , const char* text_fmt ); // !!! To do
    std::string  makeKeyFilter();
    bool  testKeyFilter();
};


// TCModule container
class TModuleList: public std::vector<std::shared_ptr<TCModule>>
{
public:
    TModuleList():
      std::vector<std::shared_ptr<TCModule>>()
    {}

    ~TModuleList();
};

extern TModuleList aMod;

#endif  // _v_module_h
