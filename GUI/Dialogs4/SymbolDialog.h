//-------------------------------------------------------------------
// $Id: ColorDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of SymbolDialog class
//
// Copyright (C) 1996-2013  S.Dmytriyeva, A.Rysin
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

#ifndef SymbolDialog_included
#define SymbolDialog_included

#include <QDialog>

#include "ui_SymbolDialog4.h"
#include "graph.h"

class SymbolDialog : public QDialog, public Ui::SymbolDialogData
{
    Q_OBJECT

    QButtonGroup *smbGroup;
    TPlotLine dat;

protected slots:
    virtual void languageChange();
    virtual void CmSelectColor();

public:

    SymbolDialog( TPlotLine& aData, QWidget* parent = NULL );
    virtual ~SymbolDialog();

    TPlotLine& GetPlotLine();

};

#endif // SymbolDialog_included
