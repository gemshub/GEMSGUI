//-------------------------------------------------------------------
// $Id$
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
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <qlayout.h>
#include <qtoolbar.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qaccel.h>
#include <qapplication.h>

#include "visor.h"
#include "visor_w.h"
#include "module_w.h"
#include "page_win_w.h"
#include "query_w.h"



/*!
    The constructor
*/

TCModuleImp::TCModuleImp(int irt, int page, bool viewmode):
        QMainWindow(0),
        iMod(irt),
        rMod( aMod[irt] )
        , last_update( 0 )
{
    setWFlags( getWFlags() | WDestructiveClose );

    gstring s = rMod.GetName();
    s += " :: ";
    rMod.SetTitle();
    s += rMod.GetString();
    SetCaption( s.c_str() );

    // size is set up from TCWindow
    //  resize(rInfo.init_width, rInfo.init_height);
    setMinimumSize( 300, 200 );

    if( !viewmode )
        LoadMenu();

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


void
TCModuleImp::closeEvent(QCloseEvent* e)
{
    // focus out for last focused widget
    if( qApp->focusWidget() )
        qApp->focusWidget()->clearFocus();
    // save state for user window

    pWin->close();

    // close module
    rMod.EvClose();
    rMod.pImp = 0;
    //  rMod.sBw = width();
    // rMod.sBh = height();

    QWidget::closeEvent(e);
}

/*!
    Updates screen contents of the module
    if update interval elapsed since the last update 
*/
void
TCModuleImp::Update(bool force)
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
    pWin->EvTabSelChange(page);
}

/*!
    Starts dialog for remake parameters
*/
void
TCModuleImp::MakeQuery()
{
    TQueryWindow qd(aWinInfo[iMod]);
    qd.exec();

    Update(true);
}

/*!
    Loads menu for General or Project mode
*/
void
TCModuleImp::LoadMenu()
{
    if( IsSubModule() )
        return;

    if( pVisor->ProfileMode )
        LoadProfileMenu();
    else
        LoadGeneralMenu();
}



