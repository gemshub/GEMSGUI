//-------------------------------------------------------------------
// $Id$
//
// Implementation of CalcCheckDialog class
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

#include <qcombobox.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlabel.h>

#include "CalcCheckDialog.h"
#include "units.h"
#include "service.h"

#define Inherited CalcCheckDialogData

CalcCheckDialog::CalcCheckDialog(QWidget* parent,
                                 const TObject& obj, const gstring& Vals, QRect& selection):
        Inherited( parent, 0, true ),
        sel( selection ),
        rObj(obj)
{
    setCaption( "Check boxes Calculator" );
    pObjectBox->setTitle(obj.GetKeywd());

    // set selection
    pN1->setValue( sel.top() );
    pN2->setValue( sel.bottom() );
    pM->setText( QString().setNum(sel.left()) );

    pN1->setRange( 0, obj.GetN()-1 );
    pN2->setRange( 0, obj.GetN()-1 );

    for( uint ii=0; ii<Vals.length(); ii++ )
        pValue->insertItem( Vals.substr(ii, 1).c_str() );

    pValue->setCurrentItem(0);
}


CalcCheckDialog::~CalcCheckDialog()
{}



void
CalcCheckDialog::CmSelectAll()
{
    pN1->setValue(0);
    pN2->setValue(rObj.GetN()-1);
}

void
CalcCheckDialog::CmOk()
{
    if( pN1->value() > pN2->value() )
    {
        vfMessage(this, "Checkbox calculator", "Invalid intervals", vfErr);
        return;
    }

    sel.setTop( pN1->value() );
    sel.setBottom( pN2->value() );

    accept();
}

int
CalcCheckDialog::fun()
{
    return pValue->currentItem();
}

//--------------------- End of CalcCheckDialog.cpp ---------------------------

