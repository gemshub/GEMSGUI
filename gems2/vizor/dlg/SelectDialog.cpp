//-------------------------------------------------------------------
// Id: gems/vizor/dlg/SelectDialog.cpp  version 2.0.0    2001
//
// Implementation of SelectDialog class
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

#include <qlistbox.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "SelectDialog.h"
#include "visor_w.h"

#define Inherited SelectDialogData

SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, int sel):
        Inherited( parent, 0, true ),
        multi(false), sel_to_all(false)
{
    pList->setFont( pVisorImp->getCellFont() );

    setCaption(title);
    unsigned int lens=30;
    for( uint ii=0; ii<list.GetCount(); ii++ )
    {
        pList->insertItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    pList->setMultiSelection(false);
    pList->setSelected(sel, true);

    pButton3->hide();
    pButton2->hide();

    pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}
// Added Sveta 14/06/01 Selection to all
SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, int sel, bool ):
        Inherited( parent, 0, true ),
        multi(false), sel_to_all(false)
{
    pList->setFont( pVisorImp->getCellFont() );

    setCaption(title);
    unsigned int lens=30;
    for( uint ii=0; ii<list.GetCount(); ii++ )
    {
        pList->insertItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    pList->setMultiSelection(false);
    pList->setSelected(sel, true);

    pButton3->hide();
    pButton2->setText( tr( "Ok to &All"  ) );

    pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, TCIntArray& sel ):
        Inherited( parent, 0, true ),
        multi(true), sel_to_all(false)
{
    pList->setMultiSelection(true);
    setCaption(title);
    unsigned int lens=30;
    for( uint ii=0; ii<list.GetCount(); ii++ )
    {
        pList->insertItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    pList->setCurrentItem(0); // for Qt-2.0 bug
    //  pList->setSelected(0, true);
    for( uint jj=0; jj<sel.GetCount(); jj++ )
        pList->setSelected(sel[jj], true);


    pList->setFocus();
    resize( lens*10, 300);
}

void
SelectDialog::CmSelectAll()
{
    if( multi == true )
    {// select all gstrings
       for( uint ii=0; ii<pList->count(); ii++ )
        pList->setSelected(ii, true);
     }
     else
    {
     sel_to_all = true;
     accept();
    }
}


void
SelectDialog::CmClearAll()
{
    pList->clearSelection();
}

/*! returns selection array
    array is empty if nothing is selected
*/

TCIntArray
SelectDialog::allSelected()
{
    TCIntArray arr;

    if( !result() )
        return arr;

    for( uint ii=0; ii<pList->count(); ii++ )
        if( pList->isSelected(ii) )
            arr.Add(ii);
    return arr;
}

SelectDialog::~SelectDialog()
{}
//--------------------- End of SelectDialog.cpp ---------------------------

