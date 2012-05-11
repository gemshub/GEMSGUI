//-------------------------------------------------------------------
// $Id$
//
// Implementation of ExcludeFillDialog class
//
// Copyright (C) 1996-2001    S.Dmytriyeva
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
#include "gstring.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QButtonGroup;
class QLabel;
class QLineEdit;
class QPushButton;

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

    QLabel* TextLabel1;
    QLabel* TextLabel1_3_2;
    QLabel* TextLabel1_3;
    QPushButton* pExcludeAll;
    QPushButton* pFillAll;
    QPushButton* buttonOk;
    QPushButton* buttonHelp;
    QPushButton* buttonCancel;
    QLabel* TextLabel1_2_2;
    QLabel* TextLabel4;
    QLabel* TextLabel1_2;
    QLabel* TextLabel3;
    QLineEdit* LineEdit1;

    QButtonGroup* GroupBox1;
    QCheckBox* CheckBox1;
    QVBoxLayout* Layout3;

};

#endif // EXCLUDEFILLDIALOG_H
