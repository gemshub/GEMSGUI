//-------------------------------------------------------------------
// $Id: ProgressDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ProgressDialog class
//
// Copyright (C) 1996-2009  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QMutex>
#include "ProgressDialog.h"
#include "ui_ProgressDialog4.h"
#include "NewSystemDialog.h"
#include "m_param.h"

void IPNCalcObject::IPM_run()
{
    try
    {
        gui_logger->debug("IPM_run");
        QMutexLocker  loker(&pVisorImp->getMutexCalc());
        showMss = 1L;
        double dummy = -1.;
        TProfil::pm->CalcEqstat( dummy, -1, 0. );
        emit IPM_OK();
    }
    catch( TError& xcpt )
    {
        emit IPM_exception( xcpt.mess );
    }
    catch(...)
    {
        emit IPM_exception( "IPM undefined error" );
    }

    emit IPM_finish();
}

//=============================================================================

ProgressDialog* ProgressDialog::pDia = nullptr;

void ProgressDialog::switchToAccept(bool isAccept)
{
    ui->pClose->setToolTip( tr( "Cancel this GEM IPM calculation" ) );

    if( isAccept ) {
        ui->pAccept->disconnect();
        connect( ui->pAccept, SIGNAL(clicked()), this, SLOT(CmAccept()) );
        ui->pAccept->setToolTip( tr("Close and save results to SysEq database record" ) );
        ui->pAccept->show();
    }
    else {
        ui->pAccept->disconnect();
    }
}


ProgressDialog::ProgressDialog(QWidget* parent ):
    QDialog( parent ),
    ui(new Ui::ProgressDialogData)
{
    ui->setupUi(this);
    
    pDia = this;
    //TProfil::pm->userCancel = false;
    //TProfil::pm->stepWise = step;
    ui->pAccept->hide();

    setWindowTitle( "Running..." );
    ui->pAccept->hide();
    ui->pClose->setText("&Cancel");
    Update(true);
}


ProgressDialog::~ProgressDialog()
{
     delete ui;
}


/*!	CalcFinished
    changes buttons, their names and caption
*/

void ProgressDialog::CalcFinished()
{
    switchToAccept(true);
    ui->pAccept->show();
    ui->pClose->setText("&Dismiss");
    ui->pClose->setToolTip( tr( "Close and do not save results to SysEq database record" ) );
    QString str = QString("Converged at DK=%1").arg(TMulti::sm->GetPM()->PCI);
    setWindowTitle(str);
    Update(true);
    //pVisorImp->Update(true);
}

/*! CmAccept
    Saves system and exit
*/

void ProgressDialog::CmAccept()
{
    try
    {
        pVisorImp->CmSave();
        close();
    }
    catch( TError& err)
    {
        vfMessage(this, err.title, err.mess);
    }
}

//
//    Cancel and Dismiss commands
//    If calculation is on, tells calcThread to stop and exit
//    waits for it to exit and closes the window
//
void ProgressDialog::CmClose()
{
    // we have to cancel calculation here ????
    // or discard changes
    ///	if( calcThread->isRunning() ) {
    ///	    TProfil::pm->userCancel = true;
    ///            ThreadControl::wakeOne();	// let's tell the calc that all is over
    ///	    calcThread->wait();
    ///	}

    //    calcTread->clean_up(); // will be called in closeEvent()
    close();
}


void ProgressDialog::closeEvent(QCloseEvent* ev)
{
    if( NewSystemDialog::pDia )
        NewSystemDialog::pDia->resetList();
    // pDia = nullptr;
    QDialog::closeEvent(ev);
}

// paintEvent
//    paints the bottle with Gas/Liquid/Solid phases
//

void ProgressDialog::paintEvent(QPaintEvent* ev)
{
    QPainter p(this);
    QRect r = ui->pBottle->geometry();
    r.setHeight( ht_g );
    p.fillRect(r, QBrush(Qt::white));
    r.setY( r.y()+ ht_g + 1);
    r.setHeight(ht_a );
    p.fillRect(r, QBrush(Qt::blue));
    r.setY( r.y()+ ht_a );
    r.setHeight(ht_l);
    p.fillRect(r, QBrush(Qt::green));
    r.setY( r.y()+ ht_l );
    r.setHeight(ht_s);
    p.fillRect(r, QBrush(Qt::black));

    QDialog::paintEvent(ev);
}


