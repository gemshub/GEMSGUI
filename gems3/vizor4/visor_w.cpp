//--------------------------------------------------------------------
// $Id: visor_w.cpp 999 2008-01-17 13:34:35Z gems $
//
// Implementation of TVisorImp class and some GUI service functions
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
//#include <stdio.h>
//#include <errno.h>
//#include <iostream>

using namespace std;
//#ifdef __unix
//#include <unistd.h>
//#else
//#include <io.h>
//#endif

#include <QCustomEvent>
#include <QString>
#include <QPixmap>
#include <QCloseEvent>
#include <qapplication.h>
#include <qthread.h>

#include <QMutex>
#include <QMutexLocker>
#include <QDesktopWidget>

#include "service.h"
#include "visor_w.h"
#include "visor.h"
#include "module_w.h"
#include "m_syseq.h"
#include "ProgressDialog.h"
#include "NewSystemDialog.h"
#include "LoadMessage.h"
#include "HelpWindow.h"

const char* GEMS_LOGO_ICON = "img/gems1.png";
const char* GEMS_SYS_ICON = "img/sciences_section.xpm";
const char* GEMS_DEFAULT_FONT_NAME = "Courier New";
const char* GEMS_VERSION_STAMP = "GEM-Selektor 3 (GEMS3)";
#ifdef __unix
#ifdef __APPLE__
const int GEMS_DEFAULT_FONT_SIZE = 14;
#else
const int GEMS_DEFAULT_FONT_SIZE = 11;
#endif
#else
const int GEMS_DEFAULT_FONT_SIZE = 10;
#endif
extern const char* GEMS_ABOUT_HTML;
extern const char* GEMS_TOC_HTML;
TVisorImp* pVisorImp;

//----------------------------------------------------------------
// TVisor
//----------------------------------------------------------------

TVisorImp::TVisorImp(int c, char** v):
        QMainWindow( 0 ),
        argc(c),
        argv(v),
        configAutosave(false),
        proc(0)
{
// just in case
	charWidth = 12;
	charHeight = 18;

    pVisorImp = this;
//    assistantClient = new Helper();//0;
    
#ifdef Use_mt_mode
    updateTime = 10; // centiseconds
#else
    updateTime = 1; // second
#endif

//    defaultFont = QApplication::font();
    defaultFont = QFont(GEMS_DEFAULT_FONT_NAME, GEMS_DEFAULT_FONT_SIZE);
    setCellFont( defaultFont );
    //setAxisLabelFont(defaultFont);
    axisLabelFont.setPointSize( 11);

#ifndef __unix
    if( font().pixelSize() < 12 ) {
	QFont dialogFont = font();
	dialogFont.setPixelSize(12);
	setFont(dialogFont);
    }
#endif

    pVisor = new TVisor(argc, argv);
    pVisor->Setup();

    gstring logoFile = pVisor->sysGEMDir() + GEMS_LOGO_ICON;
    pixLogo = new QPixmap( logoFile.c_str() );
    logoFile = pVisor->sysGEMDir() + GEMS_SYS_ICON;
    pixSys = new QPixmap( logoFile.c_str() );
    pThread = QThread::currentThreadId();

    setWindowTitle( GEMS_VERSION_STAMP );
}


TVisorImp::~TVisorImp()
{
    delete pixLogo;
    delete pixSys;
    delete pVisor;
//      delete assistantClient;
}

void TVisorImp::closeEvent ( QCloseEvent * ev )
{
    if( pVisor->CanClose() )
    {
        for(uint ii=0; ii<aMod.GetCount(); ii++ )
            if( aMod[ii].pImp )
                aMod[ii].pImp->close();

        if( HelpWindow::pDia )
          delete HelpWindow::pDia;

    QWidget* central = centralWidget();

    // if there's central widget already open
    if( central )
        // and it could be closed OK
        if( central->close() )
        {
// do we need this deleting ?????
// under Win32/Qt3 it gives crash, other configs seems ok :-(
#ifdef __unix
    		 delete central;
             central = 0; /* Sveta*/ // why? AR
#endif
        }

        QWidget::closeEvent(ev);
    }
}

void TVisorImp::CmExit()
{
    close();
}

