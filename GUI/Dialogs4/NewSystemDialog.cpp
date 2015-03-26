//-------------------------------------------------------------------
// $Id: NewSystemDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of NewSystemDialog class
//
// Copyright (C) 1996-2008 S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <cmath>

#include <qstatusbar.h>
#include <qapplication.h>
#include <QEvent>
#include <QCloseEvent>

#include "NewSystemDialog.h"
#include "GemsMainWindow.h"
#include "m_syseq.h"
#include "tmltsystem.h"
#include "service.h"

NewSystemDialog* NewSystemDialog::pDia = 0;

NewSystemDialog::NewSystemDialog(QWidget* parent, const char* /*name*/):
	QMainWindow( parent )
{
	setupUi(this);
        string titl; // = pVisorImp->getGEMTitle();
            titl = "EqStat:: Single Thermodynamic System in Project ";
            titl+= rt[RT_PARAM].FldKey(0);
            setWindowTitle( trUtf8(titl.c_str()) );

    pDia = this;

// define new TTreeView window Input: System Definition
   defineInputList();
    
// define new TTreeView window Results: Equilibrium State
   ListViewResult = 0;
   //defineResultList();

    //--setActions();
    Update();

    // moved    Create, if no syseq is present in the project
    //if( rt[RT_SYSEQ].RecCount() <= 0)
      //  CmCreate();
}

NewSystemDialog::~NewSystemDialog()
{}

void NewSystemDialog::languageChange()
{
    retranslateUi(this);
}

QSize NewSystemDialog::sizeHint() const
{
    return QSize(aWinInfo[RT_SYSEQ]->init_width,
                 aWinInfo[RT_SYSEQ]->init_height);
}
void NewSystemDialog::objectChanged()
{
	TSysEq::pm->CellChanged();
}

