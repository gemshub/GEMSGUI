//-------------------------------------------------------------------
// $Id$
//
// Implementation of AutoPhaseDialog class
//
// Copyright (C) 2003 S.Dmytriyeva
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

const char *AUTOPHASE_HTML = "autophase_setup";
const char * dfAqKeyD =  "a   AQELIA  aq_gen          aq  Davies          ";
const char * dfAqKeyH =  "a   AQELIA  aq_gen          aq  EDH_H           ";
const char * dfAqKey3 =  "a   AQELIA  aq_gen          aq  EDH_K           ";
const char * dfAqKey2 =  "a   AQELIA  aq_gen          aq  DH_K            ";
const char * dfAqKey1 =  "a   AQELIA  aq_gen          aq  DH_LL           ";
const char * dfAqKeyY =  "a   AQELIA  aq_gen          aq  EDH_Y           ";
const char * dfAqKeyU =  "a   AQELSI  aq_gen          aq  User-Provided   ";
const char * dfGasKey =  "g   GASMXID gas_gen         gm  Ideal           ";
const char * dfFluKey =  "f   FLUIDMX fluid_gen       gm  GC_EoS          ";

// List of maximum values of ionic strength up to which the respective model is applicable
// (not used in calculations, just to inform the user)
const double dfImaxD = 0.5;
const double dfImaxH = 1.5;
const double dfImax3 = 1.0;
const double dfImax2 = 0.3;
const double dfImax1 = 0.01;
const double dfImaxS = 3.0;
const double dfImaxU = 9.9;

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qvalidator.h>

#include "AutoPhaseDialog.h"
#include "visor_w.h"

#define Inherited AutoPhaseDialogData

AutoPhaseDialog::AutoPhaseDialog (
    const char *pr_key, char acode, char gcode, QWidget* parent):
         Inherited( parent, 0, true )
{
    gstring str= "Setup of aqueous and gas phases in project:  ";
            str += pr_key;
    setCaption( str.c_str() );

    aqu_code = acode;
    switch( acode )
    {
      case '-': aselNo->setChecked( true ); break;
      case 'U': aselU->setChecked( true );  break;
      case 'Y': aselY->setChecked( true ); break;
      case 'H': aselH->setChecked( true ); break;
      case '3': asel3->setChecked( true ); break;
      case '2': asel2->setChecked( true ); break;
      case '1': asel1->setChecked( true ); break;
      default:
      case 'D': aselD->setChecked( true ); break;
    }

    gas_code = gcode;
    switch( gcode )
    {
      case '-': gselNo->setChecked( true ); break;
      case 'U': gselU->setChecked( true );
                break;
      case 'F': gselF->setChecked( true ); break;
      case 'I':
      default: gselI->setChecked( true ); break;
    }
}

AutoPhaseDialog::~AutoPhaseDialog()
{}
/*
int    // merged into get_apar() and set_apar()
AutoPhaseDialog::get_resp()   // T-calculation mode of b_g Helgeson parameter
{
  return pBG_T->currentItem();
}

void
AutoPhaseDialog::set_resp( int resp_t )
{
  pBG_T->setCurrentItem( resp_t );
}
*/
void
AutoPhaseDialog::get_apar ( float par[8] )
{
    par[0] = QString( apEdit0->currentText() ).toDouble();
    par[1] = QString( apEdit1->currentText() ).toDouble();
    par[2] = (float)apEdit2->currentItem();
    par[3] = (float)apEdit3->currentItem();
    par[4] = (float)pBG_T->currentItem();
    par[5] = (float)comMolal->currentItem();
}

void
AutoPhaseDialog::set_apar ( float par[8] )
{
    QString str;
 //   double I_max;

    apEdit0->setValidator( new QDoubleValidator( apEdit0 ) );
    apEdit0->setCurrentText( str.setNum( (double)par[0] ) );

    apEdit1->setValidator( new QDoubleValidator( apEdit1 ) );
    apEdit1->setCurrentText( str.setNum( (double)par[1] ) );

//    apEdit2->setValidator( new QDoubleValidator( apEdit2 ) ); // SD 2008
    apEdit2->setCurrentItem( ( (int)par[2] ) );

/*    switch( aqu_code )
    {
       case '-':
       default:  I_max = 0.; break;
       case 'Q':
       case 'U': I_max =  dfImaxU; break;
       case 'S': I_max =  dfImaxS; break;
       case 'H': I_max =  dfImaxH; break;
       case '3': I_max =  dfImax3; break;
       case '2': I_max =  dfImax2; break;
       case '1': I_max =  dfImax1; break;
       case 'D': I_max =  dfImaxD; break;
    }
    apEdit3->setValidator( new QDoubleValidator( apEdit3 ) );
*/
    apEdit3->setCurrentItem( (int)par[3] );
//   apEdit3->setText( str.setNum( (double)par[3] ) );
    pBG_T->setCurrentItem( (int)par[4] );
    comMolal->setCurrentItem( (int)par[5] );
}

