//-------------------------------------------------------------------
// Id: gems/vizor/dlg/EQDialog.cpp  version 2.0.0    2001
//
// Implementation of EQDialog class
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

const char *GEMS_EQ_HTML = "ge_howto";

#include <qlabel.h>

#include "EQDialog.h"
#include "SystemDialog.h"
#include "MainDialog.h"
#include "HLresDialog.h"
#include "HLinpDialog.h"

#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "v_mod.h"
#include "module_w.h"


#define Inherited EQDialogData

EQDialog::EQDialog(QWidget* parent, const char* name):
        Inherited( parent, name )
{
    pLogoImg->setPixmap (pVisorImp->getLogo ());
}


EQDialog::~EQDialog()
{}


void
EQDialog::CmSystemDialog()
{
    if( !SystemDialog::pDia )
        (new SystemDialog(0/*pVisorImp*/))->show();
    else
        SystemDialog::pDia->raise();
}

void
EQDialog::CmBack()
{
    try
    {
        pVisorImp->SetGeneralMode();
        pVisorImp->SetDialog( new MainDialog(pVisorImp) );
    }
    catch(TError& err)
    {
        vfMessage(this, err.title, err.mess, vfErr);
    }
    //  close();
}


void
EQDialog::CmAdvanced()
{
    pVisorImp->OpenModule(this, RT_PARAM, 1);
}

void
EQDialog::CmHelp()
{
//    pVisorImp->OpenHelp("GEMS-Linux-index");
    pVisorImp->OpenHelp( GEMS_EQ_HTML );  // 05.01.01
}

void
EQDialog::CmOpen_PROBE()
{
//    pVisorImp->OpenModule(this, RT_PROBE);
}

void
EQDialog::CmOpen_DUTERM()
{
//    pVisorImp->OpenModule(this, RT_DUTERM);
}

void
EQDialog::CmOpen_PROCESS()
{
    pVisorImp->OpenModule(this, RT_PROCES);
}

void
EQDialog::CmOpen_GTDEMO()
{
    pVisorImp->OpenModule(this, RT_GTDEMO);
}


void
EQDialog::CmOpen_INTEG()
{
//    pVisorImp->OpenModule(this, RT_INTEG);
}


void
EQDialog::closeEvent(QCloseEvent* ev)
{
    // close all opened modules
    for(uint ii=0; ii<aMod.GetCount(); ii++)
        if( aMod[ii].pImp )
            aMod[ii].pImp->CloseWin();

    // close SystemDialog also
    if( SystemDialog::pDia )
        SystemDialog::pDia->close();

    // close ListDialogs also
    if( HLresDialog::pDia )
        HLresDialog::pDia->close();

    if( HLinpDialog::pDia )
        HLinpDialog::pDia->close();

    QWidget::closeEvent(ev);
}

// ---------------------- End of EQDialog.cpp ----------------------

