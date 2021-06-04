//-----------------------------------------------------------
// $Id: KeyDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyDialog class
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

#include <QListWidget>

#include "ui_KeyDialog4.h"
#include "KeyDialog.h"
#include "KeyFilter.h"
#include "GemsMainWindow.h"
#include "v_module.h"
#include "v_mod.h"
#include "service.h"

KeyDialog::KeyDialog(QWidget* win, size_t irt, const char* key,
                     const char* caption, bool filter):
    QDialog( win),
    ui(new Ui::KeyDialogData),
    multi(false), iRt(irt)
{
    ui->setupUi(this);

    old_sel.clear();
    ui->pList->setFont( pVisorImp->getCellFont() );
    QObject::connect(ui->PushButton1, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->PushButton3, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->pFilterButton, SIGNAL(clicked()), this, SLOT(CmFilter()));
    QObject::connect(ui->pButton2, SIGNAL(clicked()), this, SLOT(CmSelectAll()));
    QObject::connect(ui->pButton3, SIGNAL(clicked()), this, SLOT(CmClearAll()));
    QObject::connect(ui->pList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
    setWindowTitle( caption );

    TCIntArray temp;
    TCStringArray keyList;

    ErrorIf(!key, "KeyDialog", "pkey is null");

    if( strpbrk(key, "*?") == nullptr )
        keyFilter = "*";
    else
        keyFilter = key;

    string s = "Please, select one record key. Filter: ";
    s +=  keyFilter;
    ui->pLabel->setText(s.c_str());

    s  = string(rt[irt]->UnpackKey());
    auto n = rt[irt]->GetKeyList( keyFilter.c_str(), keyList, temp);
    uint sel = 0;

    for( uint ii=0; ii<n; ii++ )
    {
        ui->pList->addItem(keyList[ii].c_str());
        if(  keyList[ii]==s )
            sel = ii;
    }

    ui->pList->setSelectionMode(QAbstractItemView::SingleSelection); // ui->pList->setMultiSelection(false);
    if(  sel > n )
        sel = 0;
    ui->pList->setCurrentRow(sel); // ui->pList->setSelected(sel, true);
    ui->pButton3->hide();
    ui->pButton2->hide();
    QObject::connect( ui->bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ui->pList->setFocus();

    if( !filter )
        ui->pFilterButton->hide();
}

KeyDialog::KeyDialog(QWidget* win, size_t irt, TCStringArray& sel,
                     const char* key, const char* caption):
    QDialog( win),
    ui(new Ui::KeyDialogData),
    multi(true), iRt(irt)
{
    ui->setupUi(this);
    QObject::connect(ui->PushButton1, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->PushButton3, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->pFilterButton, SIGNAL(clicked()), this, SLOT(CmFilter()));
    QObject::connect(ui->pButton2, SIGNAL(clicked()), this, SLOT(CmSelectAll()));
    QObject::connect(ui->pButton3, SIGNAL(clicked()), this, SLOT(CmClearAll()));

    old_sel.clear();
    for(size_t ii=0; ii<sel.size(); ii++)
    {   if( strchr( sel[ii].c_str(), ':' ))  // key in packed form
        {
            rt[irt]->SetKey( sel[ii].c_str() );
            old_sel.push_back(rt[irt]->UnpackKey());
        }
        else
            old_sel.push_back( sel[ii] );
    }
    
    ui->pList->setFont( pVisorImp->getCellFont() );
    ui->pList->setSelectionMode(QAbstractItemView::MultiSelection);  // ui->pList->setMultiSelection(true);
    setWindowTitle( caption );
    QObject::connect( ui->bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ErrorIf(!key, "KeyDialog", "pkey is null");
    if( strpbrk(key, "*?") == nullptr )
        keyFilter = "*";
    else
        keyFilter = key;

    SetList();
    ui->pList->setFocus();
}


KeyDialog::~KeyDialog()
{
    delete ui;
}

void KeyDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_SELECT_HTML );
}

void KeyDialog::SetList()
{
    TCIntArray temp;
    TCStringArray keyList;

    string s = "Please, mark one or more record keys. Filter: ";
    s +=  keyFilter;
    ui->pLabel->setText(s.c_str());

    auto n = rt[iRt]->GetKeyList( keyFilter.c_str(), keyList, temp);

    for( uint ii=0; ii<n; ii++ )
        ui->pList->addItem(keyList[ii].c_str());

    if( multi )
    {  for(size_t jj=0; jj<old_sel.size(); jj++)
            for( uint ii=0; ii<n; ii++ )
            {
                // comparing parts before '*' for overwrite dcomp, reacdc ....
                size_t pos = old_sel[jj].find('*');
                string str(old_sel[jj], 0, pos);
                if( keyList[ii].find(str) != string::npos )
                {
                    ui->pList->item(ii)->setSelected( true); //ui->pList->setSelected(ii, true);
                    break;
                }
            }
    }
}


// olvase unpaked keys?
string KeyDialog::getKey()
{
    int sel = ui->pList->currentRow(); //ui->pList->currentItem();
    if( sel != -1 )
    {
        dynamic_cast<TCModule*>(aMod[iRt].get())->setFilter(keyFilter.c_str());
        string res;
        string s = ui->pList->item(sel)->text().toStdString();
        //string s = ss;
        uint ln;
        for( uint ii=0, jj=0; ii<rt[iRt]->KeyNumFlds(); ii++)
        {
            //pos = strchr( s+jj, ":" );
            ln = rt[iRt]->FldLen(ii);
            // if( pos) ln = min( ln, pos-s+jj );

            //res += string(s+jj, 0, ln);
            res += string(s, jj, ln);
            StripLine(res);
            res += ":";
            jj += ln;
        }
        return res;
    }
    return string();
}


void KeyDialog::CmFilter()
{
    string str_name = "Template for ";
    str_name +=  rt[iRt]->GetKeywd();
    str_name += " record key";
    KeyFilter dbFilter(this, iRt, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->pList->clear();
        SetList();
        ui->pList->setCurrentRow(0);
    }
}

void KeyDialog::CmSelectAll()
{
    // select all strings
    for( int ii=0; ii<ui->pList->count(); ii++ )
        ui->pList->item(ii)->setSelected( true);
}

void KeyDialog::CmClearAll()
{
    ui->pList->clearSelection();
}

/*! returns selection array
    array is empty if nothing is selected
*/
TCStringArray KeyDialog::allSelectedKeys()
{
    TCStringArray arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<ui->pList->count(); ii++ )
        if( ui->pList->item(ii)->isSelected() )
        {
            string s = ui->pList->item(ii)->text().toStdString();;
            arr.push_back(s.c_str());
        }
    return arr;
}


