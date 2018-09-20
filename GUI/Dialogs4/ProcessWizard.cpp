//-------------------------------------------------------------------
// $Id: ProcessWizard.cpp 968 2007-12-13 13:23:32Z gems $                   
//
// Implementation of ProcessWizard class
//
// Copyright (C) 2005-2013  S.Dmytriyeva, D.Kulik
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

#include "ProcessWizard.h"
#include "GemsMainWindow.h"
#include "m_param.h"
#include "m_proces.h"
#include "service.h"


void ProcessWizard::languageChange()
{
    retranslateUi(this);
}

void
ProcessWizard::CmBack()
{
    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
ProcessWizard::CmNext()
{
    int ndx = stackedWidget->currentIndex();
    int nLines = pageScript->getScriptLinesNum();
    char type = getType();

    if( ndx == 2 && nLines > 0)
    {    pGraph->setValue( nLines );
         pGraphX->setValue( pageScript->getAbscissaNum() );
    }

    if( ndx == 0)
    {
//      setIter( type, 0 ); // here undefined subtype
        defineWindow( type);
    }

    if( ndx == 1)
    {
        int subtype = subTypeButtons->checkedId();
//      setIter( type, subtype ); // here hide changing
        pPoints->setValue(  getNPV( type, subtype) );
        setOutScript( type, subtype );
    }

    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void ProcessWizard::resetNextButton()
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

void 	ProcessWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


ProcessWizard::ProcessWizard( const char* pkey, char flgs[24], int size[8],
                    short tabInt[6], double tabDoubl[24],
            const char *acalcScript, const char *aoutScript,
            const char* aXname, const char* aYname, QWidget* parent):
        QDialog( parent ), curType('-'),
        calcScript(acalcScript), outScript(aoutScript), pageScript(0)
{
    setupUi(this);
    gstring str1= "GEM-Selektor Process Setup:  ";
        str1 += pkey;
        setWindowTitle( str1.c_str() );


    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();


 //Page1
    switch( flgs[9] )
     {
      case P_TITRSING: pselT->setChecked( true );
          break;
      case P_INV_TITR: pselG->setChecked( true );
          break;
      case P_REACTORS: pselR->setChecked( true );
          break;
      case P_SEQUENT: pselS->setChecked( true );
          break;
      case P_LIP: pselL->setChecked( true );
         break;
      case P_PVT: pselP->setChecked( true );
         break;
      default: // for old records
         if( flgs[8] != '-' )
              pselT->setChecked( true );
         else  if( flgs[11] != '-' )
                 pselG->setChecked( true );
               else  if( flgs[5] != '-' )
                       pselR->setChecked( true );
                     else if( flgs[2] != '-' )
                            pselS->setChecked( true );
                         else
                            pselP->setChecked( true );
      break;
  }
   if( !pselP->isChecked() )
     flgs[10] = '+';

//Page2
   subTypeButtons = new QButtonGroup( boxTypes );
   subTypeButtons->addButton(sub1, 0);
   subTypeButtons->addButton(sub2, 1);
   subTypeButtons->addButton(sub3, 2);
   subTypeButtons->addButton(sub4, 3);
   QObject::connect( subTypeButtons, SIGNAL(buttonClicked(int)), this, SLOT(setMode(int)));

   if( flgs[10] =='+' )
     textEquat1->setPlainText(calcScript.c_str());

   TProcesDelegate * deleg = new TProcesDelegate( this );
   tIters->setItemDelegate(deleg);

// iTm
QTableWidgetItem *item = new QTableWidgetItem();
item->setText( QString::number(  tabInt[0] ));
tIters->setItem(0, 0, item);
item = new QTableWidgetItem();
item->setText( QString::number(  tabInt[1] ));
tIters->setItem(1, 0, item);
item = new QTableWidgetItem();
item->setText( QString::number(  tabInt[2] ));
tIters->setItem(2, 0, item);

// iNv
item = new QTableWidgetItem();
item->setText( QString::number(  tabInt[3] ));
tIters->setItem(0, 4, item);
item = new QTableWidgetItem();
item->setText( QString::number(  tabInt[4] ));
tIters->setItem(1, 4, item);
item = new QTableWidgetItem();
item->setText( QString::number(  tabInt[5] ));
tIters->setItem(2, 4, item);

// iP
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[0], 'g', 5 ));
tIters->setItem(0, 2, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[1], 'g', 5 ));
tIters->setItem(1, 2, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[2], 'g', 5 ));
tIters->setItem(2, 2, item);

// iTC
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[3], 'g', 5 ));
tIters->setItem(0, 3, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[4], 'g', 5 ));
tIters->setItem(1, 3, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[5], 'g', 5 ));
tIters->setItem(2, 3, item);

// iV
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[6], 'g', 5 ));
tIters->setItem(0, 1, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[7], 'g', 5 ));
tIters->setItem(1, 1, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[8], 'g', 5 ));
tIters->setItem(2, 1, item);

// iTau
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[9], 'g', 5 ));
tIters->setItem(0, 5, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[10], 'g', 5 ));
tIters->setItem(1, 5, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[11], 'g', 5 ));
tIters->setItem(2, 5, item);

// ipXi
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[12], 'g', 5 ));
tIters->setItem(0, 6, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[13], 'g', 5 ));
tIters->setItem(1, 6, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[14], 'g', 5 ));
tIters->setItem(2, 6, item);

// iNu
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[15], 'g', 5 ));
tIters->setItem(0, 7, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[16], 'g', 5 ));
tIters->setItem(1, 7, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[17], 'g', 5 ));
tIters->setItem(2, 7, item);

// iPh
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[18], 'g', 5 ));
tIters->setItem(0, 8, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[19], 'g', 5 ));
tIters->setItem(1, 8, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[20], 'g', 5 ));
tIters->setItem(2, 8, item);

// ipe
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[21], 'g', 5 ));
tIters->setItem(0, 9, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[22], 'g', 5 ));
tIters->setItem(1, 9, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[23], 'g', 5 ));
tIters->setItem(2, 9, item);

for(int row=0; row<3; row++)
 for(int col=0; col<10; col++)
 {
   item = tIters->item(row,col);
   item->setFlags( item->flags() | Qt::ItemIsEditable);
   item->setTextAlignment(Qt::AlignRight);
 }

//Page3
   resetPageList(aXname, aYname);

//Page4
    pPoints->setValue(size[0]);
    pMods->setValue(size[1]);
    pGraph->setValue(size[2]);
    pGraphX->setValue(size[6]);
    pELine->setValue(size[3]);
    pECol->setValue(size[4]);

    if( flgs[12] != '-' )
      c_PvTm->setChecked( true );
    else c_PvTm->setChecked( false );

    if( flgs[13] != '-' )
      c_PvNV->setChecked( true );
    else c_PvNV->setChecked( false );

    if( flgs[17] != '-' )
      c_PvTau->setChecked( true );
    else c_PvTau->setChecked( false );

    if( flgs[14] != '-' )
      c_PvP->setChecked( true );
    else c_PvP->setChecked( false );

    if( flgs[15] != '-' )
      c_PvT->setChecked( true );
    else c_PvT->setChecked( false );

    if( flgs[16] != '-' )
      c_PvV->setChecked( true );
    else c_PvV->setChecked( false );

    if( flgs[18] != '-' )
      c_PvpXi->setChecked( true );
    else c_PvpXi->setChecked( false );

    if( flgs[19]!= '-' )
      c_PvNu->setChecked( true );
    else c_PvNu->setChecked( false );

    if( flgs[20] != '-' )
      c_PvKin->setChecked( true );
    else c_PvKin->setChecked( false );

//Page5
    if( flgs[22] != '-' )
      c_PvR1->setChecked( true );
    else c_PvR1->setChecked( false );

    if( flgs[0] >= '5' )
      pStepwise->setChecked( true );
    else pStepwise->setChecked( false );

    if( flgs[3] != '-' )
      c_PsRT->setChecked( true );
    else c_PsRT->setChecked( false );

    if( flgs[10] != '-' )
      c_PsEqn->setChecked( true );
    else c_PsEqn->setChecked( false );

    if( flgs[4] != '-' )
      c_PsSY->setChecked( true );
    else c_PsSY->setChecked( false );

//Page6
    spinBox18->setValue(size[5]);


    QObject::connect( tIters, SIGNAL(cellChanged ( int , int  ) ),
                      this, SLOT(CmItersEdit(int , int )));
     tItersChanged = false;
     // define commands
     QObject::connect( listObj, SIGNAL(currentRowChanged ( int  ) ),
                       this, SLOT(changePage( int ) ));


}


ProcessWizard::~ProcessWizard()
{}

void   ProcessWizard::getSizes( int size[8] )
{

    size[0]= pPoints->value();
    size[1]= pMods->value();
    size[2]= pGraph->value();
    size[6]= pGraphX->value();
    size[3]= pELine->value();
    size[4]= pECol->value();
    size[5]= spinBox18->value();
}

