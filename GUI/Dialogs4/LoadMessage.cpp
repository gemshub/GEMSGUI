//-------------------------------------------------------------------
// $Id: LoadMessage.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of LoadMessage class
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

#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QProgressBar>
#include <QPushButton>

#include "LoadMessage.h"

//-------------------------------------------------------------------

LoadMessage* LoadMessage::pDia = nullptr;

LoadMessage::LoadMessage( QWidget* parent,
                          const char* name,
                          const char* msg,
                          int prog,
                          int total):
    QProgressDialog ( msg, "&Cancel", 0, 100, parent )
{
    pDia = this;

    setWindowModality(Qt::WindowModal);
    // change calcel button
    QPushButton *cancelBut = new QPushButton("");
    QIcon icon5;
    icon5.addFile(
                QString::fromUtf8(":/message/Icons/CancelIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
    cancelBut->setIcon(icon5);
    //QString styleTemp ="border-radius: 1em";
    //cancelBut->setStyleSheet(styleTemp);
    setCancelButton(cancelBut);

    QList<QLabel *>llab = findChildren<QLabel *>();
    QList<QProgressBar *>lbar = findChildren<QProgressBar *>();
    //lbar.at(0)->setAlignment(Qt::AlignHCenter);
    QList<QPushButton *>lbut = findChildren<QPushButton *>();
    QGridLayout *layoutTab = new QGridLayout(this);
    layoutTab->setContentsMargins( 0, 0, 0, 0 );
    layoutTab->setSpacing(2);
    //layoutTab->setRowStretch( 1, 0 );
    //layoutTab->setMargin(1);
    llab.at(0)->setAlignment( Qt::AlignTop|Qt::AlignHCenter);

    layoutTab->addWidget( lbut.at(0), 0,0 );
    layoutTab->addWidget( lbar.at(0), 0,1 );
    layoutTab->addWidget( llab.at(0), 1,1 );

    if( total > 0)
        setMaximum(total);
    setValue(prog);
    qApp->processEvents();
    setWindowTitle( name );
    //setAutoClose ( false );
    //setAutoReset ( false );
}

void LoadMessage::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
}

void LoadMessage::Update( const char* mess, int prog, int total )
{
    qApp->processEvents();
    if( total > 0)
        setMaximum(total);
    QString str = mess;
    setLabelText(str);
    setValue(prog);
    qApp->processEvents();
}

void LoadMessage::closeEvent(QCloseEvent* ev)
{
    pDia = nullptr;
    pVisorImp->setMenuEnabled( true );
    QProgressDialog::closeEvent(ev);
}

//-------------------------------------------------------------------

ProcessProgressDialog* ProcessProgressDialog::pDia = nullptr;

ProcessProgressDialog::ProcessProgressDialog( QWidget* parent, int anRT ):
    QDialog(parent), nRT(anRT)
{
    pDia = this;
    TProfil::pm->userCancel1 = false;
    TProfil::pm->stepWise1 = false;
    TProcess::pm->stepWise = false;
    TProcess::pm->userCancel = false;
    TGEM2MT::pm->stepWise = false;
    TGEM2MT::pm->userCancel = false;

    setWindowTitle( "Calculating ..." );

    QVBoxLayout* mainBox = new QVBoxLayout( this);

    label = new QLabel( this );
    label->setText( "Calculating ..."  );
    label->setAlignment(Qt::AlignHCenter);
    mainBox->addWidget( label );

    pProgress = new QProgressBar( this );
    pProgress->setMinimum( 0 );

    switch(nRT)
    {
    case RT_PROCES: TProcess::pm->stepWise = true;
        pProgress->setMaximum( TProcess::pm->pep->NR1 );
        break;
    case RT_GEM2MT: TGEM2MT::pm->stepWise = true;
        pProgress->setMaximum( static_cast<int>(TGEM2MT::pm->mtp->ntM) );
        break;
    default:        pProgress->setMaximum( 100. );
        break;
    }

    pProgress->setToolTip( "Shows number of steps" );
    mainBox->addWidget( pProgress );

    QHBoxLayout* buttonBox = new QHBoxLayout();

    pStopStep = new QPushButton( "&Step", this );
    pStopStep->setToolTip( "Make next iteration in Stepwise mode");
    connect( pStopStep, SIGNAL(clicked()), this, SLOT(CmStep()) );
    buttonBox->addWidget(pStopStep);

    pResume = new QPushButton( "&Resume", this );
    pResume->setToolTip("Make next iteration not in Stepwise mode" );
    connect( pResume, SIGNAL(clicked()), this, SLOT(CmResume()) );
    //pResume->hide();
    buttonBox->addWidget(pResume);

    pClose = new QPushButton( "&Cancel", this );
    pClose->setToolTip( "Cancel calculations" );
    connect( pClose, SIGNAL( clicked() ), this, SLOT( CmClose() ) );
    buttonBox->addWidget(pClose);

    pBreak = new QPushButton( "&Break", this );
    pBreak->setToolTip( "Break calculations"  );
    connect( pBreak, SIGNAL( clicked() ), this, SLOT( CmBreak() ) );
    buttonBox->addWidget(pBreak);

    mainBox->addLayout( buttonBox );

    switch(nRT)
    {
    case RT_PROCES: calcThread = new ProcessThread(this);
        break;
    case RT_GEM2MT: calcThread = new GEM2MTThread(this);
        break;
    default:        CalcFinished();
        break;
    }

    connect( calcThread, SIGNAL(finished()), this, SLOT(close()) );
    calcThread->start();
}

ProcessProgressDialog::~ProcessProgressDialog()
{

    //pVisorImp->setMenuEnabled( true );
    delete calcThread;
}

void ProcessProgressDialog::Update()
{
    switch(nRT)
    {
    case RT_PROCES: pProgress->setValue(TProcess::pm->pep->c_nrk);
        label->setText(TProcess::pm->Vmessage.c_str());
        setWindowTitle( TProcess::pm->pep->stkey );
        break;
    case RT_GEM2MT: pProgress->setValue(static_cast<int>(TGEM2MT::pm->mtp->ct));
        label->setText(TGEM2MT::pm->Vmessage.c_str());
        //setWindowTitle( TGEM2MT::pm->mtp->sykey );
        break;
    default:        break;
    }
}

// CalcFinished, close window, stop thread
void ProcessProgressDialog::CalcFinished()
{
    //    pVisorImp->setMenuEnabled( true );
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

        switch(nRT)
        {
        case RT_PROCES: if( TProcess::pm->calcFinished )
                CalcFinished();
            break;
        case RT_GEM2MT: if( TGEM2MT::pm->calcFinished )
                CalcFinished();
            break;
        default:        break;
        }
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
        switch(nRT)
        {
        case RT_PROCES: TProcess::pm->stepWise = false;
            break;
        case RT_GEM2MT: TGEM2MT::pm->stepWise = false;
            break;
        default:        break;
        }

        pResume->hide();
        pStopStep->disconnect();
        pStopStep->setText("&Stop");
        connect( pStopStep, SIGNAL(clicked()), this, SLOT(CmStop()) );

        //    calcThread->emitWakeOne();
        ThreadControl::wakeOne();	// let's calc

        switch(nRT)
        {
        case RT_PROCES: if( TProcess::pm->calcFinished )
                CalcFinished();
            break;
        case RT_GEM2MT: if( TGEM2MT::pm->calcFinished )
                CalcFinished();
            break;
        default:        break;
        }
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
        switch(nRT)
        {
        case RT_PROCES: TProcess::pm->stepWise = true;
            break;
        case RT_GEM2MT: TGEM2MT::pm->stepWise = true;
            break;
        default:        break;
        }
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
        switch(nRT)
        {
        case RT_PROCES: TProcess::pm->userCancel = true;
            TProcess::pm->pep->Istat = P_MT_MODE/*'5'mt mode*/;;
            break;
        case RT_GEM2MT: TGEM2MT::pm->userCancel = true;
            TGEM2MT::pm->mtp->iStat = 0/*'5'mt mode*/;
            break;
        default:       break;
        }
    }
    close();
}

void ProcessProgressDialog::CmBreak()
{
    // we have to cancel calculation here
    if( calcThread->isRunning() )
    {
        switch(nRT)
        {
        case RT_PROCES: TProcess::pm->userCancel = true;
            break;
        case RT_GEM2MT: TGEM2MT::pm->userCancel = true;
            TGEM2MT::pm->savePoint();
            break;
        default:       break;
        }
    }
    close();
}

void ProcessProgressDialog::closeEvent(QCloseEvent* ev)
{
    if( calcThread->isRunning() )
    {
        switch(nRT)
        {
        case RT_PROCES: TProcess::pm->userCancel = true;
            break;
        case RT_GEM2MT: TGEM2MT::pm->userCancel = true;
            break;
        default:       break;
        }
    }
    // calcThread->wait();
    ThreadControl::wakeOne();	// let's calc
    pDia = nullptr;
    pVisorImp->setMenuEnabled( true );
    QDialog::closeEvent(ev);
}

//--------------------- End of LoadMessage.cpp ---------------------------

