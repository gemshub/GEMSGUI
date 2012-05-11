//-------------------------------------------------------------------
// $Id$
//
// Declaration of OnlyColorDialog class
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef OnlyColorDialog_included
#define OnlyColorDialog_included

#include "OnlyColorDialogData.h"
#include "v_module.h"

class OnlyColorDialog : public OnlyColorDialogData
{
    Q_OBJECT

protected slots:

    void SetRed(int);
    void SetGreen(int);
    void SetBlue(int);
    void ChangeRed(int);
    void ChangeGreen(int);
    void ChangeBlue(int);

    void SetColorRed();
    void SetColorGreen();
    void SetColorBlue();
    void SetColorCyan();
    void SetColorMagenta();
    void SetColorYellow();
    void SetColorBlack();
    void SetColorWhite();
    void SetColorDarkRed();
    void SetColorDarkGreen();
    void SetColorDarkBlue();
    void SetColorDarkCyan();
    void SetColorDarkMagenta();
    void SetColorDarkYellow();
    void SetColorDarkGray();
    void SetColorGray();

public:

    QColor color;

    OnlyColorDialog
    (
        QColor& aData,
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~OnlyColorDialog();
    void SetColor(const QColor& color);
    QColor& GetColor();

};
#endif // OnlyColorDialog_included
