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
#include "GemsMainWindow.h"


SettingsDialog::SettingsDialog (QWidget* parent)
    : QDialog(parent ),
      ui(new Ui::SettingsDialogData)
{
    ui->setupUi(this);
    cellFont = pVisorImp->getCellFont();

    setWindowTitle( "GEM-Selektor: Settings and preferences" );

    ui->pUpdateInterval->setValue( pVisorImp->updateInterval() );

    ui->comboBox->setCurrentIndex(pVisorImp->getColorScheme());
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

    pVisorImp->setColorScheme(ui->comboBox->currentIndex());
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

void SettingsDialog::CmSysDBDirSelect()
{
    qWarning("SettingsDialogData::CmSysDBDirSelect(): Not implemented yet");
}

void SettingsDialog::CmUserDBDirSelect()
{
    qWarning("SettingsDialogData::CmUserDBDirSelect(): Not implemented yet");
}

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

