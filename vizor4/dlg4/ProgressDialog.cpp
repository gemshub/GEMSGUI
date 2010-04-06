//-------------------------------------------------------------------
// $Id: ProgressDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ProgressDialog class
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

#include <math.h>
#include <iostream>

#include "ProgressDialog.h"
#include "NewSystemDialog.h"
#include "m_param.h"
#include "service.h"
#include "calcthread.h"
#include "visor_w.h"


void
ProgressDialog::switchToAccept(bool isAccept)
{
  pClose->setToolTip( trUtf8( "Cancel IPM run" ) );

	if( isAccept ) {
		pStepAccept->disconnect();
		connect( pStepAccept, SIGNAL(clicked()), this, SLOT(CmAccept()) );
		pStepAccept->setText("&Accept");

		pResume->disconnect();
		connect( pResume, SIGNAL(clicked()), this, SLOT(CmStop()) );
		pResume->setText("&Stop");

        pStepAccept->setToolTip( trUtf8(
                "Save results to Database" ) );
        pResume->setToolTip(trUtf8(
                  "Switch to Stepwise mode" ) );
	}
	else {
		pStepAccept->disconnect();
		connect( pStepAccept, SIGNAL(clicked()), this, SLOT(CmStep()) );
		pStepAccept->setText("&Step");
//qt3to4		pStepAccept->setAccel(Qt::Key_F8);

       	pResume->disconnect();
		connect( pResume, SIGNAL(clicked()), this, SLOT(CmResume()) );
		pResume->setText("&Resume");

        pStepAccept->setToolTip(trUtf8("Make next iteration in Stepwise mode" ) );
        pResume->setToolTip(trUtf8( "Make all iteration in Stepwise mode" ) );

	}
}


ProgressDialog* ProgressDialog::pDia = 0;

ProgressDialog::ProgressDialog(QWidget* parent,	bool step, bool autoclose_):
	QDialog( parent ),
    last_update(0),
	timer(0),
	autoclose(autoclose_)
{
    setupUi(this);
    
    pDia = this;
    TProfil::pm->userCancel = false;
    TProfil::pm->stepWise = step;

    calcThread = new CalcThread( this);

    //pStepAccept->hide();
    // switchToAccept(false);
    connect( pResume, SIGNAL(clicked()), this, SLOT(CmResume()) );

    if( step ) {
	    switchToAccept(false);
	    setWindowTitle( "Ready to proceed stepwise :)" );
	    calcThread->start();
    }
    else {
    	setWindowTitle( "Running..." );
        switchToAccept(true);
        pStepAccept->hide();
//        pResume->hide();
	    pClose->setText("&Cancel");

	timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()),
		this, SLOT(Run()) );

	calcThread->start();

	timer->start( pVisorImp->updateInterval()*100 /* qt3to4, TRUE*/ );
    }
//    t_start = clock();
    Update(true);
}


ProgressDialog::~ProgressDialog()
{
    delete calcThread;
// timer deleted with the parent
//    delete timer;
}

void ProgressDialog::languageChange()
{
    retranslateUi(this);
}

/*!
    Step
    called after pressing button step
    enforces calculation thread to proceed with another step
*/

void
ProgressDialog::CmStep()
{
    QString str;
//    MULTI* pData = TProfil::pm->pmp;

    try
    {
	if( calcThread->isRunning() ) {
		setWindowTitle( "Running next step..." );
            ThreadControl::wakeOne();	// let's calc
	}
	else {
	    if( calcThread->error.title == "" ) {
	    	setWindowTitle( "Running first step..." );
        ThreadControl::wakeOne();	// let's calc
//		calcThread->start();
	    }
	    else {
		throw calcThread->error;
	    }
	}

    // seems like wait(sec) bails out no matter what timeout we set up :( - may be Qt bug?
    // but we have to specify seconds to prevend deadlock anyway
//	ThreadControl::wait(5000); // 5 sec

	Update(true);
	pVisorImp->Update( true );

    str.sprintf("Stepped in %-15s", ThreadControl::GetPoint() );
    setWindowTitle( str );

	if( TProfil::pm->calcFinished ) {
        switchToAccept(true);
	    CalcFinished();
	    return;
	}
    }
    catch( TError& err )
    {
        vfMessage(this, err.title, err.mess);
    	pStepAccept->hide();
    }
}


/*!
    go to Step mode
*/

void
ProgressDialog::CmStop()
{

    try
    {
      timer->stop();
      TProfil::pm->stepWise = true;
     switchToAccept(false);

      pStepAccept->show();
   }
    catch( TError& err )
    {
        vfMessage(this, err.title, err.mess);
    }
}