void   ProcessWizard::getTable( short tabInt[6], double dbl[24] )
{
    tabInt[0] = tIters->item(0,0)->data(Qt::DisplayRole).toInt();
    tabInt[1] = tIters->item(1,0)->data(Qt::DisplayRole).toInt();
    tabInt[2] = tIters->item(2,0)->data(Qt::DisplayRole).toInt();
    tabInt[3] = tIters->item(0,4)->data(Qt::DisplayRole).toInt();
    tabInt[4] = tIters->item(1,4)->data(Qt::DisplayRole).toInt();
    tabInt[5] = tIters->item(2,4)->data(Qt::DisplayRole).toInt();

    dbl[0] = tIters->item(0,2)->data(Qt::DisplayRole).toDouble();
    dbl[1] = tIters->item(1,2)->data(Qt::DisplayRole).toDouble();
    dbl[2] = tIters->item(2,2)->data(Qt::DisplayRole).toDouble();
    dbl[3] = tIters->item(0,3)->data(Qt::DisplayRole).toDouble();
    dbl[4] = tIters->item(1,3)->data(Qt::DisplayRole).toDouble();
    dbl[5] = tIters->item(2,3)->data(Qt::DisplayRole).toDouble();
    dbl[6] = tIters->item(0,1)->data(Qt::DisplayRole).toDouble();
    dbl[7] = tIters->item(1,1)->data(Qt::DisplayRole).toDouble();
    dbl[8] = tIters->item(2,1)->data(Qt::DisplayRole).toDouble();
    dbl[9] = tIters->item(0,5)->data(Qt::DisplayRole).toDouble();
    dbl[10] = tIters->item(1,5)->data(Qt::DisplayRole).toDouble();
    dbl[11] = tIters->item(2,5)->data(Qt::DisplayRole).toDouble();
    dbl[12] = tIters->item(0,6)->data(Qt::DisplayRole).toDouble();
    dbl[13] = tIters->item(1,6)->data(Qt::DisplayRole).toDouble();
    dbl[14] = tIters->item(2,6)->data(Qt::DisplayRole).toDouble();
    dbl[15] = tIters->item(0,7)->data(Qt::DisplayRole).toDouble();
    dbl[16] = tIters->item(1,7)->data(Qt::DisplayRole).toDouble();
    dbl[17] = tIters->item(2,7)->data(Qt::DisplayRole).toDouble();
    dbl[18] = tIters->item(0,8)->data(Qt::DisplayRole).toDouble();
    dbl[19] = tIters->item(1,8)->data(Qt::DisplayRole).toDouble();
    dbl[20] = tIters->item(2,8)->data(Qt::DisplayRole).toDouble();
    dbl[21] = tIters->item(0,9)->data(Qt::DisplayRole).toDouble();
    dbl[22] = tIters->item(1,9)->data(Qt::DisplayRole).toDouble();
    dbl[23] = tIters->item(2,9)->data(Qt::DisplayRole).toDouble();
}


char   ProcessWizard::getType() const
{
  char type=P_SEQUENT;

  if( pselT->isChecked())
   type=P_TITRSING;
  else if( pselG->isChecked())
           type=P_INV_TITR;
       else if( pselR->isChecked())
                type=P_REACTORS;
          else if( pselP->isChecked())
                type=P_PVT;
                else  if( pselS->isChecked())
                           type=P_SEQUENT;
                    else  if( pselL->isChecked())
                                type=P_LIP;
  return type;
}

