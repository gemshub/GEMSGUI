//-------------------------------------------------------------------
// $Id: CalcDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CalcDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <cmath>
#include "ui_CalcDialog4.h"
#include "CalcDialog.h"
#include "v_vals.h"
#include "v_ipnc.h"
#include "v_mod.h"
#include "model_w.h"
#include "service.h"

CalcDialog::CalcDialog(QWidget* parent, int obj):
    QDialog( parent ),
    ui(new Ui::CalcDialogData),
    nObj(obj)
{
    
    ui->setupUi(this);
    
    allButtons = new QButtonGroup( ui->pButtonBox );
    allButtons->addButton(ui->pAssignBtn, 0);
    allButtons->addButton(ui->PushButton2, 1);
    allButtons->addButton(ui->PushButton3, 2);
    allButtons->addButton(ui->PushButton4, 3);
    allButtons->addButton(ui->PushButton5, 4);
    allButtons->addButton(ui->PushButton6, 5);
    allButtons->addButton(ui->PushButton7, 6);
    allButtons->addButton(ui->PushButton8, 7);
    allButtons->addButton(ui->PushButton9, 8);
    allButtons->addButton(ui->PushButton10, 9);
    allButtons->addButton(ui->PushButton11, 10);
    allButtons->addButton(ui->PushButton12, 11);
    allButtons->addButton(ui->PushButton13, 12);
    allButtons->addButton(ui->PushButton14, 13);
    allButtons->addButton(ui->PushButton15, 14);
    allButtons->addButton(ui->pClearBtn, 15);
    ui->pAssignBtn->setChecked( true);

    QObject::connect( allButtons, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(setMode(QAbstractButton*)));

    QObject::connect(ui->PushButton1_3, SIGNAL(clicked()), this, SLOT(ok()));
    QObject::connect(ui->PushButton1_2, SIGNAL(clicked()), this, SLOT(reject()));
    //  QObject::connect(PushButton1_2, SIGNAL(clicked()), this, SLOT(unaryMode()));
    QObject::connect(ui->PushButton42, SIGNAL(clicked()), this, SLOT(help()));
}

CalcDialog::~CalcDialog()
{
    delete allButtons;
    delete ui;
}

void CalcDialog::unaryMode()
{
    ui->pValue->setEnabled(false);
}

void CalcDialog::setMode(QAbstractButton* button)
{
    if(!button)
        return;
    auto id = allButtons->id(button);
    if( id <= 5)
        ui->pValue->setEnabled(true);
    else
        ui->pValue->setEnabled(false);
}

void CalcDialog::ok()
{
    accept();
}

QString CalcDialog::fun(double val)
{
    int ii, pos;
    double ret = val;

    ii = allButtons->checkedId();
    double val2=0.;
    if( ii <= 5 )
    {
        QString str = ui->pValue->currentText();
        pos = str.indexOf('(');
        if( pos >= 0 )
            str.truncate(pos);
        if( (str.indexOf("---") != -1) || str[0] == '`' )
            val2 = DOUBLE_EMPTY;
        else
            val2 = str.toDouble();  /// check for error
    }
    switch( ii )
    {
    case 0:
        ret = val2;
        break;
    case 1:
        ret = val + val2;
        break;
    case 2:
        ret = val - val2;
        break;
    case 3:
        ret = val * val2;
        break;
    case 4:
        if( fabs(val2) >=  IPNC_DBL_MIN ) // Attempt of zerodivide!
            ret = val / val2;
        break;
    case 5:
        if( !(fabs(val)<IPNC_DBL_MIN || fabs(val)>IPNC_DBL_MAX
              || fabs(val2) < IPNC_DBL_MIN_10_EXP
              || fabs(val2) > IPNC_DBL_MAX_10_EXP) ) // Attempt of pow() argument out of range
            ret = pow(val, val2);
        break;
    case 6:
        if( fabs(val) >= IPNC_DBL_MIN ) // Attempt of zerodivide!
            ret = 1/val;
        break;
    case 7:
        if( val >= IPNC_DBL_MIN ) // Attempt of sqrt() argument <= 0
            ret = sqrt(val);
        break;
    case 8:
        if( val >= IPNC_DBL_MIN )
            ret = log10(val);
        break;
    case 9:
        if( val >= IPNC_DBL_MIN )
            ret = log(val);
        break;
    case 10:
        if( !( fabs(val) < IPNC_DBL_MIN_10_EXP
               || fabs(val) > IPNC_DBL_MAX_10_EXP) ) // Attempt of pow() argument out of range
            ret = pow(10.,val);
        break;
    case 11:
        if( !(val < IPNC_DBL_MIN_EXP || val > IPNC_DBL_MAX_EXP) ) // Attempt of exp() argument out of range
            ret =  exp(val);
        break;
    case 12:
        ret = sin(val);
        break;
    case 13:
        ret = cos(val);
        break;
    case 14:
        ret = tan(val);
        break;
    case 15:
        ret = DOUBLE_EMPTY; //No Data value
    }

    QString retstr;
    if( IsDoubleEmpty( ret ) )
        retstr = "---";
    else
        retstr = QString::number(  ret, 'g', 6 );//QVariant(ret).toString();
    return retstr;
}

int CalcDialog::funName(double& val)
{
    int ii, pos;

    ii = allButtons->checkedId();

    val=0.;
    if( ii <= 5 )
    {
        QString str = ui->pValue->currentText();
        pos = str.indexOf('(');
        if( pos >= 0 )
            str.truncate(pos);
        if( (str.indexOf("---") != -1) || str[0] == '`' )
            val = DOUBLE_EMPTY;
        else
            val = str.toDouble();  /// check for error
    }
    return ii;
}


string CalcDialog::funText(const char * valText )
{
    int ii, pos;
    QString res;

    ii = allButtons->checkedId();
    QString strText = "";

    if( ii <= 5 )
    {
        strText = ui->pValue->currentText();
        pos = strText.indexOf('(');
        if( pos >= 0 )
            strText.truncate(pos);
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

    string rres = res.toStdString();
    return rres;
}


void CalcDialog::help()
{
    pVisorImp->OpenHelp( GEMS_CALCUL_HTML );
}
//--------------------- End of CalcDialog.cpp ---------------------------
