//-------------------------------------------------------------------
// $Id: SelectDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SelectDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
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


#include "ui_SelectDialog4.h"
#include "SelectDialog.h"
#include "GemsMainWindow.h"
#include "service.h"


SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, int sel):
    QDialog(parent),
    ui(new Ui::SelectDialogData),
    multi(false), sel_to_all(false)
{
    ui->setupUi(this);
    QObject::connect(ui->PushButton1, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->PushButton4, SIGNAL(clicked()), this, SLOT(reject()));
    //QObject::connect(ui->pButton2, SIGNAL(clicked()), this, SLOT(CmSelectAll()));
    //QObject::connect(ui->pButton3, SIGNAL(clicked()), this, SLOT(CmClearAll()));

    ui->pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);
    size_t lens=30;
    for( size_t ii=0; ii<list.size(); ii++ )
    {
        ui->pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    ui->pList->setSelectionMode(QAbstractItemView::SingleSelection); //ui->pList->setMultiSelection(false);
    if( sel < 0 || sel > static_cast<int>(list.size()) )
        sel = 0;
    ui->pList->setCurrentRow(sel); // setSelected(sel, true);

    ui->pButton3->hide();
    ui->pButton2->hide();
    QObject::connect( ui->bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ui->pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

// Added Sveta 14/06/01 Selection to all
SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, int sel, bool ):
    QDialog(parent),
    ui(new Ui::SelectDialogData),
    multi(false), sel_to_all(false)
{
    ui->setupUi(this);
    QObject::connect(ui->PushButton1, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->PushButton4, SIGNAL(clicked()), this, SLOT(reject()));
    //QObject::connect(ui->pButton2, SIGNAL(clicked()), this, SLOT(CmSelectAll()));
    //QObject::connect(ui->pButton3, SIGNAL(clicked()), this, SLOT(CmClearAll()));

    ui->pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);
    size_t lens=30;
    for( size_t ii=0; ii<list.size(); ii++ )
    {
        ui->pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    ui->pList->setSelectionMode(QAbstractItemView::SingleSelection); //ui->pList->setMultiSelection(false);
    if( sel < 0 || sel > static_cast<int>(list.size()) )
        sel = 0;
    ui->pList->setCurrentRow(sel); // setSelected(sel, true);

    ui->pButton3->hide();
    ui->pButton2->setText( tr( "Ok to &All"  ) );
    QObject::connect( ui->bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ui->pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

SelectDialog::SelectDialog( QWidget* parent, const char* title,
                            TCStringArray& list, TCIntArray& sel ):
    QDialog(parent),
    ui(new Ui::SelectDialogData),
    multi(true), sel_to_all(false)
{
    ui->setupUi(this);
    QObject::connect(ui->PushButton1, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->PushButton4, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->pButton2, SIGNAL(clicked()), this, SLOT(CmSelectAll()));
    QObject::connect(ui->pButton3, SIGNAL(clicked()), this, SLOT(CmClearAll()));

    ui->pList->setFont( pVisorImp->getCellFont() );

    setWindowTitle(title);

    ui->pList->setSelectionMode(QAbstractItemView::MultiSelection); //ui->pList->setMultiSelection(true);
    size_t lens=30;
    for( size_t ii=0; ii<list.size(); ii++ )
    {
        ui->pList->addItem(list[ii].c_str());
        if( list[ii].length() > lens )
            lens = list[ii].length();
    }

    //  ui->pList->setCurrentRow(0);
    for( size_t jj=0; jj< sel.size(); jj++ )
    {
        if( sel[jj] < ui->pList->count() )
            ui->pList->item(sel[jj])->setSelected( true);
    }
    QObject::connect(  ui->bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ui->pList->setFocus();
    if( lens > 70 ) lens = 70;
    resize( lens*10, 300);
}

void SelectDialog::CmSelectAll()
{
    if( multi == true )
    {// select all strings
        ui->pList->selectAll();
        //ui->pList->setFocus();
        //for( uint ii=0; ii<ui->pList->count(); ii++ )
        //   ui->pList->item(ii)->setSelected(true);
    }
    else
    {
        sel_to_all = true;
        accept();
    }
}


void SelectDialog::CmClearAll()
{
    ui->pList->clearSelection();
}

/*! returns selection array
    array is empty if nothing is selected
*/

TCIntArray SelectDialog::allSelected()
{
    TCIntArray arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<ui->pList->count(); ii++ )
        if( ui->pList->item(ii)->isSelected() )
            arr.push_back(ii);
    
    return arr;
}

SelectDialog::~SelectDialog()
{
    delete ui;
}

int SelectDialog::selected()
{
    return (( result() ) ? ui->pList->currentRow() : -1);
}

int SelectDialog::selected(bool &all_)
{
    all_ = sel_to_all;
    return ( ( result() ) ? ui->pList->currentRow() : -1);
}

void SelectDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_MARK_HTML );
}

//--------------------- End of SelectDialog.cpp ---------------------------

