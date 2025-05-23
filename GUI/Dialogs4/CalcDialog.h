//-------------------------------------------------------------------
// $Id: CalcDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CalcDialog class
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

#ifndef CalcDialog_included
#define CalcDialog_included

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class CalcDialogData;
}

class CalcDialog : public QDialog
{
    Q_OBJECT

    Ui::CalcDialogData *ui;
    int  nObj;
    QButtonGroup *allButtons;
    
protected slots:

    void setMode(QAbstractButton*);
    void ok();
    void unaryMode();
    void help();
    
public:
    CalcDialog(QWidget* parent, int nobj);
    virtual ~CalcDialog();

    QString fun(double val);
    int    funName(double& val);
    std::string funText(const char * valText );

};

#endif // CalcDialog_included
