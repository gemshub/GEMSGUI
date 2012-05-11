//-------------------------------------------------------------------
// $Id: ChangeKeyDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ChangeKeyDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
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

    int nSymbols; // max number of simbols in key field

protected slots:
    virtual void CmHelp();
    virtual void languageChange();
    void SetFromString();
    void SetToString();
    void SetAll( bool );
    void SetPart( bool );

public:

    ChangeKeyDialog(QWidget* win, int nsymb, const char* caption = 0 );

    virtual ~ChangeKeyDialog();

    gstring getTemplFrom();
    gstring getTemplTo();

};


#endif // ChangeKeyDialog_included
