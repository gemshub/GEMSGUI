//-----------------------------------------------------------
// $Id: KeyDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#include <QListWidget>
#include <qvariant.h>

#include "v_mod.h"
#include "service.h"
#include "KeyDialog.h"
#include "KeyFilter.h"
#include "GemsMainWindow.h"
#include "v_module.h"

KeyDialog::KeyDialog(QWidget* win, int irt, const char* key,
                     const char* caption, bool filter):
        QDialog( win),
        multi(false), iRt(irt)
{
 
	setupUi(this);
	 
	old_sel.Clear();
    pList->setFont( pVisorImp->getCellFont() );
    QObject::connect(pList, SIGNAL(itemDoubleClicked ( QListWidgetItem *) ), this, SLOT(accept()));
    setWindowTitle( caption );

    TCIntArray temp;
    TCStringArray keyList;

    ErrorIf(!key, "KeyDialog", "pkey is null");

    if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
        keyFilter = key;

    gstring s = "Please, select one record key. Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());

    s  = gstring(rt[irt].UnpackKey());
    int n = rt[irt].GetKeyList( keyFilter.c_str(), keyList, temp);
    int sel = 0;

    for( int ii=0; ii<n; ii++ )
    {
        pList->addItem(keyList[ii].c_str());
        if(  keyList[ii]==s )
            sel = ii;
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection); // pList->setMultiSelection(false);
    if( sel < 0 || sel > n )
	sel = 0;
    pList->setCurrentRow(sel); // pList->setSelected(sel, true);
    pButton3->hide();
    pButton2->hide();
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );


    pList->setFocus();

    if( !filter )
        pFilterButton->hide();
}

KeyDialog::KeyDialog(QWidget* win, int irt, TCStringArray& sel,
              const char* key, const char* caption):
        QDialog( win),
        multi(true), iRt(irt)
{
	setupUi(this);

    old_sel.Clear();
    for(uint ii=0; ii<sel.GetCount(); ii++)
    {   if( strchr( sel[ii].c_str(), ':' ))  // key in packed form
        {
          rt[irt].SetKey( sel[ii].c_str() );
          old_sel.Add(rt[irt].UnpackKey());
        }
        else
          old_sel.Add( sel[ii] );
    }
    
    pList->setFont( pVisorImp->getCellFont() );
    pList->setSelectionMode(QAbstractItemView::MultiSelection);  // pList->setMultiSelection(true);
    setWindowTitle( caption );
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ErrorIf(!key, "KeyDialog", "pkey is null");
    if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
       keyFilter = key;

    SetList();
    pList->setFocus();

}


KeyDialog::~KeyDialog()
{}

void KeyDialog::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_SELECT_HTML );
}

void KeyDialog::languageChange()
{
    retranslateUi(this);
}

void
KeyDialog::SetList()
{
    TCIntArray temp;
    TCStringArray keyList;

    gstring s = "Please, mark one or more record keys. Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());

    int n = rt[iRt].GetKeyList( keyFilter.c_str(), keyList, temp);

    for( int ii=0; ii<n; ii++ )
       pList->addItem(keyList[ii].c_str());

    if( multi )
    {  for(uint jj=0; jj<old_sel.GetCount(); jj++)
          for( int ii=0; ii<n; ii++ )
          {
            // comparing parts before '*' for overwrite dcomp, reacdc ....
            size_t pos = old_sel[jj].find('*');
            gstring str(old_sel[jj], 0, pos);
            if( keyList[ii].find(str) != gstring::npos )
            {
            	pList->item(ii)->setSelected( true); //pList->setSelected(ii, true);
              break;
            }
           }
      }
 }



// olvase unpaked keys?
gstring
KeyDialog::getKey()
{
    int sel = pList->currentRow(); //pList->currentItem();
    if( sel != -1 )
    {
        ((TCModule*)&aMod[iRt])->setFilter(keyFilter.c_str());
        gstring res;
        gstring s = pList->item(sel)->text().toLatin1().data();
        //gstring s = ss;
        int ln;
        for( int ii=0, jj=0; ii<rt[iRt].KeyNumFlds(); ii++)
        {
          //pos = strchr( s+jj, ":" );
          ln = rt[iRt].FldLen(ii);
          // if( pos) ln = min( ln, pos-s+jj );

          //res += gstring(s+jj, 0, ln);
          res += gstring(s, jj, ln);
          StripLine(res);
          res += ":";
          jj += ln;
        }
        return res;
    }
    return gstring();
}


void
KeyDialog::CmFilter()
{
    gstring str_name = "Template for ";
            str_name +=  rt[iRt].GetKeywd();
            str_name += " record key";
    KeyFilter dbFilter(this, iRt, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        pList->clear();
        SetList();
        pList->setCurrentRow(0);
     }
}