void TVisorImp::Update(bool force)
{
    if( pThread != QThread::currentThreadId() )
    {
   	   // (1)NewThread *thread = dynamic_cast<NewThread*>(QThread::currentThread());
       //if( thread )	
       //		thread->emitUpdate(force);	
       // (2) 
    	QMetaObject::invokeMethod(this, "Update",  Qt::QueuedConnection, 
       				   Q_ARG( bool, force ));
        // (3)
    	// QMetaObject::invokeMethod(ProcessProgressDialog::pDia, "slUpdate", 
    	//	   Qt::QueuedConnection, Q_ARG( bool, force ));
       return;    	
    }	
	
    if( ProgressDialog::pDia )
        ProgressDialog::pDia->Update(force);

    if( ProcessProgressDialog::pDia )
        ProcessProgressDialog::pDia->Update();

    if( NewSystemDialog::pDia )
        NewSystemDialog::pDia->Update();

    for( uint ii=0; ii<aMod.GetCount(); ii++ )
        aMod[ii].Update(force);
}

bool TVisorImp::SetProfileMode()
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

bool TVisorImp::SetGeneralMode()
{
	TProfil::pm->deleteAutoGenerated();
    TProfil::pm->dyn_kill();
    TProfil::pm->set_def();

    pVisor->ProfileMode = false;

    return true;
}

void TVisorImp::SaveSystem()
{
    TSysEq::pm->CmSave();
}

