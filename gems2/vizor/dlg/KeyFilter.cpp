//-------------------------------------------------------------------
// $Id$
//
// Implementation of KeyFilter class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
using namespace std;

#include <qtooltip.h>

#include "v_module.h"
#include "v_dbm.h"
#include "service.h"
#include "KeyFilter.h"
#include "KeyDialog.h"

const char* GEMS_RKEY_HTML = "gdb_rkey";

//============================================================
// KeyEdit dialog
//============================================================

KeyFilter::KeyFilter(QWidget* win, int irt, const char* key,
                     const char* caption, bool allowTempl ):
        QDialog(win, 0, true),
        iRt(irt),
        allowTemplates(allowTempl)
{
    //  resize( 380, 230 );

    setCaption(caption);

    TDBKey dbKey( rt[irt].GetDBKey() );

    if( key )
        dbKey.SetKey(key);

    for( int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        QLineEdit* pEdit;
        aEdit.Add( pEdit = new QLineEdit(this) );
        QString str = ((TCModule*)&aMod[irt])->GetFldHelp(ii);
        QToolTip::add( pEdit, str);
        pEdit->setGeometry(10, 10+ii*25, 200, 20);
        pEdit->setMaxLength( dbKey.FldLen(ii) );
        gstring s(dbKey.FldKey(ii), 0, dbKey.FldLen(ii));
        StripLine(s);
        pEdit->setText( s.c_str() );
    }
    aEdit[0].setFocus();

    QPushButton* btn;

    btn = new QPushButton(this);
    btn->setGeometry(230, 10, 80, 30);
    btn->setText("&Ok");
    btn->setDefault(true);
    connect( btn, SIGNAL(clicked()), this, SLOT(CmOk()) );

    btn = new QPushButton(this);
    btn->setGeometry(230, 50, 80, 30);
    btn->setText("&Reset");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvSetAll()) );

    btn = new QPushButton(this);
    btn->setGeometry(230, 90, 80, 30);
    btn->setText("&From List");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvGetList()) );

    btn = new QPushButton(this);
    btn->setGeometry(230, 130, 80, 30);
    btn->setText("&Cancel");
    btn->setAccel( Key_Escape );
    connect( btn, SIGNAL(clicked()), this, SLOT(reject()) );

    btn = new QPushButton(this);            // Added 25.01.01 KD _
    btn->setGeometry(230, 170, 80, 30);
    btn->setText("&Help");
    connect( btn, SIGNAL(clicked()), this, SLOT(CmHelp()) );
}

/*
void
KeyFilter::done(int r)
{
  if( !r )
    Key = "";
  else
    SetKeyString();

  QDialog::done(r);
}
*/
///  pVisor->SetStatus( aMod[iRt]->GetFldHelp(ii).c_str() );

void
KeyFilter::CmHelp()
{                                //  25.01.01   KD   _
   gstring dbName = "dbm_";
//   dbName += rt[iRt].GetKeywd();
   dbName += aMod[iRt].GetName();
   pVisorImp->OpenHelp( GEMS_RKEY_HTML, dbName.c_str(), this/*, true*/ );
}

void
KeyFilter::CmOk()
{
    if( allowTemplates || SetKeyString().find_first_of("*?") == gstring::npos )
    {
        accept();
        return;
    }

    vfMessage(this, "Key error", "No templates allowed!", vfErr);
}

gstring
KeyFilter::SetKeyString()
{
    TDBKey dbKey( rt[iRt].GetDBKey() );
    gstring Key;

    Key = "";
    for( uint ii=0, jj=0; ii<aEdit.GetCount(); ii++, jj=Key.length())
    {
        const char* s = aEdit[ii].text();
        Key += s;
        StripLine(Key);
//Sveta 04/09/01 ????  if( Key.length()-jj < dbKey.FldLen(ii) )
            Key += ":";
    }

    return Key;
}


void
KeyFilter::EvSetAll()
{
    for( uint ii=0; ii<aEdit.GetCount(); ii++ )
        aEdit[ii].setText("*");
}

void
KeyFilter::EvGetList()
{
    KeyDialog dlg(this, iRt, SetKeyString().c_str(), "Key template", false);
    if( !dlg.exec() )
        return;

    TDBKey dbKey( rt[iRt].GetDBKey() );

    dbKey.SetKey(dlg.getKey().c_str());

    for( int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        gstring s(dbKey.FldKey(ii), 0, dbKey.FldLen(ii));
        StripLine(s);
        aEdit[ii].setText( s.c_str() );
    }

}

gstring
KeyFilter::getFilter()
{
    //  if( result )
    return SetKeyString();
}
//--------------------- End of KeyFilter.cpp ---------------------------


