//-------------------------------------------------------------------
// $Id: ListFilesDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ListFilesDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
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

#include <algorithm>
#include <QTreeWidget>
#include <qlabel.h>
#include <qvariant.h>

#include "GemsMainWindow.h"
#include "ListFilesDialog.h"
#include "visor.h"
#include "service.h"

ListFilesDialog::ListFilesDialog(QWidget* parent, const char* prfName, const char* /*caption*/ )
    : QDialog(parent )
{
    unsigned int i, ii;
    setupUi(this);

    //setWindowTitle(prfName);
    string label = "Please, check database files to be linked to Modeling Project ";
             label += prfName;
    pLabel->setText( tr(label.c_str()) );

    pListFiles->setFont( pVisorImp->getCellFont() );
    pListFiles->setSelectionMode(QAbstractItemView::MultiSelection);    //setMultiSelection(true);
    pListFiles->setSortingEnabled( false );                            //setSorting(-1);
    pListFiles->setRootIsDecorated(true);
    pListFiles->setAllColumnsShowFocus(true);
    // setCaption(caption);
    
    QTreeWidgetItem* pdb = new QTreeWidgetItem( pListFiles );
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
        pMod->setText(0, rt[i].GetKeywd());
        vector<string> names;
        vector<int> indx;
        vector<int> sel;
        rt[i].GetFileList(closef|openf|oldself, names, indx, sel);
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
          if( std::find( sel.begin(), sel.end(), indx[ii] ) != sel.end() )
               pFile->setSelected( true );
        }
    }

    for( i=0; i<aMod.size(); i++)
    {
        if( aMod[i]->IsSubModule() )
            continue;
        pMod = new QTreeWidgetItem( pprf);
        pMod->setText(0, rt[i].GetKeywd());

        vector<string> names;
        vector<int> indx;
        vector<int> sel;
        rt[i].GetFileList(closef|openf|oldself, names, indx, sel);
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
          if( std::find( sel.begin(), sel.end(), indx[ii] ) != sel.end() )
               pFile->setSelected( true );
        }
    }
    pListFiles->expandToDepth( 0);
    pListFiles->setFocus();

}

ListFilesDialog::~ListFilesDialog()
{}

void ListFilesDialog::languageChange()
{
    retranslateUi(this);
}

void ListFilesDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_MPROJ_LF_HTML );
}

/*! returns selection array
    array is empty if nothing is selected
*/
void ListFilesDialog::allSelected( vector<string>& aFls, vector<int>& aCnt)
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
         string col =pFile->text( 0 ).toUtf8().data();
         cnt++;
         aFls.push_back( col );
       }
      }

      pMod = pprf->child(ii);
      nF = pMod->childCount();
      for( jj = 0; jj< nF; jj++ )
      {
        pFile = pMod->child( jj );
       if( pFile->isSelected() )
       {
         string col =pFile->text( 0 ).toUtf8().data();
         cnt++;
         aFls.push_back(  col );
       }
      }
      aCnt.push_back( cnt );

    }

}


// --------------------- End ListFilesDialog.cpp -------------------------

