//-------------------------------------------------------------------
// $Id: GEM2MTWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of GEM2MTWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qvariant.h>

#include "GEM2MTWizard.h"
#include "GemsMainWindow.h"
#include "num_methods.h"
#include "m_gem2mt.h"
#include "io_arrays.h"

extern outField DataBR_fields[58];

void GEM2MTWizard::languageChange()
{
    retranslateUi(this);
}

void
GEM2MTWizard::CmBack()
{
    bool chk = pselS->isChecked()||pselF->isChecked()||pselB->isChecked();  // F mode
    int ndx = stackedWidget->currentIndex();

    if( ndx == graphic_script)
    {
        int nLines = pageScript->getScriptLinesNum();
        if( nLines > 0)
           pnYS->setValue( nLines );
    }

    ndx--;

    if( ndx == vtk_format && !c_PsVTK->isChecked() )
        ndx--;
    if( ndx == graphic_script && !c_PvMSg->isChecked() )
        ndx--;
    if( ndx == auto_script && !c_PvMSt->isChecked() )
        ndx--;
    if( ndx == fluxes_transport && !chk )
        ndx--;
    if( ndx == T_P_lookup )
    {  if(!checkArrays->isChecked()|| !pTPInput->isEnabled())
            ndx--; // T P lists
    }

    stackedWidget->setCurrentIndex ( ndx );
    resetNextButton();
    resetBackButton();
}

void
GEM2MTWizard::CmNext()
{

    bool chk = pselS->isChecked()||pselF->isChecked()||pselB->isChecked();  // F mode

    int ndx = stackedWidget->currentIndex();

    if( ndx == mode_RMT )
     {
         for( int ii=7; ii<14; ii++)
         {
             if( chk )
                GroupBox1->button(ii)->setChecked(false);
             GroupBox1->button(ii)->setEnabled( !chk );
         }
       chFlux->setEnabled( chk );
       chFlux->setChecked( chk );
       setTScript( chk );
       if( chk )
       {
         c_PvMSt->setChecked( true );
         c_PvMSg->setChecked( true );
         chRMass->setChecked( true );
       }
    }

    if( ndx == fluxes_transport )
    {
      if( pselS->isChecked() || pselF->isChecked() || pselB->isChecked() )
           pnFD->setValue( pnC->value() + pnSFD->value() );
    }

    if( ndx == graphic_script )
    {
        int nLines = pageScript->getScriptLinesNum();
        if( nLines > 0)
        pnYS->setValue( nLines );
    }

    ndx++;

    if( ndx == gems3k_exchange )
    {    if( pselS->isChecked()|| pselF->isChecked()  )
        {
            checkArrays->setChecked(true);
            chInterp->setChecked(true);
            pPPoints->setValue( pnC->value() );
            pTPoints->setValue( pnC->value() );
            pTPInput->setEnabled(false);
            pTPlookup->setEnabled(false);
            //ndx++;
            //ndx++; // T P lists
        }
        else
        {
            pTPInput->setEnabled(true);
            pTPlookup->setEnabled(true);
         }
    }

    if(   ndx == T_P_lookup )
     { if( !checkArrays->isChecked() || !pTPInput->isEnabled() )
       {
       // internal setup arrays
         // setupPTArrays();
          ndx++;
       }
       else
         {
           definePArray();
           defineTArray();
           initPTable();
           initTTable();
           showPTTable();
          }
    }

    if( ndx == fluxes_transport && !chk )
        ndx++;
    else
    {
      if( pselS->isChecked() || pselF->isChecked() )
      {
        pnPG->setValue( 1 );
        pnFD->setValue( pnC->value() + pnSFD->value() );
//        pnSFD->setValue( 0 );
      }
      pnPG->setEnabled( !( pselS->isChecked() || pselF->isChecked() ) );
      pnFD->setEnabled( !( pselS->isChecked() || pselF->isChecked() ) );
//      pnSFD->setEnabled( !( pselS->isChecked() || pselF->isChecked() ));
    }

    if( ndx == auto_script && !c_PvMSt->isChecked() )
        ndx++;

    if( ndx == graphic_script && !c_PvMSg->isChecked() )
        ndx++;

    if( ndx == vtk_format && !c_PsVTK->isChecked() )
        ndx++;

   stackedWidget->setCurrentIndex ( ndx );
    resetNextButton();
    resetBackButton();
}

void 	GEM2MTWizard::resetNextButton()
{
	if( stackedWidget->currentIndex() == stackedWidget->count() - 1 )
	{	
		pNext->disconnect();
		connect( pNext, SIGNAL(clicked()), this, SLOT(accept()) );
		pNext->setText("&Finish");
	}
	else
	{	
		pNext->disconnect();
		connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()) );
		pNext->setText("&Next>");
	}
}

