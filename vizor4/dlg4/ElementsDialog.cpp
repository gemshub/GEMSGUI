//-------------------------------------------------------------------
// $Id: ElementsDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ElementsDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
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

#include <qlabel.h>

#include "ElementsDialog.h"
#include "SetFiltersDialog.h"
#include <qbuttongroup.h>
#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "m_icomp.h"
#include "m_param.h"

ElementsDialog::ElementsDialog(QWidget* win, const char * prfName,
           const char* /*caption*/):
        QDialog( win ),
        prf_name ( prfName )
{
	setupUi(this);

    gstring str =
          "Selection of Independent Components into Modelling Project   ";
         str +=  gstring(rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));;
         setWindowTitle( trUtf8(str.c_str()) );

//    pLogoImg->setPixmap(pVisorImp->getLogo());

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

// set up buttin groups
    bgElem = new QButtonGroup( bgElemBox);
    bgElem->setExclusive(false);
    bgElem->addButton(pbE_0, 0);
    bgElem->addButton(pbE_1, 1);
    bgElem->addButton(pbE_2, 2);
    bgElem->addButton(pbE_3, 3);
    bgElem->addButton(pbE_4, 4);
    bgElem->addButton(pbE_5, 5);
    bgElem->addButton(pbE_6, 6);
    bgElem->addButton(pbE_7, 7);
    bgElem->addButton(pbE_8, 8);
    bgElem->addButton(pbE_9, 9);
    bgElem->addButton(pbE_10, 10);
    bgElem->addButton(pbE_11, 11);
    bgElem->addButton(pbE_12, 12);
    bgElem->addButton(pbE_13, 13);
    bgElem->addButton(pbE_14, 14);
    bgElem->addButton(pbE_15, 15);
    bgElem->addButton(pbE_16, 16);
    bgElem->addButton(pbE_17, 17);
    bgElem->addButton(pbE_18, 18);
    bgElem->addButton(pbE_19, 19);
    bgElem->addButton(pbE_20, 20);
    bgElem->addButton(pbE_21, 21);
    bgElem->addButton(pbE_22, 22);
    bgElem->addButton(pbE_23, 23);
    bgElem->addButton(pbE_24, 24);
    bgElem->addButton(pbE_25, 25);
    bgElem->addButton(pbE_26, 26);
    bgElem->addButton(pbE_27, 27);
    bgElem->addButton(pbE_28, 28);
    bgElem->addButton(pbE_29, 29);
    bgElem->addButton(pbE_30, 30);
    bgElem->addButton(pbE_31, 31);
    bgElem->addButton(pbE_32, 32);
    bgElem->addButton(pbE_33, 33);
    bgElem->addButton(pbE_34, 34);
    bgElem->addButton(pbE_35, 35);
    bgElem->addButton(pbE_36, 36);
    bgElem->addButton(pbE_37, 37);
    bgElem->addButton(pbE_38, 38);
    bgElem->addButton(pbE_39, 39);
    bgElem->addButton(pbE_40, 40);
    bgElem->addButton(pbE_41, 41);
    bgElem->addButton(pbE_42, 42);
    bgElem->addButton(pbE_43, 43);
    bgElem->addButton(pbE_44, 44);
    bgElem->addButton(pbE_45, 45);
    bgElem->addButton(pbE_46, 46);
    bgElem->addButton(pbE_47, 47);
    bgElem->addButton(pbE_48, 48);
    bgElem->addButton(pbE_49, 49);
    bgElem->addButton(pbE_50, 50);
    bgElem->addButton(pbE_51, 51);
    bgElem->addButton(pbE_52, 52);
    bgElem->addButton(pbE_53, 53);
    bgElem->addButton(pbE_54, 54);
    bgElem->addButton(pbE_55, 55);
    bgElem->addButton(pbE_56, 56);
    bgElem->addButton(pbE_57, 57);
    bgElem->addButton(pbE_58, 58);
    bgElem->addButton(pbE_59, 59);
    bgElem->addButton(pbE_60, 60);
    bgElem->addButton(pbE_61, 61);
    bgElem->addButton(pbE_62, 62);
    bgElem->addButton(pbE_63, 63);
    bgElem->addButton(pbE_64, 64);
    bgElem->addButton(pbE_65, 65);
    bgElem->addButton(pbE_66, 66);
    bgElem->addButton(pbE_67, 67);
    bgElem->addButton(pbE_68, 68);
    bgElem->addButton(pbE_69, 69);
    bgElem->addButton(pbE_70, 70);
    bgElem->addButton(pbE_71, 71);
    bgElem->addButton(pbE_72, 72);
    bgElem->addButton(pbE_73, 73);
    bgElem->addButton(pbE_74, 74);
    bgElem->addButton(pbE_75, 75);
    bgElem->addButton(pbE_76, 76);
    bgElem->addButton(pbE_77, 77);
    bgElem->addButton(pbE_78, 78);
    bgElem->addButton(pbE_79, 79);
    bgElem->addButton(pbE_80, 80);
    bgElem->addButton(pbE_81, 81);
    bgElem->addButton(pbE_82, 82);
    bgElem->addButton(pbE_83, 83);
    bgElem->addButton(pbE_84, 84);
    bgElem->addButton(pbE_85, 85);
    bgElem->addButton(pbE_86, 86);
    bgElem->addButton(pbE_87, 87);
    bgElem->addButton(pbE_88, 88);
    bgElem->addButton(pbE_89, 89);
    bgElem->addButton(pbE_90, 90);
    bgElem->addButton(pbE_91, 91);
    bgElem->addButton(pbE_92, 92);
    bgElem->addButton(pbE_93, 93);
    bgElem->addButton(pbE_94, 94);
    bgElem->addButton(pbE_95, 95);
    bgElem->addButton(pbE_96, 96);
    bgElem->addButton(pbE_97, 97);
    bgElem->addButton(pbE_98, 98);

    bgElem->addButton(pbE_100, 100);
    bgElem->addButton(pbE_101, 101);



    bgOther = new QButtonGroup( bgOtherBox);
    bgOther->setExclusive(false);
     bgOther->addButton(pbA_1, 1);
     bgOther->addButton(pbA_2, 2);
     bgOther->addButton(pbA_3, 3);
     bgOther->addButton(pbA_4, 4);
     bgOther->addButton(pbA_5, 5);
     bgOther->addButton(pbA_6, 6);
     bgOther->addButton(pbA_7, 7);
     bgOther->addButton(pbA_8, 8);
     bgOther->addButton(pbA_9, 9);
     bgOther->addButton(pbA_10, 10);
     bgOther->addButton(pbA_11, 11);
     bgOther->addButton(pbA_12, 12);
     bgOther->addButton(pbA_0, 0);

     EmptyData();
    rbKernel->setChecked( true );
    rbComplem->setChecked( false );
    rbOrganic->setChecked( false );
    rbSpecific->setChecked( false );
    cbIsotopes->setChecked( false );
    setFilesList();

    SetICompList();
    SetAqueous();

    // signals and slots connections
    connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( bNext, SIGNAL( clicked() ), this, SLOT( CmOk() ) );
    connect( bReset, SIGNAL( clicked() ), this, SLOT( CmSetFilters() ) );
