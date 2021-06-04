//-------------------------------------------------------------------
// $Id: ListFilesDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ListFilesDialog class
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

#include <QTreeWidget>

#include "ui_ListFilesDialog4.h"
#include "ListFilesDialog.h"
#include "GemsMainWindow.h"
#include "visor.h"
#include "service.h"

ListFilesDialog::ListFilesDialog(QWidget* parent, const char* prfName, const char* /*caption*/ )
    : QDialog(parent),
      ui(new Ui::ListFilesDialogData)
{
    unsigned int i, ii;
    ui->setupUi(this);

    //setWindowTitle(prfName);
    string label = "Please, check database files to be linked to Modeling Project ";
    label += prfName;
    ui->pLabel->setText( tr(label.c_str()) );

    ui->pListFiles->setFont( pVisorImp->getCellFont() );
    ui->pListFiles->setSelectionMode(QAbstractItemView::MultiSelection);    //setMultiSelection(true);
    ui->pListFiles->setSortingEnabled( false );                            //setSorting(-1);
    ui->pListFiles->setRootIsDecorated(true);
    ui->pListFiles->setAllColumnsShowFocus(true);
    // setCaption(caption);
    
    QTreeWidgetItem* pdb = new QTreeWidgetItem( ui->pListFiles );
    pdb->setText(0,"Database");
    pkern = new QTreeWidgetItem( pdb );
    pkern->setText(0,"Built-in" );
    pprf = new QTreeWidgetItem( pdb );
    pprf->setText(0,prfName );

    QTreeWidgetItem* pMod;
    QTreeWidgetItem* pFile;
    size_t pos1, pos2;

    for( i=0; i<aMod.size(); i++)
    {
        if( aMod[i]->IsSubModule() )
            continue;
        pMod = new QTreeWidgetItem( pkern );
        pMod->setText(0, rt[i]->GetKeywd());
        TCStringArray names;
        TCIntArray indx;
        TCIntArray sel;
        rt[i]->GetFileList(closef|openf|oldself, names, indx, sel);
        for( ii=0; ii<names.size(); ii++ )
        {
            // select only DB.default files
            if( names[ii].find( pVisor->sysDBDir())== string::npos )
                continue;
            // get 2 colums
            pos1 = names[ii].find_first_of(" ");
            pos2 = names[ii].rfind("/");
            pFile = new QTreeWidgetItem( pMod );
            pFile->setText(0,  names[ii].substr( 0, pos1 ).c_str() );
            pFile->setText(1,  names[ii].substr( pos2+1 ).c_str() );
            if( findIndex<int>( sel, indx[ii]) >= 0 )
                pFile->setSelected( true );
        }
    }

    for( i=0; i<aMod.size(); i++)
    {
        if( aMod[i]->IsSubModule() )
            continue;
        pMod = new QTreeWidgetItem( pprf);
        pMod->setText(0, rt[i]->GetKeywd());
        TCStringArray names;
        TCIntArray indx;
        TCIntArray sel;
        rt[i]->GetFileList(closef|openf|oldself, names, indx, sel);
        for( ii=0; ii<names.size(); ii++ )
        {
            // select only Projecte files
            if( names[ii].find( prfName/*Path*/ ) == string::npos )
                continue;
            // get 2 colums
            pos1 = names[ii].find_first_of(" ");
            pos2 = names[ii].rfind("/");
            pFile = new QTreeWidgetItem( pMod );
            pFile->setText(0,  names[ii].substr( 0, pos1 ).c_str() );
            pFile->setText(1,  names[ii].substr( pos2+1 ).c_str() );
            if(  findIndex<int>( sel, indx[ii] ) >= 0 )
                pFile->setSelected( true );
        }
    }
    ui->pListFiles->expandToDepth( 0);
    ui->pListFiles->setFocus();

    QObject::connect( ui->pGO_OKButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect( ui->pHelpButton, SIGNAL(clicked()), this, SLOT(CmHelp()));
    QObject::connect( ui->pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

ListFilesDialog::~ListFilesDialog()
{
    delete ui;
}

void ListFilesDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_MPROJ_LF_HTML );
}

/*! returns selection array
    array is empty if nothing is selected
*/
void ListFilesDialog::allSelected( TCStringArray& aFls, TCIntArray& aCnt)
{

    aFls.clear();
    aCnt.clear();

    int ii, jj, nF, cnt = 0;
    int npMod1 = pkern->childCount();
    int npMod2 = pprf->childCount();
    ErrorIf( npMod1 != npMod2, "ListFilesDialog", "Test point");

    QTreeWidgetItem* pMod;
    QTreeWidgetItem* pFile;

    for( ii=0; ii< npMod1; ii++ )
    {
        cnt = 0;

        pMod = pkern->child(ii);
        nF = pMod->childCount();
        for( jj = 0; jj< nF; jj++ )
        {
            pFile = pMod->child( jj );
            if( pFile->isSelected() )
            {
                string col =pFile->text( 0 ).toStdString();
                cnt++;
                aFls.push_back( col.c_str() );
            }
        }

        pMod = pprf->child(ii);
        nF = pMod->childCount();
        for( jj = 0; jj< nF; jj++ )
        {
            pFile = pMod->child( jj );
            if( pFile->isSelected() )
            {
                string col =pFile->text( 0 ).toStdString();
                cnt++;
                aFls.push_back( col.c_str() );
            }
        }
        aCnt.push_back( cnt );

    }

}

// --------------------- End ListFilesDialog.cpp -------------------------

