//-------------------------------------------------------------------
// $Id: CalcDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CalcDialog class
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

#ifndef CalcDialog_included
#define CalcDialog_included

#include <QDialog>
#include <QButtonGroup>

#include "ui_CalcDialog4.h"
#include "v_object.h"



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