//  connect( bCancel, SIGNAL( clicked() ), this, SLOT( CmCancel() ) );
//  connect( bPrevious, SIGNAL( clicked() ), this, SLOT( CmPrevious() ) );

    connect( cbGas, SIGNAL( clicked() ), this, SLOT( SetGaseous() ) );
    connect( cbAqueous, SIGNAL( clicked() ), this, SLOT( SetAqueous() ) );
    connect( cbSolids, SIGNAL( clicked() ), this, SLOT( SetSolids() ) );
    connect( cbSolutions, SIGNAL( clicked() ), this, SLOT( SetSolutions() ) );
    connect( cbSorption, SIGNAL( clicked() ),this, SLOT( SetSorption() ) );
    connect( cbIsotopes, SIGNAL( clicked() ),this, SLOT( SetFiles() ) );

    connect( rbKernel, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );
    connect( rbComplem, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );
    connect( rbOrganic, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );
    connect( rbSpecific, SIGNAL( clicked() ), this, SLOT( SetFiles() ) );

   
}

ElementsDialog::~ElementsDialog()
{}

void ElementsDialog::languageChange()
{
    retranslateUi(this);
}

void
ElementsDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_ELEMENTS_HTML );
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
       TProfil::pm->useAqPhase = el_data.flags[cbAqueous_];  // Added 16.06.03 
       TProfil::pm->useGasPhase = el_data.flags[cbGaseous_];
   accept();
}

