//-------------------------------------------------------------------
// $Id: AutoPhaseDialog.cpp 1364 2009-07-16 08:28:54Z wagner $
//
// Implementation of AutoPhaseDialog class
//
// Copyright (C) 2003,2010 SD, DK
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------


// List of maximum values of ionic strength up to which the respective model is applicable
// (not used in calculations, just to inform the user)
/*const double dfImaxD = 0.5;
const double dfImaxH = 1.5;
const double dfImax3 = 1.0;
const double dfImax2 = 0.3;
const double dfImax1 = 0.01;
const double dfImaxS = 3.0;
const double dfImaxU = 9.9;*/

#include "ui_AutoPhaseDialog4.h"
#include "AutoPhaseDialog.h"
#include "GemsMainWindow.h"
#include "service.h"
#include "m_syseq.h"

const char * dfAqKeyD =  "a   AQELIA  aq_gen          aq  Davies          ";
const char * dfAqKeyH =  "a   AQELIA  aq_gen          aq  EDH_H           ";
const char * dfAqKey3 =  "a   AQELIA  aq_gen          aq  EDH_K           ";
const char * dfAqKey2 =  "a   AQELIA  aq_gen          aq  DH_K            ";
const char * dfAqKey1 =  "a   AQELIA  aq_gen          aq  DH_LL           ";
const char * dfAqKeyY =  "a   AQELIA  aq_gen          aq  EDH_Y           ";
const char * dfAqKeyU =  "a   AQELSI  aq_spec         aq  User-Provided   ";
const char * dfGasKey =  "g   GASMXID gas_gen         gm  Ideal           ";
const char * dfFluKeyF=  "f   CG      fluid_gen       gm  CG_EoS          ";
const char * dfFluKey7=  "f   PR      fluid_gen       gm  PR_EoS          ";
const char * dfFluKeyP=  "f   PRSV    fluid_gen       gm  PRSV_EoS        ";
const char * dfFluKeyE=  "f   SRK     fluid_gen       gm  SRK_EoS         ";
const char * dfFluKeyU = "f   FLUID   fluid_spec      gm  User-Provided   ";


