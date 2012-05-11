//-------------------------------------------------------------------
// $Id: module_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TCModuleImp class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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
//#include <qlayout.h>
//#include <qpixmap.h>
//#include <qtoolbutton.h>

//#include <qstatusbar.h>
//#include <qapplication.h>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>


#include "visor.h"
#include "visor_w.h"
#include "module_w.h"
#include "page_w.h"
#include "v_mod.h"

/*!
    The constructor
*/

TCModuleImp::TCModuleImp(int irt, int page, bool viewmode):
        QMainWindow(0),
       iMod(irt),
        rMod( aMod[irt] ),
        last_update( 0 )
{
    setupUi(this);
   (void)statusBar();
	
    setAttribute( Qt::WA_DeleteOnClose );
    //qt3to4 setWindowFlags( windowFlags() | Qt::WA_DeleteOnClose );

    gstring s = rMod.GetName();
    s += " :: ";
    rMod.SetTitle();
    s += rMod.GetString();
    SetCaption( s.c_str() );

    // size is set up from TCWindow
    //  resize(rInfo.init_width, rInfo.init_height);
    setMinimumSize( 300, 200 );

    setActions( viewmode );

    statusBar()->show();

    rMod.pImp = this;
    pWin = new TCWindow(this, aWinInfo[iMod], page);
    setCentralWidget(pWin);

    rMod.Setup();
}

/*!
   The desctructor
*/
TCModuleImp::~TCModuleImp()
{}

 int TCModuleImp::curPage()
 {
     return pWin->curPage();
 }

void TCModuleImp::closeEvent(QCloseEvent* e)
{
    // focus out for last focused widget
    if( qApp->focusWidget() )  // ???? not need
        qApp->focusWidget()->clearFocus();
    // save state for user window

    // close module
    if( rMod.EvClose() ) 
    {
	  pWin->close();
	  rMod.pImp = NULL;
          e->accept();
     //  QWidget::closeEvent(e);
    }
    else
         e->ignore();
}

/*!
    Updates screen contents of the module
    if update interval elapsed since the last update 
*/
void TCModuleImp::Update(bool force)
{
    // Sveta no update windows if Systat calc
    if( !force )
        return;

    //  if( !force && (time(0)-last_update) < pVisorImp->updateInterval() )
    //    return;

    pWin->Update();
    gstring s = rMod.GetName();
    s += " :: ";
    s += rMod.GetString();
    SetCaption( s.c_str() );

    last_update = time(0);
}

void
TCModuleImp::Raise(int page)
{
    show();
    raise();
    pWin->EmitEvTabSelChange(page);
}

void TCModuleImp::SelectStart()
{
   if( iMod >= rt.GetCount() || rt[iMod].RecCount() <= 0)
       return;   // Added to avoid a selection
   if( pVisor->ProfileMode )
   {
       if( iMod <= RT_SYSEQ && iMod != RT_SDATA)
        return;
       CmLoadinProfile();
   }
     else
      CmShow();
}

/*!
    Starts dialog for remake parameters
*/
void TCModuleImp::MakeQuery()
{
    TQueryWindow qd(aWinInfo[iMod]);
    qd.exec();

    Update(true);
}

/*!
    Loads menu for General or Project mode
*/
void TCModuleImp::setActions( bool viewmode )
{
    connect( action_Help, SIGNAL( triggered()), this, SLOT(CmHelp()));
    connect( actionView_Scripts, SIGNAL( triggered()), this, SLOT(CmScript()));
    connect( action_About_GEMS, SIGNAL( triggered()), pVisorImp, SLOT(CmHelpAbout()));
    connect( action_Help_2, SIGNAL( triggered()), this, SLOT(CmHelp2()));

	if( viewmode || IsSubModule() ) // no menu
	{
		menu_Record->clear();
		delete menu_Record;
		menuRecord_List->clear();
		delete menuRecord_List;
		menu_Database_Files->clear();
		delete menu_Database_Files;
		//menu_Help->clear();
		//delete menu_Help;
		toolBar_2->clear();
        return;
	}


// Records and help menu	
    connect( action_Remake, SIGNAL( triggered()), this, SLOT(CmDerive()));
    connect( action_Calculate, SIGNAL( triggered()), this, SLOT(CmCalc()));
    connect( action_Save, SIGNAL( triggered()), this, SLOT(CmSave()));
    connect( actionSave_As, SIGNAL( triggered()), this, SLOT(CmSaveAs()));
    connect( action_Delete, SIGNAL( triggered()), this, SLOT(CmDelete()));
    connect( action_Plot, SIGNAL( triggered()), this, SLOT(CmPlot()));
    connect( action_Print, SIGNAL( triggered()), this, SLOT(CmPrint()));
    connect( action_Close, SIGNAL( triggered()), this, SLOT(CloseWin()));

    connect( actionNext, SIGNAL( triggered()), this, SLOT(CmNext()));
    connect( actionPrevious, SIGNAL( triggered()), this, SLOT(CmPrevious()));
    connect( actionFilter, SIGNAL( triggered()), this, SLOT(CmFilter()));
	
	if( pVisor->ProfileMode )
	{
   	  menuRecord_List->clear();
   	  delete menuRecord_List;
	  menu_Database_Files->clear();
	  delete menu_Database_Files;
	  connect( action_Create, SIGNAL( triggered()), this, SLOT(CmCreateinProfile()));
      connect( action_New, SIGNAL( triggered()), this, SLOT(CmNewinProfile()));
      connect( action_Display, SIGNAL( triggered()), this, SLOT(CmLoadinProfile()));
	}	
    else
    {    
       connect( action_Create, SIGNAL( triggered()), this, SLOT(CmCreate()));
       connect( action_New, SIGNAL( triggered()), this, SLOT(CmNew()));
       connect( action_Display, SIGNAL( triggered()), this, SLOT(CmShow()));
    
       connect( action_Copy, SIGNAL( triggered()), this, SLOT(CmCopyList()));
       connect( action_Rename, SIGNAL( triggered()), this, SLOT(CmRenameList()));
       connect( action_Move, SIGNAL( triggered()), this, SLOT(CmTransferList()));
	   connect( action_Delete_list, SIGNAL( triggered()), this, SLOT(CmDeleteList()));
       connect( action_List, SIGNAL( triggered()), this, SLOT(CmKeysToTXT()));
       connect( action_Backup, SIGNAL( triggered()), this, SLOT(CmBackup()));
	   connect( action_Restore, SIGNAL( triggered()), this, SLOT(CmRestore()));
       connect( action_Export, SIGNAL( triggered()), this, SLOT(CmExport()));
       connect( action_Import, SIGNAL( triggered()), this, SLOT(CmImport()));

           connect( action_Selection, SIGNAL( triggered()), this, SLOT(CmReOpenFileList()));
       connect( action_Add_Link, SIGNAL( triggered()), this, SLOT(CmAddOpenFile()));
	   connect( action_New_2, SIGNAL( triggered()), this, SLOT(CmAddFileToList()));
       connect( action_Compress, SIGNAL( triggered()), this, SLOT(CmRebildFile()));
    }
}


