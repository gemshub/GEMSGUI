//-------------------------------------------------------------------
// $Id$
//
// Implementation of DBAdvDialog class
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

const char *GEMS_MISC_HTML = "g_miscel";
const char *GEMS_FORM_HTML = "g_formats";

#include <qlabel.h>

#include "DBAdvDialog.h"
#include "DBDialog.h"
#include "v_mod.h"
#include "module_w.h"
#include "visor_w.h"

#define Inherited DBAdvDialogData

DBAdvDialog::DBAdvDialog
(
    QWidget* parent,
    const char* name
)
        :
        Inherited( parent, name )
{
    pLogoImg->setPixmap (pVisorImp->getLogo ());
}


DBAdvDialog::~DBAdvDialog()
{}


void
DBAdvDialog::CmBack()
{
    pVisorImp->SetDialog( new DBDialog(pVisorImp) );
    //  close();
}


void
DBAdvDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_MISC_HTML );
}

void
DBAdvDialog::CmFormats()
{
    pVisorImp->OpenHelp( GEMS_FORM_HTML );
}


void
DBAdvDialog::CmOpen_PROBE()
{
//    qWarning( "DBAdvDialog::CmOpen_PROBE(): Not implemented yet!" );
    pVisorImp->OpenModule(this, RT_UNSPACE);

}

void
DBAdvDialog::CmOpen_DUTERM()
{
//    qWarning( "DBAdvDialog::CmOpen_DUTERM(): Not implemented yet!" );
    pVisorImp->OpenModule(this, RT_DUALTH);
}

void
DBAdvDialog::CmOpen_PROCESS()
{
    pVisorImp->OpenModule(this, RT_PROCES);
}

void
DBAdvDialog::CmOpen_GTDEMO()
{
    pVisorImp->OpenModule(this, RT_GTDEMO);
}


void
DBAdvDialog::CmOpen_INTEG()
{
    qWarning( "DBAdvDialog::CmOpen_INTEG(): Not implemented yet!" );
//    pVisorImp->OpenModule(this, RT_INTEG);
}


void
DBAdvDialog::CmOpen_Const()
{
    pVisorImp->OpenModule(this, RT_CONST);
}

void
DBAdvDialog::CmOpen_PROFILE()
{
    pVisorImp->OpenModule(this, RT_PARAM);
}


void
DBAdvDialog::CmOpen_SData()
{
    pVisorImp->OpenModule(this, RT_SDATA);
}

void
DBAdvDialog::CmThermoSystem()
{
    pVisorImp->OpenModule(this, RT_SYSEQ);
}

void
DBAdvDialog::closeEvent(QCloseEvent* e)
{
    for( uint ii=0; ii<aMod.GetCount(); ii++)
        if( aMod[ii].pImp )
            aMod[ii].pImp->CloseWin();

    QWidget::closeEvent(e);
}

// ------------------- End of DBAdvDialog.cpp ------------------------

