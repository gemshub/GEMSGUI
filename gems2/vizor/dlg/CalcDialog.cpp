//-------------------------------------------------------------------
// $Id$
//
// Implementation of CalcDialog class
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

#include <math.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qcombobox.h>

#include "CalcDialog.h"
#include "service.h"

#define Inherited CalcDialogData

CalcDialog::CalcDialog(QWidget* parent, TObject& obj,
                       Selection& sel):
        Inherited( parent, 0, true ),
        rObj(obj),
        selection(sel)
{
    QString str = obj.GetKeywd();
    	    str += " Cell selection range";
    pObjectBox->setTitle(str/*obj.GetKeywd()*/);

   pN1->setRange( 0, obj.GetN()-1 );
   pN2->setRange( 0, obj.GetN()-1 );
   pM1->setRange( 0, obj.GetM()-1 );
   pM2->setRange( 0, obj.GetM()-1 );

   // set selection
    pN1->setValue( selection.N1 );
    pN2->setValue( selection.N2 );
    pM1->setValue( selection.M1 );
    pM2->setValue( selection.M2 );

}


CalcDialog::~CalcDialog()
{}


void
CalcDialog::unaryMode()
{
    pValue->setEnabled(false);
}

void
CalcDialog::binaryMode()
{
    pValue->setEnabled(true);
}

void
CalcDialog::selectAll()
{
    pN1->setValue(0);
    pN2->setValue(rObj.GetN()-1);
    pM1->setValue(0);
    pM2->setValue(rObj.GetM()-1);
}

void
CalcDialog::ok()
{
    if( pM1->value() > pM2->value()
            || pN1->value() > pN2->value() )
    {
        vfMessage(this, "Calculator", "Invalid intervals", vfErr);
        return;
    }

    selection.M1 = pM1->value();
    selection.M2 = pM2->value();
    selection.N1 = pN1->value();
    selection.N2 = pN2->value();

    accept();
}

double
CalcDialog::fun(double val)
{
    int ii;
    for( ii=1; ii<15; ii++ )
    {
//      QPushButton* p = dynamic_cast<QPushButton*>(pButtonBox->find(ii));
      QPushButton* p = (QPushButton*)(pButtonBox->find(ii));
        if( p->isOn() )
            goto FOUND;
    }
    ii=0;
FOUND:
    double val2;
    if( ii <= 5 )
    {  // SD oct 2005
      QString str = pValue->currentText();
      if( ( str.find("---") != -1 ) || str[0] == '`' )
         val2 = DOUBLE_EMPTY;
      else
         val2 = str.toDouble();  /// check for error
    }
    switch( ii )
    {
    case 0:
        return val2;
    case 1:
        return val + val2;
    case 2:
        return val - val2;
    case 3:
        return val * val2;
    case 4:
        return val / val2;
    case 5:
        return pow(val, val2);

    case 6:
        return 1/val;
    case 7:
        return sqrt(val);
    case 8:
        return log10(val);
    case 9:
        return log(val);
    case 10:
        return pow(10.,val);
    case 11:
        return exp(val);
    case 12:
        return sin(val);
    case 13:
        return cos(val);
    case 14:
        return tan(val);
    case 15:
        return DOUBLE_EMPTY; //No Data value
    }
    return val;
}

void
CalcDialog::help()
{}

//--------------------- End of CalcDialog.cpp ---------------------------
