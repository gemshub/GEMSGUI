//-------------------------------------------------------------------
// Id: gems/vizor/dlg/KeyDialog.cpp  version 2.0.0    2001
//
// Implementation of KeyDialog class
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

#include "v_dbm.h"
#include "v_module.h"
#include "KeyDialog.h"
#include "KeyFilter.h"

#define Inherited KeyDialogData

KeyDialog::KeyDialog(QWidget* win, int irt, const char* key,
                     const char* caption, bool filter):
        Inherited( win, 0, true),
        multi(false), iRt(irt)
{
    old_sel.Clear();
    pList->setFont( pVisorImp->getCellFont() );
    setCaption( caption );

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

    gstring s = "Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());

    s  = gstring(rt[irt].UnpackKey());
    int n = rt[irt].GetKeyList( keyFilter.c_str(), keyList, temp);
    int sel = 0;

    for( int ii=0; ii<n; ii++ )
    {
        pList->insertItem(keyList[ii].c_str());
        if( /*(yesFilter==false) && */keyList[ii]==s )
            sel = ii;
    }

    pList->setMultiSelection(false);
    pList->setSelected(sel, true);
    pButton3->hide();
    pButton2->hide();

    pList->setFocus();

    if( !filter )
        pFilterButton->hide();
}

KeyDialog::KeyDialog(QWidget* win, int irt, TCStringArray& sel,
              const char* key, const char* caption):
        Inherited( win, 0, true),
        multi(true), iRt(irt)
{
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
    pList->setMultiSelection(true);
    setCaption( caption );

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

void
KeyDialog::SetList()
{
    TCIntArray temp;
    TCStringArray keyList;

    gstring s = "Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());

    int n = rt[iRt].GetKeyList( keyFilter.c_str(), keyList, temp);

    for( int ii=0; ii<n; ii++ )
    {
        pList->insertItem(keyList[ii].c_str());
        if( multi )
        {  for(uint jj=0; jj<old_sel.GetCount(); jj++)
           {
            // comparing parts before '*' for overwrite dcomp, reacdc ....
            size_t pos = old_sel[jj].find('*');
            gstring str(old_sel[jj], 0, pos);
            if( keyList[ii].find(str) != gstring::npos )
            {
              pList->setSelected(ii, true);
              break;
            }
           }
       }
    }
}



// olvase unpaked keys?
gstring
KeyDialog::getKey()
{
    int sel = pList->currentItem();
    if( sel != -1 )
    {
        ((TCModule*)&aMod[iRt])->setFilter(keyFilter.c_str());
        gstring res;
        const char* s = pList->text(sel);
        int ln;
        for( int ii=0, jj=0; ii<rt[iRt].KeyNumFlds(); ii++)
        {
          //pos = strchr( s+jj, ":" );
          ln = rt[iRt].FldLen(ii);
          // if( pos) ln = min( ln, pos-s+jj );

          res += gstring(s+jj, 0, ln);
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
    KeyFilter dbFilter(this, iRt, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        pList->clear();
        SetList();
    }
}

void
KeyDialog::CmSelectAll()
{
    // select all gstrings
    for( uint ii=0; ii<pList->count(); ii++ )
        pList->setSelected(ii, true);
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

    for( uint ii=0; ii<pList->count(); ii++ )
        if( pList->isSelected(ii) )
        {
         const char* s = pList->text(ii);
         arr.Add(s);
        }
    return arr;
}

//--------------------- End of KeyDialog.cpp ---------------------------

