//-------------------------------------------------------------------
// $Id: KeyFilter.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyFilter class
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
#include <stdio.h>
#include <iostream>
using namespace std;

#include <qtooltip.h>

#include "v_module.h"
#include "v_dbm.h"
#include "service.h"
#include "KeyFilter.h"
#include "KeyDialog.h"

const char* GEMS_RKEY_HTML = "gdb_rkey.html";

//============================================================
// KeyEdit dialog
//============================================================

KeyFilter::KeyFilter(QWidget* win, int irt, const char* key,
                     const char* caption, bool allowTempl ):
        QDialog( win ),
        iRt(irt),
        allowTemplates(allowTempl)
{
	setWindowTitle(caption);

    TDBKey dbKey( rt[irt].GetDBKey() );

    if( key )
        dbKey.SetKey(key);

    QVBoxLayout* editBox = new QVBoxLayout();
    
    for( int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        QLineEdit* pEdit;
        aEdit.Add( pEdit = new QLineEdit(this) );
        QString str = ((TCModule*)&aMod[irt])->GetFldHelp(ii);
        pEdit->setToolTip( str);
        pEdit->setMaxLength( dbKey.FldLen(ii) );
        gstring s(dbKey.FldKey(ii), 0, dbKey.FldLen(ii));
        StripLine(s);
        pEdit->setText( s.c_str() );
        editBox->addWidget( pEdit);
    }
    aEdit[0].setFocus();

    QVBoxLayout* buttonBox = new QVBoxLayout();
    QPushButton* btn;

    btn = new QPushButton(this);
    btn->setText("&Ok");
    btn->setDefault(true);
    connect( btn, SIGNAL(clicked()), this, SLOT(CmOk()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&Reset");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvSetAll()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&From List");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvGetList()) );
    buttonBox->addWidget( btn );

    buttonBox->addStretch();

    btn = new QPushButton(this);
    btn->setText("&Cancel");
// qt3to4    btn->setAccel( Qt::Key_Escape );
    connect( btn, SIGNAL(clicked()), this, SLOT(reject()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);            
    btn->setText("&Help");
    connect( btn, SIGNAL(clicked()), this, SLOT(CmHelp()) );
    buttonBox->addWidget( btn );

    QHBoxLayout* mainBox = new QHBoxLayout(this);
    mainBox->addLayout(editBox);   
    mainBox->addLayout(buttonBox);   
    // setLayout(mainBox);
}

///  pVisor->SetStatus( aMod[iRt]->GetFldHelp(ii).c_str() );
void
KeyFilter::CmHelp()
{                               
   gstring dbName = GEMS_RKEY_HTML;
   dbName += "#dbm_";
   dbName += gstring(aMod[iRt].GetName());
   pVisorImp->OpenHelp(  dbName.c_str(), 0, this/*, true*/ );
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
        gstring s = aEdit[ii].text().toLatin1().data();
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


