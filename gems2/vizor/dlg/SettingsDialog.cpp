//-------------------------------------------------------------------
// Id: gems/vizor/dlg/SettingsDialog.cpp  version 2.0.0    2001
//
// Implementation of SettingsDialog class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
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

#include "SettingsDialog.h"

#include "visor_w.h"
#include "visor.h"

#define Inherited SettingsDialogData

SettingsDialog::SettingsDialog (QWidget* parent)
        :
        Inherited( parent, 0, true ),
        cellFont( pVisorImp->getCellFont() )
{
    setCaption( "GEM-Selektor: Settings and preferences" );

    pUpdateInterval->setValue( pVisorImp->updateInterval() );

    pSysDBDir->setText(pVisor->sysGEMDir().c_str());
    pUserDBDir->setText(pVisor->userGEMDir().c_str());
    pFontRawName->setText(cellFont.rawName());
    pNumDigits->setValue(pVisorImp->getDoubleDigits());
    pUpdateInterval->setValue(pVisorImp->updateInterval());
    pConfigAutosave->setChecked(pVisorImp->getConfigAutosave());

#ifndef __unix
    pFontRawName->setReadOnly(true);	// no meaning for Win32 (now)
#endif
}


SettingsDialog::~SettingsDialog()
{}


void
SettingsDialog::accept()
{
    CmApply();
    QDialog::accept();
}

void
SettingsDialog::CmApply()
{

    QFont font;
#ifdef __unix
    // this line is here because in Unix font raw name could be changed
    // in LineEdit element
    cellFont.setRawName(pFontRawName->text());
#endif
    pVisorImp->setCellFont(cellFont);

    pVisorImp->setDoubleDigits(pNumDigits->value());
    pVisorImp->setUpdateInterval( pUpdateInterval->value() );
    pVisorImp->setConfigAutosave( pConfigAutosave->isChecked() );

    //pVisorImp->Update(true);
}


void
SettingsDialog::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_SETUP_HTML );
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
        // for Win32 this line does not have a lot of sence
        pFontRawName->setText( selected_font.rawName() );
    }
}

//--------------------- End of SettingsDialog.cpp ---------------------------