void TCModuleImp::CmSave()
{
    if( qApp->focusWidget() )
        qApp->focusWidget()->clearFocus();
    ((TCModule*)&rMod)->CmSaveM();
}

void TCModuleImp::CmShow()
{
    if( qApp->focusWidget() )
       qApp->focusWidget()->clearFocus();
    ((TCModule*)&rMod)->CmShow();
}

void TCModuleImp::CmFilter()
{
   if( qApp->focusWidget() )
        qApp->focusWidget()->clearFocus();
    ((TCModule*)&rMod)->CmFilter();
}

void TCModuleImp::CmNext()
{
   if( qApp->focusWidget() )
       qApp->focusWidget()->clearFocus();
    ((TCModule*)&rMod)->CmNext();
}

void TCModuleImp::CmPrevious()
{
   if( qApp->focusWidget() )
        qApp->focusWidget()->clearFocus();
    ((TCModule*)&rMod)->CmPrevious();
}


// for Qt first argument of macro is ignored

#define TCM_EV_COMMAND(mm,ff)	void TCModuleImp::ff() \
                               { if( qApp->focusWidget() )\
                                   qApp->focusWidget()->clearFocus();\
                                   ((TCModule*)&rMod)->ff(); }

TCM_EV_COMMAND( CM_REBILDFILE, CmRebildFile) ;
TCM_EV_COMMAND(CM_ADDFILETOLIST, CmAddFileToList);
TCM_EV_COMMAND(CM_ADDOPENFILE, CmAddOpenFile);
TCM_EV_COMMAND(CM_REOPENFILELIST, CmReOpenFileList);

TCM_EV_COMMAND(CM_KEYSTOTXT, CmKeysToTXT);
TCM_EV_COMMAND(CM_DELETELIST, CmDeleteList);
TCM_EV_COMMAND(CM_CHOISELIST, CmCopyList);
TCM_EV_COMMAND(CM_RENAMELIST, CmRenameList);
TCM_EV_COMMAND(CM_TRANSFERLIST, CmTransferList);

//  TCM_EV_COMMAND(CM_RECSAVE, CmSave);
TCM_EV_COMMAND(CM_RECSAVEAS, CmSaveAs);
TCM_EV_COMMAND(CM_RECDELETE, CmDelete);
TCM_EV_COMMAND(CM_RECNEW, CmNew);
TCM_EV_COMMAND(CM_RECCREATE, CmCreate);
TCM_EV_COMMAND(CM_RECCALC, CmCalc);
TCM_EV_COMMAND(CM_RECPLOT, CmPlot);

TCM_EV_COMMAND(CM_RECDERIVE, CmDerive);
TCM_EV_COMMAND(CM_IMPORT, CmImport);
TCM_EV_COMMAND(CM_EXPORT, CmExport);
TCM_EV_COMMAND(CM_IMPORT2, CmBackup);
TCM_EV_COMMAND(CM_EXPORT2, CmRestore);
TCM_EV_COMMAND(CM_PRINT, CmPrint);
TCM_EV_COMMAND(CM_SCRIPT, CmScript);

TCM_EV_COMMAND(CM_11, CmLoadinProfile);
TCM_EV_COMMAND(CM_12, CmNewinProfile);
TCM_EV_COMMAND(CM_121, CmCreateinProfile);

TCM_EV_COMMAND(CM_HELP, CmHelp);
TCM_EV_COMMAND(CM_HELP2, CmHelp2);

// -------------- End of file module_w.cpp ----------------------


