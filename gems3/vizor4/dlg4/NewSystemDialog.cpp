//-------------------------------------------------------------------
// $Id: NewSystemDialog.cpp 968 2007-12-13 13:23:32Z gems $
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
//const char *GEMS_SYS_HTML = "gems_sys";
//const char *GEMS_BCC_HTML = "gems_bcc";
//const char *GEMS_IPM_HTML = "gems_ipm";
//const char *GEMS_HOWTO_HTML = "ge_howto";

#include <math.h>

#include <qstatusbar.h>
#include <qapplication.h>
#include <QEvent>
#include <QCloseEvent>

#include "NewSystemDialog.h"
//#include "ProgressDialog.h"
#include "v_mod.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "m_param.h"
#include "m_syseq.h"
#include "units.h"
#include "gdatastream.h"


NewSystemDialog* NewSystemDialog::pDia = 0;

NewSystemDialog::NewSystemDialog(QWidget* parent, const char* /*name*/):
	QMainWindow( parent )
{
	setupUi(this);
	gstring titl = pVisorImp->getGEMTitle();
            titl+= " : Single Thermodynamic System in Project  ";
            titl+= gstring(rt[RT_PARAM].FldKey(0), 0, rt[RT_PARAM].FldLen(0));
            setWindowTitle( trUtf8(titl.c_str()) );

    pDia = this;

// define new TTreeView window Input: System Definition
   defineInputList(); // loadList1();
    
// define new TTreeView window Results: Equilibrium State
   ListViewResult = 0;
   //defineResultList();  //loadList2();

    setActions();
    Update();

    // 07/08/2002 Sveta   Create, if no syseq is present in the project
    if( rt[RT_SYSEQ].RecCount() <= 0)
        CmCreate();
}

NewSystemDialog::~NewSystemDialog()
{}

void NewSystemDialog::languageChange()
{
    retranslateUi(this);
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

void
NewSystemDialog::Update()
{
    MULTI* pData = TProfil::pm->pmp;

   if( !ListViewResult  && aObj[ o_wi_l1 ].GetPtr() )
     defineResultList(); 
    pLine->setText(tr (rt[RT_SYSEQ].PackKey()));
    toolBar_5->update();
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
    update();
    // this really updates window when CPU is heavyly loaded
    qApp->processEvents();
}

void
NewSystemDialog::closeEvent(QCloseEvent* ev)
{
    // close module
    /*if( MessageToSave() )
    {
       pDia = 0;
       QWidget::closeEvent(ev);
    }*/
    if( MessageToSave() )
    {
       pDia = 0;
       ev->accept();
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

//-------- End of file NewSystemDialog.cpp ----------------------------


