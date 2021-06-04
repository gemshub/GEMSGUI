//-------------------------------------------------------------------
// $Id: ChangeKeyDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ChangeKeyDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ChangeKeyDialog_included
#define ChangeKeyDialog_included

#include <QDialog>

namespace Ui {
class ChangeKeyDialogData;
}

class ChangeKeyDialog : public QDialog
{
    Q_OBJECT

    Ui::ChangeKeyDialogData *ui;
    int nSymbols; // max number of simbols in key field

protected slots:
    virtual void CmHelp();
    void SetFromString();
    void SetToString();
    void SetAll( bool );
    void SetPart( bool );

public:

    ChangeKeyDialog(QWidget* win, int nsymb, const char* caption = nullptr );

    virtual ~ChangeKeyDialog();

    std::string getTemplFrom();
    std::string getTemplTo();

};


#endif // ChangeKeyDialog_included
