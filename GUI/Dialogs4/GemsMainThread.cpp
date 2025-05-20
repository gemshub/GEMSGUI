//-------------------------------------------------------------------
// $Id: module_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GemsMainWindow class
//
// Copyright (C) 2020  S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "visor.h"
#include "GemsMainWindow.h"
#include "ui_GemsMainWindow4.h"
#include "NewSystemDialog.h"
#include "ProgressDialog.h"
#include "LoadMessage.h"

void TVisorImp::CmRunIPM()
{
    /* ?? TCModuleImp *actwin = activeMdiChild();
    if( actwin )
        aMod[actwin->rtNum()]->clearEditFocus();
    else
    {
        NewSystemDialog *wn = activeNewSystem();
        if( wn )
            wn->clearEditFocus();
    } */

    TMulti::sm->GetPM()->pNP =
        ( ui->sactionSimplex->isChecked())? 0: 1;
    if( ui->sactionPrecise->isChecked() && !TProfil::pm->pa.p.PRD )
        TProfil::pm->pa.p.PRD = -5; // Changed
    TProfil::pm->pa.p.PRD =
        ( !ui->sactionPrecise->isChecked())? 0: TProfil::pm->pa.p.PRD;

    try
    {
        TProfil::pm->userCancel1 = false;
        if( !ProgressDialog::pDia )
        {
            auto dlg = new ProgressDialog(this);
            if(calc_model) {
                connect( calc_model, &IPNCalcObject::IPM_OK, dlg, &ProgressDialog::CalcFinished );  //!!!!
                //connect( calc_model, &IPNCalcObject::IPM_exception, dlg, &ProgressDialog::close );
            }
        }
        ProgressDialog::pDia->startThread(ui->sactionStepwise->isChecked());
    }
    catch( TError& err)
    {
        Update(true);
        vfMessage(this, err.title, err.mess);
    }
    NewSystemDialog::pDia->setCurrentTab(1);
}

void TVisorImp::finish_IPN()
{
    gui_logger->debug("finish_IPN");
    Update(true);
}

void TVisorImp::error_IPN( std::string err_mess )
{
    gui_logger->error("error_IPN");
    //Update(true);
    vfMessage(this, "error_IPN", err_mess );
    ProgressDialog::pDia->close();
}

void TVisorImp::setCalcClient()
{
    qRegisterMetaType<std::string>("std::string");
    try{

        gui_logger->debug("setCalcClient");
#ifdef USE_GEMS3K_SERVER
        calc_model = new IPNCalcObject();

        // link from GUI
        connect( this, &TVisorImp::run_IPM, calc_model, &IPNCalcObject::IPM_run );

        // link to GUI
        connect( calc_model, &IPNCalcObject::IPM_finish, this, &TVisorImp::finish_IPN );
        connect( calc_model, &IPNCalcObject::IPM_exception, this, &TVisorImp::error_IPN );

        // thread functions
        calc_model->moveToThread(&calc_thread);
        connect(&calc_thread, &QThread::finished, calc_model, &QObject::deleteLater);
        calc_thread.start();
#endif
    }
    catch(std::exception& e)
    {
        gui_logger->error("setCalcClient internal comment: {}", e.what());
        throw;
    }
}

