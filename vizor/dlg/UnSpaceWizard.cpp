//-------------------------------------------------------------------
// $Id: GEM2MTWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of UnSpaceWizard class
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

const char *UNSPACE_HTML = "gm_unspace";

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qradiobutton.h>


#include "UnSpaceWizard.h"
#include "gstring.h"
#include "visor_w.h"


#define Inherited UnSpaceWizardData

UnSpaceWizard::UnSpaceWizard( const char* pkey, char flgs[38],
                              int size[10], QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str1= "GEM-Selektor UnSpace Setup:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage_3, true);

//Page1
    int ii;
    for( ii=0; ii < (p_Mode->count()); ii++)
    {
      if(p_Mode->text(ii)[0] == QChar(flgs[27]) )  //Pa_OF
      {  p_Mode->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_St->count()); ii++)
    {
      if(p_St->text(ii)[0] == QChar(flgs[28]) )  //Pa_Crit
      {  p_St->setCurrentItem(ii);
         break;
      }
    }
    pnQ->setValue(size[0]);     // Q
    pnM->setValue(size[1]);     // quan_lev
    pnP->setValue(size[2]);     // nGB
    pLb->setValue(size[3]);     // nGN
    pLb_2->setValue(size[4]);   // nGR

// Page 2
    for( ii=0; ii < (p_PsUnInt->count()); ii++)
    {
      if(p_PsUnInt->text(ii)[0] == QChar(flgs[6]) )
      {  p_PsUnInt->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_PsUnInB->count()); ii++)
    {
      if(p_PsUnInB->text(ii)[0] == QChar(flgs[7]) )
      {  p_PsUnInB->setCurrentItem(ii);
         break;
      }
    }

    for( ii=0; ii < (p_PsUnFltI->count()); ii++)
    {
      if(p_PsUnFltI->text(ii)[0] == QChar(flgs[8]) )
      {  p_PsUnFltI->setCurrentItem(ii);
         break;
      }
    }
    for( ii=0; ii < (p_PsUnFltD->count()); ii++)
    {
      if(p_PsUnFltD->text(ii)[0] == QChar(flgs[9]) )
      {  p_PsUnFltD->setCurrentItem(ii);
         break;
      }
    }

    if( flgs[10] != '-' )
      c_PsGen1->setChecked( true );
    else c_PsGen1->setChecked( false );
    if( flgs[11] != '-' )
      c_PsGen2->setChecked( true );
    else c_PsGen2->setChecked( false );
    if( flgs[12] != '-' )
      c_PsGen3->setChecked( true );
    else c_PsGen3->setChecked( false );
    if( flgs[13] != '-' )
      c_PsGen4->setChecked( true );
    else c_PsGen4->setChecked( false );
    if( flgs[14] != '-' )
      c_PsGen5->setChecked( true );
    else c_PsGen5->setChecked( false );
    if( flgs[15] != '-' )
      c_PsGen6->setChecked( true );
    else c_PsGen6->setChecked( false );
    if( flgs[16] != '-' )
      c_PsGen7->setChecked( true );
    else c_PsGen7->setChecked( false );

    if( flgs[20] != '-' )
      c_Pa_f1->setChecked( true );
    else c_Pa_f1->setChecked( false );
    if( flgs[21] != '-' )
      c_Pa_f2->setChecked( true );
    else c_Pa_f2->setChecked( false );
    if( flgs[22] != '-' )
      c_Pa_f3->setChecked( true );
    else c_Pa_f3->setChecked( false );
    if( flgs[23] != '-' )
      c_Pa_f4->setChecked( true );
    else c_Pa_f4->setChecked( false );
    if( flgs[24] != '-' )
      c_Pa_f5->setChecked( true );
    else c_Pa_f5->setChecked( false );
    if( flgs[25] != '-' )
      c_Pa_f6->setChecked( true );
    else c_Pa_f6->setChecked( false );

// Page 3
    p_Pa_Adapt->setValue((int)(flgs[26]-'0'));

    if( flgs[18] != '-' )
      c_PsEqn->setChecked( true );
    else c_PsEqn->setChecked( false );
    if( flgs[19] != '-' )
      c_PsGraph->setChecked( true );
    else c_PsGraph->setChecked( false );
    if( flgs[29] != '-' )
      c_Pa_Zcp->setChecked( true );
    else c_Pa_Zcp->setChecked( false );
    if( flgs[36] != '-' )
      c_PvSi->setChecked( true );
    else c_PvSi->setChecked( false );
    if( flgs[17] != '-' )
      c_PsSy->setChecked( true );
    else c_PsSy->setChecked( false );

    if( flgs[32] != '-' )
      c_PvGs->setChecked( true );
    else c_PvGs->setChecked( false );
    if( flgs[34] != '-' )
      c_PvVs->setChecked( true );
    else c_PvVs->setChecked( false );
    if( flgs[35] != '-' )
      c_PvPgam->setChecked( true );
    else c_PvPgam->setChecked( false );
    if( flgs[30] != '-' )
      c_PvPOM->setChecked( true );
    else c_PvPOM->setChecked( false );
    if( flgs[33] != '-' )
      c_PvSs->setChecked( true );
    else c_PvSs->setChecked( false );
    if( flgs[31] != '-' )
      c_PvPOR->setChecked( true );
    else c_PvPOR->setChecked( false );

// Page 4
    p_Nsd->setValue(size[5]);  // Nsd
    pGraph->setValue(size[6]); // dimXY[1] must be 3
    pELine->setValue(size[7]); // dimEF[0] any
    pECol->setValue(size[8]);  // dimEF[1] must be 5

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

}


