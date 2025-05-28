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
    TSubModule class represents submodules of the program
    and is the base for TCModule class which is for full-featured modules
    'aMod' is the list of all the modules and submodules in the system
    TModuleImp class is visual representation (window) for TSubModule/TCModule
*/

class TSubModule
{
 public:
    TSubModule(uint nrt);
    virtual ~TSubModule();

    virtual bool IsSubModule()
    {
        return true;
    }
    uint rtNum() const
    {
        return nRT;
    }

    // used by TCell derivatives to mark changes in the module
    virtual void CellChanged(bool val = true)
    {
        contents_changed = val;
    }
    // used by TCell derivatives to mark changes in the module
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

    virtual const char* GetName() const = 0;
    virtual const char* GetHtml();

    virtual void ods_link( int i=0 )=0;   // link values to objects
    virtual void dyn_set( int i=0 )=0;    // set dynamic objects ptr to values
    virtual void dyn_kill(int i=0 )=0;    // free dynamic memory in objects and values
    virtual void dyn_new( int i=0 )=0;    // realloc dynamic memory
    virtual void set_def(int /*i*/=0)
    {}  // set default data or zero if nessasary

    //-- Module manipulation
    void ModUpdate(const std::string& str);  //!!??
    void Show(QWidget* parent, const char *str=nullptr, bool viewmode=false); //!!??
    void Update(bool force=true); //!!??

    virtual const std::string& GetString()
    {
        return state;
    }

    TCIntArray SelectFileList(int mode);
    virtual std::string  GetKeyofRecord( const char* /*oldKey*/, const char* /*strTitle*/,
                                       int /*keyType*/ )
    { return"";}

    uint keyEditField();
    void  setKeyEditField( uint fld )
    {
       start_key_field_edit = fld;
    }

private:
    std::string state;

    TSubModule(const TSubModule&);
    const TSubModule& operator=(const TSubModule&);

protected:
    uint nRT;
    uint start_key_field_edit;
    bool contents_changed;     // needs to save
    std::string icon_file_path;

    void set_string(const std::string s)
    {
        state = s;
    }

    TCModuleImp* pImp;  //!!??
    QWidget* window(); //!!??
    void clearEditFocus(); //!!??
};


/*
    TCModule class represents full-featured modules of the program
    and is the derived from TSubModule class
    'aMod' is the list of all the modules and submodules in the system
    TModuleImp class is visual representation (window) for TSubModule/TCModule
*/
class TCModule:  public TSubModule
{

public:
    TCModule(uint nrt);
    virtual ~TCModule();

    bool IsSubModule()
    {
        return false;
    }

    //--- Manipulation current record
    void CmSave(); //!!??

    /// Test input data (necessary recalculations)
    virtual bool check_input( const char* /*key*/, int /*level*/=1 )
    {
        return false;
    }
    bool MessageToSave();  //!!??

    /// opens window with 'Remake record' parameters
    virtual void MakeQuery(); //!!??
    //{  pImp->MakeQuery();  }
    virtual void RecSave(const char *key, bool onOld= true);
    virtual void RecInput( const char *key );
    virtual int RecBuild( const char *key, int mode = VF_UNDEF );
    virtual void RecCalc( const char *key );
    virtual void TryRecInp( const char *key, time_t& time, int q, bool save = true );
    virtual void AddRecord(const char* key);
    virtual void AddRecord(const char* key, int& fnum);
    virtual int AddRecordTest(const char* key, int& fnum );
    virtual void DeleteRecord( const char *key, bool errifNo = true  );
    virtual bool ImplementedPrint() const
    { return false; }
    virtual void RecordPrint(const char *key=nullptr);
    virtual void RecordPlot(const char *key);

    virtual std::string  GetKeyofRecord( const char *oldKey, const char *strTitle,
                                     int keyType );
    virtual std::string  makeKeyFilter();
    virtual bool  testKeyFilter();
    const char *getFilter()
    {
        if( testKeyFilter() )
           Filter = makeKeyFilter();
        return Filter.c_str();
    }
    void setFilter(const char *filt)
    {
        Filter = filt;
    }

    //  const string& GetFldHelp( int Ni ) const { return aFldKeysHelp[Ni];}
    bool  CheckEqText( const char *erscan, const char *msg=nullptr );

    //-- Module Information
    size_t NumberOpenFils() const
    {
        return rt[nRT]->GetOpenFiles().size();
    }
    const char* GetFldHelp(uint ni) const
    {
        return aFldKeysHelp[ni].c_str();
    }
    const char* GetKey()  const
    {
        return db->PackKey();
    }

    void CurrentToJSON(const std::string& filename);

    //-- for graphic data set
    virtual bool SaveChartData( jsonui::ChartData* /*graph*/ )
    { return false; }
    virtual void ClearGraphDialog()
    { }

private:
    TCModule(const TCModule&);
    const TCModule& operator=(const TCModule&);

protected:
    TDataBase* db;
    int nQ;                // number of DB structures
    std::string Filter;

    TCStringArray aFldKeysHelp;      // string help of fields
    std::string start_title;

    void PrintSDref( const char* sd_key , const char* text_fmt ); // !!! To do
};


// TSubModule and TCModule container
class TModuleList: public std::vector<std::shared_ptr<TSubModule>>
{
public:
    TModuleList():
          std::vector<std::shared_ptr<TSubModule>>()
    {}

    ~TModuleList();
};

extern TModuleList aMod;

#endif  // _v_module_h
