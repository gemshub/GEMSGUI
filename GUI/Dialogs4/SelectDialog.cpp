//-------------------------------------------------------------------
// $Id: SelectDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SelectDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------


#include <qlabel.h>
#include <qpushbutton.h>
#include <qvariant.h>

#include "SelectDialog.h"
#include "GemsMainWindow.h"
#include "service.h"


SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            const vector<string>& list, int sel):
	    QDialog(parent),
        multi(false), sel_to_all(false)
{
    setupUi(this);
    pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);
    unsigned int lens=30;
    for( int ii=0; ii<list.size(); ii++ )
    {
        pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection); //pList->setMultiSelection(false);
    if( sel < 0 || sel > list.size() )
	    sel = 0;
    pList->setCurrentRow(sel); // setSelected(sel, true);

    pButton3->hide();
    pButton2->hide();
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

// Added Sveta 14/06/01 Selection to all
SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            const vector<string>& list, int sel, bool ):
   	    QDialog(parent),
        multi(false), sel_to_all(false)
{
    setupUi(this);
    pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);
    unsigned int lens=30;
    for( int ii=0; ii<list.size(); ii++ )
    {
        pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection); //pList->setMultiSelection(false);
    if( sel < 0 || sel > list.size() )
 	    sel = 0;
     pList->setCurrentRow(sel); // setSelected(sel, true);

    pButton3->hide();
    pButton2->setText( tr( "Ok to &All"  ) );
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            const vector<string>& list,
                            const vector<int>& sel ):
   	    QDialog(parent),
        multi(true), sel_to_all(false)
{
    setupUi(this);
    pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);

    pList->setSelectionMode(QAbstractItemView::MultiSelection); //pList->setMultiSelection(true);
    unsigned int lens=30;
    for( int ii=0; ii<list.size(); ii++ )
    {
        pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    //  pList->setCurrentRow(0);
    for( int jj=0; jj< sel.size(); jj++ )
    {
        if( sel[jj] < pList->count() )
            pList->item(sel[jj])->setSelected( true);
    }
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

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

vector<int>
SelectDialog::allSelected()
{
    vector<int> arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<pList->count(); ii++ )
        if( pList->item(ii)->isSelected() )
            arr.push_back(ii);
    
    return arr;
}

SelectDialog::~SelectDialog()
{}

void SelectDialog::languageChange()
{
    retranslateUi(this);
}

void SelectDialog::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_MARK_HTML );
}

//--------------------- End of SelectDialog.cpp ---------------------------

