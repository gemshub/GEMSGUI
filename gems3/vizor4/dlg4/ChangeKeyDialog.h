//-------------------------------------------------------------------
// $Id: ChangeKeyDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ChangeKeyDialog class
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

#ifndef ChangeKeyDialog_included
#define ChangeKeyDialog_included

#include <QDialog>

#include "ui_ChangeKeyDialog4.h"
#include "gstring.h"

class ChangeKeyDialog : public QDialog, public Ui::ChangeKeyDialogData
{
    Q_OBJECT

protected slots:
    virtual void CmHelp();
    virtual void languageChange();

public:

    ChangeKeyDialog(QWidget* win, const char* caption = 0 );

    virtual ~ChangeKeyDialog();

    gstring getTemplFrom();
    gstring getTemplTo();

};


#endif // ChangeKeyDialog_included
