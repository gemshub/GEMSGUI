//-------------------------------------------------------------------
// $Id: v_module.h 1391 2009-08-10 12:58:16Z gems $
//
// Declaration of TSubModule, TModule and TModList classes
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
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
#ifndef _v_module_h_
#define _v_module_h_

#include "v_dbm.h"

class TCModuleImp;
class QWidget;

/*
    TSubModule class represents submodules of the program
    and is the base for TCModule class which is for full-featured modules
    'aMod' is the list of all the modules and submodules in the system
    TModuleImp class is visual representation (window) for TSubModule/TCModule
*/

struct GraphData;
//struct TPlotLine;

class TSubModule
{
    // friend TCModuleImp;

private:
    gstring state;

    TSubModule(const TSubModule&);
    const TSubModule& operator=(const TSubModule&);

protected:
    int nRT;
    int startKeyEdit;
    bool contentsChanged;     // needs to save
    gstring iconFileName;

    // Events part
    virtual void CloseWin();

    void SetString(const char* s)
    {
        state = s;
    }

public:
    virtual void Setup();
    virtual bool EvClose();
    virtual void CmHelp();
    virtual void CmHelp2();

 public:
    TCModuleImp* pImp;		// for visor implementation
    QWidget* window();

    TSubModule( int nrt );
    virtual ~TSubModule();

    virtual bool IsSubModule()
    {
        return true;
    }

    virtual void SetTitle()
    {}

    // this function is called when active page of submodule is changed
    virtual void EvPageChanged(int /*nPage*/)
    {}

    // used by TCell derivatives to mark changes in the module
    virtual void CellChanged( bool val = true)
    {
        contentsChanged = val;
    }

    // used by TCell derivatives to mark changes in the module
    virtual bool isCellChanged() const
    {
        return contentsChanged;
    }

    int rtNum() const
    {
        return nRT;
    }

    virtual void SetIcon(const char* aIconFileName )
    {
        iconFileName = aIconFileName;
    }

    virtual const char* GetIcon() const
    {
        return iconFileName.c_str();
    }

    virtual const char* GetHtml();

    virtual const char* GetName() const = 0;
    virtual void ods_link( int i=0 )=0;   // link values to objects
    virtual void dyn_set( int i=0 )=0;    // set dynamic objects ptr to values
    virtual void dyn_kill(int i=0 )=0;  // free dynamic memory in objects and values
    virtual void dyn_new( int i=0 )=0;   // realloc dynamic memory
    virtual void set_def(int /*i*/=0)
    {}  // set default data or zero if nessasary

    //-- Module manipulation

    void ModUpdate( const char *str );
    void Show(QWidget* parent, const char *str=0, bool viewmode=false );

    void Update(bool force=true);
    virtual const gstring& GetString()
    {
        return state;
    }

    int keyEditField();
    void  setKeyEditField( int fld )
    {
       startKeyEdit = fld;
    }


    //-- Module manipulation
    virtual gstring  GetKeyofRecord( const char */*oldKey*/, const char */*strTitle*/,
                                     int /*keyType*/ )
    { return"";}

};

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


/*
    TCModule class represents full-featured modules of the program
    and is the derived from TSubModule class
    'aMod' is the list of all the modules and submodules in the system
    TModuleImp class is visual representation (window) for TSubModule/TCModule
*/