void 	GEM2MTWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


GEM2MTWizard::GEM2MTWizard( const char* pkey, char flgs[32],
                           int size[20], double Tai[4], double Pai[4],double Tau[3],
                           const char *acalcScript, const char *aoutScript,
                           const char* aXname, const char* aYname,
                           TCIntArray vtk1, TCIntArray vtk2, QWidget* parent):
          QDialog( parent ),  calcScript(acalcScript), outScript(aoutScript), pageScript(0)
{

    PTable = 0;
    TTable = 0;
   //    setFinishEnabled( WizardPage2, true);
    setupUi(this);
    gstring str1= "GEM-Selektor GEM2MT Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

//Page1
   if( flgs[18] == 'T' )
       pselT->setChecked( true );
   else  if( flgs[18] == 'B' )
         pselB->setChecked( true );
     else  if( flgs[18] == 'A' )
         pselA->setChecked( true );
       else if( flgs[18] == 'D' )
          pselD->setChecked( true );
         else if( flgs[18] == 'W' )
           pselW->setChecked( true );
             else if( flgs[18] == 'F' )
               pselF->setChecked( true );
                else
                   pselS->setChecked( true );
//Page2

    pnC->setValue(size[0]);
    pnIV->setValue(size[1]);
    pntM->setValue(size[2]);
    pLb->setValue(size[9]);
pTauFrom->setValue(Tau[0]);
pTaustep->setValue(Tau[2]);

    if( flgs[4] != '-' )
      c_PvICi->setChecked( true );
    else c_PvICi->setChecked( false );
    if( flgs[21] != '-' )
      c_PsSYd->setChecked( true );
    else c_PsSYd->setChecked( false );
    if( flgs[22] != '-' )
      c_PsSIA->setChecked( true );
    else c_PsSIA->setChecked( false );
    if( flgs[6] != '-' )
      c_PvMSt->setChecked( true );
    else c_PvMSt->setChecked( false );
    if( flgs[7] != '-' )
      c_PvMSg->setChecked( true );
    else c_PvMSg->setChecked( false );
    if( flgs[28] != '-' )
      c_PsMO->setChecked( true );
    else c_PsMO->setChecked( false );
    if( flgs[29] != '-' )
      c_PsVTK->setChecked( true );
    else c_PsVTK->setChecked( false );
    if( flgs[31] != '-' )
      c_PsSmode->setChecked( true );
    else c_PsSmode->setChecked( false );

    QObject::connect( c_PvMSt, SIGNAL( toggled( bool) ), this, SLOT( setTScript( bool) ) );

 //Page 3

        if( flgs[15] != '-' )
          chSelect->setChecked( true );
        else chSelect->setChecked( false );
        if( flgs[23] != '-' )
          chMode->setChecked( false );
        else chMode->setChecked( true );
        if( flgs[24] != '-' )
          chAll->setChecked( true );
        else chAll->setChecked( false );
        if( flgs[25] != '-' )
          chComment->setChecked( true );
        else chComment->setChecked( false );

        pPfrom->setValue(Pai[0]);;
        pPuntil->setValue(Pai[1]);;
        pPstep->setValue(Pai[2]);;
        pPtol->setValue(Pai[3]);;

        pTfrom->setValue(Tai[0]);;
        pTuntil->setValue(Tai[1]);;
        pTstep->setValue(Tai[2]);;
        pTtol->setValue(Tai[3]);;


        if( flgs[26] != '-' )
          checkArrays->setChecked( false );
        else checkArrays->setChecked( true );
        arrayChecked(flgs[26] == '-');

        if( flgs[27] != '-' )
          chInterp->setChecked( true );
        else chInterp->setChecked( false );

        pPPoints->setValue(size[12]);
        pTPoints->setValue(size[13]);

        QObject::connect( checkArrays, SIGNAL( toggled(bool) ), this, SLOT( arrayChecked(bool) ) );

//Page 4

    pnPG->setValue(size[3]);
    pnFD->setValue(size[4]);
    pnSFD->setValue(size[5]);


    switch( flgs[30] )
    {
      case '3': moveGas->setChecked( true );
      case '1': moveAq->setChecked( true );
               break;
      case '2': moveGas->setChecked( true );
               break;
      case '4': moveSolid->setChecked( true );
                break;
      default: break;
    }

    QObject::connect( moveGas, SIGNAL( toggled( bool) ), this, SLOT( UnCheckSolid( bool) ) );
    QObject::connect( moveAq, SIGNAL( toggled( bool) ), this, SLOT( UnCheckSolid( bool) ) );
    QObject::connect( moveSolid, SIGNAL( toggled( bool) ), this, SLOT( UnCheckAqGas( bool) ) );

//Page 5
    pqpt->setValue(size[17]);

    GroupBox1 = new QButtonGroup( gbScript1 );
    GroupBox1->setExclusive(false);
    GroupBox1->addButton( chP, 0 );
    GroupBox1->addButton( chT, 1 );
    GroupBox1->addButton( chV, 2 );
    GroupBox1->addButton( chDiCp0, 3 );
    GroupBox1->addButton( chDiCp1, 4 );
    GroupBox1->addButton( chFlux, 5 );
    GroupBox1->addButton( chRMass, 6 );
    GroupBox1->addButton( chHydP0, 7 );
    GroupBox1->addButton( chHydP1, 8 );
    GroupBox1->addButton( chHydP2, 9 );
    GroupBox1->addButton( chHydP3, 10 );
    GroupBox1->addButton( chHydP4, 11 );
    GroupBox1->addButton( chHydP5, 12 );
    GroupBox1->addButton( chHydP6, 13 );
    connect( GroupBox1, SIGNAL(buttonClicked(int)), SLOT(ScriptChange(int)) );

    if( flgs[6] =='+' )
      pScript_t->setPlainText(calcScript.c_str());

//Page 6

    pnYS->setValue(size[6]);
    pnE->setValue(size[7]);
    pYE->setValue(size[8]);
    pqpg->setValue(size[11]);
    resetPageList(aXname, aYname);


//Page 7

    if( flgs[16] != '-' )
      c_PvnVTK->setChecked( true );
    else c_PvnVTK->setChecked( false );
    //pnVTK->setValue(size[18]);

    listStatic->setWrapping( true );
    listStatic->setSelectionMode(QAbstractItemView::MultiSelection);

    // define lists pages
    resetVTKList();

    // define commands
    QObject::connect( keywdList, SIGNAL(currentRowChanged ( int  ) ),
                      this, SLOT(changePage( int ) ));

    // define List from record
    setVTK(vtk1, vtk2);
    disableVTK();
    QObject::connect( c_PvnVTK, SIGNAL(clicked()), this, SLOT(disableVTK()));


//Page8

    pnPTypes->setValue(size[15]);
    pnProps->setValue(size[16]);

    if( flgs[12] != '-' )
      c_PvGrid->setChecked( true );
    else
       c_PvGrid->setChecked( false );

    if( flgs[13] != '-' )
      c_PsDDc->setChecked( true );
    else c_PsDDc->setChecked( false );
    if( flgs[14] != '-' )
      c_PsDIc->setChecked( true );
    else c_PsDIc->setChecked( false );
    spinBox18->setValue(size[14]);
    pnEl->setValue(size[10]);

}

