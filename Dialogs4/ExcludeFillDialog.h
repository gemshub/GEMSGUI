//-------------------------------------------------------------------
// $Id: ExcludeFillDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ExcludeFillDialog class
//
// Copyright (C) 1996-200    S.Dmytriyeva
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
#ifndef EXCLUDEFILLDIALOG_H
#define EXCLUDEFILLDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>

#include "gstring.h"

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

    TOArray<bool> getFillType();
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