void ElementsDialog::EmptyData()
{
 int ii;
 for( ii=0; ii<bgOther->buttons().count(); ii++ )
 {     bgOther->button(ii)->setEnabled( false );
       bgOther->button(ii)->setText( tr( "...") );
 }
 for( ii=0; ii<99/*bgElem->buttons()->count()-1*/; ii++ ) // we have not 99
      bgElem->button(ii)->setEnabled( false );

  cbGas->setChecked( true );
  cbAqueous->setChecked( true );
  cbSolids->setChecked( true );
  cbSolutions->setChecked( false );
  cbSorption->setChecked( false );
}


void ElementsDialog::ResetData()
{
 int ii;
/* for( ii=0; ii<bgOther->count(); ii++ )
  if( bgOther->find(ii)->isOn() )
      bgOther->find(ii)->toggle();
 for( ii=0; ii<99 *bgElem->count()-1*; ii++ )
  if( bgElem->find(ii)->isOn() )
      bgElem->find(ii)->toggle();
*/

 for( ii=0; ii<bgOther->buttons().count(); ii++ )
   bgOther->button(ii)->setChecked ( false );
 for( ii=1; ii<99/*bgElem->buttons()->count()-1*/; ii++ ) // 0 always
      bgElem->button(ii)->setChecked ( false );
}


bool ElementsDialog::isAqueous() const
{
 return cbAqueous->isChecked();
}

void ElementsDialog::SetAqueous()
{
 if( cbAqueous->isChecked() )
 {
   pbE_0->setChecked(true);
   pbE_1->setChecked(true);
   pbE_8->setChecked(true);
 }
}

bool ElementsDialog::isSolution() const
{
 return cbSolutions->isChecked();
}

void ElementsDialog::SetSolutions()
{
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
 return cbGas->isChecked();
}

void ElementsDialog::SetGaseous()
{
}

bool ElementsDialog::isSolids() const
{
 return cbSolids->isChecked();
}

void ElementsDialog::SetSolids()
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
  if(rbComplem->isChecked())
   names.Add(".complem.");
  if(rbOrganic->isChecked())
   names.Add(".organic.");
  if(rbSpecific->isChecked())
   names.Add(".specific.");
}

/*
// Returns; 0 no change in DB file configuration for new project
// (no kernel, specific, supplemental, or complementary selected)
// 1 to open DB file;  2 to close DB file
int ElementsDialog::isOpenFile( gstring& name )
{
   int iret=0;
   if(  name.find( ".kernel." ) != gstring::npos )
   {    if(rbKernel->isChecked())
           iret = 1;
         else
           iret = 2;
   }
   else  if(  name.find( ".supplem." ) != gstring::npos )
         {    if(rbOrganic->isChecked())
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
                       {    if(rbComplem->isChecked())
                              iret = 1;
                            else
                              iret = 2;
                       }

  return iret;
}
*/
// Returns; boolean true if a keyword was found in the file name, false otherwise
//    for each of five keywords;
// function value: 0 if no change in DB file configuration for new project
// is required;  1 to attach this DB file 'name';  2 to detach this DB file
//
int ElementsDialog::isOpenFile( gstring& name  )
{
   bool checked_kernel = rbKernel->isChecked();
   bool checked_complem = rbComplem->isChecked();
   bool checked_supplem = rbOrganic->isChecked(); // provisorial - change later
   bool checked_organic = rbOrganic->isChecked();
   bool checked_specific = rbSpecific->isChecked();
   bool is_kernel, is_complem, is_supplem, is_specific, is_organic;

   if(  name.find( ".kernel." ) != gstring::npos )
       is_kernel = true;
   else is_kernel = false;
   if(  name.find( ".complem." ) != gstring::npos )
       is_complem = true;
   else is_complem = false;
   if(  name.find( ".supplem." ) != gstring::npos )
       is_supplem = true;
   else is_supplem = false;
   if(  name.find( ".specific." ) != gstring::npos )
       is_specific = true;
   else is_specific = false;
   if(  name.find( ".organic." ) != gstring::npos )
       is_organic = true;
   else is_organic = false;
   // ......

   int iret = 0;
   if( (checked_kernel && is_kernel) || (checked_complem && is_complem) ||
           (checked_specific && is_specific) )
       iret = 1; // select all checked, disregarding 'Supplem' and 'Organic'
   if( (is_kernel && !checked_kernel) || (is_complem && !checked_complem) ||
       (is_supplem && !checked_supplem) || (is_specific && !checked_specific) )
       // || (is_organic && !checked_organic) )
       iret = 2; // This file should be detached, even if has '.supplem.',
                 // but 'Supplem' was not checked
   return iret;
}