void
TCModuleImp::LoadGeneralMenu()
{
    menuBar()->clear();

    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Create...", this, SLOT(CmCreate()) );
        p2->insertItem( "&New(Clone)...", this, SLOT(CmNew()) );
        p2->insertSeparator();
        p2->insertItem( "D&isplay", this, SLOT(CmShow()), Key_F6 );
        p2->insertItem( "&Remake...", this, SLOT(CmDerive()) );
        p2->insertItem( "Calculat&e", this, SLOT(CmCalc()), Key_F9 );
        p2->insertSeparator();
        p2->insertItem( "&Save", this, SLOT(CmSave()) );
        p2->insertItem( "Save &As...", this, SLOT(CmSaveAs()) );
        p2->insertItem( "&Delete", this, SLOT(CmDelete()) );
        p2->insertSeparator();
        p2->insertItem( "Plo&t", this, SLOT(CmPlot()) );
        p2->insertItem( "&Print...", this, SLOT(CmPrint()) );
        p2->insertSeparator();
        p2->insertItem( "C&lose", this, SLOT(CloseWin()), CTRL + Key_F4 );
        menuBar()->insertItem( "&Record", p2 );
    }
    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Copy...", this, SLOT(CmCopyList()) );
        p2->insertItem( "&Rename...", this, SLOT(CmRenameList()) );
        p2->insertItem( "&Move...", this, SLOT(CmTransferList()) );
        p2->insertItem( "&Delete...", this, SLOT(CmDeleteList()) );
        p2->insertSeparator();
        p2->insertItem( "&List...", this, SLOT(CmKeysToTXT()) );
        p2->insertItem( "&Backup...", this, SLOT(CmBackup()) );
        p2->insertItem( "&Restore...", this, SLOT(CmRestore()) );
        p2->insertSeparator();
        p2->insertItem( "&Export...", this, SLOT(CmExport()) );
        p2->insertItem( "&Import...", this, SLOT(CmImport()) );
        menuBar()->insertItem( "Record &List", p2 );
    }
    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Configure...", this, SLOT(CmReOpenFileList()) );
        p2->insertItem( "&Add Link...", this, SLOT(CmAddOpenFile()) );
        p2->insertItem( "&New...", this, SLOT(CmAddFileToList()) );
        p2->insertSeparator();
        p2->insertItem( "C&ompress...", this, SLOT(CmRebildFile()) );
        menuBar()->insertItem( "&Database Files", p2 );
    }
    {
        menuBar()->insertSeparator();
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Help Contents", pVisorImp, SLOT(CmHelpMenuItem()), CTRL+Key_F1 );
        p2->insertItem( "&Scripts...", this, SLOT(CmScript()), Key_F3 );
        p2->insertSeparator(); // Above line fixed 05.01.01
        p2->insertItem( "&About", pVisorImp, SLOT(CmHelpAbout()) );
        menuBar()->insertItem( "&Help", p2 );
    }

    toolBar = new QToolBar("mtoolbar", this );

    const gstring imgDir = pVisor->imgDir();
    gstring imgFile;

    imgFile = imgDir + "arrow_left1.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Display previous record", 0,
                     this, SLOT(CmPrevious()), toolBar, "previous" );

    imgFile = imgDir + "arrow_right1.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Display next record", 0,
                     this, SLOT(CmNext()), toolBar, "next" );

    imgFile = imgDir + "p_star.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Set filter for record keys", 0,
                     this, SLOT(CmFilter()), toolBar, "filter" );

    imgFile = imgDir + "fileopen.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Display selected record", 0,
                     this, SLOT(CmShow()), toolBar, "display" );

    toolBar->addSeparator();

    imgFile = imgDir + "iconclose.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Delete current record", 0,
                     this, SLOT(CmDelete()), toolBar, "delete" );

    toolBar->addSeparator();

    imgFile = imgDir + "toolbutton.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Remake record data structure", 0,
                     this, SLOT(CmDerive()), toolBar, "remake" );

    imgFile = imgDir + "kcalc.png"; //"reccalc.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Calculate record data", 0,
                     this, SLOT(CmCalc()), toolBar, "calc" );

    imgFile = imgDir + "filesave.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Save current record", 0,
                     this, SLOT(CmSave()), toolBar, "show" );

    toolBar->addSeparator();

    imgFile = imgDir + "xpaint.png"; //"pixlabel.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Plot data on graph dialog", 0,
                     this, SLOT(CmPlot()), toolBar, "plot" );

    imgFile = imgDir + "fileprint.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Print data to ASCII file", 0,
                     this, SLOT(CmPrint()), toolBar, "print" );

    imgFile = imgDir + "help.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Help", 0,
                     this, SLOT(CmHelp()), toolBar, "help" );

}

