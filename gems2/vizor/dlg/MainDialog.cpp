//-------------------------------------------------------------------
// $Id$
//
// Implementation of MainDialog class
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

const char *GEMS_ABOUT_HTML = "g_about";
const char *GEMS_GETST_HTML = "gemstart";

#include <qlabel.h>

#include "MainDialog.h"
#include "DBDialog.h"
#include "EQDialog.h"
#include "SettingsDialog.h"

#include "visor.h"
#include "service.h"
#include "visor_w.h"


#define Inherited MainDialogData

MainDialog::MainDialog(QWidget* parent):
        Inherited( parent, 0, 20480 )
{
   pLogoImg->setPixmap (pVisorImp->getLogo ());
}


MainDialog::~MainDialog()
{}


void
MainDialog::CmDBDialog()
{
    //  close(TRUE);
    pVisorImp->SetDialog( new DBDialog(pVisorImp) );
    //  close();  hide();
}


void
MainDialog::CmEQDialog()
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

void
MainDialog::CmSettings()
{
    SettingsDialog dlg(this);
    dlg.exec();
    /// put values
}


void
MainDialog::CmExit()
{
    pVisorImp->CmExit();
}

void
MainDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_GETST_HTML );  // 05.01.01
}

void
MainDialog::CmAbout()
{
    pVisorImp->OpenHelp( GEMS_ABOUT_HTML );  // 05.01.01
}

// ---------------------- End of MainDialog.cpp ------------------------