void TVisorImp::CalcMulti()
{
#ifdef Use_mt_mode
    TProfil::pm->userCancel = false;

    try
    {
        ProgressDialog* dlg = new ProgressDialog(/*window()*/this, false, true);
    	//dlg->exec();
    	dlg->show();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
#else
    TProfil::pm->fStopCalc = false;
    try
    {
     	showMss = 1L;
    	TProfil::pm->CalcEqstat();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
#endif
}

// Visual functions

void TVisorImp::SetDialog(QWidget* centralDialog)
{
    QWidget* oldCentral = centralWidget();
    // if there's central widget already open
    if( oldCentral )
	    oldCentral->close();
	else
	{
       QDesktopWidget* d= QApplication::desktop(); 
       QSize sz = (d->size() - centralDialog->size()) / 2;
       sz.setWidth( 200 );
      move( sz.width(), sz.height() );
    }   

    setUpdatesEnabled( FALSE );

    setCentralWidget(centralDialog);
    
    setFixedSize( centralDialog->size() );
    centralDialog->show();

    setUpdatesEnabled( TRUE );
    repaint();
}

void 
TVisorImp::PrintText( const char* /*title*/, char* /*text*/)
{}

const char* TVisorImp::version()
{
    return GEMS_VERSION_STAMP;
}

const char* TVisorImp::getGEMTitle()
{
    return GEMS_VERSION_STAMP;
}

void TVisorImp::CmHelpAbout()
{
    OpenHelp( GEMS_ABOUT_HTML );  
}

void TVisorImp::CmHelpMenuItem()
{
    OpenHelp( GEMS_TOC_HTML );  
}

void TVisorImp::CmHelp()
{
    OpenHelp( GEMS_TOC_HTML );  
}

void TVisorImp::setCellFont(const QFont& newCellFont)
{
    CellFont = newCellFont;
    QFontMetrics fm(CellFont);
    charWidth = fm.width("5");
    charHeight = fm.height();
}

int TVisorImp:: getCharWidth() const
{
    return charWidth;
}

int TVisorImp:: getCharHeight() const
{
#ifdef __unix
    return charHeight;// + 4;// 2;
#else
    return charHeight; // + 4;
#endif
}

int TVisorImp:: getCellWidth() const
{
    return charWidth * 9;
}

int TVisorImp:: getLabelWidth() const
{
    return charWidth * 6;
}

//================================================================

void TVisorImp::OpenProgress( bool step )
{
    if( ProgressDialog::pDia )
        ProgressDialog::pDia->raise();
    else
        (new ProgressDialog(NewSystemDialog::pDia, step))->show();
}

void TVisorImp::CloseProgress()
{
    if( ProgressDialog::pDia )
       ProgressDialog::pDia->CmClose();
}

/* not used if calcThread is on *
void
TVisorImp::CalcFinished()
{
    if( ProgressDialog::pDia )
        ProgressDialog::pDia->CalcFinished();
}*/

bool TVisorImp::Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total, bool move )
{
    if( LoadMessage::pDia )
     {  if( LoadMessage::pDia->wasCanceled() )
          return true;
        LoadMessage::pDia->Update(msg, prog, total);
     }
    else
    {  (new LoadMessage( parent, name, msg, prog, total))->show();
        if( move && parent  )
          LoadMessage::pDia->move(parent->x()+parent->width(), parent->y());
    }

   return false;
}


void TVisorImp::CloseMessage()
{
    if( LoadMessage::pDia )
       LoadMessage::pDia->close();
}

void TVisorImp::ProcessProgress( QWidget* parent )
{
    TProcess::pm->userCancel = false;

        ProcessProgressDialog* dlg =
             new ProcessProgressDialog( parent );
   // dlg->exec();
   	dlg->show();
}


/*!
   Opens html <file> positioned on anchor <item> (as in "file:/myfile.html#item")

   NOTE: we should not specify parent parameter for modeless dialog
   because otherwise even that it allows work with parent window it will
   be always overlapping it. For modal windows (and thus modal help) we need parent
*/

void TVisorImp::GetHelp( )
{
        (new HelpWindow(  0  ));
   //     HelpWindow::pDia->show();
}

void
TVisorImp::OpenHelp(const char* file, const char* item1, int page )
{
    if( HelpWindow::pDia )
    {
       if( item1 && page>=0 )
       {
          QString res = item1;
          res += QString("_%1").arg(page);
          gstring txt = res.toLatin1().data();
          HelpWindow::pDia->showDocumentation( file, txt.c_str() );
        }
        else
          HelpWindow::pDia->showDocumentation( file, item1 );

       HelpWindow::pDia->show();
       HelpWindow::pDia->raise();
    }
   // old help assistantClient->showDocumentation( file, item);
}

//TCModuleImp*
void TVisorImp::OpenModule(QWidget* /*par*/, int irt, int page, bool viewmode, bool select)
{
    try
    {
        TCModuleImp* p = aMod[irt].pImp;
        if( !p )
        {   p = new TCModuleImp(irt, page, viewmode);
            p->Raise(page);
            // Select record if list not empty
            if( select )
                p->SelectStart();
        }
        else          
        {    p->Update(true);
             //      aMod[irt]->SetString( "OpenModule" );
             p->Raise(page);
         }

    }
    catch(TError& e)
    {
        vfMessage(pVisorImp, "Error opening module", e.mess.c_str() );
    }
}

//=================================================================

// thread staff

void TVisorImp::theadService( int nFunction, QWidget* par )
{
       switch( nFunction  )
       {
        case thMessage:
          vfMessage( par, thdata.title, thdata.mess );
                       break;
        case thQuestion:
          thdata.res = vfQuestion( par, thdata.title, thdata.mess );
                       break;
        case thQuestion3:
          thdata.res = vfQuestion3( par, thdata.title, thdata.mess,
        		                  thdata.s1, thdata.s2, thdata.s3 );
                       break;
        case thChoice:
          thdata.res = vfChoice( par, thdata.list,
                             thdata.title.c_str(), thdata.seli );
                       break;
        case thChoice2:
          thdata.res = vfChoice2( par, thdata.list, thdata.title.c_str(), 
        		      thdata.seli, thdata.all  );
                       break;
        case thExcludeFillEdit:
          thdata.res = vfExcludeFillEdit( par, thdata.title.c_str(),
             thdata.list, thdata.sel, thdata.fill_data );
                       break;
        default:
                       break;
        }
      thdata.wait = false;
      ThreadControl::wakeOne();
}


//QMutex updateMutex;
QMutex calcMutex;
QWaitCondition calcWait;
QWaitCondition progressWait;

QMutex& TVisorImp::getMutexCalc()
{
    return calcMutex;
}

QWaitCondition& TVisorImp::getWaitCalc()
{
//    return calcWait;
   return progressWait;
}

QWaitCondition& TVisorImp::getWaitProgress()
{
   return progressWait;
}

Qt::HANDLE pThread;

void ThreadControl::wakeOne()
{
    progressWait.wakeOne();
//    pVisorImp->getWaitProgress().wakeOne();
}

void ThreadControl::wakeAll()
{
   progressWait.wakeAll();
//   pVisorImp->getWaitProgress().wakeAll();
}

bool ThreadControl::wait()
{
    return progressWait.wait(&calcMutex);
//  return pVisorImp->getWaitProgress().wait(&pVisorImp->getMutexCalc());
}

bool ThreadControl::wait(unsigned long time )
{
    return progressWait.wait(&calcMutex);
    //return pVisorImp->getWaitProgress().wait(&pVisorImp->getMutexCalc(),time);
}

char* ThreadControl::GetPoint()
{
    return pVisorImp->getTCpoint();
}

void ThreadControl::SetPoint(const char* str )
{
    pVisorImp->setTCpoint( str );
}

//--------------------- End of visor_w.cpp ---------------------------
