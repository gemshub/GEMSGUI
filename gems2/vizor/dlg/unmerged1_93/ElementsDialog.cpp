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
    EmptyData();
    rbKernel->setChecked( true );
    rbUncertain->setChecked( true );
    rbSpecific->setChecked( false );
    cbIsotopes->setChecked( false );

    SetICompList();
    SetAqueous();

    // signals and slots connections
    connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( bNext, SIGNAL( clicked() ), this, SLOT( CmOk() ) );
    connect( bReset, SIGNAL( clicked() ), this, SLOT( CmSetFilters() ) );
//  connect( bReset, SIGNAL( clicked() ), this, SLOT( CmReset() ) );
//  connect( bPrevious, SIGNAL( clicked() ), this, SLOT( CmPrevious() ) );

    connect( cbAqueous, SIGNAL( clicked() ), this, SLOT( SetAqueous() ) );
    connect( cbSorption, SIGNAL( clicked() ),this, SLOT( SetSorption() ) );
    connect( cbGaseous, SIGNAL( clicked() ), this, SLOT( SetGaseous() ) );
    connect( cbIsotopes, SIGNAL( clicked() ),this, SLOT( SetFiles() ) );

    connect( rbKernel, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );
    connect( rbUncertain, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );
    connect( rbSpecific, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );

}

ElementsDialog::~ElementsDialog()
{}


void
ElementsDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_EL_HTML );
}

void
ElementsDialog::CmSetFilters()
{
    ResetData();
    update();
    // Here to call SetFiltersDialog !!
}

// void
// ElementsDialog::CmPrevious()
// {
//   reject();
// }

void
ElementsDialog::CmOk()
{
   accept();
}

void ElementsDialog::EmptyData()
{
 int ii;
 for( ii=0; ii<bgOther->count(); ii++ )
 {     bgOther->find(ii)->setEnabled( false );
       bgOther->find(ii)->setText( tr( "...") );
 }
 for( ii=0; ii<99/*bgElem->count()-1*/; ii++ )
      bgElem->find(ii)->setEnabled( false );

  cbSorption->setChecked( false );
  cbAqueous->setChecked( true );
  cbGaseous->setChecked( true );
}


void ElementsDialog::ResetData()
{
 int ii;
 for( ii=0; ii<bgOther->count(); ii++ )
  if( bgOther->find(ii)->isOn() )
      bgOther->find(ii)->toggle();
 for( ii=0; ii<99/*bgElem->count()-1*/; ii++ )
  if( bgElem->find(ii)->isOn() )
      bgElem->find(ii)->toggle();
/*
  cbSorption->setChecked( false );
  cbAqueous->setChecked( true );
  cbGaseous->setChecked( true );
*/
}


bool ElementsDialog::isAqueous() const
{
 return cbAqueous->isChecked();
}

void ElementsDialog::SetAqueous()
{
 if( cbAqueous->isChecked() )
 {
   pbE_0->setOn(true);
   pbE_1->setOn(true);
   pbE_8->setOn(true);
 }
}

bool ElementsDialog::isSorption() const
{
 return cbSorption->isChecked();
}

void ElementsDialog::SetSorption()
{
 if( cbSorption->isChecked() )
 {
  cbAqueous->setChecked( true );
  SetAqueous();
 }
}

bool ElementsDialog::isGaseous() const
{
 return cbGaseous->isChecked();
}

void ElementsDialog::SetGaseous()
{
}

void ElementsDialog::SetFiles()
{
  ResetData();
  EmptyData();
  SetICompList();
  SetAqueous();

}

void ElementsDialog::openFiles( TCStringArray& names )
{
  names.Clear();
  if(rbKernel->isChecked())
   names.Add(".kernel.");
  if(rbUncertain->isChecked())
   names.Add(".uncertain.");
  if(rbSpecific->isChecked())
   names.Add(".specific.");
}

void
ElementsDialog::SetICompList()
{
    TCIntArray aIndMT;
    TCStringArray aIC;
    QButton* bb;
    int nmbOther=1;
    TCStringArray names;


    openFiles( names );
   // select all IComp keys and indMT (seted indMT to -1 for additional)
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->GetElements( cbIsotopes->isChecked(), names, aIC, aIndMT );

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
         bb = bgOther->find(0);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
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

    SetSorption();
    SetAqueous();

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

