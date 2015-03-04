//-------------------------------------------------------------------
// $Id: v_module.h 1391 2009-08-10 12:58:16Z gems $
//
// Declaration of TSubModule, TModule and TModList classes
//
// Copyright (C) 1996-2013 S.Dmytriyeva, A.Rysin
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
#ifndef _v_2module_h_
#define _v_2module_h_

#include <ctime>
#include "v_dbm.h"

class TCModuleImp;
class QWidget;
struct GraphData;

// --- Parametres of function GetKeyofRecord()
const int KEY_OLD = 1;
const int KEY_NEW = 2;
const int KEY_TEMP = 3;
const int KEY_NEW_SAVEAS = 4;

// --- Parametres of function RecBuild()
const int VF_UNDEF =   0;
const int VF_BYPASS =  6;
const int VF_REMAKE =  7;
const int VF_CLEARALL = 2;

/**
    TSubModule class represents submodules of the program
    and is the base for TCModule class which is for full-featured modules
    'aMod' is the list of all the modules and submodules in the system
    TModuleImp class is visual representation (window) for TSubModule/TCModule
*/
class TSubModule
{
private:
    string state;          /// Module window title

    TSubModule(const TSubModule&);
    const TSubModule& operator=(const TSubModule&);

protected:
    int nRT;                  /// Module number
    bool contentsChanged;     /// Flag "Needs to save"
    string iconFileName;     /// Module icon

    void SetString(const char* s)
    {  state = string(s);  }


public:

    void CmHelp();
    void CmHelp2();

 public:

    TCModuleImp* pImp;		  /// for visor implementation

    /// Default constructor
    TSubModule( int nrt ): nRT( nrt ), contentsChanged(false),
                 iconFileName(""),  pImp(NULL)
    {}
    /// Default destructor
    virtual ~TSubModule()
    {}
    virtual void Setup();
    virtual bool EvClose();

    //--- Selectors

    /// Get submodule/module widget
    QWidget* window();
    /// Test for existing DB files
    virtual bool IsSubModule()
      {  return true;  }
    /// Set default windoe title
    virtual void SetTitle()
      {}
    /// Return submodule number
    int rtNum() const
       {  return nRT; }
    /// Set Module icon file name
    virtual void SetIcon(const char* aIconFileName )
      {  iconFileName = aIconFileName;   }
    /// Get Module icon file name
    virtual const char* GetIcon() const
      {  return iconFileName.c_str();    }
    /// Get name of html file
    virtual const char* GetHtml();
    virtual const string& GetString()
       {  return state; }

    /// used by TCell derivatives to mark changes in the module
    virtual void CellChanged( bool val = true)
     {  contentsChanged = val;  }
    /// used by TCell derivatives to mark changes in the module
    virtual bool isCellChanged() const
    {   return contentsChanged;  }

    //-- Module window manipulation

    /// Open module window
    void Show(QWidget* parent, const char *str=0, bool viewmode=false );
    /// Updates window contents
    void Update(bool force=true);
    /// Updates contents of all windows and caption of the current one
    void ModUpdate( const char *str );
    /// This function is called when active page of submodule is changed
    virtual void EvPageChanged(int /*nPage*/)
    {}

    //-- Module manipulation

    virtual string  GetKeyofRecord( const char *, const char *, int )
    {   return "";  }

    // Module impotant functions
    virtual const char* GetName() const = 0;
    virtual void ods_link( int i=0 )=0; /// link values to objects
    virtual void dyn_set( int i=0 )=0;  /// set dynamic objects ptr to values
    virtual void dyn_kill(int i=0 )=0;  /// free dynamic memory in objects and values
    virtual void dyn_new( int i=0 )=0;  /// realloc dynamic memory
    virtual void set_def(int i=0) {}   /// set default data or zero if nessasary

};

/**
    TCModule class represents full-featured modules of the program
    and is the derived from TSubModule class
    'aMod' is the list of all the modules and submodules in the system
    TModuleImp class is visual representation (window) for TSubModule/TCModule
*/
class TCModule:  public TSubModule
{
    // friend TCModuleImp;
    TCModule(const TCModule&);
    const TCModule& operator=(const TCModule&);

protected:

    TEJDataBase* dbEJ;     /// Pointer to class contened the structure of EJDB
    TDataBase* dbOld;      /// Pointer to class contened the structure of oldDB

#ifdef oldDBusing        /// Pointer to class contened the structure of main DB
    TDataBase* db;
#else
    TEJDataBase* db;
#endif

    int nQ;                /// Number of DB structures
    string start_title;          /// Default title for window
    vector<string> aFldKeysHelp; /// Help lines for key fields
    int startKeyEdit;            /// First field in key for editing in calc mode

    // Work data
    string Filter;

    virtual void Setup();
    virtual bool EvClose();
    virtual void MakeQuery();

