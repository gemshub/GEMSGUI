//-------------------------------------------------------------------
// $Id$
//
// Implementation of ElementsDialog class
//
// Copyright (C) 2001  S.Dmytriyeva
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
const char *GEMS_EL_HTML = "elements";

#define Inherited ElementsDialogData


#include <qlabel.h>

#include "ElementsDialog.h"
#include "SetFiltersDialog.h"
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include "visor.h"
#include "visor_w.h"
#include "m_icomp.h"


ElementsDialog::ElementsDialog(QWidget* win, const char * prfName,
           const char* /*caption*/):
        Inherited( win, 0, true /* false = modeless */ ),
        prf_name ( prfName )
{

    pLogoImg->setPixmap(pVisorImp->getLogo());

   // build IComp list from template database
    TCIntArray aIndMT;
    TCStringArray aIC;
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->GetElements( true, aIC, aIndMT );
    sf_data.ic_d.oldIComps.Clear();

    for( uint ii=0; ii<aIC.GetCount(); ii++ )
    {
     sf_data.ic_d.oldIComps.Add( aIC[ii] );
     el_data.oldIComps.Add( aIC[ii] );
     if( aIndMT[ii] == -1) // additional
     {
         aICkey2_sel.Add( aIC[ii] );
     }
     else //elements
     {
       aBtmId1_sel.Add( aIndMT[ii] );
     }
    }

    EmptyData();
    rbKernel->setChecked( true );
    rbKernel_2->setChecked( false );
    rbUncertain->setChecked( false );
    rbSpecific->setChecked( false );
    cbIsotopes->setChecked( false );
    setFilesList();

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
    connect( rbKernel_2, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );
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
    // Here to call SetFiltersDialog !!
    SetData();
    sf_data.ic_d.newIComps.Clear();
    for(uint jj, ii=0; ii<el_data.ICrds.GetCount(); ii++ )
    {
      for( jj=0; jj<sf_data.ic_d.oldIComps.GetCount(); jj++)
        if( !memcmp( el_data.ICrds[ii].c_str(),
            sf_data.ic_d.oldIComps[jj].c_str(), MAXICNAME+MAXSYMB ) )
         break;
      if( jj== sf_data.ic_d.oldIComps.GetCount() )
        sf_data.ic_d.newIComps.Add( el_data.ICrds[ii] );
    }

    SetFiltersDialog  dlg( this, &files_data, &sf_data, prf_name.c_str() );
    dlg.exec();

   if(  files_data.changed  )  // we changed file cnf for icomp
   {  ResetData();
      EmptyData();
      SetICompList();
      SetAqueous();
   }
}

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
  resetFilesSelection();
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

// 0 no changed (no kernel, specifik or uncertain)
// 1 to open, 2 to close
int ElementsDialog::isOpenFile( gstring& name )
{
   int iret=0;
   if(  name.find( ".kernel." ) != gstring::npos )
   {    if(rbKernel->isChecked())
           iret = 1;
         else
           iret = 2;
   }
   else  if(  name.find( ".uncertain." ) != gstring::npos )
         {    if(rbUncertain->isChecked())
                 iret = 1;
              else
                 iret = 2;
          }
          else   if(  name.find( ".specific." ) != gstring::npos )
                {    if(rbSpecific->isChecked())
                          iret = 1;
                     else
                           iret = 2;
                 }
             else   if(  name.find( ".complem." ) != gstring::npos )
                    {    if(rbKernel_2->isChecked())
                           iret = 1;
                        else
                           iret = 2;
                     }

  return iret;
}


