//-------------------------------------------------------------------
// $Id: NewSystemMenu.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of NewSystemDialog class
//
// Copyright (C) 1996-2008 S.Dmytriyeva
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
const char *GEMS_SYS_HTML = "gems_sys";
const char *GEMS_BCC_HTML = "gems_bcc";
const char *GEMS_IPM_HTML = "gems_ipm";
const char *GEMS_HOWTO_HTML = "ge_howto";

#include <math.h>

#include <qstatusbar.h>
#include <qapplication.h>
#include <QToolBar>
#include <QEvent>

#include "NewSystemDialog.h"
#include "ProgressDialog.h"
#include "v_mod.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "m_param.h"
#include "m_syseq.h"
#include "gdatastream.h"

static bool noCommit;

void
NewSystemDialog::setActions()
{
	menu = menuBar();
	noCommit = false;
	
	connect( actionCreate, SIGNAL( triggered()), this, SLOT(CmCreate()));
    connect( actionNew, SIGNAL( triggered()), this, SLOT(CmNew()));
    connect( actionDisplay, SIGNAL( triggered()), this, SLOT(CmSelect()));
    connect( actionRemake, SIGNAL( triggered()), this, SLOT(CmRemake()));
    connect( actionSave, SIGNAL( triggered()), this, SLOT(CmSave()));
    connect( actionSave_As, SIGNAL( triggered()), this, SLOT(CmSaveAs()));
    connect( actionDelete, SIGNAL( triggered()), this, SLOT(CmDelete()));
    connect( actionClose, SIGNAL( triggered()), this, SLOT(close()));
   
 	connect( actionOpenMTPARAM, SIGNAL( triggered()), this, SLOT(CmOpen_MTPARAM()));
    connect( actionOpen_SYSTEM_ICOMP, SIGNAL( triggered()), this, SLOT(CmOpen_SYSTEM_ICOMP()));
    connect( actionINSERT_SYSTEM, SIGNAL( triggered()), this, SLOT(CmInsert_SYSTEM()));
//    connect( actionCommitISD, SIGNAL( triggered()), this, SLOT(CmCommit()));
//    connect( actionResetISD, SIGNAL( triggered()), this, SLOT(CmReset()));
    connect( actionOutMulti, SIGNAL( triggered()), this, SLOT(CmOutMulti()));
    connect( actionReadMulti, SIGNAL( triggered()), this, SLOT(CmReadMulti()));
    
    connect( action_BCC, SIGNAL( triggered()), this, SLOT(CmRunBCC()));
    connect( action_IPM, SIGNAL( triggered()), this, SLOT(CmRunIPM()));
    connect( actionSimplex, SIGNAL( triggered()), this, SLOT(CmSimplex()));
    connect( actionPrecise, SIGNAL( triggered()), this, SLOT(CmPrecise()));
    connect( actionStepwise, SIGNAL( triggered()), this, SLOT(CmStepwise()));

    connect( actionEQUSTAT_ICOMP, SIGNAL( triggered()), this, SLOT(CmOpen_EQSTAT_ICOMP()));
    connect( actionOpenMULTI, SIGNAL( triggered()), this, SLOT(CmOpen_MULTI()));

    connect( actionPrintMtparm, SIGNAL( triggered()), this, SLOT(CmPrintMtparam()));
    connect( actionPrintSystem, SIGNAL( triggered()), this, SLOT(CmPrintSystem()));
    connect( actionPrintEqstat, SIGNAL( triggered()), this, SLOT(CmPrintEqstat()));
    connect( actionPrInput, SIGNAL( triggered()), this, SLOT(CmPrInput()));
    connect( actionPrResults, SIGNAL( triggered()), this, SLOT(CmPrResults()));

    connect( actionHelp, SIGNAL( triggered()), this, SLOT(CmHelp()));
    connect( actionHowto, SIGNAL( triggered()), this, SLOT(CmHowto()));
    connect( actionScript, SIGNAL( triggered()), this, SLOT(CmScript()));
    connect( actionMoreBCC, SIGNAL( triggered()), this, SLOT(CmMoreBCC()));
    connect( actionMoreIPM, SIGNAL( triggered()), this, SLOT(CmMoreIPM()));
    connect( actionAbout, SIGNAL( triggered()), pVisorImp, SLOT(CmHelpAbout()));
   
    connect( actionPrevious, SIGNAL( triggered()), this, SLOT(CmPrevious()));
    connect( actionNext, SIGNAL( triggered()), this, SLOT(CmNext()));

    connect( TabWid, SIGNAL( currentChanged ( int ) ), this, SLOT(Update()));
    actionPrecise->setChecked(TProfil::pm->pa.p.PRD);

 //
   pLine = new QLineEdit( toolBar_5 );
   pLine->setEnabled( TRUE );
   pLine->setFocusPolicy( Qt::ClickFocus );
   pLine->setReadOnly( TRUE );
   pLine->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   toolBar_5->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   toolBar_5->addWidget( pLine ); // setStretchableWidget( pLine );
}