    /// Module calc only in project mode
    bool isCalcModules();
    virtual string  makeKeyFilter();
    virtual bool  testKeyFilter();

    //-- Database manipulation
    void DelList( const char *pattern );
    void Transfer( const char *pattern );
    void CopyRecordsList( const char *pattern, bool if_rename );
    void RecExport( const char *pattern );
    void RecImport();
    void RecListToBsonJSON( const char *pattern );
    void RecListFromBsonJSON();
    void RecListToEJDB( const char *pattern );
    void RecListFromEJDB( const char *pattern );
    void PrintSDref( const char* sd_key , const char* text_fmt );

public:

    /// Default constructor
    TCModule( int nrt ):  TSubModule( nrt ), nQ(1),
     dbEJ(const_cast<TEJDataBase*>(&rtEJ[nrt])),
     dbOld(const_cast<TDataBase*>(&rtOld[nrt])),
     start_title(" "), startKeyEdit(0),  Filter(ALLKEY)
    {
#ifdef oldDBusing        /// Pointer to class contened the structure of main DB
    db = dbOld;
#else
    db = dbEJ;
#endif

    }
    virtual ~TCModule()
    {}

    //--- Selectors

    /// Test for existing DB files
    bool IsSubModule()
       {  return false;  }
    /// Set default windoe title
    void SetTitle()
        {   SetString( start_title.c_str() );  }
    /// Get First field in key for editing in calc mode
    int keyEditField();
    /// Set First field in key for editing in calc mode
    void  setKeyEditField( int fld )
       {  startKeyEdit = fld;  }
    /// Get help line for key field Ni
    const char* GetFldHelp( int Ni ) const
        { return aFldKeysHelp[Ni].c_str();  }
    /// Get current record key
    const char* GetKey()  const
        {  return dbEJ->PackKey();  }
    /// Get current filter
    const char *getFilter()
       {
        if( testKeyFilter() )
           Filter = makeKeyFilter();
        return Filter.c_str();
       }
    /// Set current filter
    void setFilter( const char *filt)
        {    Filter = filt;  }

    // -- Work with record

    /// Test input data (nessasary recalck)
    virtual bool check_input( const char* /*key*/, int /*level*/=1 )
        {  return false; }
    bool MessageToSave();  // save solicitation
    virtual void RecSave( const char *key, bool onOld= true );
    virtual void RecInput( const char *key );
    void RecordLoadinProfile( const char *key=0 );
    virtual int RecBuild( const char *key, int mode = 0/*VF_UNDEF*/ );
    virtual void RecCalc( const char *key );
    virtual void TryRecInp( const char *key, time_t& time, int q );
    virtual void DeleteRecord( const char *key );
    virtual void RecordPrint( const char *sdkey=0 ); //sddata key
    virtual void RecordPlot( const char *key );

    // Copy all of records in Data Base to EJDataBase Project
    void CopyAllToEJDB( bool fromOld, int& fnum,
                        int fld = -1, const string& from_t="*",  const string& to_t="*",
                        const char *pattern = ALLKEY );

    //-- Module manipulation

    virtual string  GetKeyofRecord(
        const char *oldKey, const char *strTitle, int keyType );
    bool  CheckEqText( const char *erscan, const char *msg=0 );
    vector<int> SelectFileList( int mode, int dbType = 0 );

    //-- for graphic data set

    virtual bool SaveGraphData( GraphData* /*graph*/ )
    { return false; }

public:

    //--- Manipulation current record

    void CmNew(); // ==  void CmNewinProfile();
    void CmCreate(); // ==  void CmCreateinProfile();
    void CmFilter();  // set Data Record filter
    void CmDerive();
    void CmShow( const char *key=0 );  //Demonstrate (show) Data Record
    void CmLoadinProfile( const char *key=0 );
    void CmCalc();  //Calculate or create Data Record
    void CmSave();
    void CmSaveM();
    void CmSaveAs();
    void CmDelete();

    void CmPrint();
    void CmPlot();
    void CmScript();

    //void _CmNext();
    //void _CmPrevious();

    //--- Manipulation list of records

    void CmKeysToTXT();
    void CmDeleteList();
    void CmCopyList();
    void CmRenameList();
    void CmTransferList();
    virtual void CmImport();  // CmImportJson();
    virtual void CmExport();  // CmExportJson();
    void CmImportJson();
    void CmExportJson();
    void CmBackupEJDB();
    void CmRestoreEJDB();

    //--- Manipulation files of Data Base   (Servis functions )

    void CmRebildFile();
    void CmAddFileToList();
    void CmAddOpenFile();
    void CmReOpenFileList();
    void CmAddEJDBToList();
    void CmAddOpenEJDB();
    void CmReOpenEJDB();

};

// TSubModule and TCModule container
class TModuleList:
            public vector<TSubModule*>
{
public:
    TModuleList()
    {}
    ~TModuleList();
};

extern TModuleList aMod;

#endif  // _v_2module_h
