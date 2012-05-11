//-------------------------------------------------------------------
// $Id: CalcDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CalcDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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

#include <cmath>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qvariant.h>

#include "CalcDialog.h"
#include "v_vals.h"

CalcDialog::CalcDialog(QWidget* parent, int obj):
        QDialog( parent ),
        nObj(obj)
{
    
    setupUi(this);
    
    allButtons = new QButtonGroup( pButtonBox );
    allButtons->addButton(pAssignBtn, 0);
    allButtons->addButton(PushButton2, 1);
    allButtons->addButton(PushButton3, 2);
    allButtons->addButton(PushButton4, 3);
    allButtons->addButton(PushButton5, 4);
    allButtons->addButton(PushButton6, 5);
    allButtons->addButton(PushButton7, 6);
    allButtons->addButton(PushButton8, 7);
    allButtons->addButton(PushButton9, 8);
    allButtons->addButton(PushButton10, 9);
    allButtons->addButton(PushButton11, 10);
    allButtons->addButton(PushButton12, 11);
    allButtons->addButton(PushButton13, 12);
    allButtons->addButton(PushButton14, 13);
    allButtons->addButton(PushButton15, 14);
    allButtons->addButton(pClearBtn, 15);
    pAssignBtn->setChecked( true);

    QObject::connect( allButtons, SIGNAL(buttonClicked(int)), this, SLOT(setMode(int)));

    QObject::connect(PushButton1_3, SIGNAL(clicked()), this, SLOT(ok()));
    QObject::connect(PushButton1_2, SIGNAL(clicked()), this, SLOT(reject()));
  //  QObject::connect(PushButton1_2, SIGNAL(clicked()), this, SLOT(unaryMode()));

}

CalcDialog::~CalcDialog()
{}

void CalcDialog::languageChange()
{
    retranslateUi(this);
}

void CalcDialog::unaryMode()
{
    pValue->setEnabled(false);
}

void CalcDialog::setMode( int id )
{
 if( id <= 5)
    pValue->setEnabled(true);
 else
    pValue->setEnabled(false);
}

void CalcDialog::ok()
{
    accept();
}

double CalcDialog::fun(double val)
{
    int ii;

    ii = allButtons->checkedId();
    double val2=0.;
    if( ii <= 5 )
    {  
      QString str = pValue->currentText();
      if( (str.indexOf("---") != -1) || str[0] == '`' )
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

int CalcDialog::funName(double& val)
{
    int ii;

    ii = allButtons->checkedId();

    val=0.;
    if( ii <= 5 )
    {
      QString str = pValue->currentText();
      if( (str.indexOf("---") != -1) || str[0] == '`' )
         val = DOUBLE_EMPTY;
      else
         val = str.toDouble();  /// check for error
    }
    return ii;
}


gstring CalcDialog::funText(const char * valText )
{
    int ii;
    QString res;

    ii = allButtons->checkedId();
    QString strText = "";

    if( ii <= 5 )
    {
      strText = pValue->currentText();
      if( (strText.indexOf("---") != -1) || strText[0] == '`' )
         strText = "empty()";
    }

   switch( ii )
    {
    case 0:
        res = valText; break;
    case 1:
        res = QString("%1+%2").arg(valText, strText); break;
    case 2:
        res = QString("%1-%2").arg(valText, strText); break;
    case 3:
        res = QString("%1*%2").arg(valText, strText);; break;
    case 4:
        res = QString("%1/%2").arg(valText, strText);; break;
    case 5:
        res = QString("%1^%2").arg(valText, strText);; break;
    case 6:
        res = QString( "( %1 ? (1/%1) : empty() )" ).arg(valText) ; break;
    case 7:
        res = QString( "( %1 > 0. ? sqrt(%1) : empty() )" ).arg(valText) ; break;
    case 8:
        res = QString( "( %1 > 0. ? lg(%1) : empty() )" ).arg(valText) ; break;
    case 9:
        res = QString( "( %1 > 0. ? ln(%1) : empty() )" ).arg(valText) ; break;
    case 10:
        res = QString("10.^%1").arg(valText); break;
    case 11:
        res = QString("exp(%1)").arg(valText); break;
    case 12:
        res = QString("sin(%1)").arg(valText); break;
    case 13:
        res = QString("cos(%1)").arg(valText); break;
    case 14:
        res = QString("tan(%1)").arg(valText); break;
    case 15:
        res = "empty()"; //No Data value
    }

    gstring rres = res.toLatin1().data();
    return rres;
}


void CalcDialog::help()
{ }

//--------------------- End of CalcDialog.cpp ---------------------------
