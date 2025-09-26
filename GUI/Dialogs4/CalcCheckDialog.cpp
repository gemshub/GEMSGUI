//-------------------------------------------------------------------
// $Id: CalcCheckDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CalcCheckDialog class
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

#include "ui_CalcCheckDialog4.h"
#include "CalcCheckDialog.h"
#include "units.h"
#include "page_f.h"

CalcCheckDialog::CalcCheckDialog(QWidget* parent,
                                 const int nO, const std::string& Vals):
    QDialog( parent ),
    ui(new Ui::CalcCheckDialogData),
    nO_(nO)
{
    ui->setupUi(this);;
    setWindowTitle( "Check boxes Calculator" );

    if( nO >= 0 )
        rObj = aObj[static_cast<size_t>(nO)].get();
    else
        rObj = new TObject( "tempor", ftCheckBox, 1, 1, false, 'N', "temporary object");

    // QString str =  rObj->GetKeywd();
    //        str += " cell range";

    for( uint ii=0; ii<Vals.length(); ii++ )
        ui->pValue->addItem( Vals.substr(ii, 1).c_str() );

    ui->pValue->setCurrentIndex(0);
    QObject::connect(ui->PushButton1_2, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->PushButton1, SIGNAL(clicked()), this, SLOT(accept()));
}

CalcCheckDialog::~CalcCheckDialog()
{
    if( nO_ < 0 && rObj )
        delete rObj;
    delete ui;
}

int CalcCheckDialog::fun()
{
    return ui->pValue->currentIndex();
}

//--------------------- End of CalcCheckDialog.cpp ---------------------------

