//-------------------------------------------------------------------
// $Id$
//
// Declaration of TSubModule, TModule and TModList classes
//
// Copyright (C) 1996-2001 A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef _v_module_h_
#define _v_module_h_

#include "v_object.h"
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
class TPlotLine;

class TSubModule
{
    // friend TCModuleImp;

private:
    gstring state;

    TSubModule(const TSubModule&);
    const TSubModule& operator=(const TSubModule&);

protected:
    int nRT;
    bool fEdit;     // needs to save

    // Events part
    virtual void CloseWin();

    void SetString(const char* s)
    {
        state = s;
    }

public:
    virtual void Setup();
    virtual void EvClose();
    virtual void CmHelp();
    gstring GetString()
    {
        return state;
    }

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
    {
    }
    // this function is called when active page of submodule is changed
    virtual void EvPageChanged(int /*nPage*/)
    {}
    ;
    // used by TCell derivatives to mark changes in the module
    virtual void CellChanged()
    {
        fEdit = true;
    }


    int rtNum() const
    {
        return nRT;
    }
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
    const gstring& GetString() const
    {
        return state;
    }
};

// --- Parametres of function GetKeyofRecord()
const int KEY_OLD = 1;
const int KEY_NEW = 2;
const int KEY_TEMP = 3;

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

    void MessageToSave();  // save solicitation

    virtual void Setup();
    virtual void EvClose();
    void PrintSDref( const char* sd_key , char* text_fmt );

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
    void CmShow();  //Demonstrate (show) Data Record
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
    void CmSaveAs();
    void CmCalc();  //Calculate or create Data Record

    void CmNewinProfile();
    void CmCreateinProfile();
    void RecordLoadinProfile( const char *key=0 );
    void CmLoadinProfile();
    void CmPrint();
    void CmScript();

    virtual bool check_input( const char *key, int level=1 )
    {
        return false;
    }        // test input data (nessasary recalck)

    virtual void RecSave( const char *key, bool onOld= true );
    virtual void RecInput( const char *key );
    virtual int RecBuild( const char *key, int mode = VF_UNDEF );
    virtual void RecCalc( const char *key );
    virtual void TryRecInp( const char *key, time_t& time, int q );
    virtual void AddRecord(const char* key);
    virtual void AddRecord(const char* key, int& fnum );
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

    void MakeQuery();

    //-- Module manipulation
    virtual gstring  GetKeyofRecord( const char *oldKey, const char *strTitle,
                                     int keyType );

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

    virtual bool SaveGraphData( GraphData *graph )
    { return false; }

    const char *getFilter()
    {
        return Filter.c_str();
    }

    void setFilter( const char *filt)
    {
        Filter = filt;
    }

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