class TCModule:
            public TSubModule
{
    // friend TCModuleImp;

    TCModule(const TCModule&);
    const TCModule& operator=(const TCModule&);

protected:

    TDataBase* db;
    int nQ;                // number of DB structures
    gstring Filter;

    TCStringArray aFldKeysHelp;      // gstring help of fields
    gstring start_title;

    virtual void Setup();
    virtual bool EvClose();
    void PrintSDref( const char* sd_key , const char* text_fmt );

public:	// moved to public to remove 'friend' for TCModuleImp
    //--- Manipulation files of Data Base   (Servis functions )
    void CmRebildFile();
    void CmAddFileToList();
    void CmAddOpenFile();
    void CmReOpenFileList();

    //--- Manipulation list of records
    void CmKeysToTXT();
    void CmDeleteList();
    void CmCopyList();
    void CmRenameList();
    void CmTransferList();

    virtual void CmImport();
    virtual void CmExport();
    virtual void CmBackup();
    virtual void CmRestore();

    //--- Manipulation current record
    void CmDelete();
    void CmDerive();
    void CmShow( const char *key=0 );  //Demonstrate (show) Data Record
    void CmFilter();  // set Data Record filter
    void CmNew();
    void CmCreate();
    void CmNext();
    void CmPrevious();

    void CmPlot();

public:
    TCModule( int nrt );
    virtual ~TCModule();

    bool IsSubModule()
    {
        return false;
    }

    void SetTitle()
    {
        SetString( start_title.c_str() );
    }

    //--- Manipulation current record
    void CmSave();
    void CmSaveM();
    void CmSaveAs();
    void CmCalc();  //Calculate or create Data Record

    void CmNewinProfile();
    void CmCreateinProfile();
    void RecordLoadinProfile( const char *key=0 );
    void CmLoadinProfile( const char *key=0 );
    void CmPrint();
    void CmScript();

    virtual bool check_input( const char* /*key*/, int /*level*/=1 )
    {
        return false;
    }        // test input data (nessasary recalck)

    bool MessageToSave();  // save solicitation
    virtual void RecSave( const char *key, bool onOld= true );
    virtual void RecInput( const char *key );
    virtual int RecBuild( const char *key, int mode = VF_UNDEF );
    virtual void RecCalc( const char *key );
    virtual void TryRecInp( const char *key, time_t& time, int q );
    virtual void AddRecord(const char* key);
    virtual void AddRecord(const char* key, int& fnum );
    virtual int AddRecordTest(const char* key, int& fnum );
    virtual void DeleteRecord( const char *key, bool errifNo = true  );
    virtual void RecordPrint( const char *key=0 ); //sddata key
    virtual void RecordPlot( const char *key );

    //-- Database manipulation
    void DelList( const char *pattern );
    void Transfer( const char *pattern );
    void CopyRecordsList( const char *pattern, bool if_rename );
    void KeysToTXT( const char *pattern );
    void RecToTXT( const char *pattern );
    void RecOfTXT( );
    void RecExport( const char *pattern );
    void RecImport( );

    TCIntArray SelectFileList(int mode);

    virtual void MakeQuery();

    //-- Module manipulation
    virtual gstring  GetKeyofRecord( const char *oldKey, const char *strTitle,
                                     int keyType );
    virtual gstring  makeKeyFilter();
    virtual bool  testKeyFilter();
    const char *getFilter()
    {
        if( testKeyFilter() )
           Filter = makeKeyFilter();
        return Filter.c_str();
    }
    void setFilter( const char *filt)
    {
        Filter = filt;
    }


    //  const gstring& GetFldHelp( int Ni ) const { return aFldKeysHelp[Ni];}
    bool  CheckEqText( const char *erscan, const char *msg=0 );

    //-- Module Information
    int NumberOpenFils() const
    {
        return rt[nRT].GetOpenFiles().GetCount();
    }
    const char* GetFldHelp( int Ni ) const
    {
        return aFldKeysHelp[Ni].c_str();
    }
    const char* GetKey()  const
    {
        return db->PackKey();
    }
    //-- for graphic data set

    virtual bool SaveGraphData( GraphData* /*graph*/ )
    { return false; }


 };


// TSubModule and TCModule container
class TModuleList:
            public TIArray<TSubModule>
{
public:
    TModuleList():
            TIArray<TSubModule>(40)
    {}

    ~TModuleList();
};

extern TModuleList aMod;

#endif  // _v_module_h