void   ProcessWizard::getFlags( char flgs[24] )
{
  char type=getType();

  switch( type )
   {
    case P_TITRSING: memcpy( flgs, "0++-+--++T+-", 12);
        break;
    case P_INV_TITR: memcpy( flgs, "0++-+--+-G++", 12);
        break;
    case P_REACTORS: memcpy( flgs, "0++-++-+-R+-", 12);
        break;
    case P_SEQUENT: memcpy( flgs, "0++-+----S+-", 12);
        break;
    case P_LIP: memcpy( flgs, "0++-+----L+-", 12);
       break;
    case P_PVT:
    default:  memcpy( flgs, "0*-------P--", 12 );
   }
// Putting other flags
   if( c_PvTm->isChecked() )
       flgs[12] = '+';
    else flgs[12] = '-';

   if( c_PvNV->isChecked() )
       flgs[13] = '+';
    else flgs[13] = '-';

   if( c_PvTau->isChecked() )
       flgs[17] = '+';
    else flgs[17] = '-';

   if( c_PvP->isChecked() )
       flgs[14] = '+';
    else flgs[14] = '-';

   if( c_PvT->isChecked() )
       flgs[15] = '+';
    else flgs[15] = '-';

   if( c_PvV->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';

   if( c_PvpXi->isChecked() )
       flgs[18] = '+';
    else flgs[18] = '-';

   if( c_PvNu->isChecked() )
       flgs[19] = '+';
    else flgs[19] = '-';

   if( c_PvKin->isChecked() )
       flgs[20] = '+';
    else flgs[20] = '-';

//Page 3
   if( c_PvR1->isChecked() )
       flgs[22] = '+';
    else flgs[22] = '-';

   if( pStepwise->isChecked() )
       flgs[0] = '5';
    else flgs[0] = '0';

   if( c_PsRT->isChecked() )
       flgs[3] = '+';

   if( c_PsEqn->isChecked() ) //set before, do not switch off
       flgs[10] = '+';

   if( c_PsSY->isChecked() ) //set before, do not switch off
       flgs[4] = '+';
    else flgs[4] = '-';

//Putting flags, using sizes is out of this function
}

void
ProcessWizard::help()
{
   pVisorImp->OpenHelp( GM_PROCES_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//=============================================================================
// Setup of P_expr script for different types of Processes

void ProcessWizard::defineWindow(char type)
{
  uint jj;

  page1Changed = false;
  if( type == curType )
    return;
  curType = type;

  // define object pages
  pgData.Clear();
  switch(type)
    {
     case P_PVT:
            pgData.Add( new pagesSetupData("Phases", o_w_xf));
            break;
     case P_SEQUENT:
            pgData.Add( new pagesSetupData("Compos", o_syxea));
            pgData.Add( new pagesSetupData("DComp", o_syxed));
            pgData.Add( new pagesSetupData("IComp", o_sybi));
            pgData.Add( new pagesSetupData("Phases", o_syphm)); // xp_
            pgData.Add( new pagesSetupData("DC-lower", o_sydll)); // dll_
            pgData.Add( new pagesSetupData("DC-upper", o_sydul)); // dul_
            pgData.Add( new pagesSetupData("Molality",o_wd_ym ));
            pgData.Add( new pagesSetupData("Sorbed", o_wo_bfc));
           break;
    case P_LIP:
           pgData.Add( new pagesSetupData("Phases", o_syyof)); // Yof_
//           pgData.Add( new pagesSetupData("xd_", o_syxed));
           pgData.Add( new pagesSetupData("AqIons",o_wd_yla ));
           pgData.Add( new pagesSetupData("AqElements", o_wd_icm));
          break;
    case P_INV_TITR:
          pgData.Add( new pagesSetupData("AcidBase", o_syxea));
          pgData.Add( new pagesSetupData("Molality",o_wd_ym ));
          pgData.Add( new pagesSetupData("Sorbed",o_w_x )); // x
          break;
    case P_TITRSING:      
            pgData.Add( new pagesSetupData("Compos", o_syxea));
            pgData.Add( new pagesSetupData("DComp", o_syxed));
            pgData.Add( new pagesSetupData("IComp", o_sybi));
            pgData.Add( new pagesSetupData("DCg0in", o_sygex));
            pgData.Add( new pagesSetupData("lga", o_wd_yla)); 
          break;
    case P_REACTORS:
           pgData.Add( new pagesSetupData("Compos", o_syxea)); // xa_
           pgData.Add( new pagesSetupData("DComp", o_syxed));  // xd_
           pgData.Add( new pagesSetupData("IComp", o_sybi));   // bi_
           pgData.Add( new pagesSetupData("Phases", o_syphm)); // xp_
           pgData.Add( new pagesSetupData("DC-lower", o_sydll)); // dll_
           pgData.Add( new pagesSetupData("DC-upper", o_sydul)); // dul_
           break;

     default: break;
     }

   // define pages window
    setupPages();

  // define other selectors
   switch(type)
   {
   case P_PVT:
       {
           lAbout->setText("Please,check iP and iT iterator contents, and set Step to 0 in all other iterators."
                           "\n\nFor PT phase diagram: select phases to plot, then skip the next wizard page.");
         sub1->setText("No script");
         sub2->setText("User-defined script");
         sub3->setText("PT phase diagram");
         sub4->hide();
         if( c_PsEqn->isChecked() )
           sub2->setChecked(true); // to keep the old script
         else
           sub1->setChecked(true); // to keep the old script

         pLsts[0]->setDisabled(true);
         listObj->setDisabled(true);
       }
       break;
   case P_SEQUENT:
       {
         lAbout->setText(
   "Linear titration and logD diagram use iNu; logarithmic titration and logKd digram use ipXi. "
   "Titrations: select items from 'Compos', 'DComp', 'IComp' or 'Phase' lists to act as titrants,"
   " optionally also from 'DC-lower' or 'DC-upper' to change metastability constraints.\n\n"
   "To plot logD vs linear x scale, (i) select minor then host end member from the 'DComp' list,"
   " then trace then host ions from the 'Molality' list. "
   "To plot logKd and isotherms vs log(molality) scale, select trace and host compositions from the"
   " 'Compos' list, then trace and host elements from the 'Sorbed' list. "
   "In both cases, skip the next wizard page. ");
         sub1->setText("Titration cNu (linear)");
         sub2->setText("Diagram logD vs linear x");
         sub3->setText("Titration cpXi logarithmic");
         sub4->setText("Diagram logKd vs log(m)");

         for(jj=0; jj<6; jj++ )
           QObject::connect( pLsts[jj], SIGNAL(itemSelectionChanged()),
                           this, SLOT(CmSetMode()));
         }
       break;
   case P_LIP:
       {
       lAbout->setText("In both variants, select a binary solid solution in 'Phases'.\n  Ion-activity diagram:"
                       " in the 'AqIons' property item list, select a common counterion, then the substituting ion1, ion2"
                       " in the same order as the end members in the selected Phase.\n  Total-scale diagram:"
                       " in the 'AqElements' property item list, select a common 'counterionic' element; then the"
                       " substituting ionic1, ionic2 elements in the order given by end members in the selected Phase.\n"
                       "  In both cases, when ready, skip the next wizard page.");
         sub1->setText("Ion-activity Lippmann diagram");
         sub2->setText("Total-scale Lippmann diagram");
         sub3->hide();
         sub4->hide();
         sub1->setChecked(true);
         QObject::connect( pLsts[0], SIGNAL(itemSelectionChanged()),
                           this, SLOT(CmSetMode()));
        }
         break;
   case P_INV_TITR:
         {
           lAbout->setText(
 "To plot the pH diagram: please, select acid and base from the 'AcidBase' list, and go to the next wizard page.\n\n"
 "To plot constant-pH isotherms: in addition, select the trace element addition in the 'AcidBase' list, set the 'ipe' "
 "iterator accordingly; select aqueous species in the 'Molality' list for the abscissa; finally select one or more "
 "sorbed species from the 'Sorbed' list, then skip the next wizard page.");
           sub1->setText("Property-vs-pH diagram");
           sub2->setText("Constant-pH isotherm diagram");
           sub3->hide();
           sub4->hide();
           QObject::connect( pLsts[0], SIGNAL(itemSelectionChanged()),
                             this, SLOT(CmSetMode()));
          }
          break;

    case P_TITRSING:
          {
              lAbout->setText(
  "Please, select a GEM input parameter from 'Compos', 'DComp', 'IComp', 'DCg0in' list; set its initial increment in"
  " iNu[0]; if an increase of this input decreases a GEM output parameter, set iNu[0] to -1, otherwise to +1.\n\n"
  "Then select the GEM output parameter in 'lga' or 'Other items'; set its prescribed value in ipH[0], set ipH[1] = 0"
  " and a tolerance for matching the output parameter in ipH[2]; and skip the next wizard page. ");
              sub1->setText("Single inverse titration");
              sub1->setChecked(true);
              sub2->hide();
              sub3->hide();
              sub4->hide();

              TIArray<pagesSetupData> scalarsList;
              TIArray<pagesSetupData> pgData1;
              GetListsnRT( MD_MULTI, pgData1,  scalarsList );

              pgData.Add( new pagesSetupData("Other items", -1));
              /*QListWidgetItem *item1 =*/ new QListWidgetItem( "Other items",  listObj);

              // add page
              QWidget *page1 = new QWidget();
              QHBoxLayout *horizontalLayout1 = new QHBoxLayout(page1);
              QListWidget *lstIndexes1 = new QListWidget(page1);
              lstIndexes1->setWrapping( true );
              lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
              horizontalLayout1->addWidget(lstIndexes1);
              pageLists->addWidget(page1);

              // insert items to list of indexes
               for(  jj=0; jj<scalarsList.GetCount(); jj++ )
                  {
                     /*item1 =*/ new QListWidgetItem( scalarsList[jj].pageName.c_str(), lstIndexes1);
                  }
               pLsts.append(lstIndexes1);

              for(jj=0; jj<5; jj++ )
                QObject::connect( pLsts[jj], SIGNAL(itemSelectionChanged()),
                                this, SLOT(CmSetMode()));

          }
              break;

   case P_REACTORS:
              {
                lAbout->setText(
   "'Flushing' scenario: the fluid part evolves while reacting at each step with the same solid part composition;\n"
   "'Leaching' scenario: the solid part changes while reacting at each step with the same fluid part composition.\n"
   " To set a 'Compos' constant source of solid ('Flushing') or fluid ('Leaching'), select either from the Compos list."
   " To use a SysEq record as constant source, first check in the parent system whether the link to that SysEq is set,"
   " and 'xp_' array and 'MbXs' data object is allocated.\n"
   " If needed, select the 'Compos, 'DComp', 'IComp' items that must be zeroed off because their inputs were"
   " already covered in constant-source compositions. When done, proceed to the next wizard page and select what to plot:"
   " some aqueous concentrations in 'Flushing', or some properties of solid phases in 'Leaching' with modC as abscissa." );
                sub1->setText("Flushing: SysEq source");
                sub2->setText("Flushing: Compos source");
                sub3->setText("Leaching: SysEq source");
                sub4->setText("Leaching: Compos source");
                for(jj=0; jj<6; jj++ )
                  QObject::connect( pLsts[jj], SIGNAL(itemSelectionChanged()),
                                  this, SLOT(CmSetMode()));
               }
               break;
          break;

   default: break;
   }

}

int  ProcessWizard::getNPV( char type, int subtype)   // get number of points
{
  int i1, i2, ret = pPoints->value();

  if( !page1Changed && !tItersChanged )   // Doubtful! DK 25.02.10
          return ret;

  switch(type)
   {
   case P_PVT: i1 = getNPoints( 2 ); // nP
             i2 = getNPoints( 3 ); // nT

             if( subtype == 2 )
             {  // PT diagram
                if( abs(i1) == 1 )
                {  // default P iterator
                   setIterColumn( 2, 1., 1100., 100. );
                   i1 = 11;
                }
                if( abs(i2) == 1 )
                {  // default TC iterator
                   setIterColumn( 3, 0., 500., 25. );
                   i2 = 21;
                }
                ret = i1*i2;
             }
             else { // usual T or P sequence
               if(abs(i1)==1 && i2 >= 1)
                 ret = i2;  // Psat curve calculation
               else if(abs(i2)==1 && i1 >= 1)
                      ret = i1;  // Pressure curves at constant TC
                    else {  // generic parallel TP change
                        if( i1 < 1 )
                            i1 = 1;
                        if( i2 < 1 )
                            i2 = 1;
                        ret = min( i1, i2);
                        }
              }   // foolproof
             tIters->item(2, 0)->setText( QString::number( 0 ));  // set iTm[2] to 0
             tIters->item(2, 1)->setText( QString::number( 0., 'g', 5 ));  // set iV[2] to 0
             tIters->item(2, 4)->setText( QString::number( 0 ));  // set iNv[2] to 0
             tIters->item(2, 5)->setText( QString::number( 0., 'g', 5 ));  // set iTau[2] to 0
             tIters->item(2, 6)->setText( QString::number( 0., 'g', 5 ));  // set ipXi[2] to 0
             tIters->item(2, 7)->setText( QString::number( 0., 'g', 5 ));  // set iNu[2] to 0
             tIters->item(2, 8)->setText( QString::number( 0., 'g', 5 ));  // set ipH[2] to 0
             tIters->item(2, 9)->setText( QString::number( 0., 'g', 5 ));  // set ipe[2] to 0
            break;
   case P_SEQUENT:
            if(subtype == 0 || subtype==1)
            {

                ret = getNPoints( 7 ); // iNu
                if( ret == -1 )
                {
                    if( subtype == 0 )
                    { setIterColumn( 7, 0.1, 3.1, 0.1 );
                      ret = 31;
                    }
                    else
                    { setIterColumn( 7, 0.001, 0.999, 0.01 );
                      ret = 100;
                    }
                }

            }
             if( subtype == 2|| subtype==3 )
             {
                 ret = getNPoints( 6 ); // iPxi
                 if( ret == -1 )
                 {
                     if( subtype == 2 )
                     { setIterColumn( 6, 5.1, 0.1, -0.1 );
                       ret = 51;
                     }
                     else
                     { setIterColumn( 6, 7.001, 0.001, -0.02 );
                       ret = 351;
                     }
                 }
             }
             i1 = getNPoints( 0 ); // iTm
             if( i1 < ret )
             {
                  tIters->item(2, 0)->setText( QString::number( 1 ));  // set iTm[2] to 1
                  i2 = tIters->item(0, 0)->data(Qt::DisplayRole).toInt(); // get iTm[0]
                  tIters->item(1, 0)->setText( QString::number( i2+ret ));  // set iTm[1]
             }
            break;
   case P_LIP:
            ret = getNPoints( 6 ); // iPxi
            if( ret == -1 )
            {
                setIterColumn( 6, -5., 5., 0.1 );
//                tIters->item(0, 6)->setText( QString::number( -5., 'g', 3 ));  // set ipXi[0] to -5
//                tIters->item(1, 6)->setText( QString::number( 5., 'g', 3 ));  // set ipXi[1] to 5
//                tIters->item(2, 6)->setText( QString::number( 0.1, 'g', 3 ));  // set ipXi[2] to 0.1
                ret = 101;
            }
            i1 = getNPoints( 0 ); // iTm
            if( i1 < ret )
            {
                 tIters->item(2, 0)->setText( QString::number( 1 ));  // set iTm[2] to 1
                 i2 = tIters->item(0, 0)->data(Qt::DisplayRole).toInt(); // get iTm[0]
                 tIters->item(1, 0)->setText( QString::number( i2+ret ));  // set iTm[1]
            }
            tIters->item(2, 1)->setText( QString::number( 0., 'g', 3 ));  // set iV[2] to 0
            tIters->item(2, 2)->setText( QString::number( 0., 'g', 3 ));  // set iV[2] to 0
            tIters->item(2, 3)->setText( QString::number( 0., 'g', 3 ));  // set iV[2] to 0
            tIters->item(2, 4)->setText( QString::number( 0 ));  // set iNv[2] to 0
            tIters->item(2, 5)->setText( QString::number( 0., 'g', 3 ));  // set iTau[2] to 0
            tIters->item(2, 7)->setText( QString::number( 0., 'g', 3 ));  // set iNu[2] to 0
            tIters->item(2, 8)->setText( QString::number( 0., 'g', 3 ));  // set ipH[2] to 0
            tIters->item(2, 9)->setText( QString::number( 0., 'g', 3 ));  // set ipe[2] to 0
            break;
   case P_INV_TITR:
            if( subtype == 0 )
            {
                i1 = getNPoints( 7 ); // iNu
                if( i1 == -1 )
                   tIters->item(1, 7)->setText( QString::number( 0.01, 'g', 3 ));  // set iNu[1] to 0.01
                i2 = getNPoints( 6 ); // ipXi
                if( i2 == -1 )
                {
                    setIterColumn( 6, -0.1, 0.1, 1e-11 );
//                   tIters->item(0, 6)->setText( QString::number( -0.1, 'g', 3 ));  // set ipXi[0] to -0.1
//                   tIters->item(1, 6)->setText( QString::number( 0.1, 'g', 3 ));  // set ipXi[1] to 0.1
//                   tIters->item(2, 6)->setText( QString::number( 1e-11, 'g', 4 ));  // set ipXi[2] to 1e-11
                }
                ret = getNPoints( 8 ); // ipH
                if( ret == -1 )
                {
                    setIterColumn( 8, 4., 12., 0.25 );
//                    tIters->item(0, 8)->setText( QString::number( 4., 'g', 3 ));  // set ipH[0] to 4
//                    tIters->item(1, 8)->setText( QString::number( 12., 'g', 3 ));  // set ipH[1] to 12
//                    tIters->item(2, 8)->setText( QString::number( 0.25, 'g', 3 ));  // set ipH[2] to 0.1
                    ret = 33;
                }
                tIters->item(2, 9)->setText( QString::number( 0., 'g', 3 ));  // set ipe[2] to 0
            }
            if( subtype == 1 )
            {
                i1 = getNPoints( 7 ); // iNu
                if( i1 == -1 )
                   tIters->item(1, 7)->setText( QString::number( 0.01, 'g', 3 ));  // set iNu[1] to 0.01
                i2 = getNPoints( 6 ); // ipXi
                if( i2 == -1 )
                {
                    setIterColumn( 6, -0.03, 0.03, 1e-10 );
//                    tIters->item(0, 6)->setText( QString::number( -0.03, 'g', 3 ));  // set ipXi[0] to -0.03
//                    tIters->item(1, 6)->setText( QString::number( 0.03, 'g', 3 ));  // set ipXi[1] to 0.03
//                    tIters->item(2, 6)->setText( QString::number( 1e-10, 'g', 4 ));  // set ipXi[2] to 1e-10
                }
                ret = getNPoints( 9 ); // ipe
                if( ret == -1 )
                {
                    setIterColumn( 9, -12., -2., 0.2 );
//                    tIters->item(0, 9)->setText( QString::number( -12., 'g', 3 ));  // set ipe[0] to -12
//                    tIters->item(1, 9)->setText( QString::number( -2., 'g', 3 ));  // set ipe[1] to -2
//                    tIters->item(2, 9)->setText( QString::number( 0.2, 'g', 3 ));  // set ipe[2] to 0.2
                    ret = 51;
                }
                tIters->item(2, 8)->setText( QString::number( 0., 'g', 3 ));  // set ipH[2] to 0
            }
            i1 = getNPoints( 0 ); // iTm
            if( i1 < ret )
            {
                 tIters->item(2, 0)->setText( QString::number( 1 ));  // set iTm[2] to 1
                 i2 = tIters->item(0, 0)->data(Qt::DisplayRole).toInt(); // get iTm[0]
                 tIters->item(1, 0)->setText( QString::number( i2+ret ));  // set iTm[1]
            }
            tIters->item(2, 1)->setText( QString::number( 0., 'g', 3 ));  // set iV[2] to 0
            tIters->item(2, 2)->setText( QString::number( 0., 'g', 3 ));  // set iP[2] to 0
            tIters->item(2, 3)->setText( QString::number( 0., 'g', 3 ));  // set iTC[2] to 0
            tIters->item(2, 4)->setText( QString::number( 0 ));  // set iNv[2] to 0
            tIters->item(2, 5)->setText( QString::number( 0., 'g', 3 ));  // set iTau[2] to 0
            tIters->item(2, 7)->setText( QString::number( 0., 'g', 3 ));  // set iNu[2] to 0
            break;
    case P_TITRSING:
           if( subtype == 0 )
           {
               ret = 1;
               i1 = isUndefined( 7 ); // iNu
               if( i1 == 1 )
               {
                  setIterColumn( 7, 0.001, 1., 0. );
                  tIters->item(0, 7)->setText( QString::number( 0.001, 'g', 3 ));  // set iNu[0] to 0.01
                  tIters->item(1, 7)->setText( QString::number( 1, 'g', 3 ));  // set iNu[1] to 1
                  tIters->item(2, 7)->setText( QString::number( 0, 'g', 3 ));  // set iNu[2] to 0
               }
               i2 = isUndefined( 8 ); // ipH
               if( i2 == 1 )
               {
                   setIterColumn( 8, 7., 0., 0.01 );
                   tIters->item(0, 8)->setText( QString::number( 7., 'g', 3 ));  // set ipH[0] to 7
                   tIters->item(1, 8)->setText( QString::number( 0, 'g', 3 ));  // set ipH[1] to 0
                   tIters->item(2, 8)->setText( QString::number( 0.01, 'g', 3 ));  // set ipH[2] to 0.01
               }
               tIters->item(2, 0)->setText( QString::number( 0 ));  // set iTm[2] to 0
               tIters->item(2, 1)->setText( QString::number( 0., 'g', 3 ));  // set iV[2] to 0
               tIters->item(2, 2)->setText( QString::number( 0., 'g', 3 ));  // set iP[2] to 0
               tIters->item(2, 3)->setText( QString::number( 0., 'g', 3 ));  // set iTC[2] to 0
               tIters->item(2, 4)->setText( QString::number( 0 ));  // set iNv[2] to 0
               tIters->item(2, 5)->setText( QString::number( 0., 'g', 3 ));  // set iTau[2] to 0
               tIters->item(2, 6)->setText( QString::number( 0., 'g', 3 ));  // set ipXi[2] to 0
               tIters->item(2, 7)->setText( QString::number( 0., 'g', 3 ));  // set iNu[2] to 0
               tIters->item(2, 9)->setText( QString::number( 0., 'g', 3 ));  // set ipe[2] to 0
           }
           // To be done
           break;
    case P_REACTORS:
           ret = getNPoints( 0 ); // iTm
           if( ret <= 1 )
           {
               ret = 101;
               setIterColumn( 0, 1000, 1100, 1 );
           }
           break;
   default: break;
   }
  return ret;
}


//  DK - actually, must be merged into getNPV()    26.02.2010
// set up default
/*
void  ProcessWizard::setIter( char type, int subtype )
{
  switch(type)
   {
   case P_PVT:
      if( isUndefined( 2 ) ) //iP
        setIterColumn( 2, 500, 500, 0 );
      if( isUndefined( 3 ) ) //iTC
        setIterColumn( 3, 400, 700, 1 );
      break;
   case P_SEQUENT:
      if( isUndefined( 7 ) ) //iNu
        setIterColumn( 7, 0.001, 0.999, 0.01 );
      if( isUndefined( 6 ) ) //ipXi
        setIterColumn( 6, 0.001, 7.001, 0.02 );
      break;
   case P_LIP:
      if( isUndefined( 6 ) ) //ipXi
        setIterColumn( 6, -5, 5, 0.1 );
      break;
   case P_INV_TITR:
      if( isUndefined( 7 ) ) //iNu
        setIterColumn( 7, 0, 0.01, 0 );
      if( isUndefined( 6 ) ) //ipXi
        setIterColumn( 6, -0.1, 0.1, 1e-011 );
      if( isUndefined( 8 ) ) //ipH
        setIterColumn( 8, 3.5, 12.5, 0.25 );
      if( isUndefined( 9 ) ) //ipe
        setIterColumn( 9, -12, -2, 0.2 );
      break;
    case P_TITRSING:
           if( isUndefined( 7 ) ) //iNu
             setIterColumn( 7, 0.01, 0, 0 );
           if( isUndefined( 8 ) ) //ipXi
             setIterColumn( 8, 6.4, 0, 0.01 );
           break;
    case P_REACTORS:
           break;
   default: break;
   }

}
*/

void  ProcessWizard::setCalcScript( char type, int subtype )   // get process script
{
  int ii;
  QString ret = textEquat1->toPlainText();
  QStringList lst;
  TCStringArray dclst;

  if( !page1Changed )
      return;

  switch(type)
  {
    case P_PVT:
      if( subtype == 0 )
         c_PsEqn->setChecked(false);
      else
         c_PsEqn->setChecked(true);
      if( subtype == 0 )
        ret = QString("");
      if( subtype == 1 )
        ret = QString("$ Example script for a P,T path"
                      "cTC =: iTC[0] + J*iTC[2];\n"
                      "cP =: iP[0] + J*iP[2];\n"
                      "$cP =: 1 + 2 * cTC + 0.01 * cTC^2;\n"
                      "$ End of script\n");
      if( subtype == 2 )
        ret = QString("$ Calculation of number of TC points\n"
                      "I =: floor( (iTC[1]-iTC[0])/iTC[2] )+1; \n"
                      "$ Calculation of current P value from J index\n"
                      "cP =: iP[0]+ floor( J/I )*iP[2]; vP[J] =: cP; \n"
                      "$ Calculation of current T value from J index \n"
                      "cTC =: iTC[0] + mod( J, I ) * iTC[2];\n"
                      "cT =: cTC+273.15; vT[J] =: cT; \n"
                      "$ End of script\n");
       break;
//-------------------------------------------------------------------------
  case P_SEQUENT:
          c_PsEqn->setChecked(true);
          if( subtype == 0 )
          {
              ret = QString("modC[J] =: cNu;\n");

             for(int jj=0; jj<6; jj++ )
             {
               lst = getSelected( jj );
               gstring oName = aObj[pgData[jj].nObj].GetKeywd();
               for(ii=0; ii<lst.count();ii++)
               {
                  lst[ii] = lst[ii].trimmed();
                  ret  += QString("%1[{%2}] =: cNu * 1;\n").arg(oName.c_str(), lst[ii]);
               }

             }
          }
         if( subtype == 1 )
         {
           QString BL = "BL", CL = "CL";
           lst = getSelected( "DComp" );
           if( lst.count() > 0 )
             BL = lst[0].trimmed();
           if( lst.count() > 1 )
             CL = lst[1].trimmed();
           ret = QString("xd_[{%1}] =: cNu * 1;\n"
                         "xd_[{%2}] =: 1 - cNu * 1;\n").arg(BL, CL);
         }
         if( subtype == 2 )
         {
             ret = QString("modC[J] =: cpXi;\n");

            for(int jj=0; jj<6; jj++ )
            {
              lst = getSelected( jj );
              gstring oName = aObj[pgData[jj].nObj].GetKeywd();
              for(ii=0; ii<lst.count();ii++)
              {
                 lst[ii] = lst[ii].trimmed();
                 ret  += QString("%1[{%2}] =: cXi * 1;\n").arg(oName.c_str(), lst[ii]);
              }

            }
         }
         if( subtype == 3 )
         {
           QString Trace = "Trace", Host = "Host";
           lst = getSelected( "Compos" );
           if( lst.count() > 0 )
             Trace = lst[0].trimmed();
           if( lst.count() > 1 )
             Host = lst[1].trimmed();
           ret = QString("modC[J] =: cpXi;\n"
                         "xa_[{%1}] =: cXi * 1;\n"
                         "xa_[{%2}] =: 1-cXi * 1;\n").arg(Trace, Host);
         }
     break;
     //-------------------------------------------------------------------------
   case P_LIP:
     c_PsEqn->setChecked(true);
     if( subtype == 0 || subtype == 1 )
     {
      QString EM0 = "EM0", EM1 = "EM1";
      lst = getSelected( "Phases" );
      if( lst.count() > 0 )
      {   gstring phname = lst[0].trimmed().toLatin1().data();

          dclst = TProfil::pm->DCNamesforPh( phname.c_str() , true );
          // Here may be message if Invalid phase
          if( dclst.GetCount() > 0 )
             EM0 = dclst[0].c_str();
          if( dclst.GetCount()  > 1 )
             EM1 = dclst[1].c_str();
      }
      ret = QString("if(cXi < 0.5) begin\n"
                    "   xd_[{%1}] =: 1-cXi; xd_[{%2}] =: cXi;\n"
                    "   modC[J] =: cXi;\n"
                    "  end \n"
                    "if( cXi >= 0.5 & cXi <= 2 ) begin\n"
                    "   xd_[{%1}] =: 0.5; xd_[{%2}] =: 0.5; \n"
                    "   modC[J] =: 0.5;\n"
                    "  end \n"
                    "if(cXi > 2) begin \n"
                    "   xd_[{%1}] =: 1/cXi; xd_[{%2}] =: 1-1/cXi; \n"
                    "   modC[J] =: 1-1/cXi;\n"
                    "  end  \n").arg(EM0, EM1);
      }
      break;
  //-------------------------------------------------------------------------
   case P_INV_TITR:
     c_PsEqn->setChecked(true);
     if( subtype == 0 || subtype == 1 )
     {
        QString Acid = "Acid", Base = "Base", TraceM="TraceM";
       lst = getSelected( "AcidBase" );
       if( lst.count() > 0 )
         Acid = lst[0].trimmed();
       if( lst.count() > 1 )
         Base = lst[1].trimmed();
       ret = QString("$ pH sequence of inverse titrations\n"
                     "if ( Next=1) begin \n"
                     "  cNu =: cpH-pH; end \n"
                     "if (Next=2) begin \n"
                     "  xa_[{%2}] =: ((cEh < 0)? 1e-9: cEh);\n"
                     "  xa_[{%1}] =: ((0-cEh < 0)? 1e-9: 0-cEh);\n"
                     "  modC[J] =: cEh; \n").arg( Acid, Base );
       if( lst.count() > 2 )
       {
         TraceM = lst[2].trimmed();
         ret += QString( "xa_[{%3}] =: 10^cpe; \n").arg(TraceM);
       }
       ret += QString( " end \n"
                     "$ modC[J]: acid or base addition\n");
     }
     break;
     //-------------------------------------------------------------------------
   case P_TITRSING:
               c_PsEqn->setChecked(true);
           //    if( subtype == 0 )
               {
                 QString pH = "Output", com = "Input";
                 gstring oName;

                 // select input (titrant)
                 for(int jj=0; jj<4; jj++ )
                 {
                     lst = getSelected( jj );
                     if( lst.count() > 0 )
                     {
                       oName = aObj[pgData[jj].nObj].GetKeywd();
                       lst[0] = lst[0].trimmed();
                       com  = QString("%1[{%2}]").arg(oName.c_str(), lst[0]);
                       break;
                     }
                 }
                 // select output (target function)
                 for(int jj=4; jj<6; jj++ )
                 {
                     lst = getSelected( jj );
                     if( lst.count() > 0 )
                     {
                       if( jj== 4)
                       {  oName = aObj[pgData[jj].nObj].GetKeywd();
                          lst[0] = lst[0].trimmed();
                          pH  = QString("%1[{%2}]").arg(oName.c_str(), lst[0]);
                       }
                       else
                          pH =  lst[0].trimmed();
                       break;
                     }
                 }
                 ret = QString(
                             "$ Difference between given and computed output value \n"
                             " cpH =: (ipH[0]-%2)*sign(ipH[0])*sign(iNu[1]); \n"
                             "if( NeIt[1] = 0 ) \n"
                             "  begin \n"
                             "    ipH[1]=: cpH; cNu=: iNu[0]*sign(iNu[0]); \n"
                             "    iNu[2]=: %1; modC[0] =: cNu; \n"
                             "  end \n"
                             "$ Recalculation of increment size at the sign inversion \n"
                             " cNu =: ( ( cpH * ipH[1] ) <= 0 ? modC[0]*0.382 : modC[0] ); \n"
                             " ipH[1] =: cpH; modC[0] =: cNu; \n"
                             "$ Calculation of new value of input parameter \n"
                             " iNu[2] =: iNu[2] + cNu*sign(cpH); \n"
                             " %1 =: iNu[2]; \n"
                             "$ Check for convergence \n"
                             " Next =: ( (abs(cpH)<ipH[2])|(J>99)? 0: 1 ); \n"
                             "$ End of script \n"
                             ).arg(com, pH);
               }
       break;
   case P_REACTORS:
       {  c_PsEqn->setChecked(true);
          QString Aqg = "aq_gen";
          QString xaName = "Selected_first";

          lst = getSelected( "Phases" );
          if( lst.count() > 0 )
           Aqg = lst[0].trimmed();

          if( subtype == 1 || subtype == 3  ) //xa select_first
          {    lst = getSelected( "Compos" );
              if( lst.count() > 0 )
               xaName = lst[0].trimmed();
          }
          double from, until;
          int iNu = 0, ipXi = 0;
          from = tIters->item(0,7)->data(Qt::DisplayRole).toDouble();
          until = tIters->item(1,7)->data(Qt::DisplayRole).toDouble();
          if( fabs(from)>1e-30 ||  fabs(until)>1e-30 )
              iNu = 1;
          from = tIters->item(0,6)->data(Qt::DisplayRole).toDouble();
          until = tIters->item(1,6)->data(Qt::DisplayRole).toDouble();
          if( fabs(from)>1e-30 ||  fabs(until)>1e-30 )
              ipXi = 1;

          if( subtype == 0  )
          {         // Flushing with SysEq source for solid composition
              ret = QString("$ Fluid-rock interaction (Flushing, SysEq source)\n"
                            "$ cNu: current amount of evolving aqueous fluid\n");
              if( !iNu )
                  ret += QString("$ To use if iNu is not set \n"
                              " cNu =: phM[{%1}]; \n").arg(Aqg);
              ret += QString( " xp_[{%1}] =: cNu; \n"  // potentially incorrect - bXa[0] should be copied to bi_
                                "$ cXi is current solid/water mass ratio \n").arg(Aqg);
              if( !ipXi )
                  ret += QString( "$ Take current mass of solid if ipXi is not set \n"
                              " cXi =: pmXs / cNu; \n" );
              ret += QString( " MbXs =: cXi * cNu; \n"
                              "$ Cumulative reacted solid/water ratio \n"
                             " modC[J] =: ( J>0 ? 10^(modC[J-1])+cXi : cXi );\n"
                             "$ modC[J] =: cXi*cNu;\n"
                             " modC[J] =: lg(modC[J]); \n" );
        }
        if( subtype == 1 )
        {          // Flushing with Compos source for solid composition
            ret = QString("$ Fluid-rock interaction (Flushing, Compos source)\n"
                          "$ cNu: current amount of evolving aqueous fluid\n");
            if( !iNu )
                ret += QString("$ To use if iNu is not set \n"
                            " cNu =: phM[{%1}]; \n").arg(Aqg);
            ret += QString( " xp_[{%1}] =: cNu; \n"
                              "$ cXi is current solid/water mass ratio \n").arg(Aqg);
            if( !ipXi )
                ret += QString( "$ Take current mass of solid if ipXi is not set \n"
                            " cXi =: pmXs / cNu; \n" );
            ret += QString( " xa_[{%1}] =: cXi * cNu; \n"
                            "$ Cumulative reacted solid/water ratio \n"
                           " modC[J] =: ( J>0 ? 10^(modC[J-1])+cXi : cXi );\n"
                           "$ modC[J] =: cXi*cNu;\n"
                           " modC[J] =: lg(modC[J]); \n" ).arg(xaName);
        }
        if( subtype == 2  )
        {      // Leaching with SysEq source for fluid composition
            ret = QString("$ Fluid-rock interaction (Leaching, SysEq source)\n"
                          "$ cNu is current mass of solids (g) \n");
            if( !iNu )
                ret += QString( "$ Current mass of solids is taken if iNu is not set \n"
                                " cNu =: pmXs; \n");
            ret += QString( " MbXs =: cNu; \n" // potentially incorrect - bXs should be copied to bi_
                            "$ Stop, if no solids are left \n"
                            " Next =: ( cNu > 0? 1: 0 ); \n"
                            "if(Next > 0) begin \n"
                            "$ cXi is the water/solid ratio \n");
            if( !ipXi )
                ret += QString( "$ Take the current mass of fluid if ipXi is not set \n"
                              " cXi =:  phM[{%1}]/cNu; \n"
                               ).arg(Aqg);
            ret += QString(  " xp_[{%1}] =: cXi*cNu; \n"
                           "$ Cumulative reacted water/solid ratio \n"
                           " modC[J] =: ( J>0? 10^(modC[J-1])+cXi: cXi ); \n"
                           "$ modC[J] =: lg( cXi*cNu ); \n"
                           " modC[J] =: lg(modC[J]); \n"
                           "end \n"
                           ).arg(Aqg);
      }
       if( subtype == 3  )
        {       // Leaching with Compos source for fluid composition
          ret = QString("$ Fluid-rock interaction (Leaching, Compos source)\n"
                        "$ cNu is current mass of solids (g) \n");
          if( !iNu )
              ret += QString( "$ Current mass of solids is taken if iNu is not set \n"
                              " cNu =: pmXs; \n");
          ret += QString( " MbXs =: cNu; \n"
                          "$ Stop, if no solids are left \n"
                          " Next =: ( cNu > 0? 1: 0 ); \n"
                          "if(Next > 0) begin \n"
                          "$ cXi is the water/solid ratio \n");
          if( !ipXi )
              ret += QString( "$ Take the current mass of fluid if ipXi is not set \n"
                            " cXi =:  phM[{%1}]/cNu; \n"
                             ).arg(Aqg);
          ret += QString(  " xa_[{%1}] =: cXi*cNu; \n"
                         "$ Cumulative reacted water/solid ratio \n"
                         " modC[J] =: ( J>0? 10^(modC[J-1])+cXi: cXi ); \n"
                         "$ modC[J] =: lg( cXi*cNu ); \n"
                         " modC[J] =: lg(modC[J]); \n"
                         "end \n"
                         ).arg(xaName);
     }
       // Cleaning part for all scenarios
        ret += QString( "$ Clean up the rest of the system recipe \n" );
        for(int jj=0; jj<6; jj++ )
        {
          int nO = pgData[jj].nObj;
          lst = getSelected( jj );
          gstring oName = aObj[nO].GetKeywd();

          for(ii=0; ii<lst.count();ii++)
          {
            if( ii == 0 && nO == o_syphm ) // xp aq_gen;
              continue;
            if( ii == 0 && nO == o_syxea &&
                (subtype == 1 || subtype == 3 ) ) //xa select_first
              continue;
           lst[ii] = lst[ii].trimmed();
           ret  += QString("%1[{%2}] =: 0;\n").arg(oName.c_str(), lst[ii]);
          }
        }
        ret += QString( "$ end of script\n" );
     }
     break;
     default: break;
     }

 // if( !ret.isEmpty() )
    textEquat1->setText(ret);
 }


void  ProcessWizard::setOutScript( char type, int subtype)   // get output script
{
  int ii;
  QString ret = pageScript->textScript->toPlainText();
  QStringList lst;
  TCStringArray rowNames;
  TCStringArray dclst;

  if( !page1Changed )
          return;

  pGraphX->setValue( 1 );
  switch(type)
  {
    case P_PVT:
      if( subtype == 2 )
      {
          c_PvT->setChecked(true);
          c_PvP->setChecked(true);
          lst = getSelected( "Phases" );
          if( lst.count() < 1 )
           return;
          ret = QString("$ Plotting calculated grid as T,P diagram \n"
                        "xp[J] =: vT[J];\n"
                        "$ Ordinates: Presence of the phase in equilibrium\n");
          for(ii=0; ii<lst.count();ii++)
          {
              lst[ii] = lst[ii].trimmed();
              rowNames.Add(lst[ii].toLatin1().data());
              ret  += QString("yp[J][%1] =: (Xa[{%2}]>Pa_DS? vP[J]/10: empty());\n").arg(
                    ii).arg( lst[ii]);
          }
          ret  += QString("$ End of script\n");
          pageScript->setXname("vT");
          pageScript->setYname("vP");
          pGraph->setValue( lst.count() );
      }

      break;
  //-------------------------------------------------------------------------
  case P_SEQUENT:
         if( subtype == 1 )
         {
           QString BL, CL;
           lst = getSelected( "DComp" );
           if( lst.count() < 2 )
             return;
           BL = lst[0].trimmed();
           CL = lst[1].trimmed();

           QString b_ion, c_ion;
           lst = getSelected( "Molality" );
           if( lst.count() < 2 )
             return;
           b_ion = lst[0].trimmed();
           c_ion = lst[1].trimmed();
           // The script below corrected on 13.11.2012 by DK
           ret = QString(
            "$ Abscissa - mole fraction of ML in solid solution\n"
            "xp[J] =: Wxx[{%1}];\n"
            "$ log(Kd(Host)) in kgH2O\n"
            "yp[J][0] =: lg( Wxx[{%2}] / my[{%4}] );\n"
            "$ log(Kd(Minor)) in kgH2O\n"
            "yp[J][1] =: lg( Wxx[{%1}] / my[{%3}] );\n"
            "$ log(D), dimensionless\n"
            "$ yp[J][2] =: lg( Wxx[{%1}] / my[{%3}] / (Wxx[{%2}] / my[{%4}]) );\n"
            "yp[J][2] =: yp[J][1] - yp[J][0]; \n"
            "$ Done\n").arg(BL, CL, b_ion, c_ion);
           rowNames.Add("log(Kd(H))");
           rowNames.Add("log(Kd(M))");
           rowNames.Add("log(D_MH)");
           pGraph->setValue( 3 );
           pageScript->setXname("x(Minor)");
           pageScript->setYname("log10");
         }
         if( subtype == 3 )
         {
           QString TraceE1, HostE1;
           lst = getSelected( "Sorbed" );
           if( lst.count() < 2 )
             return;
           TraceE1 = lst[0].trimmed();
           HostE1 = lst[1].trimmed();

           ret = QString(
            "$ Abscissa - log total molality of Trace element\n"
            "xp[J] =: lgm_t[{%1}];\n"
            "$ lg(Concentration of Trace element sorbed)\n"
            "yp[J][0] =: lg( bXs[{%1}]/( bXs[{%1}]+ bXs[{%2}]) );\n"
            "$ lg(Kd(Trace element))\n"
            "yp[J][1] =: lg( bXs[{%1}]/( bXs[{%1}]+ bXs[{%2}])/m_t[{%1}] );\n"
            "$ lg(D) Trace-Host\n"
            "yp[J][2] =: yp[J][1]\n"
            "    - lg( bXs[{%2}]/( bXs[{%1}]+ bXs[{%2}])/m_t[{%2}] );\n"
            "$ Done\n").arg(TraceE1, HostE1);
           rowNames.Add("x(Trace)");
           rowNames.Add("Kd(Trace)");
           rowNames.Add("D(Trace)");
           pGraph->setValue( 3 );
           pageScript->setXname("log_m(Tr)");
           pageScript->setYname("log10");
         }
     break;
  //-------------------------------------------------------------------------
   case P_LIP:
     if( subtype == 0 )
     {
       QString EM1;
       lst = getSelected( "Phases" );
       if( lst.count() < 1 )
         return;
       gstring phname = lst[0].trimmed().toLatin1().data();
       dclst = TProfil::pm->DCNamesforPh( phname.c_str(), true );
       if( dclst.GetCount() < 2 )
         return;
       EM1 = dclst[1].c_str();

       QString ComIon, EMion0, EMion1;
       lst = getSelected( "AqIons" );
       if( lst.count() < 3 )
         return;
       ComIon = lst[0].trimmed();
       EMion0 = lst[1].trimmed();
       EMion1 = lst[2].trimmed();

       ret = QString(
        "$ Abscissa - Solidus \n"
        "xp[J][0] =: Wxx[{%1}]; \n"
        "$ Abscissa - Solutus \n"
        "xp[J][1] =: 10^lga[{%4}] / ( 10^lga[{%3}] + 10^lga[{%4}] ); \n"
        "$ Ordinates - log10 of Lippmann's total solubility product \n"
        "yp[J][0] =: lga[{%2}] + lg( 10^lga[{%3}] +  10^lga[{%4}] );\n"
        "yp[J][1] =: yp[J][0];\n"
        "$ Done \n" ).arg(EM1, ComIon, EMion0, EMion1);
       rowNames.Add("Solidus");
       rowNames.Add("Solutus");
       pGraph->setValue( 2 );
       pGraphX->setValue( 2 );
       pageScript->setYname("logSigPi");
       pageScript->setXname("x(s);x(aq)");
     }
     if( subtype == 1 )
     {
/*         QString EM1;
         lst = getSelected( "Phases" );
         if( lst.count() < 1 )
           return;
         gstring phname = lst[0].trimmed().toLatin1().data();
         dclst = TProfil::pm->DCNamesforPh( phname.c_str(), true );
         if( dclst.GetCount() < 2 )
           break;
         EM1 = dclst[1].c_str();
*/
         QString ComIC, EM0IC, EM1IC;
         lst = getSelected( "AqElements" );
         if( lst.count() < 3 )
           return;
         ComIC = lst[0].trimmed();
         EM0IC = lst[1].trimmed();
         EM1IC = lst[2].trimmed();

         ret = QString(
          "$ Abscissa - Solidus \n"
          "xp[J][0] =: bXs[{%3}] / ( bXs[{%2}] + bXs[{%3}] ); \n"
          "$ Abscissa - Solutus \n"
          "xp[J][1] =: m_t[{%3}] / ( m_t[{%2}] + m_t[{%3}] ); \n"
          "$ Ordinates - log10 of total dissolved elements product \n"
          "yp[J][0] =: lg( m_t[{%1}] ) + lg( m_t[{%2}] +  m_t[{%3}] ); \n"
          "yp[J][1] =: yp[J][0]; \n"
          "$ Done \n").arg( ComIC, EM0IC, EM1IC );  // removed EM1 as first argument
         rowNames.Add("Solidus");
         rowNames.Add("Solutus");
         pGraph->setValue( 2 );
         pGraphX->setValue( 2 );
         pageScript->setYname("lgSigPiT");
         pageScript->setXname("x(s);x(aq)");
     }
     break;
  //-------------------------------------------------------------------------
  case P_INV_TITR:
        if( subtype == 1 )
        {
          QString M_c1;
          lst = getSelected( "Molality" );
          if( lst.count() <1  )
           return;
          M_c1 = lst[0].trimmed();
          ret = QString( "xp[J]=: lg(my[{%1}]").arg(M_c1);
          for( ii=1; ii<lst.count(); ii++)
          {
             M_c1 = lst[ii].trimmed();
             ret += QString(" + my[{%1}]").arg( M_c1);
          }
          ret += QString (");\n");

          ret += QString("$ ... more molalities of M aqueous complexes can be added \n");
          ret += QString("cTau =: 0.001;\n");
          ret += QString("$ Change the above to assign a real sorbent mass in kg\n" );
          lst = getSelected( "Sorbed" );
          if( lst.count() <1  )
           return;
          for( ii=0; ii<lst.count(); ii++)
          {
             M_c1 = lst[ii].trimmed();
             rowNames.Add(M_c1.toLatin1().data());
             ret += QString(" yp[J][%1]=: (x[{%2}]> 0? \n"
                            "   lg(x[{%2}]/cTau) : empty() ); \n").arg( QString("%1").arg(ii),M_c1);
          }
          ret += QString("$ log total M sorbed in mol/kg \n");
          rowNames.Add("log sorbed");
          ret += QString( "yp[J][%1] =: lg((").arg(ii);
          for( ii=0; ii<lst.count(); ii++)
          {
             M_c1 = lst[ii].trimmed();
             if( ii > 0) ret += QString(" + ");
             ret += QString("x[{%1}]").arg( M_c1);
          }
          ret += QString( ")/cTau); \n$ log Kd \n");
          rowNames.Add("log Kd");
          ret += QString( "yp[J][%1] =: ").arg(ii+1);
          ret += QString( " yp[J][%1]- xp[J];\n").arg(ii);
          pGraph->setValue( ii+2 );
          pageScript->setXname("log_m");
          pageScript->setYname("logC;logKd");
        }
         break;
//-------------------------------------------------------------------------
   case P_TITRSING:
               if( subtype == 0 )
               {
                 QString pH, com;
                 gstring oName;

                 // find titrant
                 for(ii=0; ii<4; ii++ )
                 {
                     lst = getSelected( ii );
                     if( lst.count() > 0 )
                     {
                       oName = aObj[pgData[ii].nObj].GetKeywd();
                       lst[0] = lst[0].trimmed();
                       com  = QString("%1[{%2}]").arg(oName.c_str(), lst[0]);
                       break;
                     }
                 }
                 // find function
                 for(ii=4; ii<6; ii++ )
                 {
                     lst = getSelected( ii );
                     if( lst.count() > 0 )
                     {
                       if( ii== 4)
                       {  oName = aObj[pgData[ii].nObj].GetKeywd();
                          lst[0] = lst[0].trimmed();
                          pH  = QString("%1[{%2}]").arg(oName.c_str(), lst[0]);
                       }
                       else
                          pH =  lst[0].trimmed();
                       break;
                     }
                 }
                 if( pH.isEmpty() || com.isEmpty() )
                   return;
                 ret = QString(
                         "xp[0] =: %2; \n"
                         "yp[0][0] =: %1;  \n"
                         "yp[0][1] =: %2 - ipH[0]; \n").arg(com, pH);
                pGraph->setValue( 2 );
                pageScript->setXname(pH.toLatin1().data());
                pageScript->setYname(com.toLatin1().data());
               }
       break;
   case P_REACTORS:
        // No special script actions for now
   default: break;
     }

   if( rowNames.GetCount() > 0)
       pageScript->setNames(rowNames);
   pageScript->textScript->setText( ret );
 }

//=================================================================================
// special slots

void ProcessWizard::CmSetMode()
{
  setMode(subTypeButtons->checkedId());
}

void ProcessWizard::setMode(int subtype)
{
    char type = getType();
    switch(type)
    {
    case P_PVT:   page1Changed = true;
                setCalcScript( type, subtype );
                pLsts[0]->setDisabled(subtype!=2);
                listObj->setDisabled(subtype!=2);
                break;
    case P_REACTORS:
    case P_TITRSING:
    case P_INV_TITR:
    case P_LIP:
    case P_SEQUENT: page1Changed = true;
                 setCalcScript( type, subtype );
                break;
    default : break;
    }
}

void ProcessWizard::CmItersEdit(int , int )
{
    tItersChanged = true;
}

void ProcessWizard::changePage( int nPage )
{
    if(nPage < 0 )
      return;
    pageLists->setCurrentIndex ( nPage );
  }


// ==================================================================================
// Internal functions

void ProcessWizard::setupPages()
{
  int nO;
  QString str;
  //QListWidgetItem* item1;
  QWidget* page1;
  QHBoxLayout* horizontalLayout1;
  QListWidget* lstIndexes1;
  TCStringArray lst;

  sub1->setChecked(true);
  sub1->setVisible(true);
  sub2->setVisible(true);
  sub3->setVisible(true);
  sub4->setVisible(true);

  // clear old pages
    if( pageLists->count() > 0)
    {
       while(pageLists->count()>0)
      //  for(jj=pageLists->count()-1; jj>0; jj--)
      {
        pageLists->removeWidget( pageLists->widget(pageLists->count()-1/*jj*/) );
      }
    }
    pLsts.clear();  // and all connect
    listObj->clear();
    listObj->setDisabled(false);

   // init new pages
   for(uint ii=0; ii<pgData.GetCount();ii++)
    {
      nO = pgData[ii].nObj;
      TProfil::pm->getNamesList( nO, lst);

      str = QString("%1").arg( pgData[ii].pageName.c_str());
      /*item1 =*/ new QListWidgetItem( str,  listObj);

      // add page
      page1 = new QWidget();
      horizontalLayout1 = new QHBoxLayout(page1);
      lstIndexes1 = new QListWidget(page1);
      lstIndexes1->setWrapping( true );
      lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
      horizontalLayout1->addWidget(lstIndexes1);
      pageLists->addWidget(page1);

      // insert items to list of indexes
       for(uint  jj=0; jj<lst.GetCount(); jj++ )
          {
             /*item1 =*/ new QListWidgetItem( lst[jj].c_str(), lstIndexes1);
          }
       pLsts.append(lstIndexes1);
    }
}

QStringList  ProcessWizard::getSelected( const char *name )
{
    QStringList ret;

    QList<QListWidgetItem*> itms = listObj->findItems(name, Qt::MatchExactly);
    if(itms.count()<1)
      return ret;

    ret = getSelected( listObj->row(itms[0]) );

   return ret;
}


QStringList  ProcessWizard::getSelected( int nI )
{
    QStringList ret;
    QListWidgetItem*  ndx;
    QListWidget *lst = pLsts[nI];

    foreach( ndx,  lst->selectedItems()  )
     ret << ndx->text();

   return ret;
}

int  ProcessWizard::getNPoints( int col )
 {
     double from, until, step;
     int nP = 1;

     from = tIters->item(0,col)->data(Qt::DisplayRole).toDouble();
     until = tIters->item(1,col)->data(Qt::DisplayRole).toDouble();
     step = tIters->item(2,col)->data(Qt::DisplayRole).toDouble();

     if( fabs(step) < 1e-30 )
     {
         if( fabs(from) > 1e-30 || fabs(until) > 1e-30 )
             nP = 1;            // changed by DK 25.02.10
         else nP = -1;
     }
     else {
         if( fabs(until) > 1e-30 )
             nP  = (int)((until-from)/step+1.000001);
         else nP = -1;          // changed by DK 21.05.10
     }
     if( ( getType() == 'G'  && col == 6 ) || ( getType() == 'T' && col == 8 )) // workaround for checking min.addition of titrant
         nP = -1;
      if( (nP < 1 || nP > 9999) && nP != -1 )
      {
          gstring str = tIters->horizontalHeaderItem( col )->text().toLatin1().data();
          vfMessage(this, str.c_str(), "Wrong number of steps - please, check values in this iterator!");
          return nP;
      }
     return nP;
 }

// test data in column col
int  ProcessWizard::isUndefined( int col )
 {
     double from, until, step;

     from = tIters->item(0,col)->data(Qt::DisplayRole).toDouble();
     until = tIters->item(1,col)->data(Qt::DisplayRole).toDouble();
     step = tIters->item(2,col)->data(Qt::DisplayRole).toDouble();

     if( fabs(from) < 1e-30 && fabs(until) < 1e-30 && fabs(step) < 1e-30  )
         return 1;
     else
         return 0;

 }

// set default data to column col
void  ProcessWizard::setIterColumn( int col, double from, double until, double step )
 {
    tIters->item(0,col)->setText( QString::number (  from, 'g', 5 ));
    tIters->item(1,col)->setText( QString::number (  until, 'g', 5 ));
    tIters->item(2,col)->setText( QString::number (  step, 'g', 5 ));
    tItersChanged = true;
 }

void  ProcessWizard::setIterColumn( int col, int from, int until, int step )
 {
    tIters->item(0,col)->setText( QString::number (  from ));
    tIters->item(1,col)->setText( QString::number (  until ));
    tIters->item(2,col)->setText( QString::number (  step ));
    tItersChanged = true;
 }

//==============================================================================

equatSetupData eqPr( "xp", "yp", "J", "J", true );

// work with lists
void ProcessWizard::resetPageList(const char* aXname, const char* aYname)
{

    TIArray<pagesSetupData> scalarsList;
    TIArray<pagesSetupData> pgData;

    if(  rt[RT_SYSEQ].GetStatus() != ONEF_) // read first record in DB
        {
            TCStringArray aRklist;
            TCIntArray anRk;
            int Nr = rt[RT_SYSEQ].GetKeyList( ALLKEY, aRklist, anRk );
            if( Nr > 0 )
              TProfil::pm->loadSystat( aRklist[0].c_str() );
        }

    GetListsnRT( MD_MULTI, pgData,  scalarsList );
    GetListsnRT( MD_SYSTEM, pgData,  scalarsList );
    GetListsnRT( MD_MTPARM, pgData,  scalarsList );
    GetListsnRT( MD_RMULTS, pgData,  scalarsList );
    GetListsnRT( RT_PROCES, pgData,  scalarsList );

//    if( pageScript )
//      pageScript->resetPageList( newRT, pgData, scalarsList );
//    else
    {
      pageScript = new EquatSetup( page_5, eqPr,
              RT_PROCES, pgData, scalarsList, outScript.c_str(), aXname, aYname  );
      verticalLayout->addWidget(pageScript);
    }

}

//-------------------------------------------------------------------------------------
// TProcesDelegate -  individual items in views are rendered and edited using delegates
//-------------------------------------------------------------------------------------

TProcesDelegate::TProcesDelegate( QObject * parent ):
        QItemDelegate( parent )
{
}

// Editing System table
QWidget *TProcesDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &/*option*/,
        const QModelIndex &index) const
{
   int nObj = o_petmi;

   switch( index.column() )
   {
   case 0: nObj = o_petmi; break;
   case 4: nObj = o_penvi; break;
   case 2: nObj = o_pepi; break;
   case 3: nObj = o_peti; break;
   case 1: nObj = o_pevi; break;
   case 5: nObj = o_petaui; break;
   case 6: nObj = o_pepxii; break;
   case 7: nObj = o_penui; break;
   case 8: nObj = o_pephi; break;
   case 9: nObj = o_pepei; break;
   }

   TCellInput* editor;
   if( nObj == o_petmi || nObj == o_penvi )
   {
     FieldInfo fld = FieldInfo( nObj, ftNumeric, 20, false, NextP, eYes, stIO, 1, 1);
     editor =  new TCellInput( fld, 0, 0, parent);

   }
   else
   { FieldInfo fld = FieldInfo( nObj, ftFloat, 20, false, NextP, eYes, stIO, 1, 1);
     editor =  new TCellInput( fld, 0, 0, parent);
   }
   return editor;
}


//--------------------- End of ProcessWizard.cpp ---------------------------
