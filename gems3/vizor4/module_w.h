//-------------------------------------------------------------------
// $Id: module_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TCModuleImp class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef _module_w_h_
#define _module_w_h_

#include <qwidget.h>
#include <qmainwindow.h>


#include <QCloseEvent>

#include "v_module.h"
#include "ui_ModuleDialog4.h"

class TCWindow;


/*!
   \class TCModuleImp
   \brief This class provides windowing for modules
*/
class TCModuleImp: public QMainWindow, public Ui::ModuleDialogData
{
    Q_OBJECT

    // friend class TCModule;

    QToolBar* toolBar;

    int	    iMod;
    TSubModule& rMod;
    TCWindow* pWin;

    time_t last_update;

protected:
    void closeEvent(QCloseEvent*);

public/* protected*/ slots:
    
    void CloseWin()
    {  close();  }
    
    void setActions(bool viewmode);

    void CmNext();
    void CmPrevious();
    void CmFilter();  // set Data Record filter

    //--- Manipulation current record

    void CmCreate();
    void CmNew();
    void CmShow();  //Demonstrate (show) Data Record
    void CmDerive();
    void CmCalc();  //Calculate or create Data Record
    void CmSave();
    void CmSaveAs();
    void CmDelete();
    void CmPrint();
    void CmPlot();

    void CmNewinProfile();
    void CmCreateinProfile();
    void CmLoadinProfile();
    
    //--- Manipulation files of Data Base   (Servis functions )
    void CmRebildFile();
    void CmAddFileToList();
    void CmAddOpenFile();
    void CmReOpenFileList();

    //--- Manipulation list of records
    void CmCopyList();
    void CmRenameList();
    void CmTransferList();
    void CmDeleteList();
    void CmKeysToTXT();
    void CmBackup();
    void CmRestore();
    void CmExport();
    void CmImport();

    void CmHelp();
    void CmHelp2();
    void CmScript();

public:
    TCModuleImp(int iMod, int page=0, bool viewmode=false);
    virtual ~TCModuleImp();

    /*! Returns whether this module is submodule */
    bool IsSubModule()
    {
        return rMod.IsSubModule();
    }

    void Update(bool force=true);
    
    /*! Shows the window or raises 'page' if it exists */
    void Raise(int page=0);
    void SelectStart();

    void MakeQuery();

    void CellChanged()
    {
        rMod.CellChanged();
    }

    //  const gstring& GetString() { return state; }
    /*! Sets the caption of the module */
    void SetCaption(const char* mess)
    {
    	setWindowTitle(mess);
    }
    
    /*! Sets the status of the module (in the statusbar) */
    void SetStatus( const char* s )
    {
        statusBar()->showMessage(s);
    }

    int curPage();
};

#endif   // _module_w_h
