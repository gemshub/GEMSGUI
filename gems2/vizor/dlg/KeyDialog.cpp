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
        iRt(irt)
{
    pList->setFont( pVisorImp->getCellFont() );

    TCIntArray temp;
    TCStringArray keyList;

    setCaption( caption );

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
    pList->setSelected(sel, true);

    if( !filter )
        pFilterButton->hide();
}


KeyDialog::~KeyDialog()
{}



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
    KeyFilter dbFilter(this, iRt, keyFilter.c_str(), "Template");
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();

        TCIntArray temp;
        TCStringArray keyList;

        int n = rt[iRt].GetKeyList( keyFilter.c_str(), keyList, temp);

        pList->clear();
        for( int ii=0; ii<n; ii++ )
            pList->insertItem(keyList[ii].c_str());
    }

    gstring s = "Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());
}

//--------------------- End of KeyDialog.cpp ---------------------------

