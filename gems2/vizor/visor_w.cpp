//-------------------------------------------------------------------
// Id: gems/vizor/visor_w.cpp  version 2.0.0       2001
//
// Implementation of TVisorImp class and some GUI service functions
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
#include <stdio.h>
#include <errno.h>
#include <iostream>
using namespace std;
#ifdef __unix
#include <unistd.h>
#endif

#include <qdialog.h>
#include <qfiledlg.h>
#include <qmsgbox.h>
#include <qapplication.h>
#include <qaccel.h>

#include <qtextbrowser.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include "helpwindow.h"

#include "service.h"
#include "visor_w.h"
#include "visor.h"
#include "module_w.h"
#include "m_param.h"
#include "m_syseq.h"
#include "page_win_w.h"
#include "dlg/MainDialog.h"
#include "dlg/SelectDialog.h"
#include "dlg/KeyFilter.h"
#include "dlg/ProgressDialog.h"
#include "dlg/SystemDialog.h"
#include "dlg/ListFilesDialog.h"
#include "dlg/LoadMessage.h"
#include "dlg/HLinpDialog.h"

#ifdef __unix
const char* GEMS_LOGO_ICON = "img/gems1.png";
const char* GEMS_SYS_ICON = "img/sciences_section.xpm";
const char* GEMS_VERSION_STAMP = "GEM-Selektor v1.93b-PSI (Linux)";
const char* GEMS_DEFAULT_FONT_NAME = "Lucidatypewriter";
const int GEMS_DEFAULT_FONT_SIZE = 12;
#else
const char* GEMS_LOGO_ICON = "img/gems1.png";
const char* GEMS_SYS_ICON = "img/sciences_section.xpm";
const char* GEMS_VERSION_STAMP = "GEM-Selektor v1.93b-PSI (Win32)";
const char* GEMS_DEFAULT_FONT_NAME = "Courier";
const int GEMS_DEFAULT_FONT_SIZE = 9;
#endif
extern const char* GEMS_ABOUT_HTML;
extern const char* GEMS_TOC_HTML;

//----------------------------------------------------------------
// TVisor
//----------------------------------------------------------------

TVisorImp::TVisorImp(int c, char** v):
        QMainWindow( 0, GEMS_VERSION_STAMP ),
        argc(c),
        argv(v),
        CellFont( GEMS_DEFAULT_FONT_NAME, GEMS_DEFAULT_FONT_SIZE )
{
    pVisorImp = this;
    pHelpWidget = 0;
#ifdef Use_mt_mode
    updateTime = 10; // centiseconds
#else
    updateTime = 1; // second
#endif

  try{
    pVisor = new TVisor(argc, argv);
    pVisor->Setup();
  }
  catch(TError err) {
    vfMessage(0, "GEMS setup error", "Could not load Vizor parameters!", vfErr);
    exit(1);	
  }

    gstring logoFile = pVisor->sysGEMDir() + GEMS_LOGO_ICON;
    pixLogo = new QPixmap( logoFile.c_str() );
    logoFile = pVisor->sysGEMDir() + GEMS_SYS_ICON;
    pixSys = new QPixmap( logoFile.c_str() );

    setCaption( GEMS_VERSION_STAMP );
}


TVisorImp::~TVisorImp()
{
    delete pixLogo;
    delete pixSys;
    delete pVisor;
}

void
TVisorImp::closeEvent ( QCloseEvent * ev )
{
    if( pVisor->CanClose() )
    {
        for(uint ii=0; ii<aMod.GetCount(); ii++ )
            if( aMod[ii].pImp )
                aMod[ii].pImp->close();

    QWidget* central = centralWidget();
    // if there's central widget already open
    if( central )
        // and it could be closed OK
        if( central->close() )
        {    delete central;
             central = 0; /* Sveta*/
        }

        QWidget::closeEvent(ev);
    }
}

void
TVisorImp::evHelpClosed ( )
{
    disconnect( SIGNAL( destroyed() ), this, SLOT( evHelpClosed() ) );
    pHelpWidget = 0;
}

void
TVisorImp::CmExit()
{
    close();
}

void
TVisorImp::Update(bool force)
{
#ifndef Use_mt_mode
    if( ProgressDialog::pDia )
        ProgressDialog::pDia->Update(force);
#endif
    if( SystemDialog::pDia )
        SystemDialog::pDia->Update();

    if( HLinpDialog::pDia )   // may be resized
        HLinpDialog::pDia->Update();

    for( uint ii=0; ii<aMod.GetCount(); ii++ )
        aMod[ii].Update(force);
}
/* not used if calcThread is on */
void
TVisorImp::CalcFinished()
{
    if( ProgressDialog::pDia )
        ProgressDialog::pDia->CalcFinished();
}

