//-------------------------------------------------------------------
// $Id: ChangeKeyDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ChangeKeyDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#include <qcombobox.h>
#include <qvariant.h>
#include "visor_w.h"
#include "service.h"
#include "ChangeKeyDialog.h"


ChangeKeyDialog::ChangeKeyDialog(QWidget* win, const char* caption ):
        QDialog( win)
{
	setupUi(this);
//    setCaption( caption );
}


ChangeKeyDialog::~ChangeKeyDialog()
{}

void ChangeKeyDialog::languageChange()
{
    retranslateUi(this);
}


gstring
ChangeKeyDialog::getTemplFrom()
{
 gstring ret = cbKsearch->currentText().toLatin1().data();
 return ret;
}

gstring
ChangeKeyDialog::getTemplTo()
{
 gstring ret = cbKreplace->currentText().toLatin1().data();
 return ret;
}

void ChangeKeyDialog::CmHelp()
{
   pVisorImp->OpenHelp( GEMS_TEMPL_HTML );
}



//--------------------- End of ChangeKeyDialog.cpp ---------------------------

