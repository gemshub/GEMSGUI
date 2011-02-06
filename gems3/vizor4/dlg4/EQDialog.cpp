//-------------------------------------------------------------------
// $Id: EQDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of EQDialog class
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

#include <QCloseEvent>
#include <qlabel.h>
#include <qvariant.h>

#include "EQDialog.h"
#include "NewSystemDialog.h"
#include "MainDialog.h"

#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "v_mod.h"
#include "module_w.h"


#define Inherited EQDialogData

EQDialog::EQDialog(QWidget* parent, const char* name )
    : QWidget(parent)
{
    setupUi(this);
    // pLogoImg->setPixmap (pVisorImp->getLogo ());
    setWindowTitle(name);
}


EQDialog::~EQDialog()
{}

void EQDialog::languageChange()
{
    retranslateUi(this);
}

void EQDialog::CmSystemDialog()
{

    if( !NewSystemDialog::pDia )
    {
        (new NewSystemDialog(0/*pVisorImp*/))->show();
         NewSystemDialog::pDia->CmSelect();
    }
    else
        NewSystemDialog::pDia->raise();
	
}

void EQDialog::CmBack()
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


void EQDialog::CmAdvanced()
{
    pVisorImp->OpenModule(this, RT_PARAM, 1);
}

void EQDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_MODES_HTML );
}

void EQDialog::CmOpen_PROBE()
{
    pVisorImp->OpenModule(this, RT_UNSPACE,0,false, true);
}

void EQDialog::CmOpen_DUTERM()
{
    pVisorImp->OpenModule(this, RT_DUALTH,0,false, true);
}

void EQDialog::CmOpen_PROCESS()
{
    pVisorImp->OpenModule(this, RT_PROCES,0,false, true);
}

void EQDialog::CmOpen_GTDEMO()
{
    pVisorImp->OpenModule(this, RT_GTDEMO,0,false, true);
}


void EQDialog::CmOpen_INTEG()
{
    pVisorImp->OpenModule(this, RT_GEM2MT,0,false, true);
}


void EQDialog::closeEvent(QCloseEvent* ev)
{
    // close all opened modules
    for(uint ii=0; ii<aMod.GetCount(); ii++)
        if( aMod[ii].pImp )
            aMod[ii].pImp->CloseWin();

    // close SystemDialog also
   if( NewSystemDialog::pDia )
        NewSystemDialog::pDia->close();

    QWidget::closeEvent(ev);
}



// ---------------------- End of EQDialog.cpp ----------------------