bool
TVisorImp::SetProfileMode()
{
    try
    {
        pVisor->ProfileMode = true;
        if( !TProfil::pm->initCalcMode() )
        {
            pVisor->ProfileMode = false;
            return false;
        }
    }
    catch(...)
    {
        pVisor->ProfileMode = false;
        TProfil::pm->dyn_kill();
        TProfil::pm->set_def();
        throw;  // go on
    }


    return true;
}

//
// System control functions
//
bool
TVisorImp::SetGeneralMode()
{
    int nnrt = rt[RT_PHASE].Find(defaultAqKey);
    if( nnrt >= 0 )
        rt[RT_PHASE].Del(nnrt);
    nnrt = rt[RT_PHASE].Find(defaultGasKey);
    if( nnrt >= 0 )
        rt[RT_PHASE].Del(nnrt);
    TProfil::pm->dyn_kill();
    TProfil::pm->set_def();

    pVisor->ProfileMode = false;

    return true;
}

void
TVisorImp::ShowPhaseWindow()
{
    try
    {
        TProfil::pm->ShowPhaseWindow();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}
void

TVisorImp::ShowEqPhaseWindow()
{
    try
    {
        TProfil::pm->ShowEqPhaseWindow();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
TVisorImp::NewSystem()
{
    try
    {
        TProfil::pm->newSystat();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
TVisorImp::RemakeSystem()
{
    try
    {
        TProfil::pm->deriveSystat();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
TVisorImp::LoadSystem()
{
    try
    {
        TProfil::pm->loadSystat();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
TVisorImp::CalcMulti()
{
#ifdef Use_mt_mode
    TProfil::pm->userCancel = false;
#else
    TProfil::pm->fStopCalc = false;
#endif
    try
    {
        TProfil::pm->CalcEqstat();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
TVisorImp::SaveSystem()
{
    TSysEq::pm->CmSave();
}


//
// Visual functions
//
void
TVisorImp::SetDialog(QWidget* centralDialog)
{
    QWidget* oldCentral = centralWidget();
    // if there's central widget already open
    if( oldCentral )
	oldCentral->close(true);

    setUpdatesEnabled( FALSE );
    
    setCentralWidget(centralDialog);
    QWidget* d = QApplication::desktop();
    QSize sz = (d->size() - centralDialog->size()) / 2;
    if( sz.width()%2 )
        sz.setWidth( sz.width()+1 );
    move( sz.width(), sz.height() );
    setFixedSize( centralDialog->size() );
    centralDialog->show();

    setUpdatesEnabled( TRUE );
    repaint();


}

void
TVisorImp::PrintText( const char* title, char* text)
{}


const char*
TVisorImp::version()
{
//  return "v1.01a";
    return GEMS_VERSION_STAMP;
}

const char*
TVisorImp::getGEMTitle()
{
//  return "GEMS (GEM-Selektor) v1.01a";
    return GEMS_VERSION_STAMP;
}

void
TVisorImp::CmHelpAbout()
{
    OpenHelp( GEMS_ABOUT_HTML );  // 05.01.01
}

void
TVisorImp::CmHelpMenuItem()
{
    OpenHelp( GEMS_TOC_HTML );  // 05.01.01
}

void
TVisorImp::CmHelp()
{
    OpenHelp( GEMS_TOC_HTML );  // 05.01.01
}


void
TVisorImp::OpenProgress( bool step )
{
    if( ProgressDialog::pDia )
        ProgressDialog::pDia->raise();
    else
        (new ProgressDialog(SystemDialog::pDia, step))->show();
}

void
TVisorImp::CloseProgress()
{
    if( ProgressDialog::pDia )
       ProgressDialog::pDia->CmClose();
}

void
TVisorImp::Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total)
{
    if( LoadMessage::pDia )
        LoadMessage::pDia->Update(msg, prog, total);
    else
        (new LoadMessage( parent, name, msg, prog, total))->show();
}

void
TVisorImp::CloseMessage()
{
    if( LoadMessage::pDia )
       LoadMessage::pDia->close();
}

/*
GraphDialog*
TVisorImp::MakePlot(TCModule *md, TIArray<TPlot>& plts,
                    const char * title, float* sizeReg, float* sizePrt,
                    TPlotLine *aLines,
                    int aAxisType, const char* aXName, const char *aYName )
{
    GraphDialog* graph = new GraphDialog( md, plts, title, sizeReg, sizePrt,
                                aLines, aAxisType, aXName, aYName);
    graph->show();
    return graph;
}

GraphDialog*
TVisorImp::MakePlot(TCModule *md, TIArray<TPlot>& plts,
                    const char * title,
                    int aAxisType, const char* aXName, const char *aYName )
{
    GraphDialog* graph = new GraphDialog( md, plts, title,
                                aAxisType, aXName, aYName);
    graph->show();
    return graph;
}

*/
/*!
   Opens html <file> positioned on <item>
*/

void
TVisorImp::OpenHelp(const char* file, const char* item, QWidget* parent)
{
    gstring path (pVisor->docDir ());
    path += file;

    if( path.rfind( "#" ) == path.npos ) // look for the label
       if( gstring(path, path.length()-5, 5) != ".html" )
          path += ".html";

    if( item && *item )
    {
        path += "#";
        path += item;
    }

    // HTML Widget
    if( !pHelpWidget )
    {
//        pHelpWidget = new HelpWin(this, path.c_str());
	pHelpWidget = new HelpWindow(path.c_str(), ".", parent); //0);
	connect( pHelpWidget, SIGNAL( destroyed() ), SLOT( evHelpClosed() ) );
	pHelpWidget->show();
    }
    else
    {
        pHelpWidget->raise();
        pHelpWidget->loadFile(path.c_str(), 0);
    }


/* 
    // leave external browser code just in case
    // open external browser
    path = "file:" + path;
    const char* browser = getenv("HTML_BROWSER");
    bool netscape = false;

    if( !browser )
        browser = "kfmclient";
    else
        if( strstr(browser,"netscape") )
            netscape = true;

    if( fork() == 0 )
    {
        if( !netscape )
            execlp(browser, browser,
                   "openURL", path.c_str(), 0);
        else
            execlp(browser, browser,
                   path.c_str(), 0);
        perror(browser);
        exit(errno);
    }
    perror(browser);
*/
}


//TCModuleImp*
void
TVisorImp::OpenModule(QWidget* par, int irt, int page, bool viewmode)
{
    try
    {
        TCModuleImp* p = aMod[irt].pImp;
        if( !p )
            p = new TCModuleImp(irt, page, viewmode);
        else           //   added Sveta 24/08/01
            p->Update(true);
            //      aMod[irt]->SetString( "OpenModule" );
        p->Raise(page);
    }
    catch(TError& e)
    {
        vfMessage(pVisorImp, "Error opening module", e.mess.c_str() );
    }
}

#ifdef Use_mt_mode

QWaitCondition calcWait;
QWaitCondition progressWait;

QWaitCondition&
TVisorImp::getWaitCalc()
{
    return calcWait;
}

QWaitCondition&
TVisorImp::getWaitProgress()
{
    return progressWait;
}


//----------------------------------------------------------------
// GUI Service functions for modules
//----------------------------------------------------------------

bool
vfQuestion(QWidget* par, const gstring& title, const gstring& mess)
{
    qApp->lock();
    bool result = (QMessageBox::information(par, title.c_str(), mess.c_str(),
                                     "&Yes", "&No") == 0);
    qApp->unlock();
    return result;
}


int
vfQuestYesNoCancel(QWidget* par, const gstring& title, const gstring& mess)
{
    qApp->lock();
    int result = QMessageBox::information(
                par, title.c_str(), mess.c_str(), "&Yes", "&No", "&Cancel",
                0, 2);
    qApp->unlock();
    switch( result )
    {
    case 0:
        return VF_YES;
    case 1:
        return VF_NO;
        //   case 2:
        //     return VF_YES;
    }

    return VF_CANCEL;
}

void
vfMessage(QWidget* par, const gstring& title, const gstring& mess, WarnType type)
{
    qApp->lock();
    switch( type )
    {
    case vfWarn:
        QMessageBox::warning(par, title.c_str(), mess.c_str() );
        break;
    case vfErr:
        QMessageBox::critical(par, title.c_str(), mess.c_str() );
        break;
    default:
        QMessageBox::information(par, title.c_str(), mess.c_str() );
    }
    qApp->unlock();
}

//----------------------------------------------------------------

static int posx=0, posy=0;

// returns VF3_1, VF3_2 or VF3_3
int
vfQuestion3(QWidget* par, const gstring& title, const gstring& mess, const gstring& s1,
            const gstring& s2,  const gstring& s3, bool i_mov )
{
    qApp->lock();
    QMessageBox qm( title.c_str(), mess.c_str(),
                    QMessageBox::Information,
                    QMessageBox::Yes | QMessageBox::Default,
                    QMessageBox::No,
                    QMessageBox::Cancel | QMessageBox::Escape,
                    par);

    qm.setButtonText(QMessageBox::Yes, s1.c_str());
    qm.setButtonText(QMessageBox::No, s2.c_str());
    qm.setButtonText(QMessageBox::Cancel, s3.c_str());
    if( i_mov )
        qm.move(posx, posy);
    int res = qm.exec();
    qApp->unlock();

    if( i_mov )
    {
        posx = qm.x();
        posy = qm.y();
    }

    switch( res )
    {
    case QMessageBox::Yes :
        return VF3_1;
    case QMessageBox::No :
        return VF3_2;
    case QMessageBox::Cancel :
        return VF3_3;
    }
    return VF3_3;
}

//==================
// vfChoice
//==============

int
vfChoice(QWidget* par, TCStringArray& arr, const char* prompt, int sel)
{
    qApp->lock();
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    qApp->unlock();
    return cw.selected();
}

int
vfChoice2(QWidget* par, TCStringArray& arr, const char* prompt,
               int sel, bool& all_)
{
    qApp->lock();
    SelectDialog cw(par, prompt, arr, sel, all_);
    cw.exec();
    qApp->unlock();
    return cw.selected( all_ );
}

TCIntArray
vfMultiChoice(QWidget* par, TCStringArray& arr, const char* prompt)
{
    TCIntArray sel;
    qApp->lock();
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    qApp->unlock();
    return cw.allSelected();
}


TCIntArray
vfMultiChoiceSet(QWidget* par, TCStringArray& arr, const char* prompt, TCIntArray& sel )
{
    qApp->lock();
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    qApp->unlock();
    return cw.allSelected();
}

#else

//----------------------------------------------------------------
// GUI Service functions for modules
//----------------------------------------------------------------

bool
vfQuestion(QWidget* par, const gstring& title, const gstring& mess)
{
    return (QMessageBox::information(par, title.c_str(), mess.c_str(),
                                     "&Yes", "&No") == 0);
}


int
vfQuestYesNoCancel(QWidget* par, const gstring& title, const gstring& mess)
{
    switch( QMessageBox::information(
                par, title.c_str(), mess.c_str(), "&Yes", "&No", "&Cancel",
                0, 2) )
    {
    case 0:
        return VF_YES;
    case 1:
        return VF_NO;
        //   case 2:
        //     return VF_YES;
    }

    return VF_CANCEL;
}

void
vfMessage(QWidget* par, const gstring& title, const gstring& mess, WarnType type)
{
    switch( type )
    {
    case vfWarn:
        QMessageBox::warning(par, title.c_str(), mess.c_str() );
        break;
    case vfErr:
        QMessageBox::critical(par, title.c_str(), mess.c_str() );
        break;
    default:
        QMessageBox::information(par, title.c_str(), mess.c_str() );
    }
}

//----------------------------------------------------------------

static int posx=0, posy=0;

// returns VF3_1, VF3_2 or VF3_3
int
vfQuestion3(QWidget* par, const gstring& title, const gstring& mess, const gstring& s1,
            const gstring& s2,  const gstring& s3, bool i_mov )
{
    QMessageBox qm( title.c_str(), mess.c_str(),
                    QMessageBox::Information,
                    QMessageBox::Yes | QMessageBox::Default,
                    QMessageBox::No,
                    QMessageBox::Cancel | QMessageBox::Escape,
                    par);

    qm.setButtonText(QMessageBox::Yes, s1.c_str());
    qm.setButtonText(QMessageBox::No, s2.c_str());
    qm.setButtonText(QMessageBox::Cancel, s3.c_str());
    if( i_mov )
        qm.move(posx, posy);
    int res = qm.exec();

    if( i_mov )
    {
        posx = qm.x();
        posy = qm.y();
    }

    switch( res )
    {
    case QMessageBox::Yes :
        return VF3_1;
    case QMessageBox::No :
        return VF3_2;
    case QMessageBox::Cancel :
        return VF3_3;
    }
    return VF3_3;
}

//==================
// vfChoice
//==============

int
vfChoice(QWidget* par, TCStringArray& arr, const char* prompt, int sel)
{
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    return cw.selected();
}

int
vfChoice2(QWidget* par, TCStringArray& arr, const char* prompt,
               int sel, bool& all_)
{
    SelectDialog cw(par, prompt, arr, sel, all_);
    cw.exec();
    return cw.selected( all_ );
}

TCIntArray
vfMultiChoice(QWidget* par, TCStringArray& arr, const char* prompt)
{
    TCIntArray sel;
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    return cw.allSelected();
}


TCIntArray
vfMultiChoiceSet(QWidget* par, TCStringArray& arr, const char* prompt, TCIntArray& sel )
{
    SelectDialog cw(par, prompt, arr, sel);
    cw.exec();
    return cw.allSelected();
}

#endif // Use_mt_mode

bool
vfListFiles(QWidget* par, const char * prfName,
                TCStringArray& fls, TCIntArray& cnt )
{
    ListFilesDialog cw( par, prfName );
BACK:
    if( !cw.exec() )
      return false;

    switch( vfQuestion3(par, prfName,
       "Did you really completed  selection of \n"
       " database files to be linked to the profile?",
       "&Yes/Proceed", "&No/Go back", "&Help" ))
    {
    case VF3_1:
        break;
    case VF3_3:
    case VF3_2:
        goto BACK;
    }
    cw.allSelected( fls, cnt );
    return true;
}


//=============================================
// KeyEdit dialog
//=============================================

#include "dlg/KeyDialog.h"
#include "dlg/KeyProfile.h"

gstring
vfKeyEdit(QWidget* par, const char* caption, int iRt, const char* key)
{

    KeyDialog dbk(par, iRt, key, caption);
    if( !dbk.exec() )
        return "";

    return dbk.getKey();
}

gstring
vfKeyProfile(QWidget* par, const char* caption, int iRt,
    bool& chAqGas, bool& addFiles )
{

    KeyProfile dbk(par, iRt, caption);
    if( !dbk.exec() )
        return "";

    chAqGas = dbk.getAqGasState();
    addFiles = dbk.getFilesState();

    return dbk.getKey();
}

gstring
vfKeyTemplEdit(QWidget* par, const char* caption, int iRt, const char* key,
               bool allowTemplate)
{
    KeyFilter dbk(par, iRt, key, caption, allowTemplate);
    if( !dbk.exec() )
        return "";

    return dbk.getFilter();
}

//=============================================
// Open - Save dialogs
//=============================================

// Opens "Open file" dialog
//
// @param par - parent widget
// @param path - source directory
// @param title - dialog title

bool
vfChooseFileOpen(QWidget* par, gstring& path, const char* title)
{
    //    QString d=QFileDialog::getOpenFileName( ".", "*", par, title );
    QFileDialog file_dlg( ".", "*", par, title, true );
    file_dlg.setCaption("Open file");
    file_dlg.setMode(QFileDialog::ExistingFile);

    if( !path.empty() )
        file_dlg.setSelection(path.c_str());

    if( file_dlg.exec() )
    {
        path = file_dlg.selectedFile();
        return true;
    }
    else
    {
        path = "";
        return false;
    }

}

// Opens "Save file" dialog
//
// @param par - parent widget
// @param path - source directory
// @param title - dialog title

bool
vfChooseFileSave(QWidget* par, gstring& path, const char* title)
{
    //    QString d=QFileDialog::getSaveFileName( ".", filter.c_str(), par, title );

    gstring dir = pVisor->userGEMDir();
//    QFileDialog file_dlg( ".", "*;*.txt", par, title, true );
    QFileDialog file_dlg( dir.c_str(), "*;*.txt", par, title, true );

    file_dlg.setCaption("Save As");
    file_dlg.setMode(QFileDialog::AnyFile);

    if( !path.empty() )
        file_dlg.setSelection(path.c_str());

    if( file_dlg.exec() )
    {
        path = file_dlg.selectedFile();
        return true;
    }
    else
    {
        path = "";
        return false;
    }
}

// exports obect to TXT file
//
// @param par - parent widget
// @param obj - object to export

void
vfObjToFile(QWidget* par, TObject* obj)
{
    gstring filename;
AGAIN:
    if( vfChooseFileSave(par, filename, "Please, provide name of TXT-file") )
    {
        ios::openmode mod = ios::out;

        if( !(::access(filename.c_str(), 0 )) ) //file exists
            switch( vfQuestion3( par, filename.c_str(), "This file exists! What to do?",
                                 "&Append", "&Overwrite", "&Cancel") )
            {
            case VF3_2:
                mod = ios::out;
                break;
            case VF3_1:
                mod = ios::app;
                break;
            case VF3_3:
                goto AGAIN;
            }
        fstream f( filename.c_str(), mod );
        /// errors
        obj->toTXT( f );
    }
}

TVisorImp* pVisorImp;

//--------------------- End of visor_w.cpp ---------------------------

