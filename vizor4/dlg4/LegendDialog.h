//-------------------------------------------------------------------
// $Id: LegendDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of LegendDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#ifndef LegendDialog_included
#define LegendDialog_included

#include <qdialog.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfont.h>

#include <QDialog>

#include "ui_LegendDialog4.h"
#include "GraphDialog.h"


class LegendDialog :public QDialog, public Ui::LegendDialogData
{
    Q_OBJECT

    int oldGraphType;

    GraphData * graph;
    GraphDialog * graph_dlg;
    QPushButton *BackGrountBut;
    QColor backgroundColor;
    QFont labelFont;
    
    QButtonGroup* B_2;
    QGridLayout*  plotLayout;
    
    int apply();
    void ShowLines();
    void ShowIsoline();

public:
    LegendDialog( GraphDialog * aGraph );
    virtual ~LegendDialog();

protected slots:
    virtual void CmApply();
    virtual void SetLine(int);
    virtual void accept();
    virtual void CmBackground();
    virtual void CmChangeLabelFont();
    virtual void CmChangeGraphType(int);
    virtual void languageChange();

protected:
//    TIArray<PlotTypeBtn> pScale;
//    TIArray<PlotTypeBtn> pLines;
//    TIArray<QLineEdit>   pNames;
    TOArray<PlotTypeBtn *> pScale;
    TOArray<PlotTypeBtn *> pLines;
    TOArray<QLineEdit *>   pNames;
};


#endif // LegendDialog_included
