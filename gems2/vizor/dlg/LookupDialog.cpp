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

#include "LookupDialog.h"
#include "visor_w.h"

#define Inherited LookupDialogData

LookupDialog::LookupDialog(QWidget* parent):
        Inherited( parent, 0, true )
{
    // setCaption( "Setup for writing GEMIPM2K input files " );

	QDoubleValidator *vPfrom = new QDoubleValidator( pPfrom );
	vPfrom->setRange ( 0., 10000., 1 );  
	pPfrom->setValidator( vPfrom );
	 
	QDoubleValidator *vPuntil = new QDoubleValidator( pPuntil );
	vPuntil->setRange ( 0., 10000., 1 );  
	pPuntil->setValidator( vPuntil );

	QDoubleValidator *vpPtol = new QDoubleValidator( pPtol );
	vpPtol->setRange ( 0.001, 10., 3 );  
	pPtol->setValidator( vpPtol );
	pPtol->setText("0.5");

	QDoubleValidator *vTfrom = new QDoubleValidator( pTfrom );
	vTfrom->setRange ( 0., 2000., 2 );  
	pTfrom->setValidator( vTfrom );
	 
	QDoubleValidator *vTuntil = new QDoubleValidator( pTuntil );
	vTuntil->setRange ( 0., 2000., 2 );  
	pTuntil->setValidator( vTuntil );

	QDoubleValidator *vpTtol = new QDoubleValidator( pTtol );
	vpTtol->setRange ( 0.001, 10., 3 );  
	pTtol->setValidator( vpTtol );
	pTtol->setText("1.");
}

LookupDialog::~LookupDialog()
{}

void   LookupDialog::getTdata( int& nT, float Tai[4] )
{
   nT = pTN->value();
   Tai[0] = pTfrom->text().toDouble();
   Tai[1] = pTuntil->text().toDouble();
   if( Tai[1] < Tai[0] )
   {
	 Tai[1] = Tai[0];  
	 Tai[0] = pTuntil->text().toDouble();
   }
   if( nT> 1 )
     Tai[2] = (Tai[1]-Tai[0])/(double)(nT-1);
   else
	 { Tai[2] = 0.; nT = 1; }   
   Tai[3] = pTtol->text().toDouble();
}

void   LookupDialog::getPdata( int& nP, float Pai[4] )
{
   nP = pPN->value();
   Pai[0] = pPfrom->text().toDouble();
   Pai[1] = pPuntil->text().toDouble();
   if( Pai[1] < Pai[0] )
   {
	 Pai[1] = Pai[0];  
	 Pai[0] = pPuntil->text().toDouble();
   }
   if( nP > 1 )
     Pai[2] = (Pai[1]-Pai[0])/(double)(nP-1);
   else
	 { Pai[2] = 0.;  nP = 1; }  
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

