//-------------------------------------------------------------------
// $Id: ExcludeFillDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ExcludeFillDialog class
//
// Copyright (C) 1996-200    S.Dmytriyeva
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
