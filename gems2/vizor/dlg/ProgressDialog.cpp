//-------------------------------------------------------------------
// $Id$
//
// Implementation of ProgressDialog class
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

#include <math.h>
#include <iostream.h>

#include <qapplication.h>
#include <qprogressbar.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qthread.h>
#include <qtimer.h>
#include "calcthread.h"

#include "ProgressDialog.h"
#include "m_param.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"



#define Inherited ProgressDialogData

void 
ProgressDialog::switchToAccept(bool isAccept)
{
	if( isAccept ) {
		pStepAccept->disconnect();
		connect( pStepAccept, SIGNAL(clicked()), this, SLOT(CmAccept()) );
		pStepAccept->setText("&Accept");
	}
	else {
		pStepAccept->disconnect();
		connect( pStepAccept, SIGNAL(clicked()), this, SLOT(CmStep()) );
		pStepAccept->setText("&Step");
	}
}

#ifdef Use_mt_mode


ProgressDialog* ProgressDialog::pDia = 0;

ProgressDialog::ProgressDialog(QWidget* parent,	bool step, bool autoclose_):
        Inherited( parent, 0, /*false*/autoclose_ ),
        last_update(0),
	timer(0),
	autoclose(autoclose_)
{
    pDia = this;
    TProfil::pm->userCancel = false;
    TProfil::pm->stepWise = step;

    calcThread = new CalcThread();

    //pStepAccept->hide();
	switchToAccept(false);

    Update(true);

    if( step ) {
        setCaption( "Ready to rumble..." );
    }
    else {
	setCaption( "Running..." );
      pStepAccept->hide();
	pClose->setText("&Cancel");

	timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()),
		this, SLOT(Run()) );

	calcThread->start();

	timer->start( pVisorImp->updateInterval()*100, TRUE );
    }
}


ProgressDialog::~ProgressDialog()
{
    delete calcThread;
// timer deleted with the parent
//    delete timer;  
}


/*!
    Step
    called after pressing button step
    enforces calculation thread to proceed with another step
*/

