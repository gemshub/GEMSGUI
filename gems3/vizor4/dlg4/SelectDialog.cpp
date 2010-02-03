//-------------------------------------------------------------------
// $Id: SelectDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SelectDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
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


#include <qlabel.h>
#include <qpushbutton.h>
#include <qvariant.h>

#include "SelectDialog.h"
#include "visor_w.h"


SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, int sel):
	    QDialog(parent),
        multi(false), sel_to_all(false)
{
    setupUi(this);
    pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);
    unsigned int lens=30;
    for( uint ii=0; ii<list.GetCount(); ii++ )
    {
        pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection); //pList->setMultiSelection(false);
    if( sel < 0 || (uint)sel > list.GetCount() )
	    sel = 0;
    pList->setCurrentRow(sel); // setSelected(sel, true);

    pButton3->hide();
    pButton2->hide();

    pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

// Added Sveta 14/06/01 Selection to all
SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, int sel, bool ):
   	    QDialog(parent),
        multi(false), sel_to_all(false)
{
    setupUi(this);
    pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);
    unsigned int lens=30;
    for( uint ii=0; ii<list.GetCount(); ii++ )
    {
        pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection); //pList->setMultiSelection(false);
    if( sel < 0 || (uint)sel > list.GetCount() )
 	    sel = 0;
     pList->setCurrentRow(sel); // setSelected(sel, true);

    pButton3->hide();
    pButton2->setText( tr( "Ok to &All"  ) );

    pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, TCIntArray& sel ):
   	    QDialog(parent),
        multi(true), sel_to_all(false)
{
    setupUi(this);
    pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);

    pList->setSelectionMode(QAbstractItemView::MultiSelection); //pList->setMultiSelection(true);
    unsigned int lens=30;
    for( uint ii=0; ii<list.GetCount(); ii++ )
    {
        pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    //  pList->setCurrentRow(0);
    for( uint jj=0; jj<sel.GetCount(); jj++ )
        pList->item(sel[jj])->setSelected( true);

    pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

void
SelectDialog::CmSelectAll()
{
    if( multi == true )
    {// select all gstrings
      pList->selectAll();
      //pList->setFocus();
      //for( uint ii=0; ii<pList->count(); ii++ )
      //   pList->item(ii)->setSelected(true);
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

    for( int ii=0; ii<pList->count(); ii++ )
        if( pList->item(ii)->isSelected() )
            arr.Add(ii);
    
    return arr;
}

SelectDialog::~SelectDialog()
{}

void SelectDialog::languageChange()
{
    retranslateUi(this);
}


//--------------------- End of SelectDialog.cpp ---------------------------

