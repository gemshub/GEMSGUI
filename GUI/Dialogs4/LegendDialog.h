//-------------------------------------------------------------------
// $Id: LegendDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of LegendDialog class
//
// Copyright (C) 1996-2013 S.Dmytriyeva, A.Rysin
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

    QColor backgroundColor;
    QFont labelFont;

    int apply();

public:
    LegendDialog( GraphDialog * aGraph );
    virtual ~LegendDialog();

protected slots:
    virtual void CmApply();
    void CmHelp();
    virtual void accept();
    virtual void CmBackground();
    virtual void CmChangeLabelFont();
    virtual void CmChangeGraphType(int);
    virtual void languageChange();

};

#endif // LegendDialog_included
