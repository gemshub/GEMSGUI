//-------------------------------------------------------------------
// Id: gems/vizor/dlg/ElementsDialog.cpp  version 2.0.0    2001
//
// Implementation of ElementsDialog class
//
// Created : 011117    Modified: 011117
//
// Copyright (C) 2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
const char *GEMS_EL_HTML = "elements";

#define Inherited ElementsDialogData


#include <qlabel.h>

#include "ElementsDialog.h"
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include "visor_w.h"
#include "m_icomp.h"


ElementsDialog::ElementsDialog(QWidget* win, const char * /*prfName*/,
           const char* /*caption*/):
        Inherited( win, 0, true /* false = modeless */ )
{
    SetICompList();

    // signals and slots connections
    connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( bNext, SIGNAL( clicked() ), this, SLOT( CmOk() ) );
    connect( bReset, SIGNAL( clicked() ), this, SLOT( CmReset() ) );
    connect( bPrevious, SIGNAL( clicked() ), this, SLOT( CmPrevious() ) );
    connect( cbIsotopes, SIGNAL( toggled( bool on) ),
                this, SLOT( SetIsotopes(bool on) ) );
    connect( cbGaseous, SIGNAL( toggled( bool on) ),
                this, SLOT( SetGaseous(bool on) ) );
    connect( cbAqueous, SIGNAL( toggled( bool on) ),
                this, SLOT( SetAqueous(bool on) ) );
    connect( cbSorption, SIGNAL( toggled( bool on) ),
                this, SLOT( SetSorption(bool on) ) );
    connect( rbKernel, SIGNAL( toggled( bool on) ),
                this, SLOT( SetKernel(bool on) ) );
    connect( rbUncertain, SIGNAL( toggled( bool on) ),
                this, SLOT( SetUncertain(bool on) ) );
    connect( rbSpecific, SIGNAL( toggled( bool on) ),
                this, SLOT( SetSpecific(bool on) ) );
    cbAqueous->setChecked( true );
    cbGaseous->setChecked( true );
    cbSorption->setChecked( false );
    cbIsotopes->setChecked( false );
    rbKernel->setChecked( true );
    rbUncertain->setChecked( true );
    rbSpecific->setChecked( false );

}

ElementsDialog::~ElementsDialog()
{}


void
ElementsDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_EL_HTML );
}

void
ElementsDialog::CmReset()
{
    ResetData();
    update();
}

void
ElementsDialog::CmPrevious()
{
   reject();
}

void
ElementsDialog::CmOk()
{
   accept();
}


bool ElementsDialog::isAqueous() const
{
 return cbAqueous->isChecked();
}

void ElementsDialog::SetAqueous( bool on )
{
 if( on )
 {
   pbE_0->setOn(true);
   pbE_1->setOn(true);
   pbE_8->setOn(true);
 }
}

bool ElementsDialog::isGaseous() const
{
 return cbGaseous->isChecked();
}

void ElementsDialog::SetGaseous( bool on )
{
 if( on )
 {
  cbGaseous->setChecked( true );
 }
}

bool ElementsDialog::isSorption() const
{
 return cbSorption->isChecked();
}

void ElementsDialog::SetSorption( bool on )
{
 if( on )
 {
  cbAqueous->setChecked( true );
 }
}

bool ElementsDialog::isIsotopes() const
{
 return cbIsotopes->isChecked();
}

void ElementsDialog::SetIsotopes( bool on )
{
 if( on )
 {
  cbIsotopes->setChecked( true );
 }
}

bool ElementsDialog::isKernel() const
{
 return rbKernel->isChecked();
}

void ElementsDialog::SetKernel( bool on )
{
 if( on )
 {
  rbKernel->setChecked( true );
 }
}

bool ElementsDialog::isUncertain() const
{
 return rbUncertain->isChecked();
}

void ElementsDialog::SetUncertain( bool on )
{
 if( on )
 {
  rbUncertain->setChecked( true );
 }
}

bool ElementsDialog::isSpecific() const
{
 return rbSpecific->isChecked();
}

void ElementsDialog::SetSpecific( bool on )
{
 if( on )
 {
  rbSpecific->setChecked( true );
 }
}


void ElementsDialog::ResetData()
{
  int ii;
  for( ii=0; ii<bgOther->count(); ii++ )
   if( bgOther->find(ii)->isOn() )
       bgOther->find(ii)->toggle();
  for( ii=0; ii<bgElem->count(); ii++ )
   if( bgElem->find(ii)->isOn() )
       bgElem->find(ii)->toggle();
  cbSorption->setChecked( false );
  cbAqueous->setChecked( true );
  cbGaseous->setChecked( true );
  cbIsotopes->setChecked( false );
  rbKernel->setChecked( true );
  rbUncertain->setChecked( true );
  rbSpecific->setChecked( false );
}

void
ElementsDialog::SetICompList()
{
    TCIntArray aIndMT;
    TCStringArray aIC;
    QButton* bb;
    int nmbOther=1;

   // select all IComp keys and indMT ( indMT-100 for additional )
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->GetElements( aIC, aIndMT );

    for( uint ii=0; ii<aIC.GetCount(); ii++ )
     if( aIndMT[ii] == -1) // additional
     {
       gstring name= gstring( aIC[ii], 0, rt[RT_ICOMP].FldLen(0) );
       name.strip();
       if( name != "Vol" )
       {
         ErrorIf( nmbOther>12, aIC[ii].c_str(),
                       "Additional elements more than 12!");
         bb = bgOther->find(nmbOther);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
         aBtmId2.Add( nmbOther );
         aICkey2.Add( aIC[ii] );
         nmbOther ++;
       }
       else  //Vol
       {
         aBtmId2.Add( 0 );
         aICkey2.Add( aIC[ii] );
       }
     }
     else //elements
     {
       bb = bgElem->find(aIndMT[ii]);
       if( !bb )
        Error( aIC[ii].c_str(), "Illegal IComp ");
       aBtmId1.Add( aIndMT[ii] );
       aICkey1.Add( aIC[ii] );
       bb->setEnabled( true );
     }
}

/*! returns selection array  (IComp record keys )
    array is empty if nothing is selected
*/

void
ElementsDialog::allSelected( TCStringArray& aICkeys )
{

    uint ii;
    aICkeys.Clear();

    if( !result() )
        return;

    for( ii=0; ii<aBtmId1.GetCount(); ii++ )
    {
     if( bgElem->find( aBtmId1[ii] )->isOn()  )
        aICkeys.Add( aICkey1[ii] );
    }

    for( ii=0; ii<aBtmId2.GetCount(); ii++ )
    {
     if( bgOther->find( aBtmId2[ii] )->isOn()  )
        aICkeys.Add( aICkey2[ii] );
    }
}


// --------------------- End ElementsDialog.cpp -------------------------

