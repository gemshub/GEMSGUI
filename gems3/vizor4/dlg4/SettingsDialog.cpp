//-------------------------------------------------------------------
// $Id: SettingsDialog.cpp 290 2004-07-07 11:11:38Z andy $
//
// Implementation of SettingsDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
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

const char *GEMS_SETUP_HTML = "gemsetup";

#include <qfontdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qvariant.h>


#include "SettingsDialog.h"

#include "visor_w.h"
#include "visor.h"

SettingsDialog::SettingsDialog (QWidget* parent)
    : QDialog(parent )
{

    setupUi(this);
    cellFont = pVisorImp->getCellFont();

    setWindowTitle( "GEM-Selektor: Settings and preferences" );

    pUpdateInterval->setValue( pVisorImp->updateInterval() );

    pLocalDocDir->setText(pVisor->localDocDir().c_str());
    pRemoteDocURL->setText(pVisor->remoteDocURL().c_str());
    pLocalDoc->setChecked(pVisor->localDoc());
    pRemoteDoc->setChecked(!pVisor->localDoc());
    pSysDBDir->setText(pVisor->sysGEMDir().c_str());
    pUserDBDir->setText(pVisor->userGEMDir().c_str());
    pFontRawName->setText(cellFont.toString());
    pNumDigits->setValue(pVisorImp->getDoubleDigits());
    pUpdateInterval->setValue(pVisorImp->updateInterval());
    pConfigAutosave->setChecked(pVisorImp->getConfigAutosave());
//    pConfigAutosave->setChecked( true );

    if( pVisor->getElemPrMode() )
    {   rbNewPrMode->setChecked( true );
        rbOldPrMode->setChecked( false );
    }
    else
    {   rbNewPrMode->setChecked( false );
        rbOldPrMode->setChecked( true );
    }

    pFontRawName->setReadOnly(true);	// no meaning for Win32 (now)
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::languageChange()
{
    retranslateUi(this);
}

void
SettingsDialog::accept()
{
    CmApply();
    QDialog::accept();
}

void
SettingsDialog::CmApply()
{
    pVisorImp->setCellFont(cellFont);

    pVisorImp->setDoubleDigits(pNumDigits->value());
    pVisorImp->setUpdateInterval( pUpdateInterval->value() );
    pVisorImp->setConfigAutosave( pConfigAutosave->isChecked() );
    pVisor->setElemPrMode(rbNewPrMode->isChecked());

    pVisor->setLocalDocDir(pLocalDocDir->text().toLatin1().data());
    pVisor->setRemoteDocURL(pRemoteDocURL->text().toLatin1().data());
    pVisor->setLocalDoc(pLocalDoc->isChecked());

    //pVisorImp->Update(true);
}


void
SettingsDialog::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_SETUP_HTML, 0, this, true );
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

void
SettingsDialog::CmChangeFont()
{
    bool ok;
    QFont selected_font = QFontDialog::getFont(&ok, cellFont, this);

    if( ok )
    {
        cellFont = selected_font;
        pFontRawName->setText( cellFont.toString() );
    }
}

void
SettingsDialog::CmDefaultFont()
{
    cellFont = pVisorImp->getDefaultFont();
    pFontRawName->setText( cellFont.toString() );
}

//--------------------- End of SettingsDialog.cpp ---------------------------