void GEM2MTWizard::arrayChecked( bool check )
{
  pPstep->setEnabled ( !check );
  pTstep->setEnabled ( !check );
  pPPoints->setEnabled ( check );
  pTPoints->setEnabled ( check );
  chInterp->setEnabled ( check );
  if(!check )
   chInterp->setChecked(false);
}

void GEM2MTWizard::UnCheckSolid( bool check )
{
  if( check )
    moveSolid->setChecked( false );
}

void GEM2MTWizard::UnCheckAqGas( bool check )
{
  if( check )
  {
      moveAq->setChecked( false );
      moveGas->setChecked( false );
   }
}


GEM2MTWizard::~GEM2MTWizard()
{}

void   GEM2MTWizard::getSizes( int size[20] )
{

//Page 2
    size[0]= pnC->value();
    size[1]= pnIV->value();
    size[2]= pntM->value();
    size[9]= pLb->value();
//Page 3
       size[12]= pPPoints->value();
       size[13]= pTPoints->value();

       if(!checkArrays->isChecked())
       {
         // internal setup arrays (iterators )
           double Pai[4], Tai[4];
           getPdata( Pai );
           size[12] = getNpoints( Pai );
           getTdata( Tai );
           size[13] = getNpoints( Tai );
       }
       else
       {
           size[12]= pPPoints->value();
           size[13]= pTPoints->value();
       }

       if( chInterp->isChecked() )
       {
         size[13] = size[12];
       }
//Page 4
    size[3]= pnPG->value();
    size[4]= pnFD->value();
    size[5]= pnSFD->value();
//Page 5
    size[17]= pqpt->value();
//Page 6
    size[6]= pnYS->value();
    size[7]= pnE->value();
    size[8]= pYE->value();
    size[11]= pqpg->value();
//Page 7
    //size[18]= pnVTK->value();
//Page 8
    size[15]= pnPTypes->value();
    size[16]= pnProps->value();
    size[10]= pnEl->value();
    size[14]= spinBox18->value();
}

