//-------------------------------------------------------------------
// $Id$
//
// Declaration of ChangeKeyDialog class
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
// See http://gems.web.psi.ch/ for more information
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
