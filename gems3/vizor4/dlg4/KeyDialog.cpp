//-------------------------------------------------------------------
// $Id: KeyDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#include <QListWidget>
#include <qvariant.h>


#include "v_dbm.h"
#include "v_module.h"
#include "KeyDialog.h"
#include "KeyFilter.h"


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


    bool yesFilter = false;

    ErrorIf(!key, "KeyDialog", "pkey is null");

    if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
    {
        keyFilter = key;
	yesFilter = true;
    }

    gstring s = "Please, select one record key. Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());

    s  = gstring(rt[irt].UnpackKey());
    int n = rt[irt].GetKeyList( keyFilter.c_str(), keyList, temp);
    int sel = 0;

    for( int ii=0; ii<n; ii++ )
    {
        pList->addItem(keyList[ii].c_str());
        if( /*(yesFilter==false) && */keyList[ii]==s )
            sel = ii;
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection); // pList->setMultiSelection(false);
    if( sel < 0 || sel > n )
	sel = 0;
    pList->setCurrentRow(sel); // pList->setSelected(sel, true);
    pButton3->hide();
    pButton2->hide();

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

//--------------------- End of KeyDialog.cpp ---------------------------

