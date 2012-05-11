//-------------------------------------------------------------------
// $Id: CalcCheckDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CalcCheckDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
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

#include <qcombobox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include "CalcCheckDialog.h"
#include "units.h"
#include "service.h"
#include "page_f.h"

CalcCheckDialog::CalcCheckDialog(QWidget* parent,
     const int nO, const gstring& Vals):
        QDialog( parent ),
        nO_(nO)
{
   setupUi(this);;
   setWindowTitle( "Check boxes Calculator" );

   if( nO >= 0 )
        rObj = &aObj[nO];
   else
        rObj = new TObject( "tempor", ftCheckBox, 1, 1, false, 'N', "temporary object");
	
   // QString str =  rObj->GetKeywd();
   //        str += " cell range";

    for( uint ii=0; ii<Vals.length(); ii++ )
        pValue->addItem( Vals.substr(ii, 1).c_str() );

    pValue->setCurrentIndex(0);
}

 CalcCheckDialog::~CalcCheckDialog()
 {
   if( nO_ < 0 && rObj )
	delete rObj;  
  }

void CalcCheckDialog::languageChange()
{
    retranslateUi(this);
}

void CalcCheckDialog::CmOk()
{
    accept();
}

int CalcCheckDialog::fun()
{
    return pValue->currentIndex();
}

//--------------------- End of CalcCheckDialog.cpp ---------------------------