//! Update
//    updates the values on the ProgressDialog
//
void ProgressDialog::Update(bool force)
{
    //	time_t elapsed = time(0) - last_update;

    //    if( !force && elapsed < pVisorImp->updateInterval() )
    if( !force )
        return;

    //QString str;
    MULTI* pData = TMulti::sm->GetPM();

    //str.sprintf( "%2lu:%4lu:%4lu ", pData->W1+pData->K2, pData->ITF, pData->ITG ); // pData->IT );
    //str.sprintf( "%4lu ",  pData->IT );
    ui->pIT->setText( QString("%1:%2:%3").arg(pData->W1+pData->K2,2).arg(pData->ITF,4).arg(pData->ITG,4) );
    //str.sprintf( "%*g", 8, pData->pH );
    ui->pPH->setText(  QString("%1").arg(pData->pH,8) );
    //str.sprintf( "%*g", 8, pData->pe );
    ui->pPE->setText(  QString("%1").arg(pData->pe,8) );
    //str.sprintf( "%*g", 8, pData->IC );
    ui->pIC->setText(  QString("%1").arg(pData->IC,8) );

    ui->pKey->setText(rt[RT_SYSEQ]->PackKey());

    double g=0, a=0, s=0, l=0;
    for( int ii=0; ii<pData->FI; ii++ )
    {
        /// check if '+'
        switch( pData->PHC[ii] )
        {
        case 'g': case 'p':
        case 'f':
            g += pData->FWGT[ii];
            break;
        case 'a':
            a += pData->FWGT[ii];
            break;
        case 'x': case 'd':
        case 's': case 'y':
            s += pData->FWGT[ii];
            break;
        case 'l': case 'h':
        case 'm':
            l += pData->FWGT[ii];
            break;
        }
    }

    //str.sprintf( "%*g", 8, g );
    ui->pGas->setText(  QString("%1").arg(g,8) );
    //str.sprintf( "%*g", 8, a );
    ui->pWater->setText(  QString("%1").arg(a,8) );
    //str.sprintf( "%*g", 8, l );
    ui->pLiquid->setText(  QString("%1").arg(l,8) );
    //str.sprintf( "%*g", 8, s );
    ui->pSolid->setText(  QString("%1").arg(s,8) );


    int ht = ui->pBottle->height();
    double all = g + a + s +l;
    ht_g = noZero(all) ? int(ceil(g * ht / all)) :0;
    ht_a = noZero(all) ? int(ceil(a * ht / all)) :0;
    ht_l = noZero(all) ? int(ceil(l * ht / all)) :0;
    ht_s = noZero(all) ? int(ceil(s * ht / all)) :0;

    int progr = 24;
    ui->pProgress->setMaximum(progr);
    double dist = progr/6.;
    if( pData->PCI >0. && pData->DXM >0.)
        dist = log10( pData->PCI/ pData->DXM );
    progr -= int(floor(dist*6.));
    if(progr < 0 )
        progr = 0;
    if(progr > 24 )
        progr = 24;
    ui->pProgress->setValue(progr);//pProgress->setProgress(progr);

    //    clock_t t_end = clock();
    //    clock_t dtime = ( t_end- t_start );
    //str.sprintf("GEM IPM calculation (run time: %lg s).",
    //            pData->t_elap_sec );   //  (double)dtime/(double)CLOCKS_PER_SEC);
    ui->TextLabel1->setText(  QString("GEM IPM calculation (run time: %1 s).").arg(pData->t_elap_sec) );
    //    last_update = time(0);
    update();
    // this really updates window when CPU is heavily loaded
    qApp->processEvents();
}


//--------------------- End of ProgressDialog.cpp ---------------------------


