//-------------------------------------------------------------------
// $$
//
// Implementation of RTparmWizard class
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

const char *RTPARM_HTML = "rtparm_setup";

#include <qspinbox.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qcombobox.h>

#include "RTparmWizard.h"
#include "visor_w.h"

#define Inherited RTparmWizardData
RTparmWizard::RTparmWizard( const char* pkey, char flgs[10], int size[7],
                            QWidget* parent):
        Inherited( parent, 0, true )
{
    int ii;
    gstring str1= "Setup to Create RTparm:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage3, true);

// page1
    for( ii=0; ii < (pWhat->count()); ii++)
    {
      if(pWhat->text(ii)[0] == QChar(flgs[0]) )
      {  pWhat->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunE->count()); ii++)
    {
      if(pPunE->text(ii)[0] == QChar(flgs[1]) )
      {  pPunE->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunV->count()); ii++)
    {
      if(pPunV->text(ii)[0] == QChar(flgs[2]) )
      {  pPunV->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunP->count()); ii++)
    {
      if(pPunP->text(ii)[0] == QChar(flgs[3]) )
      {  pPunP->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunT->count()); ii++)
    {
      if(pPunT->text(ii)[0] == QChar(flgs[4]) )
      {  pPunT->setCurrentItem(ii);
         break;
      }
    }
// Page 2
    pNP->setValue(size[0]);
    pNT->setValue(size[1]);
    pdimY->setValue(size[6]);
    pECol->setValue(size[5]);
    pELine->setValue(size[4]);
    ii = min( size[2],pMode->count()-1 );
    pMode->setCurrentItem(ii);

//Page 3
   spinBox18->setValue(size[3]);

   for( ii=0; ii < (pPabs->count()); ii++)
    {
      if(pPabs->text(ii)[0] == QChar(flgs[6]) )
      {  pPabs->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPtun->count()); ii++)
    {
      if(pPtun->text(ii)[0] == QChar(flgs[7]) )
      {  pPtun->setCurrentItem(ii);
         break;
      }
    }

}


RTparmWizard::~RTparmWizard()
{}

void   RTparmWizard::getSizes( int size[7] )
{
    size[0]= pNP->value();
    size[1]= pNT->value();
    size[6]= pdimY->value();
    size[5]= pECol->value();
    size[4]= pELine->value();
    size[3]= spinBox18->value();

    size[2] = pMode->currentItem();
}

void RTparmWizard::getFlags( char flgs[6] )
{
// Page 1 - not return
// Page 2
  if( pECol->value() > 0 && pELine->value() > 0 )
       flgs[5] = '+';
  else flgs[5] = '-';
//Page 3
  QString str = pPabs->currentText();
  flgs[6] = str[0].latin1();
  str = pPtun->currentText();
  flgs[7] = str[0].latin1();
}

void
RTparmWizard::help()
{
  pVisorImp->OpenHelp( RTPARM_HTML, 0, this/*, true*/ );
}

//--------------------- End of RTparmWizard.cpp ---------------------------