AutoPhaseDialog::AutoPhaseDialog (
        const char *pr_key, char acode, char gcode, QWidget* parent):
    QDialog( parent ),
    ui(new Ui::AutoPhaseDialogData)
{
    
    ui->setupUi(this);
    string str= "Setup of aqueous and gas phases in project:  ";
    str += pr_key;
    setWindowTitle( str.c_str() );

    aqu_code = acode;
    switch( acode )
    {
    case '-': ui->aselNo->setChecked( true ); break;
    case 'U': ui->aselU->setChecked( true );  break;
    case 'Y': ui->aselY->setChecked( true ); break;
    case 'H': ui->aselH->setChecked( true ); break;
    case '3': ui->asel3->setChecked( true ); break;
    case '2': ui->asel2->setChecked( true ); break;
    case '1': ui->asel1->setChecked( true ); break;
    default:
    case 'D': ui->aselD->setChecked( true ); break;
    }

    gas_code = gcode;
    switch( gcode )
    {
    case '-': ui->gselNo->setChecked( true ); break;
    case 'U': ui->gselU->setChecked( true );  break;
    case 'F': ui->gselF->setChecked( true ); break;
    case '7': ui->gsel7->setChecked( true ); break;
    case 'P': ui->gselP->setChecked( true ); break;
    case 'E': ui->gselE->setChecked( true ); break;
    case 'I':
    default: ui->gselI->setChecked( true ); break;
    }

    connect(ui->buttonHelp, SIGNAL(clicked()), this, SLOT(CmHelp()));
    connect(ui->buttonCheck, SIGNAL(clicked()), this, SLOT(CmCheck()));
    connect(ui->buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

AutoPhaseDialog::~AutoPhaseDialog()
{
    delete ui;
}


void AutoPhaseDialog::get_apar ( float par[8] )
{
    par[0] = QString( ui->apEdit0->currentText() ).toFloat();
    par[1] = QString( ui->apEdit1->currentText() ).toFloat();
    par[2] = ui->apEdit2->currentIndex();
    par[3] = ui->apEdit3->currentIndex();
    par[4] = ui->pBG_T->currentIndex();
    par[5] = ui->comMolal->currentIndex();
}

void AutoPhaseDialog::set_apar ( float par[8] )
{
    QString str;
    //   double I_max;

    ui->apEdit0->setValidator( new QDoubleValidator( ui->apEdit0 ) );
    ui->apEdit0->setEditText( str.setNum( par[0] ) );

    ui->apEdit1->setValidator( new QDoubleValidator( ui->apEdit1 ) );
    ui->apEdit1->setEditText( str.setNum( par[1] ) );

    ui->apEdit2->setCurrentIndex( static_cast<int>(par[2])  );
    ui->apEdit3->setCurrentIndex( static_cast<int>(par[3]) );
    ui->pBG_T->setCurrentIndex( static_cast<int>(par[4]) );
    ui->comMolal->setCurrentIndex( static_cast<int>(par[5]) );
}

char AutoPhaseDialog::get_acode()
{
    aqu_code='D'; // default code

    if( ui->aselU->isChecked())
        aqu_code ='U';
    else if( ui->aselH->isChecked())
        aqu_code ='H';
    else if( ui->asel3->isChecked())
        aqu_code ='3';
    else if( ui->asel2->isChecked())
        aqu_code ='2';
    else if( ui->asel1->isChecked())
        aqu_code = '1';
    else if( ui->aselY->isChecked())
        aqu_code = 'Y';
    else if( ui->aselNo->isChecked())
        aqu_code = '-';
    return aqu_code;
}

void AutoPhaseDialog::get_akey( string& a_key )
{
    a_key = aqu_key;
}

void AutoPhaseDialog::set_akey( string& a_key )
{
    aqu_key = a_key;
    /*  if( ui->aselU->isChecked())
      aqu_key = "a:*:*:*:*:";
  else*/
    if( ui->aselD->isChecked())
    {    aqu_key = dfAqKeyD;
    }
    else if( ui->aselH->isChecked())
        aqu_key = dfAqKeyH;
    else if( ui->asel3->isChecked())
        aqu_key = dfAqKey3;
    else if( ui->asel2->isChecked())
        aqu_key = dfAqKey2;
    else if( ui->asel1->isChecked())
        aqu_key = dfAqKey1;
    else if( ui->aselY->isChecked())
        aqu_key = dfAqKeyY;
    else if( ui->aselNo->isChecked())
        aqu_key ="";

    ui->apRkeyEdit->setText( aqu_key.c_str() );
}


char AutoPhaseDialog::get_gcode()
{
    gas_code = 'I';  // default code

    if( ui->gselU->isChecked())
        gas_code ='U';
    else if( ui->gselF->isChecked())
        gas_code ='F';
    else if( ui->gselNo->isChecked())
        gas_code ='-';
    else if( ui->gselP->isChecked())
        gas_code ='P';
    else if( ui->gsel7->isChecked())
        gas_code ='7';
    else if( ui->gselE->isChecked())
        gas_code ='E';

    return gas_code;
}

void AutoPhaseDialog::get_gkey( string& g_key )
{
    g_key = gas_key;
}

void AutoPhaseDialog::set_gkey( string& g_key )
{
    gas_key = g_key;

    /*  if( ui->gselU->isChecked())
      gas_key = "g:*:*:*:*:";
    else*/ if( ui->gselI->isChecked())
        gas_key = dfGasKey;
    else if( ui->gselF->isChecked())
        gas_key = dfFluKeyF;
    else if( ui->gselNo->isChecked())
        gas_key ="";
    else if( ui->gselP->isChecked())
        gas_key =dfFluKeyP;
    else if( ui->gsel7->isChecked())
        gas_key =dfFluKey7;
    else if( ui->gselE->isChecked())
        gas_key =dfFluKeyE;
    ui->gpRkeyEdit->setText( gas_key.c_str() );
}

// slots

void AutoPhaseDialog::CmCheck()
{
    float par[16];
    string a_key = aqu_key;
    string g_key = gas_key;

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
}

void AutoPhaseDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_APHSETUP_HTML );
}

//--------------------- End of ProcessDialog.cpp ---------------------------
