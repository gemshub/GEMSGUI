//-------------------------------------------------------------------
// $Id$
//
// Implementation of ReacDCWizard class
//
// Copyright (C) 2005  S.Dmytriyeva, D.Kulik
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

const char *REACDC_HTML = "gm_reacdc";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>

#include "ReacDCWizard.h"
#include "visor_w.h"

#define Inherited ReacDCWizardData

ReacDCWizard::ReacDCWizard( const char* pkey, char flgs[12], int size[4],
                            QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str1= "GEM-Selektor ReacDC Setup:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage3, true);

//Page 1
    int ii;
    for( ii=0; ii < (p_pct_1->count()); ii++)
    {
      if(p_pct_1->text(ii)[0] == QChar(flgs[0]) )
      {  p_pct_1->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_pct_2->count()); ii++)
    {
      if(p_pct_2->text(ii)[0] == QChar(flgs[1]) )
      {  p_pct_2->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_pct_3->count()); ii++)
    {
      if(p_pct_3->text(ii)[0] == QChar(flgs[2]) )
      {  p_pct_3->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPreC->count()); ii++)
    {
      if(pPreC->text(ii)[0] == QChar(flgs[6]) )
      {  pPreC->setCurrentItem(ii);
         break;
      }
    }
//Page 2
    pnDC->setValue(size[0]);
    pnTp->setValue(size[1]);
    pnPp->setValue(size[2]);

    for( ii=0; ii < (pPunE->count()); ii++)
    {
      if(pPunE->text(ii)[0] == QChar(flgs[7]) )
      {  pPunE->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunV->count()); ii++)
    {
      if(pPunV->text(ii)[0] == QChar(flgs[8]) )
      {  pPunV->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunP->count()); ii++)
    {
      if(pPunP->text(ii)[0] == QChar(flgs[9]) )
      {  pPunP->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunT->count()); ii++)
    {
      if(pPunT->text(ii)[0] == QChar(flgs[10]) )
      {  pPunT->setCurrentItem(ii);
         break;
      }
    }

//Page3
    spinBoxSDrefLnk->setValue(size[3]);
}


ReacDCWizard::~ReacDCWizard()
{}

void   ReacDCWizard::getSizes( int size[4] )
{
    size[0]= pnDC->value();
    size[1]= pnTp->value();
    size[2]= pnPp->value();
    size[3]= spinBoxSDrefLnk->value();

}

void ReacDCWizard::getFlags( char flgs[12] )
{
  QString str;

     str = p_pct_1->currentText();
  flgs[0] = str[0].latin1();
     str = p_pct_2->currentText();
  flgs[1] = str[0].latin1();
     str = p_pct_3->currentText();
  flgs[2] = str[0].latin1();
// 3, 4, 5, reserved
     str = pPreC->currentText();
  flgs[6] = str[0].latin1();
// 7,8,9,10,11 not changed  
}


void
ReacDCWizard::help()
{
  pVisorImp->OpenHelp( REACDC_HTML, 0, this/*, true*/ );
}

//--------------------- End of ReacDCWizard.cpp ---------------------------
