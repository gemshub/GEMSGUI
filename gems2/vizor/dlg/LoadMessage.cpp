//-------------------------------------------------------------------
// $Id$
//
// Implementation of LoadMessage class
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

#include "LoadMessage.h"
#include <iostream>
#include "m_gem2mt.h"

LoadMessage* LoadMessage::pDia = 0;

LoadMessage::LoadMessage( QWidget* parent,
             const char* name,
             const char* msg,
             int prog,
             int total):
               QProgressDialog(parent, name)
{
       pDia = this;

       // reset();
        if( total > 0)
         setTotalSteps(total);
        else
         setTotalSteps(100);
        setLabelText(msg);
        setProgress(prog);
        qApp->processEvents();
        setCaption( name );

}

void LoadMessage::Update( const char* mess, int prog, int total )
{
        if( total > 0)
         setTotalSteps(total);
        setLabelText(mess);
        setProgress(prog);
        qApp->processEvents();
}

void LoadMessage::closeEvent(QCloseEvent* ev)
{
      pDia = 0;
      QProgressDialog::closeEvent(ev);
}


#ifdef Use_mt_mode

#include <qprogressbar.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qthread.h>
#include <qtimer.h>
#include "visor_w.h"
#include "service.h"

#include "calcthread.h"
#include "m_proces.h"
#include "stepwise.h"


ProcessProgressDialog* ProcessProgressDialog::pDia = 0;

ProcessProgressDialog::ProcessProgressDialog( QWidget* parent ):
               QDialog(parent, 0, true/*false*/)
{
    pDia = this;
    TProfil::pm->userCancel = false;
    TProfil::pm->stepWise = false;
    TProcess::pm->stepWise = true;
    TProcess::pm->userCancel = false;

    setGeometry( QRect( 20, 20, 350, 110 ) );
//    resize( 350, 110 );
    setCaption( "Calculating process..." );


    pProgress = new QProgressBar( TProcess::pm->pep->NR1, this, "pProgress" );
    pProgress->setGeometry( QRect( 20, 20, 280, 21 ) );
    QToolTip::add( pProgress, trUtf8( "Shows number of steps" ) );


    pStopStep = new QPushButton( "&Stop", this, "pStepAccept" );
    pStopStep->setGeometry( QRect( 20, 60, 70, 35 ) );
    QToolTip::add( pStopStep, trUtf8( "Make next iteration in Stepwise mode" ) );
    connect( pStopStep, SIGNAL(clicked()), this, SLOT(CmStop()) );


    pResume = new QPushButton( "Resume", this, "pResume" );
    pResume->setGeometry( QRect( 100, 60, 70, 35 ) );
    QToolTip::add( pResume, trUtf8( "Make next iteration not in Stepwise mode" ) );
    connect( pResume, SIGNAL(clicked()), this, SLOT(CmResume()) );
    pResume->hide();

    pClose = new QPushButton( "&Cancel", this, "pClose" );
    pClose->setGeometry( QRect( 180, 60, 70, 35 ) );
    QToolTip::add( pClose, trUtf8( "Cancel calculations" ) );
    connect( pClose, SIGNAL( clicked() ), this, SLOT( CmClose() ) );

    pBreak = new QPushButton( "&Break", this, "pBreak" );
    pBreak->setGeometry( QRect( 260, 60, 70, 35 ) );
    QToolTip::add( pBreak, trUtf8( "Break calculations" ) );
    connect( pBreak, SIGNAL( clicked() ), this, SLOT( CmBreak() ) );


    calcThread = new ProcessThread();
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()),	this, SLOT(Run()) );

    calcThread->start();
    timer->start( pVisorImp->updateInterval()*100, TRUE );


}

ProcessProgressDialog::~ProcessProgressDialog()
{
    delete calcThread;
// timer deleted with the parent
//    delete timer;
}


void
ProcessProgressDialog::CmStep()
{
    try
    {
         ThreadControl::wakeOne();	// let's calc
	    if( !(calcThread->error.title == "") )
		throw calcThread->error;

          Update();
          pVisorImp->Update(true);

	if( TProcess::pm->calcFinished ) {
	    CalcFinished();
	    return;
	}
    }
    catch( TError& err )
    {
        vfMessage(this, err.title, err.mess);
    }
}

void
ProcessProgressDialog::CmResume()
{
    try
    {

     TProcess::pm->stepWise = false;
     pResume->hide();

     pStopStep->disconnect();
     pStopStep->setText("&Stop");
     connect( pStopStep, SIGNAL(clicked()), this, SLOT(CmStop()) );
     connect( timer, SIGNAL(timeout()),	this, SLOT(Run()) );

     ThreadControl::wakeOne();	// let's calc
//   calcThread->start();
     timer->start( pVisorImp->updateInterval()*100, TRUE );

	if( TProcess::pm->calcFinished ) {
	    CalcFinished();
	    return;
	}
    }
    catch( TError& err )
    {
        vfMessage(this, err.title, err.mess);
    }
}


/*! Run
    it's called by QTimer timeout
    to display updates in stepless mode
*/

void
ProcessProgressDialog::Run()
{
    try
    {
 	pVisorImp->Update(true);
 	Update();

	if( TProcess::pm->calcFinished ) {
	    timer->stop();
	    CalcFinished();
	    return;
	}

	if( calcThread->running() ) {
	    timer->start( pVisorImp->updateInterval()*100, TRUE );
	}
	else {
	    if( calcThread->error.title != "" ) {
		throw calcThread->error;
	    }
	}

    }
    catch( TError& err)
    {
        vfMessage(this, err.title, err.mess);
 	pVisorImp->Update(true);
	timer->stop();
        close();
    }
}

/*!	CalcFinished
	changes buttons, their names and caption
*/

void
ProcessProgressDialog::CalcFinished()
{
	close();
	return;
}

/*! CmAccept
    Saves system and exit
*/

void
ProcessProgressDialog::CmStop()
{

    try
    {
      timer->stop();
      TProcess::pm->stepWise = true;

      pResume->show();

      pStopStep->disconnect();
      connect( pStopStep, SIGNAL(clicked()), this, SLOT(CmStep()) );
      pStopStep->setText("&Step");
    }
    catch( TError& err)
    {
        vfMessage(this, err.title, err.mess);
    }
}

/*!
    Cancel command
*/
void
ProcessProgressDialog::CmClose()
{
    // we have to cancel calculation here
	if( calcThread->running() ) {
	    TProcess::pm->userCancel = true;
	    TProcess::pm->pep->Istat = P_MT_MODE/*'5'mt mode*/;

	}

    close();
}

void
ProcessProgressDialog::CmBreak()
{
    // we have to cancel calculation here
	if( calcThread->running() ) {
	    TProcess::pm->userCancel = true;
//	    TProcess::pm->pep->Istat = P_MT_MODE/*'5'mt mode*/;
	}
    close();
}

void
ProcessProgressDialog::closeEvent(QCloseEvent* ev)
{
    timer->stop();
    pDia = 0;
    QDialog::closeEvent(ev);
}



void ProcessProgressDialog::Update()
{
  pProgress->setProgress(TProcess::pm->pep->c_nrk);
   setCaption( TProcess::pm->pep->stkey );
}


#endif
//--------------------- End of LoadMessage.cpp ---------------------------

