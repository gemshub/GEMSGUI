//-------------------------------------------------------------------
// $Id: CalcDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CalcDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
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

#ifndef CalcDialog_included
#define CalcDialog_included

#include <QDialog>
#include <QButtonGroup>

#include "ui_CalcDialog4.h"
#include "gstring.h"

class CalcDialog : public QDialog, public Ui::CalcDialogData
{
    Q_OBJECT

    int  nObj;
    QButtonGroup *allButtons;
    
protected slots:
    virtual void languageChange();

    virtual void setMode( int id);
    virtual void ok();
    virtual void unaryMode();
    void help();
    
public:
    CalcDialog(QWidget* parent, int nobj);
    virtual ~CalcDialog();

    double fun(double val);
    int    funName(double& val);
    gstring funText(const char * valText );

 };

#endif // CalcDialog_included
