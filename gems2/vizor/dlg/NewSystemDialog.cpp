//-------------------------------------------------------------------
// $Id$
//
// Implementation of NewSystemDialog class
//
// Copyright (C) 1996-2001 S.Dmytriyeva
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
const char *GEMS_SYS_HTML = "gems_sys";
const char *GEMS_BCC_HTML = "gems_bcc";
const char *GEMS_IPM_HTML = "gems_ipm";
const char *GEMS_HOWTO_HTML = "gems_howto";

#include <qtoolbar.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qapplication.h>
#include <qheader.h>
#include <qtabwidget.h>


#include "NewSystemDialog.h"
#include "ProgressDialog.h"
#include "v_mod.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "m_param.h"
#include "m_syseq.h"
#include "units.h"

#define Inherited NewSystemDialogData

NewSystemDialog* NewSystemDialog::pDia = 0;

NewSystemDialog::NewSystemDialog(QWidget* parent, const char* name):
        Inherited( parent, name, true )
{
    gstring titl = pVisorImp->getGEMTitle();
            titl+= " : Single Chemical Thermodynamic System";

    setCaption( titl.c_str() );
    pDia = this;

    colEdit = 0;
    ListView1->setSorting(-1);
    ListView1->setAllColumnsShowFocus(true);
     ListView1->setColumnWidthMode(0, QListView::Manual);
     ListView1->setColumnWidthMode(2, QListView::Manual);
     ListView1->setColumnWidthMode(3, QListView::Manual);
     ListView1->setColumnWidthMode(4, QListView::Manual);
     ListView1->setColumnWidthMode(6, QListView::Manual);
     ListView1->setColumnWidthMode(8, QListView::Manual);
     ListView1->setColumnWidthMode(11, QListView::Manual);


    connect( ListView1, SIGNAL(
     rightButtonPressed( QListViewItem *, const QPoint &, int )  ),
     this, SLOT( CmEdit( QListViewItem *, const QPoint &, int ) ) );
    loadList1();

    ListView2->setSorting(-1);
    ListView2->setAllColumnsShowFocus(true);
     ListView2->setColumnWidthMode(0, QListView::Manual);
     ListView2->setColumnWidthMode(2, QListView::Manual);

    loadList2();

    LoadMenu();
    menu->setItemChecked( pSimplexCheck, true );
    menu->setItemChecked( pRaisedCheck, true );
    menu->setItemChecked( pStepwiseCheck, false );

    Update();
}

