//-------------------------------------------------------------------
// Id: gems/vizor/dlg/ChangeKeyDialog.h  version 2.0.0    2001
//
// Declaration of ChangeKeyDialog class
//
// Created : 020305
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

#ifndef ChangeKeyDialog_included
#define ChangeKeyDialog_included

#include "ChangeKeyDialogData.h"

#include "gstring.h"

class ChangeKeyDialog : public ChangeKeyDialogData
{
    Q_OBJECT

protected slots:
    void CmHelp();

public:

    ChangeKeyDialog(QWidget* win, const char* caption = 0 );

    virtual ~ChangeKeyDialog();

    gstring getTemplFrom();
    gstring getTemplTo();

};

#endif // ChangeKeyDialog_included