void
ElementsDialog::SetICompList()
{
    TCIntArray aIndMT;
    TCStringArray aIC;
    QButton* bb;
    int nmbOther=1;
//    TCStringArray names;

    aBtmId1.Clear();
    aICkey1.Clear();
    aBtmId2.Clear();
    aICkey2.Clear();

    //openFiles( names );
    openFilesICOMP();
   // select all IComp keys and indMT (seted indMT to -1 for additional)
    TIComp* aICdata=(TIComp *)(&aMod[RT_ICOMP]);
    aICdata->GetElements( cbIsotopes->isChecked(), aIC, aIndMT );

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

  // set selection form template
  for( uint ii=0; ii<aBtmId1_sel.GetCount(); ii++ )
  {
       bb = bgElem->find(aBtmId1_sel[ii]);
       bb->setEnabled( false );
       ((QPushButton *)bb)->setOn( true );
  }
  for( uint ii=0; ii<aICkey2_sel.GetCount(); ii++ )
  {
       int jj;
       gstring name= gstring( aICkey2_sel[ii],
                      0, rt[RT_ICOMP].FldLen(0) );
       name.strip();
       for( jj=0; jj<nmbOther; jj++ )
       {
         bb =  bgOther->find(jj);
         const char *ttl =
             (const char*)bb->text();
         if( name == ttl )
         {
          bb->setEnabled( false );
          ((QPushButton *)bb)->setOn( true );
          break;
         }
       }
       if( jj==nmbOther )
       {
         bb = bgOther->find(nmbOther);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( false );
         ((QPushButton *)bb)->setOn( true );
         aBtmId2.Add( nmbOther );
         aICkey2.Add( aICkey2_sel[ii] );
         nmbOther ++;
       }
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

//    if( !result() )
//        return;

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

const setFiltersData&
ElementsDialog::getFilters()
{  // open all files
   openFilesSelection();
   return sf_data;
}

const elmWindowData&
ElementsDialog::getData()
{
 SetData();
 return  el_data;
}

void
ElementsDialog::SetData()
{
 el_data.flags[cbAqueous_] =  cbAqueous->isChecked();
 el_data.flags[cbSorption_] = cbSorption->isChecked();
 el_data.flags[cbGaseous_] =  cbGaseous->isChecked();
 el_data.flags[cbIsotopes_] = cbIsotopes->isChecked();

 //openFiles( el_data.flNames );
 allSelected( el_data.ICrds );
}

void
ElementsDialog::setFilesList()
{
   size_t pos1, pos2;
   int cnt, cnt_sel, ind;
 //files_data
   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
    {
        if( aMod[i].IsSubModule() )
            continue;
        TCStringArray names;
        TCIntArray indx;
        TCIntArray sel;
        rt[i].GetFileList(closef|openf|oldself, names, indx, sel);
        cnt = 0;
        cnt_sel = 0;
        for(int ii=names.GetCount()-1; ii>=0; ii-- )
        {
          // select only DB.default files
          if( names[ii].find( pVisor->sysDBDir())== gstring::npos )
              continue;
          // get 2 colums
          pos1 = names[ii].find_first_of(" ");
          pos2 = names[ii].rfind("/");
          files_data.flKeywds.Add( names[ii].substr( 0, pos1 ) );
          files_data.flNames.Add( names[ii].substr( pos2+1 ).c_str() );
          cnt++;
          ind = files_data.flKeywds.GetCount()-1;
          if( i == RT_SDATA || i == RT_CONST ||
              isOpenFile( files_data.flNames[ind] ) == 1 )
          {
            files_data.selKeywds.Add( files_data.flKeywds[ind] );
            cnt_sel++;
          }
        }
        files_data.flCnt.Add( cnt );
        files_data.selCnt.Add( cnt_sel );
    }

}

void
ElementsDialog::resetFilesSelection()
{
  TCStringArray newSelKeywds;   // list of selected files
  TCIntArray    newSelCnt;      // count of selected files  for type
   int cnt=0;
   int cnt2=0;

 //files_data

   for(uint i=0; i<files_data.flCnt.GetCount(); i++ )
    {
        int cnt_sel = 0;
        for(int ii=0; ii<files_data.flCnt[i]; ii++ )
        {
          switch( isOpenFile( files_data.flNames[cnt+ii] ) )
          {
            case 2: break;
            case 1: newSelKeywds.Add( files_data.flKeywds[cnt+ii] );
                    cnt_sel++;
                    break;
            case 0: for(int jj=0; jj<files_data.selCnt[i]; jj++ )
                      if(  files_data.flKeywds[cnt+ii] ==
                           files_data.selKeywds[cnt2+jj] )
                      {
                        newSelKeywds.Add( files_data.flKeywds[cnt+ii] );
                        cnt_sel++;
                      }
                    break;
           }
          }
        cnt += files_data.flCnt[i];
        cnt2 += files_data.selCnt[i];
       newSelCnt.Add( cnt_sel );
    }

    files_data.selKeywds.Clear();
    files_data.selCnt.Clear();
    for(uint ii=0; ii<newSelCnt.GetCount(); ii++ )
     files_data.selCnt.Add( newSelCnt[ii] );
    for(uint ii=0; ii<newSelKeywds.GetCount(); ii++ )
     files_data.selKeywds.Add( newSelKeywds[ii] );
}

void
ElementsDialog::openFilesSelection()
{
  TCStringArray newSelKeywds;   // list of selected files
  int cnt=0;
  ErrorIf( files_data.selCnt.GetCount()!=RT_PHASE+1, "", "internal error");

 //files_data
   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
   {
     newSelKeywds.Clear();

     for(int ii=0; ii<files_data.selCnt[i]; ii++ )
      newSelKeywds.Add(files_data.selKeywds[cnt+ii]);
     cnt += files_data.selCnt[i];
     // add project files keywds
     rt[i].GetProfileFileKeywds( prf_name.c_str(), newSelKeywds );
     //open all files
     rt[i].SetNewOpenFileList( newSelKeywds );
    }
}

void
ElementsDialog::openFilesICOMP()
{
  TCStringArray newSelKeywds;   // list of selected files
  int cnt=0;

 //files_data
   for(int i=RT_SDATA; i<=RT_PHASE; i++ )
   {
     if( i == RT_ICOMP )
     {  newSelKeywds.Clear();
        for(int ii=0; ii<files_data.selCnt[i]; ii++ )
           newSelKeywds.Add(files_data.selKeywds[cnt+ii]);
        rt[i].SetNewOpenFileList( newSelKeywds );
        break;
     }
     cnt += files_data.selCnt[i];
   }
}


// --------------------- End ElementsDialog.cpp -------------------------