void
NewSystemDialog::LoadMenu()
{
    menu = menuBar();
    menuBar()->clear();

    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Create...", this, SLOT(CmCreate()) );
        p2->insertItem( "&New(Clone)...", this, SLOT(CmNew()) );
        p2->insertSeparator();
        p2->insertItem( "D&isplay", this, SLOT(CmSelect()), Key_F6 );
        p2->insertItem( "&Remake...", this, SLOT(CmRemake()) );
        p2->insertSeparator();
        p2->insertItem( "&Save", this, SLOT(CmSave()) );
        p2->insertItem( "Save &As...", this, SLOT(CmSaveAs()));
        p2->insertItem( "&Delete", this, SLOT(CmDelete()) );
        p2->insertSeparator();
        p2->insertItem( "C&lose", this, SLOT(close()), CTRL + Key_F4 );
        menuBar()->insertItem( "&Record", p2 );
    }

    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Thermodynamic data at T,P...",this,SLOT(CmOpen_MTPARAM()));
        p2->insertItem("&Bulk composition...",this,SLOT(CmOpen_SYSTEM_ICOMP()));
        p2->insertSeparator();
        p2->insertItem( "&Commit ISD list view", this, SLOT(CmCommit()) );
        p2->insertItem( "&Reset ISD list view", this, SLOT(CmReset()) );
        p2->insertSeparator();
        p2->insertItem( "Out Multi (test)", this, SLOT(CmOutMulti()) );
        p2->insertItem( "Read Multi (test)", this, SLOT(CmReadMulti()) );
        menuBar()->insertItem( "&Data", p2 );
    }

    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&BCC ", this, SLOT(CmRunBCC()) );
        p2->insertItem( "&IPM ", this, SLOT(CmRunIPM()) );
        p2->insertSeparator();
        p2->polish();
        pSimplexCheck = p2->insertItem( "&Simplex", this, SLOT(CmSimplex()) );
        pRaisedCheck = p2->insertItem( "&Precise", this, SLOT(CmPrecise()) );
        pStepwiseCheck = p2->insertItem( "S&tepwise", this, SLOT(CmStepwise()) );
        p2->setCheckable( true );
        menuBar()->insertItem( "&Calc", p2 );
    }
    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Dual Solution etc...", this,
                           SLOT(CmOpen_EQSTAT_ICOMP()));
        p2->insertItem("Details for &experts...", this,
                            SLOT(CmOpen_MULTI()));
        menuBar()->insertItem( "&View", p2 );
    }
    {
        QPopupMenu* p2 = new QPopupMenu;
        p2->insertItem( "&Thermodynamic data...", this, SLOT(CmPrintMtparam()) );
        p2->insertItem( "&System Definition...", this, SLOT(CmPrintSystem()) );
        p2->insertItem( "&GEM results...", this, SLOT(CmPrintEqstat()) );
        p2->insertSeparator();
        p2->insertItem( "&Input: System Definition Tree...", this, SLOT(CmPrInput()) );
        p2->insertItem( "&Results: Equilibrium State Tree...", this, SLOT(CmPrResults()) );
        menuBar()->insertItem( "&Print", p2 );
    }
    {
        menuBar()->insertSeparator();
        QPopupMenu* p2 = new QPopupMenu();
        p2->insertItem( "&Help", this, SLOT(CmHelp()) );
        p2->insertItem( "H&ow to", this, SLOT(CmHowto()) );
        p2->insertItem( "&Scripts...", this, SLOT(CmScript()) );
        p2->insertSeparator();
        p2->insertItem( "More about &BCC", this, SLOT(CmMoreBCC()) );
        p2->insertItem( "More about &IPM", this, SLOT(CmMoreIPM()) );
        p2->insertSeparator();
        p2->insertItem( "&About", pVisorImp, SLOT(CmHelpAbout()) );
        menuBar()->insertItem( "&Help", p2 );
    }


    const gstring imgDir = pVisor->imgDir();
    gstring imgFile;

    toolBar = new QToolBar("mptoolbar", this );


   imgFile = imgDir + "arrow_left1.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Previous", 0,
                     this, SLOT(CmPrevious()), toolBar, "previous" );

    imgFile = imgDir + "arrow_right1.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Next", 0,
                     this, SLOT(CmNext()), toolBar, "next" );


    imgFile = imgDir + "fileopen.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Display", 0,
                     this, SLOT(CmSelect()), toolBar, "display" );

    imgFile = imgDir + "SetCompos.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Bulk composition", 0,
                     this, SLOT(CmOpen_SYSTEM_ICOMP()), toolBar, "bulk" );


    toolBar->addSeparator();

    imgFile = imgDir + "RunBCC.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Calculate BCC", 0,
                     this, SLOT(CmRunBCC()), toolBar, "BCC" );


    imgFile = imgDir + "RunIPM.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Calculate IPM", 0,
                     this, SLOT(CmRunIPM()), toolBar, "calc IPM" );

    imgFile = imgDir + "ordertool.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Dual Solution", 0,
                     this, SLOT(CmOpen_EQSTAT_ICOMP()), toolBar, "dual" );

    toolBar->addSeparator();

    imgFile = imgDir + "filesave.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Save", 0,
                     this, SLOT(CmSave()), toolBar, "show" );

    imgFile = imgDir + "fileprint.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Print", 0,
                     this, SLOT(CmPrintEqstat()), toolBar, "print" );

   toolBar->addSeparator();

    imgFile = imgDir + "help.png";
    new QToolButton( QPixmap(imgFile.c_str()), "Help", 0,
                     this, SLOT(CmHelp()), toolBar, "help" );


}

NewSystemDialog::~NewSystemDialog()
{}

// System Actions