void
TCModuleImp::LoadProfileMenu()
{
    menuBar()->clear();

    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Create...", this, SLOT(CmCreateinProfile()) );
        p2->insertItem( "&New(Clone)...", this, SLOT(CmNewinProfile()) );
        p2->insertSeparator();
        p2->insertItem( "D&isplay", this, SLOT(CmLoadinProfile()), Key_F6 );
        p2->insertItem( "&Remake...", this, SLOT(CmDerive()) );
        p2->insertItem( "Calculat&e", this, SLOT(CmCalc()), Key_F9 );
        p2->insertSeparator();
        p2->insertItem( "&Save", this, SLOT(CmSave()) );
        p2->insertItem( "Save &As...", this, SLOT(CmSaveAs()));
        p2->insertItem( "&Delete", this, SLOT(CmDelete()) );
        p2->insertSeparator();
        p2->insertItem( "Plo&t", this, SLOT(CmPlot()) );
        p2->insertItem( "&Print...", this, SLOT(CmPrint()) );
        p2->insertSeparator();
        p2->insertItem( "C&lose", this, SLOT(CloseWin()), CTRL + Key_F4 );
        menuBar()->insertItem( "&Record", p2 );
    }

    {
        menuBar()->insertSeparator();
        QPopupMenu* p2 = new QPopupMenu();
        p2->insertItem( "&Help", this, SLOT(CmHelp()) );
        p2->insertItem( "&Scripts...", this, SLOT(CmScript()), Key_F3 );
        p2->insertSeparator();
        p2->insertItem( "&About", pVisorImp, SLOT(CmHelpAbout()) );
        menuBar()->insertItem( "&Help", p2 );
    }


    const gstring imgDir = pVisor->imgDir();
    gstring imgFile;

    toolBar = new QToolBar("mptoolbar", this );


   imgFile = imgDir + "arrow_left1.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Display previous record", 0,
                     this, SLOT(CmPrevious()), toolBar, "previous" );

    imgFile = imgDir + "arrow_right1.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Display next record", 0,
                     this, SLOT(CmNext()), toolBar, "next" );

    imgFile = imgDir + "p_star.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Set filter for record keys", 0,
                     this, SLOT(CmFilter()), toolBar, "filter" );

    imgFile = imgDir + "fileopen.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Display selected record", 0,
                     this, SLOT(CmLoadinProfile()), toolBar, "display" );

    toolBar->addSeparator();

    imgFile = imgDir + "iconclose.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Delete current record", 0,
                     this, SLOT(CmDelete()), toolBar, "delete" );

    toolBar->addSeparator();

    imgFile = imgDir + "toolbutton.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Remake record data structure", 0,
                     this, SLOT(CmDerive()), toolBar, "remake" );

    imgFile = imgDir + "kcalc.png"; //"reccalc.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Calculate record data", 0,
                     this, SLOT(CmCalc()), toolBar, "calc" );

    imgFile = imgDir + "filesave.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Save current record", 0,
                     this, SLOT(CmSave()), toolBar, "save" ); // "show" );

    toolBar->addSeparator();

    imgFile = imgDir + "xpaint.png"; //"pixlabel.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Plot data on graph dialog", 0,
                     this, SLOT(CmPlot()), toolBar, "plot" );

    imgFile = imgDir + "fileprint.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Print data to ASCII file", 0,
                     this, SLOT(CmPrint()), toolBar, "print" );

    imgFile = imgDir + "help.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Help", 0,
                     this, SLOT(CmHelp()), toolBar, "help" );


}


void TCModuleImp::CmSave()
{
    if( qApp->focusWidget() )
        qApp->focusWidget()->clearFocus();
    ((TCModule*)&rMod)->CmSave();
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

#define TCM_EV_COMMAND(mm,ff)	void TCModuleImp::ff() { ((TCModule*)&rMod)->ff(); }

TCM_EV_COMMAND(CM_REBILDFILE, CmRebildFile);
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
// Sveta  TCM_EV_COMMAND(CM_RECFIND, CmFind);
// TCM_EV_COMMAND(CM_RECSHOW, CmShow);
TCM_EV_COMMAND(CM_IMPORT, CmImport);
TCM_EV_COMMAND(CM_EXPORT, CmExport);
TCM_EV_COMMAND(CM_IMPORT2, CmBackup);
TCM_EV_COMMAND(CM_EXPORT2, CmRestore);
TCM_EV_COMMAND(CM_PRINT, CmPrint);
TCM_EV_COMMAND(CM_SCRIPT, CmScript);

TCM_EV_COMMAND(CM_11, CmLoadinProfile);
TCM_EV_COMMAND(CM_12, CmNewinProfile);
TCM_EV_COMMAND(CM_121, CmCreateinProfile);

TCM_EV_COMMAND(CM_PRINT, CmHelp);
// TCM_EV_COMMAND(CM_PRINT, CmHelpMenuItem);

// -------------- End of file module_w.cpp ----------------------


