//-------------------------------------------------------------------
// Id: gems/vizor/dlg/ListFilesDialog.cpp  version 2.0.0    2001
//
// Implementation of ListFilesDialog class
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
const char *GEMS_LF_HTML = "lf_howto";

#define Inherited ListFilesDialogData

#include <qlistview.h>
#include <qlabel.h>

#include "ListFilesDialog.h"
#include "visor.h"
#include "visor_w.h"
#include "v_module.h"


ListFilesDialog::ListFilesDialog(QWidget* win, const char * prfName,
           const char* /*caption*/):
        Inherited( win, 0, true /* false = modeless */ )
 {
    gstring label = "Please, check files linked to Profile ";
             label += prfName;
    pLabel->setText( tr(label.c_str()) );

    pListFiles->setFont( pVisorImp->getCellFont() );
    pListFiles->setMultiSelection(true);
    pListFiles->setSorting(-1);
    pListFiles->setRootIsDecorated(true);
    pListFiles->setAllColumnsShowFocus(true);
//    setCaption(caption);

    QListViewItem* pdb = new QListViewItem( pListFiles, "Database" );
    pprf = new QListViewItem( pdb, prfName );
    pkern = new QListViewItem( pdb, "Kernel" );

//    gstring Path = pVisor->userProfDir();
//    Path += prfName;  // created before

    QListViewItem* pMod;
    QListViewItem* pFile;
    size_t pos1, pos2;

    for(int i=aMod.GetCount()-1; i>=0; i--)
    {
        if( aMod[i].IsSubModule() )
            continue;
        pMod = new QListViewItem( pkern, rt[i].GetKeywd() );
        QListViewItem* pFile;
        TCStringArray names;
        TCIntArray indx;
        TCIntArray sel;
        rt[i].GetFileList(closef|openf|oldself, names, indx, sel);
        for(int ii=names.GetCount()-1; ii>=0; ii-- )
        {
          // select only DB.default files
          if( names[ii].find( pVisor->sysDBDir())== gstring::npos )
              continue;
          // get 2 colums
          pos1 = names[ii].find_first_of(" ");
          pos2 = names[ii].rfind("/");
          pFile = new QListViewItem( pMod, names[ii].substr( 0, pos1 ).c_str(),
                 names[ii].substr( pos2+1 ).c_str() );
           if( sel.Find(indx[ii]) >= 0 )
           /*for(uint jj=0; jj<sel.GetCount(); jj++)
             if( sel[jj] == indx[ii]) */
               pFile->setSelected( true );
        }
    }
    for(int i=aMod.GetCount()-1; i>=0; i--)
    {
        if( aMod[i].IsSubModule() )
            continue;
        pMod = new QListViewItem( pprf, rt[i].GetKeywd() );
        QListViewItem* pFile;
        TCStringArray names;
        TCIntArray indx;
        TCIntArray sel;
        rt[i].GetFileList(closef|openf|oldself, names, indx, sel);
        for(int ii=names.GetCount()-1; ii>=0; ii-- )
        {
          // select only Profile files
          if( names[ii].find( prfName/*Path*/ ) == gstring::npos )
              continue;
          // get 2 colums
          pos1 = names[ii].find_first_of(" ");
          pos2 = names[ii].rfind("/");
          pFile = new QListViewItem( pMod, names[ii].substr( 0, pos1 ).c_str(),
                 names[ii].substr( pos2+1 ).c_str() );
           if( sel.Find(indx[ii]) >= 0 )
           /*for(uint jj=0; jj<sel.GetCount(); jj++)
             if( sel[jj] == indx[ii]) */
               pFile->setSelected( true );
        }
    }
    pListFiles->setFocus();


}

ListFilesDialog::~ListFilesDialog()
{}


void
ListFilesDialog::CmHelp()
{
//  pVisorImp->OpenHelp( GEMS_GP_HTML, 0, this );
    pVisorImp->OpenHelp( GEMS_LF_HTML );  // 14.03.01
}

/*! returns selection array
    array is empty if nothing is selected
*/

void
ListFilesDialog::allSelected( TCStringArray& aFls,TCIntArray& aCnt)
{

    aFls.Clear();
    aCnt.Clear();

//    if( !result() )
//        return;

    uint cnt = 0;
    QListViewItem* pMod1 = pkern->firstChild();
    QListViewItem* pMod2 = pprf->firstChild();

    QListViewItem* pFile1;
    QListViewItem* pFile2;

    while( pMod1 && pMod2 )
    {
      cnt = 0;
      pFile1 = pMod1->firstChild();
      pFile2 = pMod2->firstChild();
      while( pFile1 )
      {
       if( pFile1->isSelected() )
       {
         const char *col =pFile1->text( 0 );
         cnt++;
         aFls.Add( col );
       }
       pFile1 = pFile1->nextSibling();
      }
      while( pFile2 )
      {
       if( pFile2->isSelected() )
       {
         const char *col =pFile2->text( 0 );
         cnt++;
         aFls.Add( col );
       }
       pFile2 = pFile2->nextSibling();
      }
      aCnt.Add( cnt );
      pMod1 = pMod1->nextSibling();
      pMod2 = pMod2->nextSibling();
    }



}


// --------------------- End ListFilesDialog.cpp -------------------------

