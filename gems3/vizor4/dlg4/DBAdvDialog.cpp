//-------------------------------------------------------------------
// $Id: DBAdvDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of DBAdvDialog class
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

#include "DBAdvDialog.h"
#include "DBDialog.h"
#include "v_mod.h"
#include "module_w.h"
#include "visor_w.h"
#include "service.h"

DBAdvDialog::DBAdvDialog(QWidget* parent, const char* name)
    : QWidget(parent)
{
    setupUi(this);
    setWindowTitle(name);
    // pLogoImg->setPixmap (pVisorImp->getLogo ());
}

DBAdvDialog::~DBAdvDialog()
{}

void DBAdvDialog::languageChange()
{
    retranslateUi(this);
}

void DBAdvDialog::CmBack()
{
    pVisorImp->SetDialog( new DBDialog(pVisorImp) );
    //  close();
}

void DBAdvDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_MISCL_HTML );
}

void DBAdvDialog::CmFormats()
{
    pVisorImp->OpenHelp( GEMS_FORMT_HTML );
}

void DBAdvDialog::CmOpen_PROBE()
{
//    qWarning( "DBAdvDialog::CmOpen_PROBE(): Not implemented yet!" );
    pVisorImp->OpenModule(this, RT_UNSPACE,0,false, true);

}

void DBAdvDialog::CmOpen_DUTERM()
{
//    qWarning( "DBAdvDialog::CmOpen_DUTERM(): Not implemented yet!" );
    pVisorImp->OpenModule(this, RT_DUALTH,0,false, true);
}

void DBAdvDialog::CmOpen_PROCESS()
{
    pVisorImp->OpenModule(this, RT_PROCES,0,false, true);
}

void DBAdvDialog::CmOpen_GTDEMO()
{
    pVisorImp->OpenModule(this, RT_GTDEMO,0,false, true);
}

void DBAdvDialog::CmOpen_INTEG()
{
//    qWarning( "DBAdvDialog::CmOpen_INTEG(): Not implemented yet!" );
    pVisorImp->OpenModule(this, RT_GEM2MT,0,false, true);
}

void DBAdvDialog::CmOpen_Const()
{
    pVisorImp->OpenModule(this, RT_CONST,0,false, true);
}

void DBAdvDialog::CmOpen_PROFILE()
{
    pVisorImp->OpenModule(this, RT_PARAM,0,false, true);
}

void DBAdvDialog::CmOpen_SData()
{
    pVisorImp->OpenModule(this, RT_SDATA,0,false, true);
}

void DBAdvDialog::CmThermoSystem()
{
    pVisorImp->OpenModule(this, RT_SYSEQ,0,false, true);
}

void DBAdvDialog::closeEvent(QCloseEvent* e)
{
    for( uint ii=0; ii<aMod.GetCount(); ii++)
        if( aMod[ii].pImp )
            aMod[ii].pImp->CloseWin();

    QWidget::closeEvent(e);
}

// ------------------- End of DBAdvDialog.cpp ------------------------