/*
void TVisorImp::startGEMServer()
{
    try
    {
        killGEMServer();

        if (!GEMS3_proc)
        {
            GEMS3_proc = new QProcess();
            GEMS3_proc->setReadChannel(QProcess::StandardOutput);
            GEMS3_proc->setProcessChannelMode(QProcess::MergedChannels);
            GEMS3_proc->setCurrentReadChannel(QProcess::StandardOutput);
            connect(GEMS3_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
            connect(GEMS3_proc, SIGNAL(readyRead()), this, SLOT(readOutput()));
            connect( GEMS3_proc, SIGNAL(errorOccurred(QProcess::ProcessError)),
                     this, SLOT(GEMServerErrorOccurred(QProcess::ProcessError)));
        }

        if (GEMS3_proc->state() != QProcess::Running)
        {
            QString serverPath =  pVisor->serverGems3Dir().c_str();
            //"/home/sveta/devGEMS/gitGEMS3/standalone/gemserver-build";
            QString app;
#ifndef _WIN32
#ifdef __APPLE__
            //                    app += QLatin1String("/Applications/Gems3.app/Contents/MacOS/qcollectiongenerator");    // expected to work
            app += QLatin1String("gems3_server");
#else
            app += QLatin1String("gems3_server");
#endif
#else    // windows
            app += QLatin1String("gems3_server.exe");
#endif

            GEMS3_proc->setWorkingDirectory(serverPath);
            QStringList argumentos;
            //argumentos << "-P" << filehex  << "-Q" << "-V" <<  "after_programming";
            GEMS3_proc->start(app,argumentos);
            gui_logger->info("Start gems3 server: {}", app.toStdString());

            if(!GEMS3_proc->waitForStarted(-1))
            {
                Error( "Gems3 server", "gems3_server");
            }
        }
    }
    catch(TError& e)
    {
        gui_logger->error("Start gems3 server error: {}", e.mess.c_str());
        vfMessage(this, e.title.c_str(), e.mess.c_str() );
    }
    catch(...)
    {
        gui_logger->error("Start GEMS3 server error occurred");
    }
}

void TVisorImp::readOutput()
{
    while( GEMS3_proc->canReadLine() )
    {
        QByteArray linea = GEMS3_proc->readLine();
        // const std::string green("\033[1;32m");
        // const std::string reset("\033[0m");
        // c out << green << "GEMS3 server: " << reset << linea.toStdString()  << endl;
        gui_logger->info("GEMS3 server: {}", linea.toStdString());
    }
}

void TVisorImp::killGEMServer()
{
    if (GEMS3_proc && GEMS3_proc->state() == QProcess::Running)
    {
        disconnect( GEMS3_proc, SIGNAL(errorOccurred(QProcess::ProcessError)),
                    this, SLOT(GEMServerErrorOccurred(QProcess::ProcessError)));
        GEMS3_proc->terminate();
        GEMS3_proc->waitForFinished(3000);
        // terminal> killall gems3_server
    }
    delete GEMS3_proc;
    GEMS3_proc = nullptr;
}

void TVisorImp::GEMServerErrorOccurred(QProcess::ProcessError error)
{
    gui_logger->error("GEMS3 server error occurred:  {}", static_cast<int>(error));
    // try restart server
    if( error >0 )
        startGEMServer();
}
*/
//------------------------------------------------------------------------------------------

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
        return;
    }

    if( ProgressDialog::pDia )
        ProgressDialog::pDia->Update(force);

    if( ProcessProgressDialog::pDia )
        ProcessProgressDialog::pDia->Update();

    if( NewSystemDialog::pDia )
        NewSystemDialog::pDia->Update();

    for( size_t ii=0; ii<aMod.size(); ii++ )
        aMod[ii]->Update(force);

    int nrt = nRTofActiveSubWindow();
    if( nrt>=0 )
        pLine->setText(tr(rt[nrt]->PackKey()));
}

bool TVisorImp::Message( QWidget* /*parent*/, const char* name,
                         const char* msg, int prog, int total, bool /*move*/ )
{
    if( LoadMessage::pDia )
    {

        if( LoadMessage::pDia->wasCanceled() )
        {
            LoadMessage::pDia->setValue(LoadMessage::pDia->maximum());
            return true;
        }
        LoadMessage::pDia->Update(msg, prog, total);
    }
    else
    {
        //--QPushButton *btn = new QPushButton();
        LoadMessage* mssg = new LoadMessage(nullptr/* parent*/, name, msg, prog, total);
        //--mssg->setCancelButton(btn);
        layout2->addWidget(mssg);
        qApp->processEvents();
        bool enabled = !(pVisor->ProfileMode == MDD_SYSTEM && LoadMessage::pDia );
        setMenuEnabled( enabled );
        connect( mssg, SIGNAL(canceled()), this, SLOT(setMenuEnabled()) );

        // (new LoadMessage( parent, name, msg, prog, total))->show();
        //  if( move && parent  )
        //    LoadMessage::pDia->move(parent->x()+parent->width(), parent->y());
    }

    return false;
}

void TVisorImp::CloseMessage()
{
    if( LoadMessage::pDia )
    {
        LoadMessage::pDia->setValue(LoadMessage::pDia->maximum());
        LoadMessage::pDia->close();
    }
}

void TVisorImp::ProcessProgress( QWidget* /*parent*/, int nRT )
{
    TProcess::pm->userCancel = false;
    TGEM2MT::pm->userCancel = false;

    ProcessProgressDialog* dlg =
            new ProcessProgressDialog( nullptr/*parent*/, nRT );
    //   dlg->show();
    layout2->addWidget(dlg);
    bool enabled = !(pVisor->ProfileMode == MDD_SYSTEM );
    setMenuEnabled( enabled );
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
                                thdata.seli, thdata.all_no  );
        break;
    case thExcludeFillEdit:
        thdata.res = vfExcludeFillEdit( par, thdata.title.c_str(),
                                        thdata.list, thdata.sel, thdata.fill_data );
        break;
    case thQuestionYesNoAll:
        thdata.res = vfQuestionYesNoAll( par, thdata.title, thdata.mess, thdata.s1 );
        break;
    default:
        break;
    }
    thdata.wait = false;
    ThreadControl::wakeOne();
}


//QMutex updateMutex;
static QMutex calcMutex;
static QWaitCondition calcWait;
static QWaitCondition progressWait;

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

bool ThreadControl::wait(unsigned long /*time*/ )
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


//===================================================================

