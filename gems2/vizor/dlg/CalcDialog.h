//-------------------------------------------------------------------
// Id: gems/vizor/dlg/CalcDialog.h  version 2.0.0    2001
//
// Declaration of CalcDialog class
//
// Created : 970207    Modified: 010908
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

#ifndef CalcDialog_included
#define CalcDialog_included

#include "CalcDialogData.h"
#include "v_object.h"

class CalcDialog : public CalcDialogData
{
    Q_OBJECT

    QRect& sel;
    const TObject& rObj;

protected slots:
    void unaryMode();
    void binaryMode();
    void ok();
    void selectAll();
    void help();

public:
    CalcDialog(QWidget* parent, TObject& obj, QRect& sel);
    virtual ~CalcDialog();

    double fun(double val);
};

#endif // CalcDialog_included
