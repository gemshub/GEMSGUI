//-------------------------------------------------------------------
// $Id$
//
// Implementation of ChangeKeyDialog class
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

const char *GEMS_TEMPL_HTML = "templ_key";

#include <qcombobox.h>
#include "visor_w.h"
#include "ChangeKeyDialog.h"

#define Inherited ChangeKeyDialogData

ChangeKeyDialog::ChangeKeyDialog(QWidget* win, const char* caption ):
        Inherited( win, 0, true)
{
//    setCaption( caption );
}


ChangeKeyDialog::~ChangeKeyDialog()
{}

gstring
ChangeKeyDialog::getTemplFrom()
{
 const char* ret = (const char*)cbKsearch->currentText();
 return gstring(ret);
}

gstring
ChangeKeyDialog::getTemplTo()
{
 const char* ret = (const char*)cbKreplace->currentText();
 return gstring(ret);
}

void ChangeKeyDialog::CmHelp()
{
   pVisorImp->OpenHelp( GEMS_TEMPL_HTML, 0, this, true );
}



//--------------------- End of ChangeKeyDialog.cpp ---------------------------

