//-------------------------------------------------------------------
// $Id$
//
// Implementation of ProcessDialog class
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

const char *PROCESS_HTML = "process_setup";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>


#include "ProcessDialog.h"
#include "visor_w.h"


#define Inherited ProcessDialogData

ProcessDialog::ProcessDialog (
        const char* pkey, char type, QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str= "Setup to Create Process:  ";
            str += pkey;
    setCaption( str.c_str() );

    switch( type )
    {
      case 'T': pselT->setChecked( true ); break;
      case 'G': pselG->setChecked( true ); break;
      case 'R': pselR->setChecked( true ); break;
      case 'P': pselP->setChecked( true ); break;
      case 'S':
      default: pselS->setChecked( true ); break;
    }

}


ProcessDialog::~ProcessDialog()
{}

void   ProcessDialog::getSizes( int size[6] )
{

    size[0]= pPoints->value();
    size[1]= pMods->value();
    size[2]= pGraph->value();
    size[3]= pELine->value();
    size[4]= pECol->value();
    size[5]= pSd->value();
}

bool   ProcessDialog::getMode()
{
  return pStepwise->isChecked();
}

char   ProcessDialog::getType()
{
  char type='S';

  if( pselT->isChecked())
   type='T';
  else
     if( pselG->isChecked())
        type='G';
     else
         if( pselR->isChecked())
           type='R';
         else
            if( pselP->isChecked())
                type='P';
            else
                if( pselS->isChecked())
                   type='S';
  return type;
}



void
ProcessDialog::CmHelp()
{
  pVisorImp->OpenHelp( PROCESS_HTML, 0, this, true );
}

//--------------------- End of ProcessDialog.cpp ---------------------------
