//-------------------------------------------------------------------
// $Id$
//
// Declaration of LegendDialog class
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

#ifndef LegendDialog_included
#define LegendDialog_included

#include <qdialog.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfont.h>

#include "LegendDialogData.h"
#include "GraphDialog.h"


class LegendDialog :
	public LegendDialogData
{
    Q_OBJECT

    GraphData * graph;
    GraphDialog * graph_dlg;
    QPushButton *BackGrountBut;
    QColor backgroundColor;
    QFont labelFont;
    
    int apply();

public:
    LegendDialog( GraphDialog * aGraph );
    virtual ~LegendDialog();

protected slots:
    virtual void CmApply();
    virtual void SetLine(int);
    virtual void accept();
    virtual void CmBackground();
    virtual void CmChangeLabelFont();

protected:
    TIArray<PlotTypeBtn> pLines;
    TIArray<QLineEdit>   pNames;
};

#endif // LegendDialog_included
