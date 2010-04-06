//-------------------------------------------------------------------
// $Id: SettingsDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of LookupDialog class
//
// Copyright (C) 2009  S.Dmytriyeva
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

const char *Lookup_HTML = "gemIPM2k";

#include <qvalidator.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <QDoubleSpinBox>

#include "LookupDialog.h"
#include "visor_w.h"

#define Inherited LookupDialogData

LookupDialog::LookupDialog(QWidget* parent):
	QDialog( parent)
{
	setupUi(this);
	QObject::connect( buttonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
	QObject::connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	QObject::connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
}

void LookupDialog::languageChange()
{
    retranslateUi(this);
}


LookupDialog::~LookupDialog()
{}

void   LookupDialog::getTdata( int& nT, float Tai[4] )
{
//   nT = pTN->value();
   Tai[0] = pTfrom->text().toDouble();
   Tai[1] = pTuntil->text().toDouble();
   if( Tai[1] < Tai[0] )
   {
	 Tai[1] = Tai[0];  
	 Tai[0] = pTuntil->text().toDouble();
   }
   Tai[2] = pTstep->text().toDouble();
   if(Tai[2] <= 1e-9 ) 
	  nT = 1;
   else
	  nT = (int)((Tai[1]-Tai[0])/Tai[2])+1;
/*   if( nT> 1 )
     Tai[2] = (Tai[1]-Tai[0])/(double)(nT-1);
   else
	 { Tai[2] = 0.; nT = 1; }*/   
   Tai[3] = pTtol->text().toDouble();
}

void   LookupDialog::getPdata( int& nP, float Pai[4] )
{
//   nP = pPN->value();
   Pai[0] = pPfrom->text().toDouble();
   Pai[1] = pPuntil->text().toDouble();
   if( Pai[1] < Pai[0] )
   {
	 Pai[1] = Pai[0];  
	 Pai[0] = pPuntil->text().toDouble();
   }
   Pai[2] = pPstep->text().toDouble();
   if(Pai[2] <= 1e-9 ) 
	  nP = 1;
   else
	  nP = (int)((Pai[1]-Pai[0])/Pai[2])+1;
/* if( nP > 1 )
     Pai[2] = (Pai[1]-Pai[0])/(double)(nP-1);
   else
	 { Pai[2] = 0.;  nP = 1; }
*/	   
   Pai[3] = pPtol->text().toDouble();
}

void   LookupDialog::getFlags( char flgs[4] )
{
   if( chSelect->isChecked() )
       flgs[0] = '+';
    else flgs[0] = '-';

   if( chMode->isChecked() )
       flgs[1] = '+';
    else flgs[1] = '-';
   
   if( chAll->isChecked() )
       flgs[2] = '+';
    else flgs[2] = '-';
   
   if( chComment->isChecked() )
       flgs[3] = '+';
    else flgs[3] = '-';

}

void LookupDialog::CmHelp()
{
  pVisorImp->OpenHelp( Lookup_HTML );
}


//--------------------- End of SettingsDialog.cpp ---------------------------

