//-------------------------------------------------------------------
// $Id$
//
// Implementation of DCompWizard class
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

const char *DCOMP_HTML = "gm_dcomp";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>

#include "DCompWizard.h"
#include "visor_w.h"

#define Inherited DCompWizardData

DCompWizard::DCompWizard( const char* pkey, char flgs[15], int size[4],
                            QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str1= "GEM-Selektor DComp Setup:  ";
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
    for( ii=0; ii < (pPdcC->count()); ii++)
    {
      if(pPdcC->text(ii)[0] == QChar(flgs[6]) )
      {  pPdcC->setCurrentItem(ii);
         break;
      }
    }
//Page 2
    pNeCp->setValue(size[0]);
    pNft->setValue(size[1]);
    pNemp->setValue(size[3]);
    pPdcVT->setChecked( flgs[14] == '+' );

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
    spinBoxSDrefLnk->setValue(size[2]);
}


DCompWizard::~DCompWizard()
{}

void   DCompWizard::getSizes( int size[4] )
{
    size[0]= pNeCp->value();
    size[1]= pNft->value();
    size[2]= spinBoxSDrefLnk->value();
    size[3]= pNemp->value();

}

void DCompWizard::getFlags( char flgs[15] )
{
  QString str;

     str = p_pct_1->currentText();
  flgs[0] = str[0].latin1();
     str = p_pct_2->currentText();
  flgs[1] = str[0].latin1();
     str = p_pct_3->currentText();
  flgs[2] = str[0].latin1();
// 3, 4, 5, reserved
     str = pPdcC->currentText();
  flgs[6] = str[0].latin1();
// 7,8,9,10 not changed
  if( pNeCp->value() > 0  )
    flgs[11] = '+';
  else
    flgs[11] = '-';

  if( pNft->value() > 0  )
    flgs[12] = '+';
  else
    flgs[12] = '-';

  if( flgs[0] == 'H'  )
    flgs[13] = '+';
  else
    flgs[13] = '-';
// check
  if( pPdcVT->isChecked() )
    flgs[14] = '+';
  else
    flgs[14] = '-';


}


void
DCompWizard::help()
{
  pVisorImp->OpenHelp( DCOMP_HTML, 0, this/*, true*/ );
}

//--------------------- End of DCompWizard.cpp ---------------------------
