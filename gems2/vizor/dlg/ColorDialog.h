//-------------------------------------------------------------------
// $Id$
//
// Declaration of ColorDialog class
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


#ifndef ColorDialog_included
#define ColorDialog_included

#include "ColorDialogData.h"
#include "v_module.h"
#include "../graph.h"

class ColorDialog : public ColorDialogData
{
    Q_OBJECT

protected slots:

    void CmSelectColor();

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
