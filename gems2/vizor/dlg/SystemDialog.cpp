//-------------------------------------------------------------------
// Id: gems/vizor/dlg/SystemDialog.cpp  version 2.0.0    2001
//
// Implementation of SystemDialog class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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
const char *GEMS_SYS_HTML = "gems_sys";
const char *GEMS_BCC_HTML = "gems_bcc";
const char *GEMS_IPM_HTML = "gems_ipm";

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlcdnumber.h>

#include "SystemDialog.h"
#include "EQDialog.h"
#include "ProgressDialog.h"
#include "HLresDialog.h"
#include "HLinpDialog.h"
#include "v_mod.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "m_param.h"
#include "m_syseq.h"

#define Inherited SystemDialogData

SystemDialog* SystemDialog::pDia = 0;

SystemDialog::SystemDialog(QWidget* parent, const char* name):
        Inherited( parent, name, true )
{
    setCaption( pVisorImp->getGEMTitle() );
    pDia = this;
    pImg1->setPixmap (pVisorImp->getSysIc()/*getLogo*/);
    Update();
}

SystemDialog::~SystemDialog()
{}

// System Actions

void
SystemDialog::Update()
{
    MULTI* pData = TProfil::pm->pmp;

    LCDNumberT->display( pData->T );
    LCDNumberP->display( pData->P );
    LCDNumberV->display( pData->VX_ );

    pRKey->setText( rt[RT_SYSEQ].PackKey() );
}

// User Actions

void
SystemDialog::CmRunBCC()
{
    TProfil::pm->CalcBcc(); // Calc bulk composition
}

void
SystemDialog::CmRunIPM()
{
    TProfil::pm->pmp->pNP =
        ( pSimplexCheck->isChecked() ) ? 0 : 1;
//    if( pSimplexCheck->isChecked() )           KD 17.04.01
//        pRaisedCheck->setChecked( TRUE );
    if( pRaisedCheck->isChecked() && TProfil::pm->pa.p.DW )
          TProfil::pm->pmp->PZ = TProfil::pm->pa.p.DW;
    else  {
            TProfil::pm->pmp->PZ = 0;
            pRaisedCheck->setChecked( FALSE );
          }
//        TProfil::pm->pmp->pNP *= -1;
//  New mode: PZ = 1 - Invocation of IPM-2 precision algorithm
//            PZ = 0: IPM-2 disabled
    try
    {
#ifdef Use_mt_mode

	ProgressDialog* dlg = new ProgressDialog(this, pStepwiseCheck->isChecked());

	dlg->show();
//	delete dlg;

#else

        if( !pStepwiseCheck->isChecked() )
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
}

    // Access to Project data

void                                 // Thermodynamic data
SystemDialog::CmOpen_MTPARAM()
{
//    SetString(" OpenModule" );
    pVisorImp->OpenModule(this, MD_MTPARM);
}

void                                 // Bulk composition (b)
SystemDialog::CmOpen_SYSTEM_ICOMP()
{
    pVisorImp->OpenModule(this, MD_SYSTEM, 3); // ICOMP page
}

// System project phase - species hierarchy list view
void
SystemDialog::CmOpen_inp_PHASE_HL()
{
    if( !HLinpDialog::pDia )
        (new HLinpDialog(this))->show();
    else
        HLinpDialog::pDia->raise();
}

      // Access to MULTI data

void                                  // Dual Solution (u), pH, pe, I
SystemDialog::CmOpen_EQSTAT_ICOMP()
{
    pVisorImp->OpenModule(this, MD_EQDEMO, 0);
}

             // Eqstat phase - species hierarchy list view
void                                  // Prime Solution (x)
SystemDialog::CmOpen_res_PHASE_HL()
{
    if( !HLresDialog::pDia )
        (new HLresDialog(this))->show();
    else
        HLresDialog::pDia->raise();
}

void                                  // Details for Experts
SystemDialog::CmOpen_MULTI()
{
    pVisorImp->OpenModule(this, MD_MULTI); //... page
}

// Print - export actions: Re-implement TSysEq::CmPrint( int )

void
SystemDialog::CmPrintMtparam()
{
    try
    {
        TProfil::pm->RecordPrint( "pscript:0000:mtparm:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
SystemDialog::CmPrintSystem()
{   // Only user-spec. constraints
    try
    {
        TSysEq::pm->RecordPrint( "pscript:0000:system:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
SystemDialog::CmPrintEqstat()
{                                // Only for non-zero DC, PH
    try
    {
        TSysEq::pm->RecordPrint( "pscript:0000:multi:");
    }
    catch( TError& xcpt )
    {
        vfMessage(this, xcpt.title, xcpt.mess);
    }
}

// Database SysEq record actions

void
SystemDialog::CmSelect()
{
    //  TProfil::pm->pmp->pFAG = 1;
    pVisorImp->LoadSystem();            // Load...
    Update();
}

void
SystemDialog::CmNew()
{
    pVisorImp->NewSystem();             // Create...
    Update();
}

void
SystemDialog::CmRemake()
{
    pVisorImp->RemakeSystem();          // Remake...
    Update();
}

void
SystemDialog::CmSave()
{
    pVisorImp->SaveSystem();            // Save
}

void SystemDialog::CmBack()
{
    /// close submodules...
    close();                            // Close
}

void
SystemDialog::closeEvent(QCloseEvent* ev)
{
    pDia = 0;
    QWidget::closeEvent(ev);
}

void
SystemDialog::CmHelp()
{                        // Rename file! was "GEMS-Linux-index"
//    pVisorImp->OpenHelp("GEMS-general-index"); // General help on GEMS
    pVisorImp->OpenHelp( GEMS_SYS_HTML );  // 05.01.01
}

void
SystemDialog::CmMoreBCC()
{                        // Create this HTML file
//    pVisorImp->OpenHelp("GEMS-module-BCC"); // More about BCC...
    pVisorImp->OpenHelp( GEMS_BCC_HTML );  // 05.01.01
}

void
SystemDialog::CmMoreIPM()
{                        // Create this HTML file
//    pVisorImp->OpenHelp("GEMS-module-IPM"); // More about IPM...
    pVisorImp->OpenHelp( GEMS_IPM_HTML );  // 05.01.01
}

//-------- End of file SystemDialog.cpp ----------------------------


