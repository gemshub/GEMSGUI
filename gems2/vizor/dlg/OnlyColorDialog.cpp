//-------------------------------------------------------------------
// Id: gems/vizor/dlg/OnlyColorDialog.cpp  version 2.0.0    2001
//
// Implementation of OnlyColorDialog class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "OnlyColorDialog.h"

#define Inherited OnlyColorDialogData

OnlyColorDialog::OnlyColorDialog
(
    QColor& aData,
    QWidget* parent,
    const char* name
)
        :
        Inherited( parent, name ), color(aData)
{
    SetColor( color );
    setCaption("Background Color Setup");
}


OnlyColorDialog::~OnlyColorDialog()
{}



QColor& OnlyColorDialog::GetColor()
{
    return color;
}

void OnlyColorDialog::SetRed(int val)
{
    valRed->setValue(val);
    // Color->setBackgroundColor(
    //    QColor(valRed->value(), valGreen->value(), valBlue->value() ));

}


void OnlyColorDialog::SetGreen(int val)
{
    valGreen->setValue(val);
    // Color->setBackgroundColor(
    //    QColor(valRed->value(), valGreen->value(), valBlue->value() ));

}


void OnlyColorDialog::SetBlue(int val)
{
    valBlue->setValue(val);
    // Color->setBackgroundColor(
    //    QColor(valRed->value(), valGreen->value(), valBlue->value() ));

}


void OnlyColorDialog::ChangeRed(int val)
{
    slRed->setValue(val);
    color = QColor(valRed->value(), valGreen->value(), valBlue->value() );
    Color->setBackgroundColor( color );

}


void OnlyColorDialog::ChangeGreen(int val)
{
    slGreen->setValue(val);
    color = QColor(valRed->value(), valGreen->value(), valBlue->value() );
    Color->setBackgroundColor( color );
}

void OnlyColorDialog::ChangeBlue(int val)
{
    slBlue->setValue(val);
    color = QColor(valRed->value(), valGreen->value(), valBlue->value() );
    Color->setBackgroundColor( color );
}


void OnlyColorDialog::SetColor(const QColor& color)
{
    valRed->setValue(color.red());
    valGreen->setValue(color.green());
    valBlue->setValue(color.blue());
}


void OnlyColorDialog::SetColorRed()
{
    SetColor(red);
}

void OnlyColorDialog::SetColorGreen()
{
    SetColor(green);
}

void OnlyColorDialog::SetColorBlue()
{
    SetColor(blue);
}

void OnlyColorDialog::SetColorCyan()
{
    SetColor(cyan);
}

void OnlyColorDialog::SetColorMagenta()
{
    SetColor(magenta);
}

void OnlyColorDialog::SetColorYellow()
{
    SetColor(yellow);
}

void OnlyColorDialog::SetColorBlack()
{
    SetColor(black);
}

void OnlyColorDialog::SetColorWhite()
{
    SetColor(white);
}

void OnlyColorDialog::SetColorGray()
{
    SetColor(gray);
}


void OnlyColorDialog::SetColorDarkRed()
{
    SetColor(darkRed);
}

void OnlyColorDialog::SetColorDarkGreen()
{
    SetColor(darkGreen);
}

void OnlyColorDialog::SetColorDarkBlue()
{
    SetColor(darkBlue);
}

void OnlyColorDialog::SetColorDarkCyan()
{
    SetColor(darkCyan);
}

void OnlyColorDialog::SetColorDarkMagenta()
{
    SetColor(darkMagenta);
}

void OnlyColorDialog::SetColorDarkYellow()
{
    SetColor(darkYellow);
}

void OnlyColorDialog::SetColorDarkGray()
{
    SetColor(darkGray);
}

//--------------------- End of OnlyColorDialog.cpp ---------------------------