void   GEM2MTWizard::getTdata( double Tai[4] )
{
//   nT = pTN->value();
   Tai[0] = pTfrom->text().toDouble();
   Tai[1] = pTuntil->text().toDouble();
   if( Tai[1] < Tai[0] )
   {
         Tai[1] = Tai[0];
         Tai[0] = pTuntil->text().toDouble();
   }
   Tai[2] = pTstep->text().toDouble();
   Tai[3] = pTtol->text().toDouble();
}

void   GEM2MTWizard::getPdata( double Pai[4] )
{
//   nP = pPN->value();
   Pai[0] = pPfrom->text().toDouble();
   Pai[1] = pPuntil->text().toDouble();
   if( Pai[1] < Pai[0] )
   {
         Pai[1] = Pai[0];
         Pai[0] = pPuntil->text().toDouble();
   }
   Pai[2] = pPstep->text().toDouble();
   Pai[3] = pPtol->text().toDouble();
}

void   GEM2MTWizard::getTaudata( double Tau[3] )
{
   Tau[0] = pTauFrom->text().toDouble();
   Tau[2] = pTaustep->text().toDouble();
   Tau[1] = Tau[2]*pntM->value()+Tau[0];
}


void   GEM2MTWizard::getFlags( char flgs[32] )
{
  char type='S';

  if( pselT->isChecked())
   type='T';
  else if( pselB->isChecked())
       type='B';
   else if( pselA->isChecked())
           type='A';
     else if( pselD->isChecked())
             type='D';
       else  if( pselS->isChecked())
               type='S';
          else  if( pselW->isChecked())
                 type='W';
            else  if( pselF->isChecked())
                  type='F';
 flgs[18] = type;

 // Page 2

 if( pLb->value() > 0  )
      flgs[5] = '+';
 else flgs[5] = '-';
 if( c_PvICi->isChecked() )
     flgs[4] = '+';
 else flgs[4] = '-';
 if( c_PsSYd->isChecked() )
     flgs[21] = '+';
  else flgs[21] = '-';
 if( c_PsSIA->isChecked() )
    flgs[22] = '+';
 else flgs[22] = '-';
 if( c_PvMSt->isChecked() )
    flgs[6] = '+';
 else flgs[6] = '-';
 if( c_PvMSg->isChecked() )
    flgs[7] = '+';
 else flgs[7] = '-';
 if( c_PsMO->isChecked() )
    flgs[28] = '+';
 else flgs[28] = '-';
 if( c_PsVTK->isChecked() )
    flgs[29] = '+';
 else flgs[29] = '-';
if( c_PsSmode->isChecked() )
    flgs[31] = '+';
 else flgs[31] = '-';

 //Page 3
 if( chSelect->isChecked() )
     flgs[15] = '+';
 else flgs[15] = '-';
 if( chMode->isChecked() )
     flgs[23] = '-';
 else flgs[23] = '+';
 if( chAll->isChecked() )
     flgs[24] = '+';
 else flgs[24] = '-';
 if( chComment->isChecked() )
     flgs[25] = '+';
 else flgs[25] = '-';

 if( checkArrays->isChecked() )
     flgs[26] = '-';
 else flgs[26] = '+';
 if( chInterp->isChecked() )
     flgs[27] = '+';
 else flgs[27] = '-';

 // Page 4
 if( pnPG->value() > 0  )
      flgs[9] = '+';
 else flgs[9] = '-';
 if( pnFD->value() > 0  )
      flgs[10] = '+';
 else flgs[10] = '-';
 if( pnSFD->value() > 0  )
      flgs[11] = '+';
 else flgs[11] = '-';

 if( moveGas->isChecked() )
   flgs[30] = '2';
 if( moveAq->isChecked())
 { if( moveGas->isChecked() )
   flgs[30] = '3';
  else
   flgs[30] = '1';
 }
 if( moveSolid->isChecked() )
   flgs[30] = '4';

 //Page 6
 if( pnE->value() > 0  && pYE->value() >0  )
      flgs[8] = '+';
 else flgs[8] = '-';


//Page 7

 if( c_PvnVTK->isChecked() )
     flgs[16] = '+';
 else flgs[16] = '-';

 //Page8

 if( c_PvGrid->isChecked() )
    flgs[12] = '+';
 else flgs[12] = '-';

 if( c_PsDDc->isChecked() )
     flgs[13] = '+';
  else flgs[13] = '-';
 if( c_PsDIc->isChecked() )
     flgs[14] = '+';
  else flgs[14] = '-';


// flags 0,1,2,3, 19, 20, 17, 30, 31 not used or internal
}

