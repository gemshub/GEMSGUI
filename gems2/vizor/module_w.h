//-------------------------------------------------------------------
// Id: gems/vizor/module_w.h  version 2.0.0    2001
//
// Declaration of TCModuleImp class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef _module_w_h_
#define _module_w_h_

#include <qwidget.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <qmenubar.h>

#include "v_module.h"

class TCWindow;

/*!
   \class TCModuleImp
   \brief This class provides windowing for modules
*/

class TCModuleImp:
            public QMainWindow
{
    Q_OBJECT

    // friend class TCModule;

    QToolBar* toolBar;

    int	    iMod;
    TSubModule& rMod;
    TCWindow* pWin;

    time_t last_update;

    void LoadGeneralMenu();
    void LoadProfileMenu();

protected:
    void closeEvent(QCloseEvent*);

public/* protected*/ slots:
    void CloseWin()
    {
        close();
    }
    void LoadMenu();

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
    void CmPrint();
    void CmScript();

    //--- Manipulation current record
    void CmSave();
    void CmSaveAs();
    void CmDelete();

    void CmDerive();
    void CmShow();  //Demonstrate (show) Data Record
    void CmFilter();  // set Data Record filter
    void CmCalc();  //Calculate or create Data Record
    void CmPlot();  //
    //  void CmFind();  //Derive (find, calculate, create) Data Record
    void CmNew();
    void CmCreate();
    void CmNext();
    void CmPrevious();

    void CmNewinProfile();
    void CmCreateinProfile();
    void CmLoadinProfile();

    void CmHelp();

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

    void MakeQuery();

    void CellChanged()
    {
        rMod.CellChanged();
    }

    //  const gstring& GetString() { return state; }
    /*! Sets the caption of the module */
    void SetCaption(const char* mess)
    {
        setCaption(mess);
    }
    /*! Sets the status of the module (in the statusbar) */
    void SetStatus( const char* s )
    {
        statusBar()->message(s);
    }

    int offset()
    {
        return (!IsSubModule())?menuBar()->height()+toolBar->height():0;
    }
    int inHeight()
    {
        return height() - offset() - statusBar()->height();
    }
};

#endif   // _module_w_h