//------------------------------------------------------
// RDKeyDialog - work with ReacDC&DComp keys lists
RDKeyDialog::RDKeyDialog(QWidget* win, TCStringArray& sel,
                         const char* key, const char* caption, short aNsuT):
    QDialog( win),
    ui(new Ui::KeyDialogData), NsuT(aNsuT)
{
    ui->setupUi(this);
    QObject::connect(ui->PushButton1, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->PushButton3, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui->pFilterButton, SIGNAL(clicked()), this, SLOT(CmFilter()));
    QObject::connect(ui->pButton2, SIGNAL(clicked()), this, SLOT(CmSelectAll()));
    QObject::connect(ui->pButton3, SIGNAL(clicked()), this, SLOT(CmClearAll()));

    old_sel.clear();

    for(size_t ii=0; ii<sel.size(); ii++)
    {   if( strchr( sel[ii].c_str(), ':' ))  // key in packed form
        {
            if( sel[ii][0] == SRC_REACDC )
            {  rt[RT_REACDC]->SetKey( sel[ii].c_str()+2 );
                old_sel.append( makeKey( SRC_REACDC, rt[RT_REACDC]->UnpackKey()));
            }
            else
            {  rt[RT_DCOMP]->SetKey( sel[ii].c_str()+2 );
                old_sel.append( makeKey( SRC_DCOMP, rt[RT_DCOMP]->UnpackKey()));
            }
        }
        else
            old_sel.append( sel[ii].c_str() );
    }

    ui->pList->setFont( pVisorImp->getCellFont() );
    ui->pList->setSelectionMode(QAbstractItemView::MultiSelection);
    setWindowTitle( caption );
    QObject::connect( ui->bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ErrorIf(!key, "KeyDialog", "pkey is null");
    if( strpbrk(key, "*?") == nullptr )
        keyFilter = "*";
    else
        keyFilter = key;

    SetList();
    ui->pList->setFocus();
}


RDKeyDialog::~RDKeyDialog()
{
    delete ui;
}

void RDKeyDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_SELECT_HTML );
}

