//-------------------------------------------------------------------
// $Id: ExcludeFillDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ExcludeFillDialog class
//
// Copyright (C) 1996-200    S.Dmytriyeva
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
#ifndef EXCLUDEFILLDIALOG_H
#define EXCLUDEFILLDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>

#include "v_user.h"

class ExcludeFillDialog : public QDialog
{
    Q_OBJECT

protected slots:
    void CmExcludeAll();
    void CmFillAll();
    void CmHelp();

public:
    ExcludeFillDialog(QWidget* win, const char* caption,
                      TCStringArray& aICkeys, double fill_data );
    ~ExcludeFillDialog();

    std::vector<bool> getFillType();
    double getFillValue();

protected:

    QPushButton* pExcludeAll;
    QPushButton* pFillAll;
    QPushButton* buttonOk;
    QPushButton* buttonHelp;
    QPushButton* buttonCancel;
    QLineEdit* LineEdit;

    QButtonGroup* GroupBox1;

};

#endif // EXCLUDEFILLDIALOG_H
