//-------------------------------------------------------------------
// $Id$
//
// Implementation of DBDialog class
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

const char *GEMS_TD_HTML = "gt_howto";

#include <qlabel.h>

#include "DBDialog.h"
#include "v_mod.h"
#include "module_w.h"
#include "visor_w.h"
#include "MainDialog.h"
#include "DBAdvDialog.h"

#define Inherited DBDialogData

DBDialog::DBDialog( QWidget* parent, const char* name):
        Inherited( parent, name )
{
    pImg1->setPixmap (pVisorImp->getLogo ());
}

DBDialog::~DBDialog()
{}

void
DBDialog::CmOpen_RTPARM()
{
    pVisorImp->OpenModule(this, RT_RTPARM);
}

void
DBDialog::CmOpen_ICOMP()
{
    pVisorImp->OpenModule(this, RT_ICOMP, 0, false);
}

void
DBDialog::CmOpen_PHASE()
{
    pVisorImp->OpenModule(this, RT_PHASE);
}

void
DBDialog::CmOpen_COMPOS()
{
    pVisorImp->OpenModule(this, RT_COMPOS);
}

void
DBDialog::CmOpen_REACDC()
{
    pVisorImp->OpenModule(this, RT_REACDC);
}

void
DBDialog::CmOpen_DCOMP()
{
    pVisorImp->OpenModule(this, RT_DCOMP);
}

void
DBDialog::CmBack()
{
    pVisorImp->SetDialog( new MainDialog(pVisorImp) );
    //  close();
}

void
DBDialog::CmAdvanced()
{
    pVisorImp->SetDialog( new DBAdvDialog(pVisorImp) );
    //  close();
}

void
DBDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_TD_HTML );
}

void
DBDialog::closeEvent(QCloseEvent* ev)
{
    for(uint ii=0; ii<aMod.GetCount(); ii++)
        if( aMod[ii].pImp )
            aMod[ii].pImp->CloseWin();

    QWidget::closeEvent(ev);
}

//----------------------- End of DBDialog.cpp ------------------------