QString RDKeyDialog::makeKey( char type, const char *key )
{
    QString str = QString(type);
    str += ' ';
    str += key;
    return str;
}

void RDKeyDialog::SetList()
{
    TCIntArray temp;
    TCStringArray keyList;
    int jj=0;

    string s = "Please, mark one or more record keys. Filter: ";
    s +=  keyFilter;

    // ReacDC list
    if( NsuT > 0 )  // template for adsorption
        keyFilter[0] = CP_SSPC;
    auto n = rt[RT_REACDC]->GetKeyList( keyFilter.c_str(), keyList, temp);

    for( uint ii=0; ii<n; ii++ )
        ui->pList->addItem( makeKey( SRC_REACDC, keyList[ii].c_str()));

    // Dcomp list
    if( NsuT > 0 )  // template for adsorption
    {    keyFilter[0] = CP_SOLID;
        s+= "\\";
        s+= keyFilter;
    }

    n = rt[RT_DCOMP]->GetKeyList( keyFilter.c_str(), keyList, temp);

    for( uint ii=0; ii<n; ii++ )
        ui->pList->addItem( makeKey( SRC_DCOMP, keyList[ii].c_str()));

    for( jj=0; jj<old_sel.count(); jj++)
    {
        QString str = old_sel[jj];
        int pos = str.indexOf('*', 0);
        if( pos > 0 )
            str.truncate ( pos );
        for(int ii=0; ii<ui->pList->count(); ii++ )
        {
            // comparing parts before '*' for overwrite dcomp, reacdc ....
            if( ui->pList->item(ii)->text().contains(str/*old_sel[jj]*/) )
            {
                ui->pList->item(ii)->setSelected( true); //ui->pList->setSelected(ii, true);
                break;
            }
        }
    }
    ui->pLabel->setText(s.c_str());
}


void RDKeyDialog::CmFilter()
{
    string str_name = "Template for ";
    str_name +=  rt[RT_REACDC]->GetKeywd();
    str_name +=  "&";
    str_name +=  rt[RT_DCOMP]->GetKeywd();
    str_name += " record key";
    KeyFilter dbFilter(this, RT_DCOMP, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->pList->clear();
        SetList();
        ui->pList->setCurrentRow(0);
    }
}

void RDKeyDialog::CmSelectAll()
{
    // select all strings
    for( int ii=0; ii<ui->pList->count(); ii++ )
        ui->pList->item(ii)->setSelected( true);
}


void RDKeyDialog::CmClearAll()
{
    ui->pList->clearSelection();
}

/*! returns selection array
    array is empty if nothing is selected
*/
TCStringArray RDKeyDialog::allSelectedKeys()
{
    TCStringArray arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<ui->pList->count(); ii++ )
        if( ui->pList->item(ii)->isSelected() )
        {
            string s = ui->pList->item(ii)->text().toStdString();;
            arr.push_back(s.c_str());
        }
    return arr;
}

//--------------------- End of KeyDialog.cpp ---------------------------

