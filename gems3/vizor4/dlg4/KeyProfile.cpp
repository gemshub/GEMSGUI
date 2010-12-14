//-------------------------------------------------------------------
// $Id: KeyProfile.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyProfile class
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
const char *GEMS_GP_HTML = "gp_howto";

#include <QListWidget>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qvariant.h>

#include "v_dbm.h"
#include "KeyProfile.h"
#include "visor_w.h"
#include "visor.h"


KeyProfile::KeyProfile( QWidget* win, int irt, const char* caption):
        QDialog( win),
        iRt(irt), newKey(false)
{
	setupUi(this);

	pList->setFont( pVisorImp->getCellFont() );

    TCIntArray    temp;
    TCStringArray keyList;

    setWindowTitle( caption );
    if( pVisor->getElemPrMode() )
    {   rbNewPrMode->setChecked( true );
        rbOldPrMode->setChecked( false );
    }
    else
    {   rbNewPrMode->setChecked( false );
        rbOldPrMode->setChecked( true );
    }

//    gstring s = "Select Project record";
//    pLabel->setText(s.c_str());

    int n = rt[irt].GetKeyList( "*", keyList, temp);
    for( int ii=0; ii<n; ii++ )
        pList->addItem(keyList[ii].c_str());
     pList->setCurrentRow(0);
     pList->item(0)->setSelected(true);

     // signals and slots connections
     QObject::connect( pRe_runIA, SIGNAL( clicked() ), this, SLOT( CmReturnIA() ) );
     QObject::connect( pRe_runSmart, SIGNAL( clicked() ), this, SLOT( CmReturnSmart() ) );
  	 QObject::connect(pList, SIGNAL(itemDoubleClicked ( QListWidgetItem *) ), this, SLOT(accept()));
//     QObject::connect(pList, SIGNAL(returnPressed(QListWidgetItem *)), this, SLOT(accept()));
}

KeyProfile::~KeyProfile()
{}

void KeyProfile::languageChange()
{
    retranslateUi(this);
}

gstring KeyProfile::getKey()
{
   pVisor->setElemPrMode(rbNewPrMode->isChecked());
   if( newKey == true )
        return ALLKEY;
    int sel = pList->currentRow();
    if( sel != -1 )
        return gstring(pList->item(sel)->text().toLatin1().data());

    return gstring();
}

void KeyProfile::CmReturnIA()
{
	   if( pRe_runIA->isChecked() )
	         pRe_runSmart->setChecked(false);
}

void KeyProfile::CmReturnSmart()
{
	   if( pRe_runSmart->isChecked() )
	         pRe_runIA->setChecked(false);
}

void KeyProfile::CmNew()
{
    newKey = true;
    accept();
}

void KeyProfile::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_GP_HTML, 0 );
}

bool
KeyProfile::getAqGasState()
{
    if( newKey == true )
        return true;    // new record => selection aqueous and gas phases

    if( pAqGas->isChecked() )
        return false;
    else return true; // new selection aqueous and gas phases

}

int KeyProfile::getMakeDump()
{
    if( newKey == true )
        return 0;    // new record => no dump

    if( pRe_runIA->isChecked() )
        return 1;  // Re-run and save all sysEq in auto IA mode

    if( pRe_runSmart->isChecked() )
        return 2;  // Re-run and save all sysEq in smart
    
    return 0; 
}

bool KeyProfile::getFilesState()
{
    if( newKey == true )
        return false;    // new record => add all files

    if( pFiles->isChecked() )
        return true;
    else return false; // add files from project directory

}

bool KeyProfile::getRemakeState()
{
    if( CheckBox4->isChecked() )
        return true;   // remake flags
    else return false;

}

gstring KeyProfile::getTemplateKey()
{
    if( newKey == true && pTemplate->isChecked() )
    {
      int sel = pList->currentRow();
      if( sel != -1 )
        return gstring(pList->item(sel)->text().toLatin1().data());
      Error( "New Modelling Project", "No template record selected");
    }
    return gstring();
}

// --------------------- End KeyProfile.cpp -------------------------