UnSpaceWizard::~UnSpaceWizard()
{}

void   UnSpaceWizard::getSizes( int size[10] )
{

    size[0] = pnQ->value();     // Q
    size[1] = pnM->value();     // quan_lev
    size[2] = pnP->value();     // nGB
    size[3] = pLb->value();     // nGN
    size[4] = pLb_2->value();   // nGR
    size[5] = p_Nsd->value();  // Nsd
    size[6] = pGraph->value(); // dimXY[1] must be 3
    size[7] = pELine->value(); // dimEF[0] any
    size[8] = pECol->value();  // dimEF[1] must be 5
}


void   UnSpaceWizard::getFlags( char flgs[38] )
{
  QString str;

//Page1
   str = p_Mode->currentText();
   flgs[27] = str[0].latin1();
   str = p_St->currentText();
   flgs[28] = str[0].latin1();

// Page 2
   str = p_PsUnInt->currentText();
   flgs[6] = str[0].latin1();
   str = p_PsUnInB->currentText();
   flgs[7] = str[0].latin1();
   str = p_PsUnFltI->currentText();
   flgs[8] = str[0].latin1();
   str = p_PsUnFltD->currentText();
   flgs[9] = str[0].latin1();

   if( c_PsGen1->isChecked() )
       flgs[10] = '+';
    else flgs[10] = '-';
   if( c_PsGen2->isChecked() )
       flgs[11] = '+';
    else flgs[11] = '-';
   if( c_PsGen3->isChecked() )
       flgs[12] = '+';
    else flgs[12] = '-';
   if( c_PsGen4->isChecked() )
       flgs[13] = '+';
    else flgs[13] = '-';
   if( c_PsGen5->isChecked() )
       flgs[14] = '+';
    else flgs[14] = '-';
   if( c_PsGen6->isChecked() )
       flgs[15] = '+';
    else flgs[15] = '-';
   if( c_PsGen7->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';

   if( c_Pa_f1->isChecked() )
       flgs[20] = '+';
    else flgs[20] = '-';
   if( c_Pa_f2->isChecked() )
       flgs[21] = '+';
    else flgs[21] = '-';
   if( c_Pa_f3->isChecked() )
       flgs[22] = '+';
    else flgs[22] = '-';
   if( c_Pa_f4->isChecked() )
       flgs[23] = '+';
    else flgs[23] = '-';
   if( c_Pa_f5->isChecked() )
       flgs[24] = '+';
    else flgs[24] = '-';
   if( c_Pa_f6->isChecked() )
       flgs[25] = '+';
    else flgs[25] = '-';

// Page 3
   flgs[26] = (char)(p_Pa_Adapt->value())+'0';
    p_Pa_Adapt->setValue((int)(flgs[26]-'0'));

   if( c_PsEqn->isChecked() )
       flgs[18] = '+';
    else flgs[18] = '-';
   if( c_PsGraph->isChecked() )
       flgs[19] = '+';
    else flgs[19] = '-';
   if( c_Pa_Zcp->isChecked() )
       flgs[29] = '+';
    else flgs[29] = '-';
   if( c_PvSi->isChecked() )
       flgs[36] = '+';
    else flgs[36] = '-';
   if( c_PsSy->isChecked() )
       flgs[17] = '+';
    else flgs[17] = '-';

   if( c_PvGs->isChecked() )
       flgs[32] = '+';
    else flgs[32] = '-';
   if( c_PvVs->isChecked() )
       flgs[34] = '+';
    else flgs[34] = '-';
   if( c_PvPgam->isChecked() )
       flgs[35] = '+';
    else flgs[35] = '-';
   if( c_PvPOM->isChecked() )
       flgs[30] = '+';
    else flgs[20] = '-';
   if( c_PvSs->isChecked() )
       flgs[33] = '+';
    else flgs[33] = '-';
   if( c_PvPOR->isChecked() )
       flgs[31] = '+';
    else flgs[31] = '-';

// Page 4
   str = pPunE->currentText();
   flgs[0] = str[0].latin1();
   str = pPunV->currentText();
   flgs[1] = str[0].latin1();
   str = pPunP->currentText();
   flgs[2] = str[0].latin1();
   str = pPunT->currentText();
   flgs[3] = str[0].latin1();

// flags 0,1, 37 not used or internal
   flgs[4] = '0';
   flgs[5] = '0';
}

void
UnSpaceWizard::help()
{
  pVisorImp->OpenHelp( UNSPACE_HTML, 0, this/*, true*/ );
}

//--------------------- End of UnSpaceWizard.cpp ---------------------------