void
ProgressDialog::CmStep()
{
    try
    {
	if( calcThread->running() ) {
	    setCaption( "Running next step..." );
	    pVisorImp->getWaitCalc().wakeOne();	// let's calc
	}
	else {
	    if( calcThread->error.title == "" ) {
		setCaption( "Running first step..." );
		calcThread->start();
	    }
	    else {
		throw calcThread->error;
	    }
	}	

/*	if( pVisorImp->getWaitProgress().wait(10000) == FALSE )
	{
	    vfMessage(this, "CalcThread", "One step in calc thread is more than 10 sec!!!");
	}
*/

    // seems like wait(sec) bails out no matter what timeout we set up :( - may be Qt bug?
    // but we have to specify seconds to prevend deadlock anyway
	pVisorImp->getWaitProgress().wait(5000); // 5 sec

	Update(true);
	pVisorImp->Update( true );
	setCaption( "Stepped..." );

	if( TProfil::pm->calcFinished ) {
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
	timer->stop();
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
    
    pStepAccept->show();
    pClose->setText("&Discard");

    QString str;
    str.sprintf("Converged at DK=%.2g", TProfil::pm->pa.p.DK);
    setCaption(str);
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
	if( calcThread->running() ) {
	    TProfil::pm->userCancel = true;
	    pVisorImp->getWaitCalc().wakeOne();	// let's tell the calc that all is over
	    calcThread->wait();
	}
    
//    calcTread->clean_up(); // will be called in closeEvent()
    close();
}


void
ProgressDialog::closeEvent(QCloseEvent* ev)
{
    pDia = 0;
    QDialog::closeEvent(ev);
}

/*! paintEvent
    paints the bottle with Gas/Liquid/Solid phases
*/

void
ProgressDialog::paintEvent(QPaintEvent* ev)
{
    QRect r(0, 0, pBottle->width(), ht_g);

    QPainter p(pBottle);
    p.fillRect(r, QBrush(/*Qt::*/white));
    r.setY(ht_g + 1);
    r.setHeight(ht_a);
    p.fillRect(r, QBrush(/*Qt::*/blue));
    r.setY(ht_a + ht_g + 1);
    r.setHeight(ht_s);
    p.fillRect(r, QBrush(/*Qt::*/black));

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
    str.sprintf( "%2hu:%4hu", pData->W1, pData->IT );
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

    pProgress->setTotalSteps(7);

    int progr = (pData->DX!=0) ? 7 - int(ceil(log10(fabs(pData->PCI/pData->DX)+.1))) : 0;
    if (progr > 7)
        progr = 7;
    pProgress->setProgress(progr);

//    last_update = time(0);
    update();
    // this really updates window when CPU is heavyly loaded
    qApp->processEvents();
}

#else

ProgressDialog* ProgressDialog::pDia = 0;

ProgressDialog::ProgressDialog(QWidget* parent,	bool step):
        Inherited( parent, 0, false ),
        last_update(0)
{
    pDia = this;
    TProfil::pm->fStopCalc = false;

    setCaption( "Running..." );

    if( !step )
	switchToAccept(true);
    else
	switchToAccept(false);

    Update(true);
}


ProgressDialog::~ProgressDialog()
{}

void
ProgressDialog::Run()
{
}

#include "HLresDialog.h"

void
ProgressDialog::CmAccept()
{
    if( !HLresDialog::pDia )
        (new HLresDialog(this->topLevelWidget()))->show();
    else
        HLresDialog::pDia->raise();

}


void
ProgressDialog::CmStep()
{
    try
    {
        //     if( TProfil::pm->DebagCalcEqstatStep()==true )
        //     {
        //        vfMessage(this, "!!!!", "End calc ");
        //     	pStep->setEnabled( false );
        //     }
        //     pVisor->Update();
        pVisorImp->SaveSystem();
        close();
    }
    catch( TError& err)
    {
        vfMessage(this, err.title, err.mess);
    }
}

void
ProgressDialog::CmClose()
{
    // we have to cancel calculation here
    // or discard changes
    TProfil::pm->fStopCalc = true;

    close();
}


void
ProgressDialog::closeEvent(QCloseEvent* ev)
{
    pDia = 0;
    QDialog::closeEvent(ev);
}

void
ProgressDialog::paintEvent(QPaintEvent* ev)
{
    QRect r(0, 0, pBottle->width(), ht_g);

    QPainter p(pBottle);
    p.fillRect(r, QBrush(/*Qt::*/white));
    r.setY(ht_g + 1);
    r.setHeight(ht_a);
    p.fillRect(r, QBrush(/*Qt::*/blue));
    r.setY(ht_a + ht_g + 1);
    r.setHeight(ht_s);
    p.fillRect(r, QBrush(/*Qt::*/black));

    QDialog::paintEvent(ev);
}


void
ProgressDialog::CalcFinished()
{
    MULTI* pData = TProfil::pm->pmp;

    pClose->setText("&Discard");
    switchToAccept(true);
    QString str;
    str.sprintf("Converged at DK=%.2g", pData->DX );
    setCaption(str);
}


void
ProgressDialog::Update(bool force)
{
    time_t elapsed = time(0) - last_update;

    if( !force && elapsed < pVisorImp->updateInterval() )
        return;


    QString str;
    MULTI* pData = TProfil::pm->pmp;

//    str.sprintf( "%*hu", 7, pData->IT );
    str.sprintf( "%2hu:%4hu", pData->W1, pData->IT );
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

    pProgress->setTotalSteps(7);

    int progr = (pData->DX!=0) ? 7 - int(ceil(log10(fabs(pData->PCI/pData->DX)+.1))) : 0;
    if (progr > 7)
        progr = 7;
    pProgress->setProgress(progr);

    last_update = time(0);
    update();
    // this really updates window when CPU is heavyly loaded
    qApp->processEvents();
}

#endif

//--------------------- End of ProgressDialog.cpp ---------------------------

