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

/*void
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
    connect( actionHelp_2, SIGNAL( triggered()), this, SLOT(CmHelp2()));

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
*/

// System Actions
//----------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------

/*void NewSystemDialog::CmRunIPM()
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
        //ListViewResult->resetList();
        Update();
    }
    catch( TError& err)
    {
        //ListViewResult->resetList();
        Update();
        vfMessage(this, err.title, err.mess);
    }
   // set ta2 as default
   TabWid->setCurrentIndex(1); // qt3to4 setCurrentPage(1);
}
*/

//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Help system

/*void
NewSystemDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_ONESYS_HTML );
}*/

/*
void
NewSystemDialog::CmMoreBCC()
{                        // Create this HTML file
    pVisorImp->OpenHelp( GEMS_HOWTOB_HTML );
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
*/
/*void
NewSystemDialog::CmScript()
{
    TSysEq::pm->CmScript();
}*/


//-------- End of file NewSystemMenu.cpp ----------------------------