char
AutoPhaseDialog::get_acode()
{
     aqu_code='D'; // default code

     if( aselU->isChecked())
       aqu_code ='U';
     else if( aselH->isChecked())
           aqu_code ='H';
        else if( asel3->isChecked())
              aqu_code ='3';
            else if( asel2->isChecked())
                  aqu_code ='2';
               else if( asel1->isChecked())
                     aqu_code = '1';
                 else if( aselY->isChecked())
                       aqu_code = 'Y';
                   else if( aselNo->isChecked())
                         aqu_code = '-';
  return aqu_code;
}

void
AutoPhaseDialog::get_akey( gstring& a_key )
{
   a_key = aqu_key;
}

void
AutoPhaseDialog::set_akey( gstring& a_key )
{
  aqu_key = a_key;
/*  if( aselU->isChecked())
      aqu_key = "a:*:*:*:*:";
  else*/
  if( aselD->isChecked())
  {    aqu_key = dfAqKeyD;
  }
     else if( aselH->isChecked())
           aqu_key = dfAqKeyH;
        else if( asel3->isChecked())
              aqu_key = dfAqKey3;
            else if( asel2->isChecked())
                  aqu_key = dfAqKey2;
               else if( asel1->isChecked())
                      aqu_key = dfAqKey1;
                   else if( aselY->isChecked())
                         aqu_key = dfAqKeyY;
                      else if( aselNo->isChecked())
                            aqu_key ="";

  apRkeyEdit->setText( aqu_key.c_str() );
}

void
AutoPhaseDialog::get_gpar ( float par[4] )
{
    par[0] = QString( gpEdit0->text() ).toDouble();
    par[1] = QString( gpEdit1->text() ).toDouble();
    par[2] = QString( gpEdit2->text() ).toDouble();
    par[3] = QString( gpEdit3->text() ).toDouble();
}

void
AutoPhaseDialog::set_gpar ( float par[4] )
{
    QString str;

    gpEdit0->setValidator( new QDoubleValidator( gpEdit0 ) );
    gpEdit0->setText( str.setNum( (double)par[0] ) );

    gpEdit1->setValidator( new QDoubleValidator( gpEdit1 ) );
    gpEdit1->setText( str.setNum( (double)par[1] ) );

    gpEdit2->setValidator( new QDoubleValidator( gpEdit2 ) );
    gpEdit2->setText( str.setNum( (double)par[2] ) );

    gpEdit3->setValidator( new QDoubleValidator( gpEdit3 ) );
    gpEdit3->setText( str.setNum( (double)par[3] ) );
}


char
AutoPhaseDialog::get_gcode()
{
  gas_code = 'I';  // default code

    if( gselU->isChecked())
      gas_code ='U';
    else if( gselF->isChecked())
          gas_code ='F';
       else if( gselNo->isChecked())
             gas_code ='-';

  return gas_code;
}

void
AutoPhaseDialog::get_gkey( gstring& g_key )
{
   g_key = gas_key;
}

void
AutoPhaseDialog::set_gkey( gstring& g_key )
{
  gas_key = g_key;

/*  if( gselU->isChecked())
      gas_key = "g:*:*:*:*:";
    else*/ if( gselI->isChecked())
            gas_key = dfGasKey;
         else if( gselF->isChecked())
               gas_key = dfFluKey;
            else if( gselNo->isChecked())
                  gas_key ="";
  gpRkeyEdit->setText( gas_key.c_str() );
}

// slots

void
AutoPhaseDialog::CmCheck()
{
  float par[4];
  gstring a_key = aqu_key;
  gstring g_key = gas_key;

  if( a_key.empty()  )
      a_key = "a:*:*:*:*:";
  if( g_key.empty()  )
      g_key = "g:*:*:*:*:";

  set_akey( a_key );
  get_acode();
  get_apar( par );
  set_apar( par );

  set_gkey( g_key );
  get_gcode();
  get_gpar( par );
// set_gpar( par );
}

void
AutoPhaseDialog::CmHelp()
{
  pVisorImp->OpenHelp( AUTOPHASE_HTML, 0, this/*, true*/ );
}

//--------------------- End of ProcessDialog.cpp ---------------------------
