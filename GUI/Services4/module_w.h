//-------------------------------------------------------------------
// $Id: module_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TCModuleImp class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

    int	    iMod;
    TSubModule& rMod;
    TCWindow* pWin;

    time_t last_update;
    int viewmode;

protected:
    void closeEvent(QCloseEvent*);

public/* protected*/ slots:
    
    void CloseWin()
    {  close();  }
    
    //--void setActions(bool viewmode);

    //--void CmNext();
    //--void CmPrevious();
    //--void CmFilter();  // set Data Record filter

    //--- Manipulation current record

    //--void CmCreate();
    //--void CmNew();
    //--void CmShow();  //Demonstrate (show) Data Record
    //--void CmDerive();
    //--void CmCalc();  //Calculate or create Data Record
    //--void CmSave();
    //--void CmSaveAs();
    //--void CmDelete();
    //--void CmPrint();
    //--void CmPlot();

    //--void CmNewinProfile();
    //--void CmCreateinProfile();
    //--void CmLoadinProfile();
    
    //--- Manipulation files of Data Base   (Servis functions )
    //--void CmRebildFile();
    //--void CmAddFileToList();
    //--void CmAddOpenFile();
    //--void CmReOpenFileList();

    //--- Manipulation list of records
    //--void CmCopyList();
    //--void CmRenameList();
    //--void CmTransferList();
    //--void CmDeleteList();
    //--void CmKeysToTXT();
    //--void CmBackup();
    //--void CmRestore();
    //--void CmExport();
    //--void CmImport();

    //--void CmHelp();
    //--void CmHelp2();
    //--void CmScript();

public:
    TCModuleImp(int iMod, int page=0, int viewmode=0);
    virtual ~TCModuleImp();

    /*! Returns whether this module is submodule */
    bool IsSubModule()
    { return rMod.IsSubModule();  }

    gstring moduleName() const
    {  return   rMod.GetName();   }

    int rtNum() const;
    int rtNumRecord() const;
    gstring iconFile() const;

    void Update(bool force=true);
    
    /*! Shows the window or raises 'page' if it exists */
    //--void Raise(int page=0);
    void SelectStart();

    void MakeQuery();

    void CellChanged()
    {   rMod.CellChanged();  }

    //  const gstring& GetString() { return state; }
    /*! Sets the caption of the module */
    void SetCaption(const char* mess)
    { 	setWindowTitle(mess);   }
    
    /*! Sets the status of the module (in the statusbar) */
    //--void SetStatus( const char* s )
    //{   statusBar()->showMessage(s);    }

    void setViewMode( int mode )
    { viewmode = mode; }

    int getViewMode() const
    { return viewmode; }

    int curPage();
};

#endif   // _module_w_h
