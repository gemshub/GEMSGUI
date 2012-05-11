//-------------------------------------------------------------------
// $Id: CalcCheckDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CalcCheckDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
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

#ifndef CalcCheckDialog_included
#define CalcCheckDialog_included

#include <QDialog>
#include "ui_CalcCheckDialog4.h"
#include "v_object.h"

class CalcCheckDialog: public QDialog, public Ui::CalcCheckDialogData
{
    Q_OBJECT

    TObject* rObj;
    int nO_;
    
public slots:
    virtual void CmOk();

protected slots:
    virtual void languageChange();

public:

    CalcCheckDialog(QWidget* parent, const int nO, const gstring& Vals);

    virtual ~CalcCheckDialog();

    int fun();
};

#endif // CalcCheckDialog_included
