//-------------------------------------------------------------------
// $Id: LoadMessage.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of LoadMessage class
//
// Copyright (C) 1996-2009  A.Rysin, S.Dmytriyeva
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

#include <iostream>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QProgressBar>
#include <QPushButton>

#include "LoadMessage.h"
#include "service.h"
#include "m_proces.h"
#include "stepwise.h"
#include "visor_w.h"


//-------------------------------------------------------------------

LoadMessage* LoadMessage::pDia = 0;

LoadMessage::LoadMessage( QWidget* parent,
             const char* name,
             const char* msg,
             int prog,
             int total):
               QProgressDialog ( msg, "&Cancel", 0, 100, parent )
{
       pDia = this;

       // reset();
        if( total > 0)
         setMaximum(total);
        setValue(prog);
        qApp->processEvents();
        setWindowTitle( name );

}

void LoadMessage::Update( const char* mess, int prog, int total )
{
        if( total > 0)
         setMaximum(total);
        QString str = mess;
        setLabelText(str);
        setValue(prog);
        qApp->processEvents();
}

void LoadMessage::closeEvent(QCloseEvent* ev)
{
      pDia = 0;
      QProgressDialog::closeEvent(ev);
}

//-------------------------------------------------------------------

ProcessProgressDialog* ProcessProgressDialog::pDia = 0;

ProcessProgressDialog::ProcessProgressDialog( QWidget* parent ):
               QDialog(parent)
{
    pDia = this;
    TProfil::pm->userCancel = false;
    TProfil::pm->stepWise = false;
    TProcess::pm->stepWise = true;
    TProcess::pm->userCancel = false;

    setWindowTitle( "Calculating process..." );

    QVBoxLayout* mainBox = new QVBoxLayout( this);
      
    pProgress = new QProgressBar( this );
    pProgress->setMinimum( 0 );
    pProgress->setMaximum( TProcess::pm->pep->NR1 );
    pProgress->setToolTip( trUtf8( "Shows number of steps" ) );
    mainBox->addWidget( pProgress );
    
    QHBoxLayout* buttonBox = new QHBoxLayout();
 
    pStopStep = new QPushButton( "&Step", this );
    pStopStep->setToolTip( trUtf8( "Make next iteration in Stepwise mode" ) );
    connect( pStopStep, SIGNAL(clicked()), this, SLOT(CmStep()) );
    buttonBox->addWidget(pStopStep);

    pResume = new QPushButton( "&Resume", this );
    pResume->setToolTip( trUtf8( "Make next iteration not in Stepwise mode" ) );
    connect( pResume, SIGNAL(clicked()), this, SLOT(CmResume()) );
    //pResume->hide();
    buttonBox->addWidget(pResume);

    pClose = new QPushButton( "&Cancel", this );
    pClose->setToolTip( trUtf8( "Cancel calculations" ) );
    connect( pClose, SIGNAL( clicked() ), this, SLOT( CmClose() ) );
    buttonBox->addWidget(pClose);

    pBreak = new QPushButton( "&Break", this );
    pBreak->setToolTip( trUtf8( "Break calculations" ) );
    connect( pBreak, SIGNAL( clicked() ), this, SLOT( CmBreak() ) );
    buttonBox->addWidget(pBreak);

    mainBox->addLayout( buttonBox );

    calcThread = new ProcessThread(this);
    connect( calcThread, SIGNAL(finished()), this, SLOT(close()) );
    calcThread->start();
}
/*
void ProcessProgressDialog::slUpdate(bool force)
{ 
   // cout << "Update " <<QThread::currentThreadId() << "  " << pThread  << "  " <<  force << endl;
    pVisorImp->Update(force);
}

void ProcessProgressDialog::slQuestion( void *result, QWidget* par, QString title, QString mess)
{
  cout << "Question " <<QThread::currentThreadId() << "  " << pThread   << endl;
  *((int *)result) = vfQuestion( par, 
		  (const char*)title.toLatin1().data(), (const char*)mess.toLatin1().data());	
  ThreadControl::wakeOne();	// let's calc
}
*/
ProcessProgressDialog::~ProcessProgressDialog()
{ 
  delete calcThread;
}

void ProcessProgressDialog::Update()
{
  pProgress->setValue(TProcess::pm->pep->c_nrk);
  setWindowTitle( TProcess::pm->pep->stkey );
}

// CalcFinished, close window, stop thread 
void ProcessProgressDialog::CalcFinished()
{
	close();
}


void ProcessProgressDialog::CmStep()
{
    try
    {
 	  if( !(calcThread->error.title == "") )
		throw calcThread->error;

         // calcThread->emitWakeOne();
         ThreadControl::wakeOne();	// let's calc

 	 if( TProcess::pm->calcFinished ) 
	     CalcFinished();
    }
    catch( TError& err )
    {
        vfMessage(this, err.title, err.mess);
    }
}

void ProcessProgressDialog::CmResume()
{
    try
    {
     TProcess::pm->stepWise = false;
     pResume->hide();
     pStopStep->disconnect();
     pStopStep->setText("&Stop");
     connect( pStopStep, SIGNAL(clicked()), this, SLOT(CmStop()) );
 
 //    calcThread->emitWakeOne();
    ThreadControl::wakeOne();	// let's calc

	 if( TProcess::pm->calcFinished ) 
	    CalcFinished();
    }
    catch( TError& err )
    {
        vfMessage(this, err.title, err.mess);
    }
}

void ProcessProgressDialog::CmStop()
{
    try
    {
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

void ProcessProgressDialog::CmClose()
{
    // we have to cancel calculation here
	if( calcThread->isRunning() ) 
	{
	    TProcess::pm->userCancel = true;
	    TProcess::pm->pep->Istat = P_MT_MODE/*'5'mt mode*/;
	}
    close();
}

void ProcessProgressDialog::CmBreak()
{
    // we have to cancel calculation here
	if( calcThread->isRunning() ) 
	{
	    TProcess::pm->userCancel = true;
//	    cout << TProcess::pm->pep->Istat << endl;
	}
    close();
}

void ProcessProgressDialog::closeEvent(QCloseEvent* ev)
{
   if( calcThread->isRunning() ) 
	   TProcess::pm->userCancel = true;
 //   calcThread.wait(); 
    pDia = 0;
    QDialog::closeEvent(ev);
}

//--------------------- End of LoadMessage.cpp ---------------------------

