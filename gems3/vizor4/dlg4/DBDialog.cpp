//-------------------------------------------------------------------
// $Id: DBDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of DBDialog class
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
#include <QCloseEvent>

#include "DBDialog.h"
#include "v_mod.h"
#include "module_w.h"
#include "visor_w.h"
#include "MainDialog.h"
#include "DBAdvDialog.h"
#include "service.h"

DBDialog::DBDialog(QWidget* parent, const char* name)
    : QWidget(parent)
{
    setupUi(this);
 //   pImg1->setPixmap (pVisorImp->getLogo ());
    setWindowTitle(name); 
}

DBDialog::~DBDialog()
{}

void DBDialog::languageChange()
{
    retranslateUi(this);
}

void
DBDialog::CmOpen_RTPARM()
{
    pVisorImp->OpenModule(this, RT_RTPARM,0,false, true);
}

void
DBDialog::CmOpen_ICOMP()
{
    pVisorImp->OpenModule(this, RT_ICOMP, 0, false, true);
}

void
DBDialog::CmOpen_PHASE()
{
    pVisorImp->OpenModule(this, RT_PHASE,0,false, true);
}

void
DBDialog::CmOpen_COMPOS()
{
    pVisorImp->OpenModule(this, RT_COMPOS,0,false, true);
}

void
DBDialog::CmOpen_REACDC()
{
    pVisorImp->OpenModule(this, RT_REACDC,0,false, true);
}

void
DBDialog::CmOpen_DCOMP()
{
    pVisorImp->OpenModule(this, RT_DCOMP,0,false, true);
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
    pVisorImp->OpenHelp( GEMS_TDBAS_HTML );
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

