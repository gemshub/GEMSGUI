//-------------------------------------------------------------------
// $Id: MainDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of MainDialog class
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <qlabel.h>
#include <qvariant.h>

#include "MainDialog.h"
#include "DBDialog.h"
#include "EQDialog.h"
#include "SettingsDialog.h"

#include "service.h"
#include "visor_w.h"

MainDialog::MainDialog(QWidget* parent):
    QWidget(parent )
{
    setupUi(this);
//qt3to4    pLogoImg->setPixmap (pVisorImp->getLogo ());

}

MainDialog::~MainDialog()
{}

void MainDialog::languageChange()
{
    retranslateUi(this);
}

void MainDialog::CmDBDialog()
{
    //  close(TRUE);
    pVisorImp->SetDialog( new DBDialog(pVisorImp) );
    //  close();  hide();
}


void MainDialog::CmEQDialog()
{
    try
    {
        if( !pVisorImp->SetProfileMode() )
            return;  // cancelled by user
        pVisorImp->SetDialog( new EQDialog(pVisorImp) );
        //        close();  /* hide(); */
    }

    catch(TError& err)
    {
        vfMessage(this, err.title, err.mess, vfErr);
    }
}

void MainDialog::CmSettings()
{
    SettingsDialog dlg(this);
    dlg.exec();
    /// put values
}


void MainDialog::CmExit()
{
    pVisorImp->CmExit();
}

void MainDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_TUTOR_HTML );
}

void MainDialog::CmAbout()
{
    pVisorImp->OpenHelp( GEMS_ABOUT_HTML ); 
}


// ---------------------- End of MainDialog.cpp ------------------------

