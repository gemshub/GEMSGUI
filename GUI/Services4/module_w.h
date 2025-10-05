//-------------------------------------------------------------------
// $Id: module_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TCModuleImp class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#ifndef _module_w_h_
#define _module_w_h_

#include <QMainWindow>
#include <QDialog>
#include <QStatusBar>
#include <QCloseEvent>
#include "v_module.h"

class TCWindow;

/*!
   \class TCModuleImp
   \brief This class provides windowing for modules
*/
//class TCModuleImp: public QMainWindow, public Ui::ModuleDialogData
class TCModuleImp: public QDialog//QMainWindow
{
    Q_OBJECT

    // friend class TCModule;

    //QToolBar* toolBar;

    size_t    iMod;
    TCModule& rMod;
    TCWindow* pWin;

    time_t last_update;
    int viewmode;


protected:
    void closeEvent(QCloseEvent*);

public slots:
    void CloseWin()
    {  close();  }
    void saveGraphData( jsonui::ChartData* );

    //void Setup(); empty
    bool EvClose();
    void CmHelp();
    void CmHelp2();

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

    void CmImport();
    void CmExport();
    void CmBackup();
    void CmRestore();
    void CmBackuptoJson();
    void CmRestorefromJson();

    //--- Manipulation current record
    void CmDelete();
    void CmDerive();
    void CmShow( const char *key=nullptr );  //Demonstrate (show) Data Record
    //void CmFilter();  // set Data Record filter
    void CmNew();
    void CmCreate();
    //void CmNext();
    //void CmPrevious();

    void CmPlot();

    //--- Manipulation current record
    //void CmSave();
    void CmSaveM();
    void CmSaveAs();
    void CmCalc();  //Calculate or create Data Record

    void CmNewinProfile();
    void CmCreateinProfile();
    //void RecordLoadinProfile( const char *key=nullptr );
    void CmLoadinProfile( const char *key=nullptr );
    void CmPrint();

public:
    TCModuleImp(size_t aiMod, int page=0, int viewmode=0);
    virtual ~TCModuleImp();

    /*! Returns whether this module is submodule */
    bool IsSubModule()
    { return rMod.IsSubModule();  }

    std::string moduleName() const
    {  return   rMod.GetName();   }

    size_t rtNum() const;
    size_t rtNumRecord() const;
    std::string iconFile() const;

    void Update(bool force=true);
    QSize sizeHint() const;

    /* Shows the window or raises 'page' if it exists */
    //--void Raise(int page=0);
    void SelectStart();

    void MakeQuery();

    void CellChanged()
    {   rMod.CellChanged();  }

    /*! Sets the caption of the module */
    void SetCaption(const char* mess)
    { 	setWindowTitle(mess);   }
    
    void setViewMode( int mode )
    { viewmode = mode; }

    int getViewMode() const
    { return viewmode; }

    int curPage();
    void clearFocus();
};

#endif   // _module_w_h
