//-------------------------------------------------------------------
// $$
//
// Implementation of ProcessWizard class
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


#include "ProcessWizard.h"
#include "visor_w.h"


#define Inherited ProcessWizardData

ProcessWizard::ProcessWizard( const char* pkey, char flgs[24],
                              int size[6], QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str1= "Setup to Create Process:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage2, true);

//Page1
   if( flgs[8] != '-' )
       pselT->setChecked( true );
   else  if( flgs[11] != '-' )
            pselG->setChecked( true );
          else  if( flgs[5] != '-' )
                  pselR->setChecked( true );
                else if( flgs[2] != '-' )
                       pselS->setChecked( true );
                     else
                       pselP->setChecked( true );

//Page2
    pPoints->setValue(size[0]);
    pMods->setValue(size[1]);
    pGraph->setValue(size[2]);
    pELine->setValue(size[3]);
    pECol->setValue(size[4]);

    if( flgs[12] != '-' )
      c_PvTm->setChecked( true );
    else c_PvTm->setChecked( false );

    if( flgs[13] != '-' )
      c_PvNV->setChecked( true );
    else c_PvNV->setChecked( false );

    if( flgs[17] != '-' )
      c_PvTau->setChecked( true );
    else c_PvTau->setChecked( false );

    if( flgs[14] != '-' )
      c_PvP->setChecked( true );
    else c_PvP->setChecked( false );

    if( flgs[15] != '-' )
      c_PvT->setChecked( true );
    else c_PvT->setChecked( false );

    if( flgs[16] != '-' )
      c_PvV->setChecked( true );
    else c_PvV->setChecked( false );

    if( flgs[18] != '-' )
      c_PvpXi->setChecked( true );
    else c_PvpXi->setChecked( false );

    if( flgs[19]!= '-' )
      c_PvNu->setChecked( true );
    else c_PvNu->setChecked( false );

    if( flgs[20] != '-' )
      c_PvKin->setChecked( true );
    else c_PvKin->setChecked( false );

//Page3
    if( flgs[22] != '-' )
      c_PvR1->setChecked( true );
    else c_PvR1->setChecked( false );

    if( flgs[0] >= '5' )
      pStepwise->setChecked( true );
    else pStepwise->setChecked( false );

    if( flgs[3] != '-' )
      c_PsRT->setChecked( true );
    else c_PsRT->setChecked( false );

    if( flgs[10] != '-' )
      c_PsEqn->setChecked( true );
    else c_PsEqn->setChecked( false );

    if( flgs[4] != '-' )
      c_PsSY->setChecked( true );
    else c_PsSY->setChecked( false );

//Page4
    spinBox18->setValue(size[5]);

}


ProcessWizard::~ProcessWizard()
{}

void   ProcessWizard::getSizes( int size[6] )
{

    size[0]= pPoints->value();
    size[1]= pMods->value();
    size[2]= pGraph->value();
    size[3]= pELine->value();
    size[4]= pECol->value();
    size[5]= spinBox18->value();
}


void   ProcessWizard::getFlags( char flgs[24] )
{
  char type='S';

  if( pselT->isChecked())
   type='T';
  else if( pselG->isChecked())
           type='G';
       else if( pselR->isChecked())
                type='R';
          else if( pselP->isChecked())
                type='P';
                else  if( pselS->isChecked())
                           type='S';
   switch( type )
   {
    case 'T': strncpy( flgs, "0++-+--++-+-", 12);
        break;
    case 'G': strncpy( flgs, "0++-+--+--++", 12);
        break;
    case 'R': strncpy( flgs, "0++-++-+--+-", 12);
        break;
    case 'S': strncpy( flgs, "0++-+-----+-", 12);
        break;
    case 'P':
    default:  memcpy( flgs, "0*----------", 12 );
   }
// Putting other flags
   if( c_PvTm->isChecked() )
       flgs[12] = '+';
    else flgs[12] = '-';

   if( c_PvNV->isChecked() )
       flgs[13] = '+';
    else flgs[13] = '-';

   if( c_PvTau->isChecked() )
       flgs[17] = '+';
    else flgs[17] = '-';

   if( c_PvP->isChecked() )
       flgs[14] = '+';
    else flgs[14] = '-';

   if( c_PvT->isChecked() )
       flgs[15] = '+';
    else flgs[15] = '-';

   if( c_PvV->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';

   if( c_PvpXi->isChecked() )
       flgs[18] = '+';
    else flgs[18] = '-';

   if( c_PvNu->isChecked() )
       flgs[19] = '+';
    else flgs[19] = '-';

   if( c_PvKin->isChecked() )
       flgs[20] = '+';
    else flgs[20] = '-';

//Page 3
   if( c_PvR1->isChecked() )
       flgs[22] = '+';
    else flgs[22] = '-';

   if( pStepwise->isChecked() )
       flgs[0] = '5';
    else flgs[0] = '0';

   if( c_PsRT->isChecked() )
       flgs[3] = '+';

   if( c_PsEqn->isChecked() ) //setted before, not switch off
       flgs[10] = '+';

   if( c_PsSY->isChecked() ) //setted before, not switch off
       flgs[4] = '+';
    else flgs[4] = '-';

//Putting flags, using sizes  is out of function
}

void
ProcessWizard::help()
{
  pVisorImp->OpenHelp( PROCESS_HTML, 0, this/*, true*/ );
}

//--------------------- End of ProcessWizard.cpp ---------------------------
