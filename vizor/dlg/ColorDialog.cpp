//-------------------------------------------------------------------
// $Id$
//
// Implementation of ColorDialog class
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

#include <qcolordialog.h>
#include <qframe.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include "ColorDialog.h"

#define Inherited ColorDialogData

ColorDialog::ColorDialog
(
    TPlotLine& aData,
    QWidget* parent
)
        :
        Inherited( parent, 0, true ), dat(aData)
{
    pColor->setBackgroundColor (QColor (dat.red, dat.green, dat.blue));
    pLine->setValue( dat.line_size );
    pSize->setValue( dat.size );
    switch( dat.type)
    {
    case 0:
        p0->setChecked(true);
        break;
    case 1:
        p1->setChecked(true);
        break;
    case 2:
        p2->setChecked(true);
        break;
    case 3:
        p3->setChecked(true);
        break;
    case 4:
        p4->setChecked(true);
        break;
    case 5:
        p5->setChecked(true);
        break;
    case 6:
        p6->setChecked(true);
        break;
    case 7:
        p7->setChecked(true);
        break;
    case 8:
        p8->setChecked(true);
        break;
    case 9:
        p9->setChecked(true);
        break;
    }
    gstring s= "Set up plot line: ";
    s+= dat.name;
    setCaption(s.c_str());
}


ColorDialog::~ColorDialog()
{}



TPlotLine& ColorDialog::GetPlotLine()
{

    //  if(p0->isChecked() )
    dat.type = 0;
    if(p1->isChecked() )
        dat.type = 1;
    if(p2->isChecked() )
        dat.type = 2;
    if(p3->isChecked() )
        dat.type = 3;
    if(p4->isChecked() )
        dat.type = 4;
    if(p5->isChecked() )
        dat.type = 5;
    if(p6->isChecked() )
        dat.type = 6;
    if(p7->isChecked() )
        dat.type = 7;

    if(p8->isChecked() )
        dat.type = 8;
    if(p9->isChecked() )
        dat.type = 9;

    dat.line_size  = pLine->value();
    dat.size  = pSize->value();
    QColor cl = pColor->backgroundColor();
    dat.red =   cl.red();
    dat.green = cl.green();
    dat.blue  = cl.blue();

    return dat;

}


void ColorDialog::CmSelectColor()
{
    QColor cl = QColorDialog::getColor(pColor->backgroundColor(), this);

    if( cl.isValid() )
        pColor->setBackgroundColor(cl);
}
//--------------------- End of ColorDialog.cpp ---------------------------

