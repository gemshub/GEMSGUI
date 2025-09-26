//-------------------------------------------------------------------
// $Id: CalcCheckDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CalcCheckDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
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

#ifndef CalcCheckDialog_included
#define CalcCheckDialog_included

#include <QDialog>
#include "v_object.h"

namespace Ui {
class CalcCheckDialogData;
}

class CalcCheckDialog: public QDialog
{
    Q_OBJECT

    Ui::CalcCheckDialogData *ui;
    TObject* rObj;
    int nO_;
    
public:

    CalcCheckDialog(QWidget* parent, const int nO, const std::string& Vals);

    virtual ~CalcCheckDialog();

    int fun();
};

#endif // CalcCheckDialog_included
