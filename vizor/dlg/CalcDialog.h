//-------------------------------------------------------------------
// $Id$
//
// Declaration of CalcDialog class
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

#ifndef CalcDialog_included
#define CalcDialog_included

#include "CalcDialogData.h"
#include "v_object.h"

struct Selection {
    int N1;
    int N2;
    int M1;
    int M2;
};

class CalcDialog : public CalcDialogData
{
    Q_OBJECT

    const TObject& rObj;
    Selection& selection;

protected slots:
    void unaryMode();
    void binaryMode();
    void ok();
    void selectAll();
    void help();

public:
    CalcDialog(QWidget* parent, TObject& obj, Selection& sel);
    virtual ~CalcDialog();

    double fun(double val);
};

#endif // CalcDialog_included