// returns true if user pressed 'save' or 'discard' and false on 'cancel'
bool NewSystemDialog::MessageToSave()
{

    gstring key_str = rt[RT_SYSEQ].PackKey();
    if( TSysEq::pm->isCellChanged() && key_str.find_first_of("*?") == gstring::npos )
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

// System Actions
//----------------------------------------------------------------------------------------
// Database SysEq record actions
void NewSystemDialog::CmCreate()
{
 try
 {
     if( !MessageToSave() )
	    return;
	 
	 // noCommit = true;
     if( ListViewResult  )
     {    delete ListViewResult; 
          ListViewResult = 0;
     }     
     TProfil::pm->newSystat( VF_CLEARALL );
      // Create...
     Update();
   // noCommit = false;
 }
    catch( TError& xcpt )
    {
      noCommit = false;
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmNew()
{
 try
 {
   if( !MessageToSave() )
	    return;
   
   TProfil::pm->newSystat( VF_BYPASS );
   ListViewResult->resetList();
      // Create...
   Update();
 }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmSelect()
{
    if( rt[RT_SYSEQ].RecCount() <= 0)
      return;   // Added to avoid a pitfall at creating new project
    try
    {
       if( !MessageToSave() )
           return;
    	
      TProfil::pm->loadSystat();
      ListViewResult->resetList();
      Update();
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmRemake()
{
  try
  {
      if( !MessageToSave() )
	    return;
	  
    TProfil::pm->deriveSystat();
    ListViewResult->resetList();
    Update();
   //  pVisor->OpenModule(this, MD_SYSTEM);
 }
 catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmSave()
{
  try
  {
    if( TSysEq::pm->ifCalcFlag()== false )
    {
      gstring key_s = rt[RT_SYSEQ].PackKey();
      if( rt[RT_SYSEQ].Find( key_s.c_str()  ) < 0)
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

void
NewSystemDialog::CmSaveAs()
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

void
NewSystemDialog::CmDelete()
{
    TSysEq::pm->CmDelete();            // Delete
}

void
NewSystemDialog::CmNext()
{
	if( rt[RT_SYSEQ].RecCount() <= 0 )
      return;    // Added to avoid a pitfall at creating new project
    try
    {
        if( !MessageToSave() )
             return;

        int i_next = 0;
       // get current record key
       gstring str=rt[RT_SYSEQ].UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       int Nrec = rt[RT_SYSEQ].GetKeyList(
            TSysEq::pm->getFilter(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       // get current record key
       if( !(str.find_first_of("*?" ) != gstring::npos) )
          //Current record key is defined!
       {
         for(uint i=0; i<aKey.GetCount(); i++ )
          if( str == aKey[i])
            {
              i_next = i+1;
              if( i_next >=  Nrec ) i_next--;
              break;
            }
        }
       TProfil::pm->loadSystat( aKey[i_next].c_str() );
       ListViewResult->resetList();
       Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Show previous record from template list
void
NewSystemDialog::CmPrevious()
{
    
    if( rt[RT_SYSEQ].RecCount() <= 0)
      return; // Added to avoid a pitfall at creating new project
    try
    {
        if( !MessageToSave() )
           return;
    	
    	int i_next = 0;
       // get current record key
       gstring str=rt[RT_SYSEQ].UnpackKey();
       // select scroll list
       TCStringArray aKey;
       TCIntArray anR;
       int Nrec = rt[RT_SYSEQ].GetKeyList(
            TSysEq::pm->getFilter(), aKey, anR );
       if( Nrec <= 0 )
           return; // no records to scroll
       // get current record key
       if( !(str.find_first_of("*?" ) != gstring::npos) )
          //Current record key is defined!
       {
         for(uint i=0; i<aKey.GetCount(); i++ )
          if( str == aKey[i])
            {
             i_next = i-1;
              if( i_next <  0 ) i_next++;
              break;
             }
        }
       TProfil::pm->loadSystat( aKey[i_next].c_str() );
       ListViewResult->resetList();
       Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------------------------------------
//Data insert actions 
void                                 // Thermodynamic data
NewSystemDialog::CmOpen_MTPARAM()
{
//    SetString(" OpenModule" );
    pVisorImp->OpenModule(this, MD_MTPARM);
}

void                                 // Bulk composition (b)
NewSystemDialog::CmOpen_SYSTEM_ICOMP()
{
    pVisorImp->OpenModule(this, MD_SYSTEM, 0); // ICOMP page
}

void                                 // Bulk composition (b)
NewSystemDialog::CmInsert_SYSTEM()
{
    gstring key_s = rt[RT_SYSEQ].PackKey();
    TProfil::pm->systbcInput( this, key_s.c_str() );
    pVisor->Update();
   // pVisor->OpenModule(this, MD_SYSTEM);
}

void
NewSystemDialog::CmOutMulti()
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

void
NewSystemDialog::CmReadMulti()
{
    try
    {
        if( !MessageToSave() )
	       return;
    	    	
        // 14/12/2009 SD
        // Clone current record
        TProfil::pm->newSystat( VF_BYPASS );
        // Clear syp->XeD, syp->XeA, syp->Phm and copy pmp->B to syp->BI
        TProfil::pm->Clear_XeA_XeD_Phm_BIun();
        //Copy pmp->B to syp->BI
        for( int i=0; i<TProfil::pm->pmp->N; i++ )
         TProfil::pm->syp->BI[TProfil::pm->pmp->mui[i]] = TProfil::pm->pmp->B[i];

        //ListViewResult->resetList();
        //Update();

        // open file for input
        gstring filename;
        if( vfChooseFileOpen(this, filename,
          "Get *.lst file name for reading", "*.lst" ) == false )
               return;
        TProfil::pm->CmReadMulti( this, filename.c_str() );
        // Unpack the pmp->B vector (b) into syp->BI (BI_ vector).
        for( int i=0; i<TProfil::pm->pmp->N; i++ )
         TProfil::pm->syp->BI[TProfil::pm->pmp->mui[i]] = TProfil::pm->pmp->B[i];
        ListViewResult->resetList(); //
        Update();
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------------------------------------
//Calculate actions

void
NewSystemDialog::CmRunBCC()
{
 try
  {
   TProfil::pm->CalcBcc(); // Calc bulk composition
   ListViewResult->resetList();
   Update();
  }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmRunIPM()
{
    TProfil::pm->pmp->pNP =
        ( actionSimplex->isChecked())? 0: 1;
    if( actionPrecise->isChecked() && !TProfil::pm->pa.p.PRD )
          TProfil::pm->pa.p.PRD = -5; // Changed
    TProfil::pm->pa.p.PRD =
        ( !actionPrecise->isChecked())? 0: TProfil::pm->pa.p.PRD;
    try
    {
     //  saveList1();

#ifdef Use_mt_mode
       TProfil::pm->userCancel = false;

	ProgressDialog* dlg = new ProgressDialog(this,actionStepwise->isChecked()  );

	dlg->show();
//	delete dlg;

#else

        if( !menu->isItemChecked( pStepwiseCheck ) )
        {
           pVisorImp->CalcMulti();
//           CmOpen_res_PHASE_HL();    // Added Sveta 17/01/2002
//         Update();   Provisional
        }
#endif
        ListViewResult->resetList();
        Update();
    }
    catch( TError& err)
    {
        ListViewResult->resetList();
        Update();
        vfMessage(this, err.title, err.mess);
    }
   // set ta2 as default
   TabWid->setCurrentIndex(1); // qt3to4 setCurrentPage(1);
}

void
NewSystemDialog::CmSimplex()
{
//
//	menu->setItemChecked( pSimplexCheck,
//     !menu->isItemChecked( pSimplexCheck ));
}

void
NewSystemDialog::CmPrecise()
{
//    menu->setItemChecked( pRaisedCheck,
//     !menu->isItemChecked( pRaisedCheck ));
}

void
NewSystemDialog::CmStepwise()
{
//    menu->setItemChecked( pStepwiseCheck,
//     !menu->isItemChecked( pStepwiseCheck ));
}

//----------------------------------------------------------------------------------------
// Show resalts (view) actions
// Access to MULTI data
void                                  // Dual Solution (u), pH, pe, I
NewSystemDialog::CmOpen_EQSTAT_ICOMP()
{
    pVisorImp->OpenModule(this, MD_EQDEMO, 0);
}


void                                  // Details for Experts
NewSystemDialog::CmOpen_MULTI()
{
    pVisorImp->OpenModule(this, MD_MULTI); //... page
}

//----------------------------------------------------------------------------------------
// Print actions
void
NewSystemDialog::CmPrintMtparam()
{
    try
    {
        if( !MessageToSave() )
	       return;
    	TProfil::pm->RecordPrint( "pscript*:*:mtparm:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmPrintSystem()
{   // Only user-spec. constraints
    try
    {
        if( !MessageToSave() )
	         return;
    	TSysEq::pm->RecordPrint( "pscript*:*:system:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmPrintEqstat()
{                                // Only for non-zero DC, PH
    try
    {
        if( !MessageToSave() )
	        return;
    	TSysEq::pm->RecordPrint( "pscript*:*:multi:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


void
NewSystemDialog::CmPrResults()
{
    try
    {
      if( !MessageToSave() )
	      return;
      // open file to output
      gstring filename;
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

      printResultList(f);
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}


void
NewSystemDialog::CmPrInput()
{
    try
    {
      if( !MessageToSave() )
	       return;
    	// open file to output
      gstring filename;
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

      printInputList(f);
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

//----------------------------------------------------------------------------------------
// Help system

void
NewSystemDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_SYS_HTML );
}

void
NewSystemDialog::CmMoreBCC()
{                        // Create this HTML file
    pVisorImp->OpenHelp( GEMS_BCC_HTML );
}

void
NewSystemDialog::CmMoreIPM()
{                        // Create this HTML file
    pVisorImp->OpenHelp( GEMS_IPM_HTML );
}


void
NewSystemDialog::CmHowto()
{
    pVisorImp->OpenHelp( GEMS_HOWTO_HTML );
}

void
NewSystemDialog::CmScript()
{
    TSysEq::pm->CmScript();
}


//-------- End of file NewSystemMenu.cpp ----------------------------


