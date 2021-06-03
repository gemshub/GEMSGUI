//-------------------------------------------------------------------
// $Id: SettingsDialog.cpp 290 2004-07-07 11:11:38Z andy $
//
// Implementation of SettingsDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QFontDialog>

#include "ui_SettingsDialog4.h"
#include "SettingsDialog.h"
#include "service.h"
#include "visor.h"
#include "help.h"
#include "GemsMainWindow.h"


SettingsDialog::SettingsDialog (QWidget* parent)
    : QDialog(parent ),
      ui(new Ui::SettingsDialogData)
{
    ui->setupUi(this);
    cellFont = pVisorImp->getCellFont();

    setWindowTitle( "GEM-Selektor: Settings and preferences" );

    ui->pUpdateInterval->setValue( pVisorImp->updateInterval() );

    ui->pLocalDocDir->setText(pVisor->localDocDir().c_str());
    ui->pRemoteHTML->setText(pVisor->remoteHTML().c_str());
    //pLocalDoc->setChecked(pVisor->localDoc());
    //pRemoteDoc->setChecked(!pVisor->localDoc());
    ui->pSysDBDir->setText(pVisor->sysGEMDir().c_str());
    ui->pUserDBDir->setText(pVisor->userGEMDir().c_str());
    ui->pUserDBDir->setText(pVisor->userProfDir().c_str());
    ui->pFontRawName->setText(cellFont.toString());
    ui->pNumDigits->setValue(pVisorImp->getDoubleDigits());
    ui->pUpdateInterval->setValue(pVisorImp->updateInterval());
    ui->pConfigAutosave->setChecked(pVisorImp->getConfigAutosave());
    //    pConfigAutosave->setChecked( true );

    if( pVisor->getElemPrMode() )
    {   ui->rbNewPrMode->setChecked( true );
        ui->rbOldPrMode->setChecked( false );
    }
    else
    {   ui->rbNewPrMode->setChecked( false );
        ui->rbOldPrMode->setChecked( true );
    }
    ui->pBuiltinTDB->setText(pVisor->defaultBuiltinTDBL().c_str());

    QObject::connect(ui->pButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->pButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->pButtonHelp, SIGNAL(clicked()), this, SLOT(CmHelp()));
    QObject::connect(ui->pButtonChFont, SIGNAL(clicked()), this, SLOT(CmChangeFont()));
    QObject::connect(ui->pushButton5, SIGNAL(clicked()), this, SLOT(CmDefaultFont()));
    connect(ui->butGenerate, SIGNAL(clicked()), this, SLOT(CmHelpGenerate()));

    ui->pFontRawName->setReadOnly(true);	// no meaning for Win32 (now)
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::accept()
{
    CmApply();
    QDialog::accept();
}

void SettingsDialog::CmApply()
{
    pVisorImp->setCellFont(cellFont);

    pVisorImp->setDoubleDigits(ui->pNumDigits->value());
    pVisorImp->setUpdateInterval( ui->pUpdateInterval->value() );
    pVisorImp->setConfigAutosave( ui->pConfigAutosave->isChecked() );
    pVisor->setElemPrMode(ui->rbNewPrMode->isChecked());

    pVisor->setLocalDocDir(ui->pLocalDocDir->text().toStdString());
    pVisor->setRemoteHTML(ui->pRemoteHTML->text().toStdString());

    pVisor->setDefaultBuiltinTDB(ui->pBuiltinTDB->text().toStdString());

    //pVisor->setLocalDoc(pLocalDoc->isChecked());

    //pVisorImp->Update(true);
}


void SettingsDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_SETUP_HTML, nullptr );
}

void SettingsDialog::CmHelpGenerate()
{
    try
    {
        QString qhpFile = ui->pRemoteHTML->text()+"gems3helpconfig.qhp";
        HelpConfigurator rr;
        if( rr.readDir(ui->pRemoteHTML->text().toLatin1().data()))
            rr.writeFile(qhpFile.toLatin1().data());

        //if( HelpWindow::pDia )
        //{
        //   HelpWindow::pDia->close();
        //   delete HelpWindow::pDia;
        //   build new file
        if(!pVisorImp->proc) {
            pVisorImp->proc = new QProcess();
        }
        if (pVisorImp->proc->state() != QProcess::Running)
        {

            QString docPath =  ui->pRemoteHTML->text();
            QString app;
#ifndef _WIN32
#ifdef __APPLE__
            //                    app += QLatin1String("/Applications/Gems3.app/Contents/MacOS/qcollectiongenerator");    // expected to work
            app += QLatin1String("qcollectiongenerator");
#else
            // app = pVisor->sysGEMDir().c_str() + QLatin1String("/qcollectiongenerator");
            //                   app = QLatin1String(getenv("HOME"));
            //                   app += QLatin1String("/Gems3-app/qcollectiongenerator");
            app += QLatin1String("qcollectiongenerator");
#endif
#else    // windows
            app += QLatin1String("qcollectiongenerator.exe");
#endif
            QStringList args;
            args << docPath + QLatin1String("gems3helpconfig.qhcp")
                 << QLatin1String("-o")
                 << docPath + QLatin1String("gems3help.qhc");
            ;

            pVisorImp->proc->start(app, args);
            cout << app.toStdString() << endl;
            cout << args[2].toStdString() << endl;
            
            if (!pVisorImp->proc->waitForStarted())
            {
                Error( "Gems3", "Unable to launch qcollectiongenerator");
            }
        }
        // open it
        //   (new HelpWindow(  0  ));
        // }

    }
    catch(TError& e)
    {
        vfMessage(this, e.title.c_str(), e.mess.c_str() );
    }
}


void SettingsDialog::CmSysDBDirSelect()
{
    qWarning("SettingsDialogData::CmSysDBDirSelect(): Not implemented yet");
}

void SettingsDialog::CmUserDBDirSelect()
{
    qWarning("SettingsDialogData::CmUserDBDirSelect(): Not implemented yet");
}

/*
void
SettingsDialog::CmSysDBDirSelect()
{
    QString dir( pDBDir->text() );

    QFileDialog file_dlg( dir, QString::null, this, 0, true);
    file_dlg.setMode(QFileDialog::DirectoryOnly);
    if( file_dlg.exec() )
    {
//	pDBDir->setText( file_dlg.dirPath() );
    pDBDir->setText( "Changing DB dirs on the fly is not supported! :(");
    }
}


void
SettingsDialog::CmUserDBDirSelect()
{
    QString dir( pUserDBDir->text() );

    QFileDialog file_dlg( dir, QString::null, this, 0, true);
    file_dlg.setMode(QFileDialog::DirectoryOnly);
    if( file_dlg.exec() )
    {
//	pUserDBDir->setText( file_dlg.dirPath() );
    pUserDBDir->setText( "Changing DB dirs on the fly is not supported! :(");
    }
}
*/

void SettingsDialog::CmChangeFont()
{
    bool ok;
    QFont selected_font = QFontDialog::getFont(&ok, cellFont, this);

    if( ok )
    {
        cellFont = selected_font;
        ui->pFontRawName->setText( cellFont.toString() );
    }
}

void SettingsDialog::CmDefaultFont()
{
    cellFont = pVisorImp->getDefaultFont();
    ui->pFontRawName->setText( cellFont.toString() );
}

//--------------------- End of SettingsDialog.cpp ---------------------------

