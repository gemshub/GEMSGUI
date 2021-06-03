//-------------------------------------------------------------------
// $Id: ChangeKeyDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ChangeKeyDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#include "ui_ChangeKeyDialog4.h"
#include "ChangeKeyDialog.h"
#include "GemsMainWindow.h"
#include "service.h"


ChangeKeyDialog::ChangeKeyDialog(QWidget* win, int nsymb, const char* /*caption*/ ):
    QDialog( win),
    ui(new Ui::ChangeKeyDialogData),
    nSymbols(nsymb)
{
    ui->setupUi(this);
    //    setCaption( caption );

    ui->lTo1->setMaxLength( nSymbols );
    ui->lfirst->setMaxLength( nSymbols );
    ui->llast->setMaxLength( nSymbols );
    ui->nfirst->setMaximum(nSymbols-1);
    ui->nlast->setMaximum(nSymbols-1);

    connect( ui->lTo1, SIGNAL(editingFinished()), this, SLOT(SetToString()) );
    connect( ui->lfirst, SIGNAL(editingFinished()), this, SLOT(SetToString()) );
    connect( ui->llast, SIGNAL(editingFinished()), this, SLOT(SetToString()) );
    connect( ui->bInv1, SIGNAL(toggled(bool)), this, SLOT(SetToString()) );
    connect( ui->badd1, SIGNAL(toggled(bool)), this, SLOT(SetToString()) );
    connect( ui->bchange1, SIGNAL(toggled(bool)), this, SLOT(SetToString()) );
    connect( ui->chfirst, SIGNAL(toggled(bool)), this, SLOT(SetToString()) );
    connect( ui->chlast, SIGNAL(toggled(bool)), this, SLOT(SetToString()) );

    connect( ui->nfirst, SIGNAL(valueChanged(int)), this, SLOT(SetFromString()) );
    connect( ui->nlast, SIGNAL(valueChanged(int)), this, SLOT(SetFromString()) );

    connect( ui->rAll, SIGNAL(toggled(bool)), this, SLOT(SetAll(bool)) );
    connect( ui->rPart, SIGNAL(toggled(bool)), this, SLOT(SetPart(bool)) );

    QObject::connect(ui->pbHelp, SIGNAL(clicked()), this, SLOT(CmHelp()));
    QObject::connect(ui->pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->pbOk, SIGNAL(clicked()), this, SLOT(accept()));
}


ChangeKeyDialog::~ChangeKeyDialog()
{
    delete ui;
}

string ChangeKeyDialog::getTemplFrom()
{
    SetFromString();
    string ret = ui->templFrom->text().toStdString();
    return ret;
}

string ChangeKeyDialog::getTemplTo()
{
    SetToString();
    string ret = ui->templTo->text().toStdString();
    return ret;
}

void ChangeKeyDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_TEMPL_HTML );
}


void ChangeKeyDialog::SetFromString()
{
    string ret;

    if(ui->rAll->isChecked())
        ret = "*";
    else //  ??*???, *?, ??*
    {
        auto n = ui->nfirst->value();
        ret = string(n, '?');
        ret+= "*";
        n = ui->nlast->value();
        ret += string(n, '?');
    }

    ui->templFrom->setText( ret.c_str() );
}

void ChangeKeyDialog::SetToString()
{
    string ret="";
    string part;

    if(ui->rAll->isChecked()) // *abc, abc, invcase
    {
        if( ui->bInv1->isChecked())
            ret = "invcase";
        else
        {
            if( ui->badd1->isChecked())
                ret = "*";
            part = ui->lTo1->text().toStdString();
            ret += part;
        }
    }
    else //  ab*abc, invcase*a, bb*invcase, *bb, a*
    {
        if(ui->chfirst->isChecked())
            ret = "invcase";
        else
            ret =  ui->lfirst->text().toStdString();
        ret += "*";
        if(ui->chlast->isChecked())
            ret += "invcase";
        else
            ret +=  ui->llast->text().toStdString();
    }

    ui->templTo->setText( ret.c_str() );
}

void ChangeKeyDialog::SetAll( bool vl )
{
    ui->group1->setEnabled(vl);
    ui->rPart->setChecked(!vl);
    SetFromString();
    SetToString();
}

void ChangeKeyDialog::SetPart( bool vl )
{
    ui->group1_2->setEnabled(vl);
    ui->rAll->setChecked(!vl);
    SetFromString();
    SetToString();
}

//--------------------- End of ChangeKeyDialog.cpp ---------------------------