void
ElementsDialog::SetICompList()
{
    TCIntArray aIndMT;
    TCStringArray aIC;
    QAbstractButton* bb;
    int nmbOther=1;
//    TCStringArray names;

    aBtmId1.Clear();
    aICkey1.Clear();
    aBtmId2.Clear();
    aICkey2.Clear();

    //openFiles( names );
    openFilesICOMP();
   // select all IComp keys and indMT (set indMT to -1 for additional)
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
         bb = bgOther->button(nmbOther);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
         aBtmId2.Add( nmbOther );
         aICkey2.Add( aIC[ii] );
         nmbOther ++;
       }
       else  //Vol
       {
         bb = bgOther->button(0);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( true );
         aBtmId2.Add( 0 );
         aICkey2.Add( aIC[ii] );
       }
     }
     else //elements
     {
       bb = bgElem->button(aIndMT[ii]);
       if( !bb )
        Error( aIC[ii].c_str(), "E01EDrem: Invalid IComp ");
       aBtmId1.Add( aIndMT[ii] );
       aICkey1.Add( aIC[ii] );
       bb->setEnabled( true );
     }

  // set selection form template
  for( uint ii=0; ii<aBtmId1_sel.GetCount(); ii++ )
  {
       bb = bgElem->button(aBtmId1_sel[ii]);
       bb->setEnabled( false );
       bb->setChecked( true );
  }
  
  for( uint ii=0; ii<aICkey2_sel.GetCount(); ii++ )
  {
       int jj;
       gstring name= gstring( aICkey2_sel[ii],
                      0, rt[RT_ICOMP].FldLen(0) );
       name.strip();
       for( jj=0; jj<nmbOther; jj++ )
       {
         bb =  bgOther->button(jj);
         gstring ttl = bb->text().toLatin1().data();
         if( name == ttl )
         {
          bb->setEnabled( false );
          bb->setChecked( true );
          break;
         }
       }
       if( jj==nmbOther )
       {
         bb = bgOther->button(nmbOther);
         bb->setText( tr( name.c_str() ) );
         bb->setEnabled( false );
         bb->setChecked( true );
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
     if( bgElem->button( aBtmId1[ii] )->isChecked()  )
        aICkeys.Add( aICkey1[ii] );
    }

    for( ii=0; ii<aBtmId2.GetCount(); ii++ )
    {
     if( bgOther->button( aBtmId2[ii] )->isChecked()  )
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
  el_data.flags[cbSolids_] = cbSolids->isChecked();
 el_data.flags[cbGaseous_] =  cbGas->isChecked();
  el_data.flags[cbSolutions_] =  cbSolutions->isChecked();
 el_data.flags[cbSorption_] = cbSorption->isChecked();
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
  TCIntArray    newSelCnt;      // count of newly selected files for type
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
            case 2: // 2 to close DB file
                    break;
            case 1: // 1 to open DB file
                    newSelKeywds.Add( files_data.flKeywds[cnt+ii] );
                    cnt_sel++;
                    break;
            case 0: // 0 no change in DB file configuration for new project
                    for(int jj=0; jj<files_data.selCnt[i]; jj++ )
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
  ErrorIf( files_data.selCnt.GetCount()!=RT_PHASE+1, "",
               "E00EDrem: internal error");

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

