//-------------------------------------------------------------------
// $Id: CalcCheckDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CalcCheckDialog class
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

#include <qcombobox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include "CalcCheckDialog.h"
#include "units.h"
//#include "service.h"
#include "page_f.h"

CalcCheckDialog::CalcCheckDialog(QWidget* parent,
     const int nO, const gstring& Vals):
        QDialog( parent ),
        nO_(nO)
{
   setupUi(this);;
   setWindowTitle( "Check boxes Calculator" );

   if( nO >= 0 )
        rObj = &aObj[static_cast<size_t>(nO)];
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