void
ProgressDialog::CmResume()
{
    try
    {

     TProfil::pm->stepWise = false;
     switchToAccept(true);
     pStepAccept->hide();
     //pResume->hide();
     //pStepAccept->disconnect();
     ThreadControl::wakeOne();	// let's calc

     timer = new QTimer( this );
     connect( timer, SIGNAL(timeout()),	this, SLOT(Run()) );
     timer->start( pVisorImp->updateInterval()*100/* qt3to4, TRUE*/ );

     if( TProfil::pm->calcFinished ) {
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
ProgressDialog::Run()
{
    try
    {
	Update(true);
	pVisorImp->Update(true);

	if( TProfil::pm->calcFinished ) {
	    timer->stop();
	    CalcFinished();
	    return;
	}

	if( calcThread->isRunning() ) {
	    timer->start( pVisorImp->updateInterval()*100/* qt3t04, TRUE */);
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
ProgressDialog::CalcFinished()
{
    if( autoclose ) {
	close();
	return;
    }

    switchToAccept(true);

    pResume->hide();
    pStepAccept->show();
    pClose->setText("&Discard");
    pClose->setToolTip( trUtf8( "Do not save IPM results to database" ) );

    QString str;
    str.sprintf("Converged at DK=%.2g", TProfil::pm->pa.p.DK);
    setWindowTitle(str);
}

/*! CmAccept
    Saves system and exit
*/

void
ProgressDialog::CmAccept()
{
    try
    {
        pVisorImp->SaveSystem();
        close();
    }
    catch( TError& err)
    {
        vfMessage(this, err.title, err.mess);
    }
}

/*!
    Cancel and Discard commands
    If calculation is on tells calcThread to stop and exit
    waits for it to exit and closes the window
*/
void
ProgressDialog::CmClose()
{
    // we have to cancel calculation here
    // or discard changes
	if( calcThread->isRunning() ) {
	    TProfil::pm->userCancel = true;
            ThreadControl::wakeOne();	// let's tell the calc that all is over
	    calcThread->wait();
	}

//    calcTread->clean_up(); // will be called in closeEvent()
    close();
}


void
ProgressDialog::closeEvent(QCloseEvent* ev)
{
    if( NewSystemDialog::pDia )
        NewSystemDialog::pDia->resetList();
    pDia = 0;
    QDialog::closeEvent(ev);
}

/*! paintEvent
    paints the bottle with Gas/Liquid/Solid phases
*/

void
ProgressDialog::paintEvent(QPaintEvent* ev)
{
  QPainter p(this);
  QRect r = pBottle->geometry();
  r.setHeight( ht_g );
  p.fillRect(r, QBrush(Qt::white));
  r.setY( r.y()+ ht_g + 1);
  r.setHeight(ht_a );
  p.fillRect(r, QBrush(Qt::blue));
  r.setY( r.y()+ ht_a );
  r.setHeight(ht_s);
  p.fillRect(r, QBrush(Qt::black));

  QDialog::paintEvent(ev);
}


/*! Update
    updates the values on the ProgressDialog
*/
void
ProgressDialog::Update(bool force)
{
//	time_t elapsed = time(0) - last_update;

//    if( !force && elapsed < pVisorImp->updateInterval() )
    if( !force )
        return;

    QString str;
    MULTI* pData = TProfil::pm->pmp;

//    str.sprintf( "%*hu", 7, pData->IT );
    str.sprintf( "%2lu:%4lu:%4lu ", pData->W1+pData->K2, pData->ITF, pData->ITG ); // pData->IT );
    pIT->setText( str );
    str.sprintf( "%*g", 8, pData->pH );
    pPH->setText( str );
    str.sprintf( "%*g", 8, pData->pe );
    pPE->setText( str );
    str.sprintf( "%*g", 8, pData->IC );
    pIC->setText( str );

    pKey->setText(rt[RT_SYSEQ].PackKey());


    float g=0, a=0, s=0;
    for( int ii=0; ii<pData->FI; ii++ )
    {
        /// check if '+'
        switch( pData->PHC[ii] )
        {
        case 'g':
        case 'f':
            g += pData->FWGT[ii];
            break;
        case 'a':
            a += pData->FWGT[ii];
            break;
        case 'x':
        case 's':
            s += pData->FWGT[ii];
            break;
        }
    }

    str.sprintf( "%*g", 8, g );
    pGas->setText( str );
    str.sprintf( "%*g", 8, a );
    pWater->setText( str );
    str.sprintf( "%*g", 8, s );
    pSolid->setText( str );


    int ht = pBottle->height();
    float all = g + a + s;
    ht_g = (all!=0) ? int(ceil(g * ht / all)) :0;
    ht_a = (all!=0) ? int(ceil(a * ht / all)) :0;
    ht_s = (all!=0) ? int(ceil(s * ht / all)) :0;

    pProgress->setMaximum(7); //pProgress->setTotalSteps(7);

    int progr = (pData->DXM!=0) ? 7 - int(ceil(log10(fabs(pData->PCI/pData->DXM)+.1))) : 0;
    if (progr > 7)
        progr = 7;
    pProgress->setValue(progr);//pProgress->setProgress(progr);

//    clock_t t_end = clock();
//    clock_t dtime = ( t_end- t_start );
    str.sprintf("GEM IPM Minimization (pure run time: %lg s).",
       pData->t_elap_sec );   //  (double)dtime/(double)CLOCKS_PER_SEC);
    TextLabel1->setText(str);


//    last_update = time(0);
    update();
    // this really updates window when CPU is heavyly loaded
    qApp->processEvents();
}


//--------------------- End of ProgressDialog.cpp ---------------------------

