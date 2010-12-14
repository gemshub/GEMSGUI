//-------------------------------------------------------------------
// $Id: GEM2MTWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of GEM2MTWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
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

const char *GEM2MT_HTML = "gm_gem2mt";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qvariant.h>


#include "GEM2MTWizard.h"
#include "gstring.h"
#include "visor_w.h"

void GEM2MTWizard::languageChange()
{
    retranslateUi(this);
}

void
GEM2MTWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
GEM2MTWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	GEM2MTWizard::resetNextButton()
{
	if( stackedWidget->currentIndex() == stackedWidget->count() - 1 )
	{	
		pNext->disconnect();
		connect( pNext, SIGNAL(clicked()), this, SLOT(accept()) );
		pNext->setText("&Finish");
	}
	else
	{	
		pNext->disconnect();
		connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()) );
		pNext->setText("&Next>");
	}
}

void 	GEM2MTWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}




GEM2MTWizard::GEM2MTWizard( const char* pkey, char flgs[22],
                              int size[17], QWidget* parent):
          QDialog( parent )
{

//    setFinishEnabled( WizardPage2, true);
    setupUi(this);
    gstring str1= "GEM-Selektor GEM2MT Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

//Page1
   if( flgs[14] == 'S' )
       pselS->setChecked( true );
   else  if( flgs[14] == 'F' )
         pselG->setChecked( true );
     else  if( flgs[14] == 'A' )
         pselT->setChecked( true );
       else if( flgs[14] == 'D' )
          pselR->setChecked( true );
         else if( flgs[14] == 'W' )
           pselW->setChecked( true );
             else if( flgs[14] == 'V' )
               pselV->setChecked( true );
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
    pnPTypes->setValue(size[15]);
    pnProps->setValue(size[16]);

//Page 3

    pLb->setValue(size[9]);
    pnEl->setValue(size[10]);
    pqpc->setValue(size[11]);
    pPai->setValue(size[12]);
    pTai->setValue(size[13]);

    if( flgs[4] != '-' )
      c_PvICi->setChecked( true );
    else c_PvICi->setChecked( false );
    if( flgs[17] != '-' )
      c_PsSYd->setChecked( true );
    else c_PsSYd->setChecked( false );
    if( flgs[18] != '-' )
      c_PsSdat->setChecked( true );
    else c_PsSdat->setChecked( false );
    if( flgs[19] != '-' )
      c_PsDDc->setChecked( true );
    else c_PsDDc->setChecked( false );
    if( flgs[20] != '-' )
      c_PsDIc->setChecked( true );
    else c_PsDIc->setChecked( false );
    if( flgs[21] != '-' )
      c_PvTPai->setChecked( true );
    else c_PvTPai->setChecked( false );
    if( flgs[6] != '-' )
      c_PsMO->setChecked( true );
    else c_PsMO->setChecked( false );

   if( flgs[12] != '-' )
     c_PvGrid->setChecked( true );
   else
	  c_PvGrid->setChecked( false );
   if( flgs[13] != '-' )
     c_PvSIA->setChecked( true );
   else c_PvSIA->setChecked( false );
//Page4
    spinBox18->setValue(size[14]);

}


GEM2MTWizard::~GEM2MTWizard()
{}

void   GEM2MTWizard::getSizes( int size[17] )
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
    size[15]= pnPTypes->value();
    size[16]= pnProps->value();

//Page 3
    size[9]= pLb->value();
    size[10]= pnEl->value();
    size[11]= pqpc->value();
    size[12]= pPai->value();
    size[13]= pTai->value();
    size[14]= spinBox18->value();
}


void   GEM2MTWizard::getFlags( char flgs[22] )
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
          else  if( pselW->isChecked())
                 type='W';
            else  if( pselV->isChecked())
                  type='V';

   flgs[14] = type;
   // Putting other flags
   if( c_PvICi->isChecked() )
       flgs[4] = '+';
    else flgs[4] = '-';
   if( c_PsSYd->isChecked() )
       flgs[17] = '+';
    else flgs[17] = '-';
   if( c_PsSdat->isChecked() )
       flgs[18] = '+';
    else flgs[18] = '-';
   if( c_PsDDc->isChecked() )
       flgs[19] = '+';
    else flgs[19] = '-';
   if( c_PsDIc->isChecked() )
       flgs[20] = '+';
    else flgs[20] = '-';
   if( c_PvTPai->isChecked() )
       flgs[21] = '+';
    else flgs[21] = '-';
   if( c_PsMO->isChecked() )
       flgs[6] = '+';
    else flgs[6] = '-';
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

  if( c_PvGrid->isChecked() )
     flgs[12] = '+';
  else flgs[12] = '-';
  if( c_PvSIA->isChecked() )
     flgs[13] = '+';
  else flgs[13] = '-';
  
  
// flags 0,1,2,3, 15, 16 not used or internal
}

void
GEM2MTWizard::help()
{
  pVisorImp->OpenHelp( GEM2MT_HTML, 0 );
}

//--------------------- End of GEM2MTWizard.cpp ---------------------------