// define new TTreeView window Input: System Definition
void NewSystemDialog::defineInputList()
{
    TTreeModel* model;
    TTreeDelegate* deleg;
    QList<FieldInfo>	afldsPh;
    QList<FieldInfo>	afldsDC;
    
// define columns of list	
    afldsPh.clear();
    afldsPh.append(FieldInfo( o_musf, ftRecord, 30, false, First, eNo, stHelp, 1, 1));
    afldsPh.append(FieldInfo( o_mul1, ftNumeric, 4, false, NextP, eNo, stHelp, 1, 1));
    afldsPh.append(FieldInfo( o_muphc,ftCheckBox, 8, false,	NextP, eNo, stHelp, 1, 1));
    afldsPh.append(FieldInfo( o_sypcl, ftCheckBox, 1, false, NextP, eYes, stIO, 1, 1));
    afldsPh.append(FieldInfo( o_syxpun, ftCheckBox, 5, false, NextP, eYes, stIO, 1, 1));
    afldsPh.append(FieldInfo( o_syphm, ftFloat, 10, false, NextP, eYes, stIO, 1, 1));
    afldsPh.append(FieldInfo( -1, ftCheckBox, 7, false,	NextP, eYes, stIO, 1, 1));
    afldsPh.append(FieldInfo( o_syyof, ftFloat, 10, false, NextP, eYes, stIO, 1, 1));
    afldsPh.append(FieldInfo( -1, ftCheckBox, 4, false,	NextP, eNo, stIO, 1, 1));
    afldsPh.append(FieldInfo( -1, ftFloat, 10, false, NextP, eNo, stIO, 1, 1));
    afldsPh.append(FieldInfo( -1, ftFloat, 10, false, NextP, eNo, stIO, 1, 1));
    afldsPh.append(FieldInfo( -1, ftCheckBox, 3, false,	NextP, eNo, stIO, 1, 1)); 
      
    afldsDC.clear();
    afldsDC.append(FieldInfo( o_musm, ftRecord, 30, false, First, eNo, stHelp, 1, 1));
    afldsDC.append(FieldInfo( -1, ftNumeric, 4, false, NextP, eNo, stHelp, 1, 1));
    afldsDC.append(FieldInfo( o_mudcc,ftCheckBox, 9, false,	NextP, eNo, stHelp, 1, 1));
    afldsDC.append(FieldInfo( o_sydcl, ftCheckBox, 1, false, NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( o_syxdun, ftCheckBox, 5, false, NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( o_syxed, ftFloat, 10, false, NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( -1, ftCheckBox, 7, false,	NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( o_sygex, ftFloat, 10, false, NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( o_syrsc, ftCheckBox, 4, false, NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( o_sydll, ftFloat, 10, false, NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( o_sydul, ftFloat, 10, false, NextP, eYes, stIO, 1, 1));
    afldsDC.append(FieldInfo( o_syrlc, ftCheckBox, 3, false, NextP, eYes, stIO, 1, 1)); 
     
    QStringList header;
    // header.append( tr("Phase/species name"));
    header.append( tr("Phase/species"));        // Ok
    header.append(  tr("L"));
    header.append(  tr("Type"));
    header.append(  tr("On/off"));
    header.append(  tr("UC"));
    header.append(  tr("Add to BC"));
    header.append(  tr("UG"));
    header.append(  tr("G0 corr."));
    header.append(  tr("UK"));
    header.append(  tr("Lower_KC"));
    header.append(  tr("Upper_KC"));
    header.append(  tr("KC type"));

    model = new TTreeModel( afldsPh, afldsDC, header, this /*tab*/ );
    deleg = new TTreeDelegate( this/*tab*/ );

    ListViewInput = new TTreeView(tab);
    ListViewInput->setObjectName(QString::fromUtf8("ListViewInput"));
    ListViewInput->setItemDelegate(deleg);
    ListViewInput->setModel(model);
    
    for(int ii=0; ii<afldsDC.count(); ii++ )
    	ListViewInput->setColumnWidth( ii, wdF(afldsDC[ii].fType, afldsDC[ii].npos, afldsDC[ii].edit) );

    ListViewInput->setAllColumnsShowFocus(false);
    vboxLayout1->addWidget(ListViewInput);

// end of setup ListView1
}

// define new TTreeView window Results: Equilibrium State
void NewSystemDialog::defineResultList()
{
    TTreeModel* model;
    TTreeDelegate* deleg;
    QList<FieldInfo>	afldsPh;
    QList<FieldInfo>	afldsDC;
    
// define columns of list	
    afldsPh.clear();
    afldsPh.append(FieldInfo( o_wd_sf, ftRecord, 30, false, First, eNo, stHelp, 1, 1));
    afldsPh.append(FieldInfo( o_wi_l1, ftNumeric, 4, false, NextP, eNo, stHelp, 1, 1));
    afldsPh.append(FieldInfo( o_wi_phc,ftCheckBox, 8, false,	NextP, eNo, stHelp, 1, 1));
    afldsPh.append(FieldInfo( o_w_xf, ftFloat, 16, false, NextP, eNo, stResult, 1, 1));
    afldsPh.append(FieldInfo( o_wo_falp, ftFloat, 17, false, NextP, eNo, stResult, 1, 1));
    // afldsPh.append(FieldInfo( -1, ftFloat, 17, false, NextP, eNo, stResult, 1, 1)); // Disagree DK
    afldsPh.append(FieldInfo( -1, ftFloat, 16, false, NextP, eNo, stResult, 1, 1));
    afldsPh.append(FieldInfo( -1, ftFloat, 16, false, NextP, eNo, stResult, 1, 1));
      
    afldsDC.clear();
    afldsDC.append(FieldInfo( o_wd_sm, ftRecord, 30, false, First, eNo, stHelp, 1, 1));
    afldsDC.append(FieldInfo( -1, ftNumeric, 4, false, NextP, eNo, stHelp, 1, 1));
    afldsDC.append(FieldInfo( o_wi_dcc,ftCheckBox, 9, false,	NextP, eNo, stHelp, 1, 1));
    afldsDC.append(FieldInfo( o_w_x, ftFloat, 16, false, NextP, eNo, stResult, 1, 1));
    afldsDC.append(FieldInfo( o_wd_yla, ftFloat, 17, false, NextP, eNo, stResult, 1, 1));
    afldsDC.append(FieldInfo( o_wo_wx, ftFloat, 16, false, NextP, eNo, stResult, 1, 1));
    // afldsDC.append(FieldInfo( o_wo_lngam, ftFloat, 16, false, NextP, eNo, stResult, 1, 1));
    afldsDC.append(FieldInfo( o_wd_gamma, ftFloat, 16, false, NextP, eNo, stResult, 1, 1)); // corrected 16.04.2010 (TW)
     
    QStringList header;
    // header.append( tr("Name of phase/species"));
    header.append( tr("Phase/species"));        // Ok
    header.append(  tr("L"));
    header.append(  tr("Type"));
    // header.append(  tr("Amount n(x),mol"));  // Ok
    header.append(  tr("Amount (mol)"));
    header.append(  tr("logSI/Activity"));
    // header.append(  tr("Activity (DualTh)"));  // Disagree DK
    header.append(  tr("Concentration"));
    header.append(  tr("Activity coeff."));

    model = new TTreeModel( afldsPh, afldsDC, header, this /*tab_2*/ );
    deleg = new TTreeDelegate( this /*tab_2*/ );

    ListViewResult = new TTreeView(tab);
    ListViewResult->setObjectName(QString::fromUtf8("ListViewResult"));
    ListViewResult->setItemDelegate(deleg);
    ListViewResult->setModel(model);
    
    for(int ii=0; ii<afldsDC.count(); ii++ )
    	ListViewResult->setColumnWidth( ii, wdF(afldsDC[ii].fType, afldsDC[ii].npos, afldsDC[ii].edit) );

    ListViewResult->setAllColumnsShowFocus(false);
    vboxLayout2->addWidget(ListViewResult);

// end of setup ListView2
}

void NewSystemDialog::Update()
{
    MULTIBASE* pData = TMultiSystem::sm->pmp;

   if( !ListViewResult  && aObj[ o_wi_l1 ].GetPtr() )
     defineResultList(); 
    //pLine->setText(tr (rt[RT_SYSEQ].PackKey()));
    //toolBar_5->update();
    QString msg  = tr ("    ");
            msg += QString("System:  T = %1 K;").arg(pData->Tc, 7, 'f', 2);
            msg += QString("  P = %1 bar;").arg(pData->Pc, 8, 'f', 2);
            msg += QString("  V = %1 L;").arg(pData->VXc/1000., 9, 'g', 4);
    if( pData->PHC )
    {
       if( pData->PHC[0] == (char)PH_AQUEL )
       {
          char* sMod;
          msg += QString("  Aqueous:");
          sMod = pData->sMod[0];
          switch( sMod[0 /*SPHAS_TYP*/ ] )
          {
            case SM_AQDAV: // Davies
                      msg += QString(" built-in Davies;");
                      break;
            case SM_AQDH1: // DH LL
                      msg += QString(" built-in DH1;");
                      break;
            case SM_AQDH2: // DH Kielland
                      msg += QString(" built-in DH2;");
                      break;
            case SM_AQDH3: // EDH Karpov
                      msg += QString(" built-in EDH(K);");
                      break;
            case SM_AQDHH: // EDH Helgeson
                      msg += QString(" built-in EDH(H);");
                      break;
            case SM_AQDHS: // EDH Shvarov
                      msg += QString(" built-in EDH(S);");
                      break;
            case SM_AQSIT: // SIT PSI (under construction)
                      msg += QString(" built-in SIT;");
                      break;
            default:  // Other (user-def) aqueous models
                      msg += QString(" user-defined;");
                      break;
          }
            msg += QString("  pH = %1;").arg(pData->pH, 6, 'f', 3);
            msg += QString("  pe = %1;").arg(pData->pe, 7, 'f', 3);
            msg += QString("  IS = %1 m").arg(pData->IC, 6, 'f', 3);
       }
    }
    statusBar()->showMessage( msg );

    //    last_update = time(0);
    if(ListViewResult)
      ListViewResult->resetList();
    ListViewInput->resetList();
}

void NewSystemDialog::closeEvent(QCloseEvent* ev)
{
    if( !(windowState() & Qt::WindowMaximized) )
    {
        aWinInfo[RT_SYSEQ]->init_width = parentWidget()->width();
        aWinInfo[RT_SYSEQ]->init_height = parentWidget()->height();
    }

    // close module
    if( MessageToSave() )
    {
       pDia = 0;
       ev->accept();
       pVisorImp->closeMdiChild( this );
     } else {
         ev->ignore();
     }
}

void NewSystemDialog::printResultList( fstream& f )
{
	ListViewResult->printList( f );
}

void NewSystemDialog::printInputList( fstream& f )
{
	ListViewInput->printList( f );
}

//---------------------------------------------------------------------
// From NewSystemMenu

// returns true if user pressed 'save' or 'discard' and false on 'cancel'
bool NewSystemDialog::MessageToSave()
{
    string key_str = rt[RT_SYSEQ].PackKey();
    if( TSysEq::pm->isCellChanged() && key_str.find_first_of("*?") == string::npos )
    {
     int res = vfQuestion3(this, key_str.c_str(),
                       "Data record has been changed!",
                       "Save changes", "Discard changes", "Cancel");
         if( res == VF3_3 )
             return false;

        if( res == VF3_1 )
        CmSave();
    }
    TSysEq::pm->CellChanged( false );
    return true;
}

void NewSystemDialog::CmHelp2()
{
    if( TabWid->currentIndex() == 0 )
       pVisorImp->OpenHelp( GEMS_BCC_HTML, "TAB_INPUT");
    else
       pVisorImp->OpenHelp( GEMS_ONESYS_HTML, "TAB_RESULTS" );
}

void NewSystemDialog::CmOutMulti()
{
    try
    {
        if( !MessageToSave() )
               return;
        TProfil::pm->makeGEM2MTFiles( this );
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmReadMulti()
{
    try
    {
        if( !MessageToSave() )
               return;

 // Clone current record
 //       TProfil::pm->newSystat( VF_BYPASS );
        // Clear syp->XeD, syp->XeA, syp->Phm
         TProfil::pm->Clear_XeA_XeD_Phm_BIun();

        //ListViewResult->resetList();
        //Update();

        // open file for input
        string filename;
        if( vfChooseFileOpen(this, filename,
          "Browse for GEMS3K *.lst file to import ", "*.lst" ) == false )
               return;
        TProfil::pm->CmReadMulti( this, filename.c_str() );

        // Set SysEq record key as read
        //rt[RT_SYSEQ].SetKey( TMulti::sm->GetPM()->stkey );

        // Clone current record   Moved here by DK on 10.04.2012
        //TProfil::pm->newSystat( VF_BYPASS );

        //ListViewResult->resetList();
        Update();
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//Calculate actions

void NewSystemDialog::CmRunBCC()
{
 try
  {
   TProfil::pm->CalcBcc(); // Calc bulk composition
   //ListViewResult->resetList();
   Update();
  }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Print actions

void NewSystemDialog::CmPrintMtparam()
{
    try
    {
        if( !MessageToSave() )
               return;
        TProfil::pm->RecordPrint( "pscript*:*:mtparm*:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmPrintSystem()
{   // Only user-spec. constraints
    try
    {
        if( !MessageToSave() )
                 return;
        TSysEq::pm->RecordPrint( "pscript*:*:system*:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmPrintEqstat()
{                                // Only for non-zero DC, PH
    try
    {
        if( !MessageToSave() )
                return;
        TSysEq::pm->RecordPrint( "pscript*:*:multi*:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmPrResults()
{
    try
    {
      if( !MessageToSave() )
              return;
      // open file to output
      string filename;
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename, "Fileopen error");

      printResultList(f);
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}


void NewSystemDialog::CmPrInput()
{
    try
    {
      if( !MessageToSave() )
               return;
        // open file to output
      string filename;
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename, "Fileopen error");

      printInputList(f);
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//--------------------------------------------------------------------
// Database SysEq record actions
void NewSystemDialog::CmCreate()
{
 try
 {
     if( !MessageToSave() )
            return;

     if( ListViewResult  )
     {    delete ListViewResult;
          ListViewResult = 0;
     }

     TProfil::pm->newSystat( VF_CLEARALL );
      // Create...
     Update();
 }
    catch( TError& xcpt )
    {
       vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmNew()
{
 try
 {
   if( !MessageToSave() )
            return;

   TProfil::pm->newSystat( VF_BYPASS );
   //ListViewResult->resetList();
      // Create...
   Update();
 }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmSelect( const char *key )
{
    if( rt[RT_SYSEQ].RecCount() <= 0)
      return;   // Added to avoid a pitfall at creating new project
    try
    {
       if( !MessageToSave() )
           return;

      TProfil::pm->loadSystat( key );
      //ListViewResult->resetList();
      Update();
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmRemake()
{
  try
  {
      if( !MessageToSave() )
            return;

    TProfil::pm->deriveSystat();
    //ListViewResult->resetList();
    Update();
 }
 catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmSave()
{
  try
  {
    if( TSysEq::pm->ifCalcFlag()== false )
    {
      string key_s = rt[RT_SYSEQ].PackKey();
      if( !rt[RT_SYSEQ].Find( key_s.c_str()  ) ) // FindCurrent
      { vfMessage( this, key_s.c_str(),
        "Please, calculate the equilibrium state before saving this record!");
            return;
      }
    }
    TProfil::pm->SyTestSizes();
    TSysEq::pm->CmSaveM();
  }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmSaveAs()
{
  try
  {
    TProfil::pm->SyTestSizes();
    TSysEq::pm->CmSaveAs();
  }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void NewSystemDialog::CmDelete()
{
    TSysEq::pm->CmDelete();            // Delete
}
/*
void NewSystemDialog::CmNext()
{
        if( rt[RT_SYSEQ].RecCount() <= 0 )
      return;    // Added to avoid a pitfall at creating new project
    try
    {
        if( !MessageToSave() )
             return;

        int i_next = 0;
       // get current record key
       string str=rt[RT_SYSEQ].UnpackKey();
       // select scroll list
       vector<string> aKey;
       //TCIntArray anR;
       int Nrec = rt[RT_SYSEQ].GetKeyList(
                   TSysEq::pm->getFilter(), aKey );
       if( Nrec <= 0 )
           return; // no records to scroll
       // get current record key
       if( !(str.find_first_of("*?" ) != string::npos) )
          //Current record key is defined!
       {
         for(int i=0; i<aKey.size(); i++ )
          if( str == aKey[i])
            {
              i_next = i+1;
              if( i_next >=  Nrec ) i_next--;
              break;
            }
        }
       TProfil::pm->loadSystat( aKey[i_next].c_str() );
       //ListViewResult->resetList();
       Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Show previous record from template list
void NewSystemDialog::CmPrevious()
{

    if( rt[RT_SYSEQ].RecCount() <= 0)
      return; // Added to avoid a pitfall at creating new project
    try
    {
        if( !MessageToSave() )
           return;

        int i_next = 0;
       // get current record key
       string str=rt[RT_SYSEQ].UnpackKey();
       // select scroll list
       vector<string> aKey;
       int Nrec = rt[RT_SYSEQ].GetKeyList(
            TSysEq::pm->getFilter(), aKey );
       if( Nrec <= 0 )
           return; // no records to scroll
       // get current record key
       if( !(str.find_first_of("*?" ) != string::npos) )
          //Current record key is defined!
       {
         for(int i=0; i<aKey.size(); i++ )
          if( str == aKey[i])
            {
             i_next = i-1;
              if( i_next <  0 ) i_next++;
              break;
             }
        }
       TProfil::pm->loadSystat( aKey[i_next].c_str() );
       //ListViewResult->resetList();
       Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}
*/
//-------- End of file NewSystemDialog.cpp ----------------------------


