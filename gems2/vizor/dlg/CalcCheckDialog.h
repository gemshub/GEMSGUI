//-------------------------------------------------------------------
// $Id$
//
// Declaration of CalcCheckDialog class
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
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef CalcCheckDialog_included
#define CalcCheckDialog_included

#include "CalcCheckDialogData.h"
#include "v_object.h"

class CalcCheckDialog:
            public CalcCheckDialogData
{
    Q_OBJECT

    QRect& sel;
    const TObject& rObj;

protected slots:
    void CmOk();
    void CmSelectAll();

public:

    CalcCheckDialog(QWidget* parent,
                    const TObject& obj, const gstring& Vals, QRect& selection);

    virtual ~CalcCheckDialog();

    int fun();
};

#endif // CalcCheckDialog_included