void
NewSystemDialog::Update()
{
    MULTI* pData = TProfil::pm->pmp;

    QString msg  = tr (rt[RT_SYSEQ].PackKey());
            msg += QString("     T = %1 K;").arg(pData->T, 6, 'f', 2);
            msg += QString("  P = %1 bar;").arg(pData->P, 6, 'f', 2);
            msg += QString("    V = %1 L;").arg(pData->VX_, 6, 'g', 2);
            msg += QString("  pH = %1;").arg(pData->pH, 7, 'g', 4);
            msg += QString("  pe = %1").arg(pData->pe, 7, 'g', 4);
   statusBar()->message( msg );
}

void
NewSystemDialog::CmOutMulti()
{
    try
    {
      // open file to output
      gstring filename;
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out|ios::binary);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

//Ask Dima!!! 20/04/2002
//Setting start data before calc in calc_gems
      gstring keyp = rt[RT_SYSEQ].UnpackKey();
      TProfil::pm->PMtest( keyp.c_str() );
      TProfil::pm->MultiCalcInit( keyp.c_str() );

      TProfil::pm->outMulti(f);
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
      // open file to output
      gstring filename;
      if( vfChooseFileOpen(this, filename,
          "Get file name for reading" ) == false )
               return;
      fstream f(filename.c_str(), ios::in|ios::binary);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");
      TProfil::pm->readMulti(f);
       loadList1();            // Load...
       loadList2();
       Update();
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmNext()
{
    try
    {
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
       loadList1();            // Load...
       loadList2();
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
    try
    {
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
       loadList1();            // Load...
       loadList2();
       Update();
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}


// Database SysEq record actions

void
NewSystemDialog::CmSelect()
{
    try
    {
    //  TProfil::pm->pmp->pFAG = 1;
    TProfil::pm->loadSystat();
    loadList1();            // Load...
    loadList2();
    Update();
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmCreate()
{
 try
 {
   TProfil::pm->newSystat( VF_CLEARALL );
      // Create...
    loadList1();
    loadList2();
    Update();
 }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmNew()
{
 try
 {
   TProfil::pm->newSystat( VF_BYPASS );
      // Create...
    loadList1();
    loadList2();
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
    TProfil::pm->deriveSystat();
    loadList1();
    loadList2();
    Update();
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
    saveList1();
    TSysEq::pm->CmSave();
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
    saveList1();
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
NewSystemDialog::closeEvent(QCloseEvent* ev)
{
    pDia = 0;
    QWidget::closeEvent(ev);
}

<<<<<<< NewSystemDialog.cpp
=======
bool
NewSystemDialog::event(QEvent* ev)
{
    if( ev->type() == QEvent::WindowActivate ) {
	CmReset();
    }
    if( ev->type() == QEvent::WindowDeactivate ) {
	CmCommit();
    }
    return QWidget::event(ev);
}

>>>>>>> 1.5
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

void
NewSystemDialog::CmCommit()
{
  try
  {
    saveList1();
  }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
NewSystemDialog::CmReset()
{
  try
  {
   loadList1();
   loadList2();
  }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// User Actions

void
NewSystemDialog::CmRunBCC()
{
 try
  {
   saveList1();
   TProfil::pm->CalcBcc(); // Calc bulk composition
   loadList1();
   loadList2();
  }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
/// old
///   TProfil::pm->CalcBcc(); // Calc bulk composition
}

void
NewSystemDialog::CmRunIPM()
{
    TProfil::pm->pmp->pNP =
        ( menu->isItemChecked( pSimplexCheck ) ) ? 0 : 1;
//    if( pSimplexCheck->isChecked() )           KD 17.04.01
//        pRaisedCheck->setChecked( TRUE );
    if( menu->isItemChecked( pRaisedCheck ) && TProfil::pm->pa.p.DW )
          TProfil::pm->pmp->PZ = TProfil::pm->pa.p.DW;
    else  {
            TProfil::pm->pmp->PZ = 0;
            menu->setItemChecked( pRaisedCheck, FALSE );
          }
//        TProfil::pm->pmp->pNP *= -1;
//  New mode: PZ = 1 - Invocation of IPM-2 precision algorithm
//            PZ = 0: IPM-2 disabled
    try
    {
       saveList1();

#ifdef Use_mt_mode

	ProgressDialog* dlg = new ProgressDialog(this,
           menu->isItemChecked( pStepwiseCheck )  );

	dlg->show();
//	delete dlg;

#else

        if( !menu->isItemChecked( pStepwiseCheck ) )
        {
           pVisorImp->CalcMulti();
//           CmOpen_res_PHASE_HL();    // Added Sveta 17/01/2002
//         Update();   Provisional
        }
        else  // Stepwise mode - under construction !
          TProfil::pm->DebagCalcEqstatInit();
#endif
    }
    catch( TError& err)
    {
        vfMessage(this, err.title, err.mess);
    }
   loadList1();
   loadList2();
   // set ta2 as default
   TabWid->setCurrentPage(1);
}

void
NewSystemDialog::CmSimplex()
{
    menu->setItemChecked( pSimplexCheck,
     !menu->isItemChecked( pSimplexCheck ));
}

void
NewSystemDialog::CmPrecise()
{
    menu->setItemChecked( pRaisedCheck,
     !menu->isItemChecked( pRaisedCheck ));
}

void
NewSystemDialog::CmStepwise()
{
    menu->setItemChecked( pStepwiseCheck,
     !menu->isItemChecked( pStepwiseCheck ));
}

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



void
NewSystemDialog::CmPrintMtparam()
{
    try
    {
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
      // open file to output
      gstring filename;
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

      printList2(f);
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
      // open file to output
      gstring filename;
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

      printList1(f);
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

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





//---------------------------------------------------------------

//-------------  HLresDialog  -------------------------------------

void NewSystemDialog::loadList2()
{

   ListView2->clear();
   item2 = new QListViewItem( ListView2, tr( "Equilibrium" ) );
   QListViewItem * phase;
   QListViewItem * elem;
   short *l1_    = (short *)aObj[ o_wi_l1 ].GetPtr();
   vstr buf(20);
   int jj, je;
    jj = je = aObj[o_wd_sm].GetN()-1;


   for( int ii=aObj[o_wd_sf].GetN()-1; ii>=0 ; ii--)
   {
    if( l1_ == NULL )
      break;
    sprintf( buf, " %d", l1_[ii] );
    phase = new QListViewItem( item2,
     aObj[o_wd_sf].GetStringEmpty(ii).c_str(), buf.p,
     aObj[o_wi_phc].GetStringEmpty(ii).c_str(),
     aObj[o_w_xf].GetStringEmpty(ii).c_str() );
    je = je - l1_[ii];
        for( ; jj>je ; jj--)
         switch( aObj[o_wi_dcc].GetStringEmpty(jj)[0])
         {
           case DC_SCP_CONDEN:
           case DC_AQ_SOLVENT: /* mol fractions in solvent */
           case DC_AQ_SOLVCOM:
           case DC_GAS_COMP:
           case DC_GAS_H2O:
           case DC_GAS_CO2:   /* gases */
           case DC_GAS_H2:    /* volume */
           case DC_GAS_N2:
           case DC_SOL_IDEAL:
           case DC_SOL_MINOR:   /* volume */
           case DC_SOL_MAJOR:
           case DC_PEL_CARRIER:
           case DC_SUR_MINAL:
           case DC_SUR_CARRIER: /* sorbent */
             elem = new QListViewItem( phase,
                aObj[o_wd_sm].GetStringEmpty(jj).c_str()," ",
                aObj[o_wi_dcc].GetStringEmpty(jj).c_str(),
                aObj[o_w_x].GetStringEmpty(jj).c_str(),
                aObj[o_wd_yla].GetStringEmpty(jj).c_str(),
                aObj[o_wo_wx].GetStringEmpty(jj).c_str(),
                aObj[o_wo_lngam].GetStringEmpty(jj).c_str() );
            break;
        default:
             elem = new QListViewItem( phase,
                aObj[o_wd_sm].GetStringEmpty(jj).c_str()," ",
                aObj[o_wi_dcc].GetStringEmpty(jj).c_str(),
                aObj[o_w_x].GetStringEmpty(jj).c_str(),
                aObj[o_wd_yla].GetStringEmpty(jj).c_str(),
                aObj[o_wd_ym].GetStringEmpty(jj).c_str(),
                aObj[o_wo_lngam].GetStringEmpty(jj).c_str() );
            break; /* error in DC class code */
        }
    }
    item2->setOpen(true);
    ListView2->update();
    ListView2->setFocus();
}


void NewSystemDialog::printList2( fstream& f )
{
    QListViewItem* pPhase = item2->firstChild();
    QListViewItem* pComp;
    const char *col;
    vstr buf(30);

    while( pPhase )
    {
      pComp = pPhase->firstChild();
      for(int ii=0; ii<4; ii++)
      {
        col =pPhase->text( ii );
        switch(ii)
        {
         case 0: sprintf( buf, "%16s", col );
                 break;
         case 1:
         case 2: sprintf( buf, "%2s", col );
                 break;
         default: sprintf( buf, "%15s", col );
                  break;
        }
         f<<  buf << " ";
      }
      f<< "\n";

      while( pComp )
      {
         for(int ii=0; ii<7; ii++)
        {
           col =pComp->text( ii );
           switch(ii)
           {
              case 0: sprintf( buf, "%16s", col );
                 break;
              case 1:
              case 2: sprintf( buf, "%2s", col );
                 break;
              default: sprintf( buf, "%15s", col );
                  break;
           }
         f<<  buf << " ";
        }
         f<< "\n";
         pComp = pComp->nextSibling();
      }
      pPhase = pPhase->nextSibling();
    }

}

//-------------  HLinpDialog  -------------------------------------

void NewSystemDialog::CmEdit( QListViewItem *it,
       const QPoint &, int col)
{
  if( it == 0 || col < 3  )
    return;
    
  QString data = it->text( col );
  if( data.isEmpty() )
    return;
    
  if( colEdit==0 )
    colEdit = new MLineEdit(ListView1/*this*/);
    
  QRect rec = ListView1->itemRect( it );
/*
  rec.setLeft( rec.left()+ ListView1->header()->sectionPos(col )+
               ListView1->frameWidth() + ListView1->x() );
  rec.setTop( rec.top() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y()+
              menu->height()+toolBar->height() );
  rec.setBottom( rec.bottom() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y()+
              menu->height()+toolBar->height() );
  rec.setWidth( ListView1->columnWidth( col ) );
*/
  rec.setLeft( rec.left()+ ListView1->header()->sectionPos(col )+
               ListView1->frameWidth());
  rec.setTop( rec.top() + ListView1->frameWidth()+
              ListView1->header()->height() );
  rec.setBottom( rec.bottom() + ListView1->frameWidth()+
              ListView1->header()->height() );
  rec.setWidth( ListView1->columnWidth( col ) );

  colEdit->setData( it, col );
  colEdit->setGeometry( rec );
  colEdit->setText( data );
  colEdit->setFocus();
  colEdit->show();
  connect( colEdit, SIGNAL( returnPressed () ),
     colEdit, SLOT( CmAccept() ));

  update();
}

void NewSystemDialog::loadList1()
{

   ListView1->clear();
   if( colEdit )
    colEdit->setData( 0, -1 );
   item1 = new QListViewItem( ListView1, tr( "System" ) );
   QListViewItem * phase;
   QListViewItem * elem;
   short *l1_    = (short *)aObj[ o_mul1 ].GetPtr();
   vstr buf(20);
   int jj, je;
    jj = je = aObj[o_musm].GetN()-1;


   for( int ii=aObj[o_musf].GetN()-1; ii>=0 ; ii--)
   {
     sprintf( buf, " %d", l1_[ii] );
     phase = new QListViewItem( item1,
     gstring( aObj[o_musf].GetStringEmpty(ii), 12, 16).c_str(), buf.p,
     aObj[o_muphc].GetStringEmpty(ii).c_str(),
     aObj[o_sypcl].GetStringEmpty(ii).c_str(),
     aObj[o_syxpun].GetStringEmpty(ii).c_str(),
     aObj[o_syphm].GetStringEmpty(ii).c_str(),
     "J" /*aObj[].GetStringEmpty(ii).c_str()*/,
     aObj[o_syyof].GetStringEmpty(ii).c_str() );
    je = je - l1_[ii];
        for( ; jj>je ; jj--)
        {
             elem = new QListViewItem( phase,
                gstring( aObj[o_musm].GetStringEmpty(jj), 12, 16).c_str(),"",
                aObj[o_mudcc].GetStringEmpty(jj).c_str(),
                aObj[o_sydcl].GetStringEmpty(jj).c_str(),
                aObj[o_syxdun].GetStringEmpty(jj).c_str(),
                aObj[o_syxed].GetStringEmpty(jj).c_str(),
                "J", //aObj[].GetStringEmpty(jj).c_str(),
                aObj[o_sygex].GetStringEmpty(jj).c_str());
                elem->setText( 8, aObj[o_syrsc].GetStringEmpty(jj).c_str());
                elem->setText( 9, aObj[o_sydll].GetStringEmpty(jj).c_str());
                elem->setText( 10, aObj[o_sydul].GetStringEmpty(jj).c_str());
                elem->setText( 11, aObj[o_syrlc].GetStringEmpty(jj).c_str() );
        }
    }
    item1->setOpen(true);
    ListView1->update();
    ListView1->setFocus();
}

void NewSystemDialog::saveList1()
{
   if( colEdit )
    colEdit->getData();

    QListViewItem* pPhase = item1->firstChild();
    QListViewItem* pComp;
    const char *col;
    vstr buf(30);
    int ii, nPh =0, nDc =0;

    while( pPhase )
    {
      pComp = pPhase->firstChild();
      for( ii=0; ii<8; ii++)
      {
         col =pPhase->text( ii );
         switch(ii)
         {
              case 0:
              case 1:
              case 2:
              case 6: // no edited
                 break;
              case 3: if( !aObj[o_sypcl].IsNull() )
                       aObj[o_sypcl].SetString( col, nPh );
                      break;
              case 4: if( !aObj[o_syxpun].IsNull() )
                       aObj[o_syxpun].SetString( col, nPh );
                      break;
              case 5: if( !aObj[o_syphm].IsNull() )
                       aObj[o_syphm].SetString( col, nPh );
                      break;
              case 7: if( !aObj[o_syyof].IsNull() )
                       aObj[o_syyof].SetString( col, nPh );
                      break;
           }
      }
      while( pComp )
      {
        for( ii=0; ii<12; ii++)
        {
            col =pComp->text( ii );
            switch(ii)
            {
              case 0:
              case 1:
              case 2:
              case 6: // no edited
                 break;
              case 3: if( !aObj[o_sydcl].IsNull() )
                       aObj[o_sydcl].SetString( col, nDc );
                      break;
              case 4: if( !aObj[o_syxdun].IsNull() )
                       aObj[o_syxdun].SetString( col, nDc );
                      break;
              case 5: if( !aObj[o_syxed].IsNull() )
                       aObj[o_syxed].SetString( col, nDc );
                      break;
              case 7: if( !aObj[o_sygex].IsNull() )
                       aObj[o_sygex].SetString( col, nDc );
                      break;
              case 8: if( !aObj[o_syrsc].IsNull() )
                       aObj[o_syrsc].SetString( col, nDc );
                      break;
              case 9: if( !aObj[o_sydll].IsNull() )
                       aObj[o_sydll].SetString( col, nDc );
                      break;
              case 10: if( !aObj[o_sydul].IsNull() )
                       aObj[o_sydul].SetString( col, nDc );
                      break;
              case 11: if( !aObj[o_syrlc].IsNull() )
                       aObj[o_syrlc].SetString( col, nDc );
                      break;
           }
         }
         nDc++;
         pComp = pComp->nextSibling();
      }
     nPh++;
     pPhase = pPhase->nextSibling();
    }

}


void NewSystemDialog::printList1( fstream& f )
{
    QListViewItem* pPhase = item1->firstChild();
    QListViewItem* pComp;
    const char *col;
    vstr buf(30);

    while( pPhase )
    {
      pComp = pPhase->firstChild();
      for(int ii=0; ii<8; ii++)
      {
         col =pPhase->text( ii );
         switch(ii)
         {
              case 0: sprintf( buf, "%16s", col );
                 break;
              case 1: sprintf( buf, "%3s", col );
                 break;
              case 7:
              case 9:
              case 10: sprintf( buf, "%15s", col );
                 break;
              default: sprintf( buf, "%1s", col );
                  break;
           }
         f<<  buf << " ";
      }
      f<< "\n";

      while( pComp )
      {
         for(int ii=0; ii<12; ii++)
        {
            col =pComp->text( ii );
            switch(ii)
            {
              case 0: sprintf( buf, "%16s", col );
                 break;
              case 1: sprintf( buf, "%3s", col );
                 break;
              case 7:
              case 9:
              case 10: sprintf( buf, "%15s", col );
                 break;
              default: sprintf( buf, "%1s", col );
                  break;
           }
          f<<  buf << " ";
         }
         f<< "\n";
         pComp = pComp->nextSibling();
      }
      pPhase = pPhase->nextSibling();
    }

}



//---------  MLineEdit  ----------------------------------

int fldSymbols[20] = { //phase
     -1, -1, -1, 1,
     16, -1, 4, -1,
                   //dcomp&readc
     -1, -1, -1, 1,
     16, -1, 4, -1,
     23, -1, -1, 22
 };


MLineEdit::MLineEdit( QWidget * parent, const char * name ):
  QLineEdit( parent, name ), it(0), col(-1)
{

   connect( parent, SIGNAL( contentsMoving ( int, int) ),
   this, SLOT( CmHide( int, int) ) );

}

void MLineEdit::CmHide( int , int  )
{
    CmAccept();
}

void MLineEdit::CmAccept()
{
    getData();
    hide();
}

void
MLineEdit::focusOutEvent(QFocusEvent* e)
{
    getData();
    QLineEdit::focusOutEvent(e);
    hide();
}

void MLineEdit::setData( QListViewItem *ait, int acol)
{
 it = ait;
 col = acol;
 if( !it )
   return;
 if( it->text( 1 ).isEmpty() ) //dcomp&reacdc
  type =  fldSymbols[col+8];
 else
  type =  fldSymbols[col];      //phase
  if( type == -1 )
     setMaxLength( 20 );
  else
  {   setMaxLength( 2 );
      Vals = aUnits[type].getVals(0);
  }
}

const gstring emptiness("---");

void MLineEdit::getData()
{
 if( it == 0 || col <= 2 )
   return;

 gstring old_data = (const char*)it->text( col );

 gstring ss = (const char*)text();
 ss.strip();
 if( type == -1  ) // double
 {
   if( ss == S_EMPTY || ss == emptiness )
   {
      it->setText( col, S_EMPTY );
   }
   else
   {
    double dd;
    vstr sv(20);
    if( sscanf(ss.c_str(), "%lg%s", &dd, sv.p ) != 1 )
         vfMessage(topLevelWidget(), ss.c_str(), "Sorry! Wrong value typed!" );
    else
      it->setText( col, ss.c_str() );
   }
  }
  else // spec simbol
  {
   if( ss == S_EMPTY || ss == emptiness )
   {
      it->setText( col, S_EMPTY );
   }
   else
   {
//    Vals = aUnits[type].getVals(0);
    size_t ind1 = Vals.find(ss);
    if( ind1 == gstring::npos )
      vfMessage(topLevelWidget(), ss.c_str(), "Sorry! Wrong value typed!" );
    else
      it->setText( col, ss.c_str() );
   }
  }
  if( col == 3 ) // + or -
  {
    QListViewItem* pp = it->firstChild();
    while( pp )
    {
      if( ss[0] == '-' ||
         ( ( ss[0] == '+' || ss[0] == '*' ) && old_data[0] == '-' ))
      pp->setText( col, ss.c_str() );
      pp = pp->nextSibling();
    }
  }
}

void
MLineEdit::mousePressEvent(QMouseEvent *e)
{
    if( e->button() != RightButton || type < 0 )
    {
        QLineEdit::mousePressEvent(e);
        return;
    }
    QPopupMenu* menu = new QPopupMenu;
    CHECK_PTR(menu);

    for(uint ii=0; ii<Vals.length(); ii++)
    {
        gstring s(Vals, ii, 1);
        int id = menu->insertItem(s.c_str(), ii);
        menu->setItemEnabled(id, true);
    }
    connect(menu, SIGNAL(activated(int)),
            this, SLOT(SetIndex(int)));
    menu->exec(e->globalPos());
}

void
MLineEdit::SetIndex(int ii)
{
    if( ii>=Vals.length() )
        return;

    gstring val(Vals, ii, 1);
    setText(val.c_str());
    getData();
    //it->setText( col, val.c_str() );
}


//-------- End of file NewSystemDialog.cpp ----------------------------