void
KeyDialog::CmSelectAll()
{
    // select all gstrings
    for( int ii=0; ii<pList->count(); ii++ )
        pList->item(ii)->setSelected( true);
}


void
KeyDialog::CmClearAll()
{
    pList->clearSelection();
}

/*! returns selection array
    array is empty if nothing is selected
*/

TCStringArray
KeyDialog::allSelectedKeys()
{
    TCStringArray arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<pList->count(); ii++ )
        if( pList->item(ii)->isSelected() )
        {
         gstring s = pList->item(ii)->text().toLatin1().data();;
         arr.Add(s.c_str());
        }
    return arr;
}


//------------------------------------------------------
// RDKeyDialog - work with ReacDC&DComp keys lists
RDKeyDialog::RDKeyDialog(QWidget* win, TCStringArray& sel,
              const char* key, const char* caption, short aNsuT):
        QDialog( win), NsuT(aNsuT)
{
    setupUi(this);
    old_sel.clear();

    for(uint ii=0; ii<sel.GetCount(); ii++)
    {   if( strchr( sel[ii].c_str(), ':' ))  // key in packed form
        {
          if( sel[ii][0] == SRC_REACDC )
          {  rt[RT_REACDC].SetKey( sel[ii].c_str()+2 );
             old_sel.append( makeKey( SRC_REACDC, rt[RT_REACDC].UnpackKey()));
          }
          else
          {  rt[RT_DCOMP].SetKey( sel[ii].c_str()+2 );
             old_sel.append( makeKey( SRC_DCOMP, rt[RT_DCOMP].UnpackKey()));
          }
        }
        else
          old_sel.append( sel[ii].c_str() );
    }

    pList->setFont( pVisorImp->getCellFont() );
    pList->setSelectionMode(QAbstractItemView::MultiSelection);
    setWindowTitle( caption );
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ErrorIf(!key, "KeyDialog", "pkey is null");
    if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
       keyFilter = key;

    SetList();
    pList->setFocus();
}


RDKeyDialog::~RDKeyDialog()
{}

void RDKeyDialog::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_SELECT_HTML );
}

void RDKeyDialog::languageChange()
{
    retranslateUi(this);
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
    int ii;

    gstring s = "Please, mark one or more record keys. Filter: ";
    s +=  keyFilter;

    // ReacDC list
    if( NsuT > 0 )  // template for adsorption
        keyFilter[0] = CP_SSPC;
    int n = rt[RT_REACDC].GetKeyList( keyFilter.c_str(), keyList, temp);

    for( int ii=0; ii<n; ii++ )
       pList->addItem( makeKey( SRC_REACDC, keyList[ii].c_str()));

    // Dcomp list
    if( NsuT > 0 )  // template for adsorption
    {    keyFilter[0] = CP_SOLID;
         s+= "\\";
         s+= keyFilter;
    }

    n = rt[RT_DCOMP].GetKeyList( keyFilter.c_str(), keyList, temp);

    for( int ii=0; ii<n; ii++ )
       pList->addItem( makeKey( SRC_DCOMP, keyList[ii].c_str()));

   for( jj=0; jj<old_sel.count(); jj++)
    {
       QString str = old_sel[jj];
       int pos = str.indexOf('*', 0);
       if( pos > 0 )
        str.truncate ( pos );
       for( ii=0; ii<pList->count(); ii++ )
        {
            // comparing parts before '*' for overwrite dcomp, reacdc ....
            if( pList->item(ii)->text().contains(str/*old_sel[jj]*/) )
            {
              pList->item(ii)->setSelected( true); //pList->setSelected(ii, true);
              break;
            }
        }
   }
   pLabel->setText(s.c_str());
 }


void RDKeyDialog::CmFilter()
{
    gstring str_name = "Template for ";
            str_name +=  rt[RT_REACDC].GetKeywd();
            str_name +=  "&";
            str_name +=  rt[RT_DCOMP].GetKeywd();
            str_name += " record key";
    KeyFilter dbFilter(this, RT_DCOMP, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        pList->clear();
        SetList();
        pList->setCurrentRow(0);
     }
}

void RDKeyDialog::CmSelectAll()
{
    // select all gstrings
    for( int ii=0; ii<pList->count(); ii++ )
        pList->item(ii)->setSelected( true);
}


void RDKeyDialog::CmClearAll()
{
    pList->clearSelection();
}

/*! returns selection array
    array is empty if nothing is selected
*/
TCStringArray RDKeyDialog::allSelectedKeys()
{
    TCStringArray arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<pList->count(); ii++ )
        if( pList->item(ii)->isSelected() )
        {
         gstring s = pList->item(ii)->text().toLatin1().data();;
         arr.Add(s.c_str());
        }
    return arr;
}

//--------------------- End of KeyDialog.cpp ---------------------------

