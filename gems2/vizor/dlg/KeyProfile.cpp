//-------------------------------------------------------------------
// $Id$
//
// Implementation of KeyProfile class
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
const char *GEMS_GP_HTML = "gp_howto";

#define Inherited KeyProfileData

#include <qlistbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

#include "v_dbm.h"
#include "KeyProfile.h"
#include "visor_w.h"
#include "visor.h"


KeyProfile::KeyProfile( QWidget* win, int irt, const char* caption):
        Inherited( win, 0, true /* false = modeless */ ),
        iRt(irt), newKey(false)
{
    pList->setFont( pVisorImp->getCellFont() );

    TCIntArray    temp;
    TCStringArray keyList;

    setCaption( caption );
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
        pList->insertItem(keyList[ii].c_str());
    pList->setSelected(0, true);
    pList->setCurrentItem(0); // for Qt-2.0 bug
}

KeyProfile::~KeyProfile()
{}

gstring
KeyProfile::getKey()
{

   pVisor->setElemPrMode(rbNewPrMode->isChecked());
   if( newKey == true )
        return ALLKEY;
    int sel = pList->currentItem();
    if( sel != -1 )
        return gstring(pList->text(sel));

    return gstring();
}

void
KeyProfile::CmNew()
{
    newKey = true;
    accept();
}

void
KeyProfile::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_GP_HTML, 0, this, true );
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

bool
KeyProfile::getFilesState()
{

    if( newKey == true )
        return false;    // new record => add all files

    if( pFiles->isChecked() )
        return true;
    else return false; // add files from project directory

}

bool
KeyProfile::getRemakeState()
{

    if( newKey == false )
        return false;    // not new record => only read

    if( CheckBox4->isChecked() )
        return true;   // remake flags
    else return false;

}

gstring
KeyProfile::getTemplateKey()
{
    if( newKey == true && pTemplate->isChecked() )
    {
      int sel = pList->currentItem();
      if( sel != -1 )
        return gstring(pList->text(sel));
      Error( "New Modelling Project", "No template record selected");
    }
    return gstring();
}

// --------------------- End KeyProfile.cpp -------------------------

