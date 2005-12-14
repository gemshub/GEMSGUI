//-------------------------------------------------------------------
// $Id: GEM2MTWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of GEM2MTWizard class
//
// Copyright (C) 2005  S.Dmytriyeva
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

const char *GEM2MT_HTML = "gm_gem2mt";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>


#include "GEM2MTWizard.h"
#include "gstring.h"
#include "visor_w.h"


#define Inherited GEM2MTWizardData

GEM2MTWizard::GEM2MTWizard( const char* pkey, char flgs[20],
                              int size[15], QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str1= "GEM-Selektor GEM2MT Setup:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage2, true);

//Page1
   if( flgs[12] == 'S' )
       pselS->setChecked( true );
   else  if( flgs[12] == 'F' )
            pselG->setChecked( true );
          else  if( flgs[12] == 'A' )
                  pselT->setChecked( true );
                else if( flgs[12] == 'D' )
                       pselR->setChecked( true );
                     else
                       pselP->setChecked( true );
//Page2

    pnC->setValue(size[0]);
    pnIV->setValue(size[1]);
    pntM->setValue(size[2]);
    pnPG->setValue(size[3]);
    pnFD->setValue(size[4]);
    pnSFD->setValue(size[5]);
    pnYS->setValue(size[6]);
    pnE->setValue(size[7]);
    pYE->setValue(size[8]);

//Page 3

    pLb->setValue(size[9]);
    pnEl->setValue(size[10]);
    pqpc->setValue(size[11]);
    pPai->setValue(size[12]);
    pTai->setValue(size[13]);

    if( flgs[4] != '-' )
      c_PvICi->setChecked( true );
    else c_PvICi->setChecked( false );
    if( flgs[15] != '-' )
      c_PsSYd->setChecked( true );
    else c_PsSYd->setChecked( false );
    if( flgs[16] != '-' )
      c_PsSdat->setChecked( true );
    else c_PsSdat->setChecked( false );
    if( flgs[17] != '-' )
      c_PsDDc->setChecked( true );
    else c_PsDDc->setChecked( false );
    if( flgs[18] != '-' )
      c_PsDIc->setChecked( true );
    else c_PsDIc->setChecked( false );
    if( flgs[19] != '-' )
      c_PvTPai->setChecked( true );
    else c_PvTPai->setChecked( false );

//Page4
    spinBox18->setValue(size[14]);

}


GEM2MTWizard::~GEM2MTWizard()
{}

void   GEM2MTWizard::getSizes( int size[15] )
{

//Page2
    size[0]= pnC->value();
    size[1]= pnIV->value();
    size[2]= pntM->value();
    size[3]= pnPG->value();
    size[4]= pnFD->value();
    size[5]= pnSFD->value();
    size[6]= pnYS->value();
    size[7]= pnE->value();
    size[8]= pYE->value();
//Page 3
    size[9]= pLb->value();
    size[10]= pnEl->value();
    size[11]= pqpc->value();
    size[12]= pPai->value();
    size[13]= pTai->value();
    size[14]= spinBox18->value();
}


void   GEM2MTWizard::getFlags( char flgs[20] )
{
  char type='S';

  if( pselS->isChecked())
   type='S';
  else if( pselG->isChecked())
           type='F';
       else if( pselT->isChecked())
                type='A';
          else if( pselR->isChecked())
                  type='D';
                else  if( pselP->isChecked())
                           type='T';

   flgs[12] = type;
   // Putting other flags
   if( c_PvICi->isChecked() )
       flgs[4] = '+';
    else flgs[4] = '-';
   if( c_PsSYd->isChecked() )
       flgs[15] = '+';
    else flgs[15] = '-';
   if( c_PsSdat->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';
   if( c_PsDDc->isChecked() )
       flgs[17] = '+';
    else flgs[17] = '-';
   if( c_PsDIc->isChecked() )
       flgs[18] = '+';
    else flgs[18] = '-';
   if( c_PvTPai->isChecked() )
       flgs[19] = '+';
    else flgs[19] = '-';
//Putting flags, using sizes  is out of function
// 5,7,8,9, 10, 11
  if( pLb->value() > 0  )
       flgs[5] = '+';
  else flgs[5] = '-';
  if( pnYS->value() > 0  )
       flgs[7] = '+';
  else flgs[7] = '-';
  if( pnE->value() > 0  && pYE->value() >0  )
       flgs[8] = '+';
  else flgs[8] = '-';

  if( pnPG->value() > 0  )
       flgs[9] = '+';
  else flgs[9] = '-';
  if( pnFD->value() > 0  )
       flgs[10] = '+';
  else flgs[10] = '-';
  if( pnSFD->value() > 0  )
       flgs[11] = '+';
  else flgs[11] = '-';

// flags 0,1,2,3,6, 13, 14 not used or internal
}

void
GEM2MTWizard::help()
{
  pVisorImp->OpenHelp( GEM2MT_HTML, 0, this/*, true*/ );
}

//--------------------- End of GEM2MTWizard.cpp ---------------------------