void GEM2MTWizard::help()
{
  pVisorImp->OpenHelp( GM_GEM2MT_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//==============================================================================

void GEM2MTWizard::setTScript( bool c_PvMSt_checked )
{
    if( c_PvMSt_checked && calcScript.empty() )
        ScriptChange( 0 );
}

// This function assembles the nodes/boxes initialization script
//
void GEM2MTWizard::ScriptChange(int )
{
    QString ret;// = textEquat1->toPlainText();

    ret += "if( ct=0 ) begin \n"
           "$Initialization script \n"
           " if( qc < nC ) begin \n";

    if( chDiCp0->isChecked() ) // Initial system variant assignment to nodes
     ret += "  DiCp[qc][0] =: ( qc=0 ? 0 : ( nIV > 1 ?  1 : 0 ) );\n";

    if( chDiCp1->isChecked() ) // Initial assignment of node type
    {
        if( pselS->isChecked() || pselF->isChecked() )
           ret += "  DiCp[qc][1] =: ( qc=0 ? ( nSFD=1 ? 0 : 3 ) : 0 ); \n";
        else
            ret += "$ Source: node 0, more: ( qc=0 | qc=xx )? ... \n"
                   "  DiCp[qc][1] =: ( qc=0 ? 3 : 0 ); \n";
        ret += "  DiCp[qc][1] =: (  qc<nC-1 ? DiCp[qc][1] : (-3) );\n";
    }

    if( chP->isChecked() )   // Initial node pressure, bar (for GEM)
    { ret += "   StaP[qc][0] =: PTVi[(DiCp[qc][0])][0] + qc*0;\n";
      if( pselS->isChecked() || pselF->isChecked() )
       ret += "   Pval[qc] =: StaP[qc][0];\n";
    }

    if( chT->isChecked() )   // Initial node temperature, C (for GEM)
    {  ret += "   StaP[qc][1] =: PTVi[(DiCp[qc][0])][1] + qc*0;\n";
       if( pselS->isChecked() || pselF->isChecked() )
         ret += "   Tval[qc] =: StaP[qc][1];\n";
    }
    if( chV->isChecked() )    // Volume constraint for GEM (usually 0)
     ret += "   StaP[qc][2] =: PTVi[(DiCp[qc][0])][2];\n";

    if( chRMass->isChecked() ) // Initial (reactive) mass of the node
     ret += "   StaP[qc][3] =: PTVi[(DiCp[qc][0])][3];\n";

    if( chHydP0->isChecked() ) // Initial total volume of the node, m3
     ret += "   HydP[qc][0] =: ADpar[1];\n";

    if( chHydP1->isChecked() ) // Initial advection velocity
     ret += "   HydP[qc][1] =: ADpar[2];\n";

    if( chHydP2->isChecked() )  // Initial effective porosity
     ret += "   HydP[qc][2] =: ADpar[3];\n";

    if( chHydP3->isChecked() )  // initial effective permeability
     ret += "   HydP[qc][3] =: ADpar[4];\n";

    if( chHydP4->isChecked() )   // Initial specific longitudinal dispersivity
     ret += "   HydP[qc][4] =: ADpar[5];\n";

    if( chHydP5->isChecked() )   // Initial general diffusivity
     ret += "   HydP[qc][5] =: ADpar[6];\n";

    if( chHydP6->isChecked() )  // Initial tortuosity factor
     ret += "   HydP[qc][6] =: ADpar[7];\n";

    ret += " end \n";

    if( chFlux->isChecked() )
    { ret +=  "   if( qc < nFD ) begin \n"
             "     FDLi[qc][0] =: ( qc=0? ( nSFD=1?(-1):0): FDLi[qc-1][1] ); \n"
             "     FDLi[qc][1] =: ( qc < nC-1+nSFD ? FDLi[qc][0]+1 : (-1) ); \n"
//             "     FDLf[qc][0] =: 1;\n"
             "     FDLf[qc][0] =: 0;\n"          // Debugging 28.11.2011 DK
             "$   Flux rate can be set below\n";
        if( pselB->isChecked() )
           ret+=  "     FDLf[qc][1] =: 0.1;\n";
        else
           ret+=  "     FDLf[qc][1] =: 1;\n";
      ret+=  "    end \n";
    }
    ret += "end\n"
           "$else\n"
           "$ begin \n"
           "$ Optional changes at ct > 1 ... \n"
           "$ ... \n"
           "$ end \n";

    pScript_t->setPlainText(ret);
}

//==============================================================================

equatSetupData eqMT( "xt", "yt", "qc", "qc" );

// work with lists
void GEM2MTWizard::resetPageList(const char* aXname, const char* aYname)
{

    TIArray<pagesSetupData> scalarsList;
    TIArray<pagesSetupData> pgData;

    GetListsnRT( -2, pgData,  scalarsList );
    GetListsnRT( RT_GEM2MT, pgData,  scalarsList );

    pageScript = new EquatSetup( page_6, eqMT,
              RT_GEM2MT, pgData, scalarsList, outScript.c_str(), aXname, aYname  );
    verticalLayout_9->addWidget(pageScript);


}

//==============================================================================

// work with lists
void GEM2MTWizard::resetVTKList()
{
    uint ii, jj;
    int nO;

    TIArray<pagesSetupData> scalarsList;
    TIArray<pagesSetupData> scalarsList2;
    TIArray<pagesSetupData> wnData;

    for( ii=0; ii<38; ii++ )
       scalarsList.Add( new pagesSetupData(DataBR_fields[ii].name.c_str(), ii));
    GetListsnRT( -2, wnData,  scalarsList2 );

    pNdx.Add(f_bIC);
    pNdx.Add(f_uIC);
    pNdx.Add(f_xDC);
    pNdx.Add(f_gam);
    pNdx.Add(f_xPH);
    pNdx.Add(f_mPS);
    pNdx.Add(f_vPS);
    pNdx.Add(f_xPA);
    pNdx.Add(f_bSP);
    pNdx.Add(f_aPH);
    pNdx.Add(f_rMB);
    pNdx.Add(f_dll);
    pNdx.Add(f_dul);
    pNdx.Add(f_mPH);
    pNdx.Add(f_vPH);
    pNdx.Add(f_m_t);
    pNdx.Add(f_con);
    pNdx.Add(f_mju);
    pNdx.Add(f_lga);


    pNdx.Add(f_bPS);

    QString str;
    QListWidgetItem* item1;
    QWidget* page1;
    QHBoxLayout* horizontalLayout1;
    QListWidget* lstIndexes1;

    // delete old data
    cPage = 0;
    pgData.Add( new pagesSetupData("Scalars", -1));
    pLists.append( listStatic  );

    listStatic->clear();
    for(  ii=0; ii<scalarsList.GetCount(); ii++ )
    {
      stData.Add( new pagesSetupData(scalarsList[ii]));
      item1 = new QListWidgetItem( scalarsList[ii].pageName.c_str(),  listStatic);
      item1->setToolTip( DataBR_fields[ii].comment.c_str() );
    }


    // init new pages
    for( ii=0; ii<wnData.GetCount(); ii++ )
    {
        nO = wnData[ii].nObj;

        TCStringArray lst;
        TProfil::pm->getNamesList( nO, lst);
        if( lst.GetCount() < 1 )  // undefined indexation
          continue;

        pgData.Add( new pagesSetupData(wnData[ii]));
        str = QString("%1").arg( wnData[ii].pageName.c_str());
        item1 = new QListWidgetItem( str,  keywdList);

        // add page
        page1 = new QWidget();
        horizontalLayout1 = new QHBoxLayout(page1);
        lstIndexes1 = new QListWidget(page1);
        lstIndexes1->setWrapping( true );
        lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
        horizontalLayout1->addWidget(lstIndexes1);
        winStac->addWidget(page1);

        // insert items to list of indexes
        for(  jj=0; jj<lst.GetCount(); jj++ )
        {
          item1 = new QListWidgetItem( lst[jj].c_str(), lstIndexes1);
        }
        pLists.append(lstIndexes1);
    }

    // define current page
    cPage = 0;
    keywdList->setCurrentItem(0);
    keywdList->item(0)->setSelected(true);
    //changePage( cPage );

    for(int  ii=0; ii<pLists.count(); ii++ ) // listStatic added as first
    {    QObject::connect( pLists[ii]->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
           this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );
    }

}

void GEM2MTWizard::changePage( int nPage )
{
    if(nPage < 0 )
      return;
    cPage = nPage;
    winStac->setCurrentIndex ( nPage );
    if( cPage <= 0 )
     lDesc->setText( "Static Objects List ");
    else
      lDesc->setText( aObj[pgData[nPage].nObj].GetDescription(0,0).c_str());
}

void GEM2MTWizard::changeTable(const QItemSelection & selected, const QItemSelection & deselected)
{
   QModelIndex  ndx;
   int row;

  // added selected
  foreach( ndx,  selected.indexes()  )
  {
    gstring stt = ndx.data(Qt::DisplayRole).toString().toLatin1().data();
    tableInsertRow( pgData[cPage].nObj, ndx.row(), stt.c_str() );
  }
  // delete deselected
  foreach( ndx,  deselected.indexes()  )
  {
    row = tableFindRow( pgData[cPage].nObj, ndx.row());
    if( row >= 0)
       tableDeleteRow( row );
  }
}

// internal functions
int GEM2MTWizard::tableFindRow( int nO, int ndx)
{
  int nRow = -1;
  for(uint ii=0; ii<scriptData.GetCount(); ii++ )
  {
   if(scriptData[ii].nObj == nO && scriptData[ii].nIdx == ndx )
      { nRow = ii; break; }
  }
  return nRow;
}

void GEM2MTWizard::tableInsertRow( int nO, int ndx, const char * andName )
{
     if(cPage == 0)
     {  scriptData.Add( new scriptSetupData( cPage, nO, andName,
             ndx, andName, "" ));
     }
       else
        {
           scriptData.Add( new scriptSetupData( cPage, nO, aObj[nO].GetKeywd(),
             ndx, andName, "" ));
       }
     listUpdate();
}


void GEM2MTWizard::tableDeleteRow( int nRow )
{
   scriptData.Remove(nRow);
    listUpdate();
}

void GEM2MTWizard::disableVTK()
{
    groupBox_3->setEnabled( c_PvnVTK->isChecked() );
}

void GEM2MTWizard::listUpdate()
{
  QString buf;

  listVTK->clear();
  for(uint ii=0; ii<scriptData.GetCount(); ii++ )
  {
      if( scriptData[ii].nObj >= 0 )
          buf = QString("%1(%2)").arg(
               pgData[scriptData[ii].nWin].pageName.c_str()/*scriptData[ii].objName.c_str()*/,
                      scriptData[ii].ndxName.c_str() );
      else
          buf = QString("%1").arg( scriptData[ii].ndxName.c_str() );

      listVTK->addItem(buf);
  }
  listVTK->setCurrentRow(0);
}

inline int GEM2MTWizard::findVTKarr( int vtk1  )
{
  //if( vtk1 == f_bPS )
  // return po strokam

  for( uint ii=0; ii<pNdx.GetCount(); ii++ )
   if( pNdx[ii] == vtk1 )
     return ii+1;

  return -1;
}

void GEM2MTWizard::setVTK( TCIntArray vtk1, TCIntArray vtk2  )
{

  scriptData.Clear();

  for(uint ii=0; ii<vtk1.GetCount(); ii++ )
  {
    int cPage;
    int ndx;

    if( vtk1[ii] < f_bIC )
     { cPage = 0;
       ndx = vtk1[ii];
     }
    else
      {
         cPage =  findVTKarr( vtk1[ii] );
         ndx = vtk2[ii];

         if(vtk1[ii] == f_bPS) // bPS
         { cPage +=  vtk2[ii]/TMulti::sm->GetPM()->N;
           ndx = vtk2[ii]%TMulti::sm->GetPM()->N;
           cPage = min(cPage, pLists.count()-1);
         }
      }

    if( cPage >=0 && ndx >=0 )
    {       keywdList->setCurrentItem(keywdList->item(cPage ));
            pLists[cPage]->item(ndx)->setSelected(true);
    }
  }
}

void GEM2MTWizard::getVTK( TCIntArray& vtk1, TCIntArray& vtk2  )
{
  uint page_;
  int vt_1;
  vtk1.Clear();
  vtk2.Clear();
  if( !c_PvnVTK->isChecked() )
   return; // select all

  for(uint ii=0; ii<scriptData.GetCount(); ii++ )
  {
      if( scriptData[ii].nObj >= 0 )
      {
        page_ = scriptData[ii].nWin;
        if(page_ > pNdx.GetCount() )
          vt_1 = f_bPS;
        else vt_1 = pNdx[page_-1];


        if( vt_1 == f_bPS ) //bPs
        {
           vtk1.Add( vt_1 );
           vtk2.Add(scriptData[ii].nIdx + pgData[page_].ndx*TMulti::sm->GetPM()->N);
        }
        else
        {
           vtk1.Add( vt_1 );
           vtk2.Add(scriptData[ii].nIdx);
         }
      }
      else
      {
        vtk1.Add( scriptData[ii].nIdx );
        vtk2.Add( 0 );
      }
  }

}

void GEM2MTWizard::setupPTArrays()
{
   int nT, nP, ii;
   double cT, cP;
   double Pai[4], Tai[4];
   double  *arT, *arP;

   //init P array
   getPdata( Pai );
   nP = getNpoints( Pai );
   arP = (double *) aObj[ o_mtpval].Alloc( nP, 1, D_);
   cP = Pai[START_];
   for( ii=0; ii<nP; ii++ )
   {
     arP[ii] = cP;
     cP+= Pai[2];
   }

   //init T array
   getTdata( Tai );
   nT = getNpoints( Tai );
   arT = (double *) aObj[ o_mtpval].Alloc( nT, 1, D_);
   cT = Tai[START_];
   for( ii=0; ii<nT; ii++ )
   {
     arT[ii] = cT;
     cT+= Tai[2];
   }

}

void GEM2MTWizard::definePArray()
{
   int nPs, nP, ii;
   double Pai[4];
   double *arP;

   nP = pPPoints->value();
  //init P array
   nPs = aObj[ o_mtpval].GetN();
   arP = (double *) aObj[ o_mtpval].Alloc( nP, 1, D_);

   if( nPs==1 && nP>1 )
   {
     double stepP = getStep( Pai, nP );
     double cP = Pai[0];
     for( ii=0; ii<nP; ii++ )
     {
      arP[ii] = cP;
      cP += stepP;
     }
   }
}

void GEM2MTWizard::initPTable()
 {
    // init table
   TObjectModel* model;
   QList<FieldInfo>	aFlds;

   if( PTable )
   {   formLayout->removeWidget(PTable);
       delete PTable; //????
   }

   aFlds.clear();
   aFlds.append(FieldInfo( o_mtpval, ftFloat, 15, false, First, eYes, stIO, 20, 1));

   if(chInterp->isChecked())
       aFlds.append(FieldInfo( o_mttval, ftFloat, 15, false, Tied, eYes, stIO, 20, 1));
   model = new TObjectModel( aFlds, this );

   PTable =  new TObjectTable( aFlds, this );
   TObjectDelegate *deleg = new TObjectDelegate( PTable, this);
   PTable->setItemDelegate(deleg);
   PTable->setModel(model);

    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    PTable->setSizePolicy(sizePolicy1);

   int rowSize =0, colSize=0;
   PTable->getObjectSize(rowSize, colSize);
   PTable->setMaximumSize(QSize(colSize, 16777215));

//   if(!chInterp->isChecked())
//     formLayout->setWidget(3, QFormLayout::LabelRole,PTable/*, 3, 0, 1, 1*/);
//   else
//     formLayout->setWidget(3, QFormLayout::SpanningRole,PTable/*, 3, 0, 1, 2*/);

}

void GEM2MTWizard::defineTArray()
{
   int nT, nTs, ii;
   double Tai[4];
   double  *arT;

   getTdata( Tai );

   if(chInterp->isChecked())
       nT = pPPoints->value();
   else
       nT = pTPoints->value();

   //init T array
   nTs =aObj[ o_mttval].GetN();
   arT = (double *) aObj[ o_mttval].Alloc( nT, 1, D_);

   if( nTs == 1 && nT >1 )
   {
       double stepT = getStep( Tai, nT );
       double cT = Tai[0];
       for( ii=0; ii<nT; ii++ )
       {
          arT[ii] = cT;
          cT += stepT;
       }
   }

}

void GEM2MTWizard::initTTable()
 {
   // init table
   TObjectModel* model;
   QList<FieldInfo>	aFlds;

   if( TTable )
   {   formLayout->removeWidget(TTable);
       delete TTable; //????
       TTable = 0;
   }

   if(chInterp->isChecked())  // only one table
           return;

   aFlds.clear();
   aFlds.append(FieldInfo( o_mttval, ftFloat, 15, false, First, eYes, stIO, 20, 1));
   model = new TObjectModel( aFlds, this );

   TTable =  new TObjectTable( aFlds, this );
   TObjectDelegate *deleg = new TObjectDelegate( TTable, this);
   TTable->setItemDelegate(deleg);
   TTable->setModel(model);

   QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
   sizePolicy1.setHorizontalStretch(0);
   sizePolicy1.setVerticalStretch(0);
   TTable->setSizePolicy(sizePolicy1);

   int rowSize =0, colSize=0;
   TTable->getObjectSize(rowSize, colSize);
   TTable->setMaximumSize(QSize(colSize, 16777215));

   //formLayout->setWidget(3, QFormLayout::FieldRole,TTable/*, 3, 1, 1, 1*/);

}

void GEM2MTWizard::showPTTable()
{
  if(!chInterp->isChecked())
     formLayout->insertRow(3, PTable ,TTable );
  else
    formLayout->insertRow(3, PTable );
}

void GEM2MTWizard::objectChanged()
{

}

//--------------------- End of GEM2MTWizard.cpp ---------------------------

void GEM2MTWizard::on_moveAq_toggled(bool checked)
{

}
