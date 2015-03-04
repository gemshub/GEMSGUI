//-------------------------------------------------------------------
// $Id: CalcCheckDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CalcCheckDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
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

    CalcCheckDialog(QWidget* parent, const int nO, const string& Vals);

    virtual ~CalcCheckDialog();

    int fun();
};

#endif // CalcCheckDialog_included
