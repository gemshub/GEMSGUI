//-------------------------------------------------------------------
// $Id: ColorDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ColorDialog class
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


#ifndef ColorDialog_included
#define ColorDialog_included

#include <QDialog>

#include "ui_ColorDialog4.h"
#include "graph.h"


class ColorDialog : public QDialog, public Ui::ColorDialogData
{
    Q_OBJECT

protected slots:
    virtual void languageChange();
    virtual void CmSelectColor();

public:

    TPlotLine dat;

    ColorDialog
    (
        TPlotLine& aData,
        QWidget* parent = NULL
    );

    virtual ~ColorDialog();
    TPlotLine& GetPlotLine();

};

#endif // ColorDialog_included
