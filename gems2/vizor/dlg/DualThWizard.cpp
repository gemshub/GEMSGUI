//-------------------------------------------------------------------
// $Id: RTparmWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of DualThWizard class
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

const char *DUALTH_HTML = "gm_dualth";

#include <qspinbox.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include "DualThWizard.h"
#include "visor_w.h"

#define Inherited DualThWizardData
DualThWizard::DualThWizard( const char* pkey, char flgs[20], int size[8],
                            QWidget* parent):
        Inherited( parent, 0, true )
{
    int ii;
    gstring str1= "GEM-Selektor DualTh Setup:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage2, true);

// page1

    for( ii=0; ii < (p_Mode->count()); ii++)
    {
      if(p_Mode->text(ii)[0] == QChar(flgs[12]) )
      {  p_Mode->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_St->count()); ii++)
    {
      if(p_St->text(ii)[0] == QChar(flgs[13]) )
      {  p_St->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_IPf->count()); ii++)
    {
      if(p_IPf->text(ii)[0] == QChar(flgs[17]) )
      {  p_IPf->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_IPu->count()); ii++)
    {
      if(p_IPu->text(ii)[0] == QChar(flgs[18]) )
      {  p_IPu->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_LSF->count()); ii++)
    {
      if(p_LSF->text(ii)[0] == QChar(flgs[19]) )
      {  p_LSF->setCurrentItem(ii);
         break;
      }
    }

// page2

    pnQ->setValue(size[0]);
    pnM->setValue(size[2]);
    pnP->setValue(size[7]);
    pLb->setValue(size[1]);

    for( ii=0; ii < (pPunE->count()); ii++)
    {
      if(pPunE->text(ii)[0] == QChar(flgs[0]) )
      {  pPunE->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunV->count()); ii++)
    {
      if(pPunV->text(ii)[0] == QChar(flgs[1]) )
      {  pPunV->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunP->count()); ii++)
    {
      if(pPunP->text(ii)[0] == QChar(flgs[2]) )
      {  pPunP->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (pPunT->count()); ii++)
    {
      if(pPunT->text(ii)[0] == QChar(flgs[3]) )
      {  pPunT->setCurrentItem(ii);
         break;
      }
    }

// Page 3

    if( flgs[5] != '-' )
      c_ICb->setChecked( true );
    else c_ICb->setChecked( false );
    if( flgs[6] != '-' )
      c_ICn->setChecked( true );
    else c_ICn->setChecked( false );
    if( flgs[7] != '-' )
      c_AUb->setChecked( true );
    else c_AUb->setChecked( false );
    if( flgs[11] != '-' )
      c_TPI->setChecked( true );
    else c_TPI->setChecked( false );
    if( flgs[9] != '-' )
      c_Chi->setChecked( true );
    else c_Chi->setChecked( false );
    if( flgs[10] != '-' )
      c_Gam->setChecked( true );
    else c_Gam->setChecked( false );
    if( flgs[4] != '-' )
      c_Coul->setChecked( true );
    else c_Coul->setChecked( false );
    if( flgs[16] != '-' )
      c_SYd->setChecked( true );
    else c_SYd->setChecked( false );

//Page 4
   p_Nsd->setValue(size[3]);
}


DualThWizard::~DualThWizard()
{}

void   DualThWizard::getSizes( int size[8] )
{
// Nqpn, Nqpg, Nb internal
    size[0]= pnQ->value();
    size[1]= pLb->value();
    size[2]= pnM->value();
    size[3]= p_Nsd->value();
    size[7]= pnP->value();
}

void DualThWizard::getFlags( char flgs[20] )
{

// Page 1
  QString str;

  str = p_Mode->currentText();
  flgs[12] = str[0].latin1();
  str = p_St->currentText();
  flgs[13] = str[0].latin1();
  str = p_IPf->currentText();
  flgs[17] = str[0].latin1();
  str = p_IPu->currentText();
  flgs[18] = str[0].latin1();
  str = p_LSF->currentText();
  flgs[19] = str[0].latin1();

// Page 2 disabled flags

//Page 3
   if( c_ICb->isChecked() )
       flgs[5] = '+';
    else flgs[5] = '-';

   if( c_ICn->isChecked() )
       flgs[6] = '+';
    else flgs[6] = '-';

   if( c_AUb->isChecked() )
       flgs[7] = '+';
    else flgs[7] = '-';

   if( c_TPI->isChecked() )
       flgs[11] = '+';
    else flgs[11] = '-';

   if( c_Chi->isChecked() )
       flgs[9] = '+';
    else flgs[9] = '-';

   if( c_Gam->isChecked() )
       flgs[10] = '+';
    else flgs[10] = '-';

   if( c_Coul->isChecked() )
       flgs[4] = '+';
    else flgs[4] = '-';

   if( c_SYd->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';

// page 4
  if( p_Nsd->value() > 0  )
       flgs[8] = '+';
  else flgs[8] = '-';
}

void
DualThWizard::help()
{
  pVisorImp->OpenHelp( DUALTH_HTML, 0, this/*, true*/ );
}

//--------------------- End of DualThWizard.cpp ---------------------------
