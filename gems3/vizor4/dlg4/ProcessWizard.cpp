//-------------------------------------------------------------------
// $Id: ProcessWizard.cpp 968 2007-12-13 13:23:32Z gems $                   
//
// Implementation of ProcessWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
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

const char *PROCESS_HTML = "gm_proces";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qvariant.h>

#include "ProcessWizard.h"
#include "gstring.h"
#include "visor_w.h"
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
        pGraph->setValue( nLines );

    if( ndx == 0)
    {
      defineWindow( type);
    }

    if( ndx == 1)
    {
        int subtype = subTypeButtons->checkedId();
        pPoints->setValue(  getNPV( type, subtype));
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


ProcessWizard::ProcessWizard( const char* pkey, char flgs[24], int size[6],
                    short tabInt[6], double tabDoubl[24],
            const char *acalcScript, const char *aoutScript, QWidget* parent):
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
   if( !pselP->isChecked() )
     flgs[10] = '+';

//Page2
   subTypeButtons = new QButtonGroup( boxTypes );
   subTypeButtons->addButton(sub1, 0);
   subTypeButtons->addButton(sub2, 1);
   subTypeButtons->addButton(sub3, 2);
   QObject::connect( subTypeButtons, SIGNAL(buttonClicked(int)), this, SLOT(setMode(int)));

   textEquat1->setText(calcScript.c_str());

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
item->setText( QString::number (  tabDoubl[0], 'g', 3 ));
tIters->setItem(0, 2, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[1], 'g', 3 ));
tIters->setItem(1, 2, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[2], 'g', 3 ));
tIters->setItem(2, 2, item);

// iTC
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[3], 'g', 3 ));
tIters->setItem(0, 3, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[4], 'g', 3 ));
tIters->setItem(1, 3, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[5], 'g', 3 ));
tIters->setItem(2, 3, item);

// iV
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[6], 'g', 3 ));
tIters->setItem(0, 1, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[7], 'g', 3 ));
tIters->setItem(1, 1, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[8], 'g', 3 ));
tIters->setItem(2, 1, item);

// iTau
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[9], 'g', 3 ));
tIters->setItem(0, 5, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[10], 'g', 3 ));
tIters->setItem(1, 5, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[11], 'g', 3 ));
tIters->setItem(2, 5, item);

// ipXi
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[12], 'g', 3 ));
tIters->setItem(0, 6, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[13], 'g', 3 ));
tIters->setItem(1, 6, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[14], 'g', 3 ));
tIters->setItem(2, 6, item);

// iNu
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[15], 'g', 3 ));
tIters->setItem(0, 7, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[16], 'g', 3 ));
tIters->setItem(1, 7, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[17], 'g', 3 ));
tIters->setItem(2, 7, item);

// iPh
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[18], 'g', 3 ));
tIters->setItem(0, 8, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[18], 'g', 3 ));
tIters->setItem(1, 8, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[20], 'g', 3 ));
tIters->setItem(2, 8, item);

// ipe
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[21], 'g', 3 ));
tIters->setItem(0, 9, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[22], 'g', 3 ));
tIters->setItem(1, 9, item);
item = new QTableWidgetItem();
item->setText( QString::number (  tabDoubl[23], 'g', 3 ));
tIters->setItem(2, 9, item);

for(int row=0; row<3; row++)
 for(int col=0; col<10; col++)
 {
   item = tIters->item(row,col);
   item->setFlags( item->flags() | Qt::ItemIsEditable);
   item->setTextAlignment(Qt::AlignRight);
 }

//Page3
   resetPageList();

//Page4
    pPoints->setValue(size[0]);
    pMods->setValue(size[1]);
    pGraph->setValue(size[2]);
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

void   ProcessWizard::getSizes( int size[6] )
{

    size[0]= pPoints->value();
    size[1]= pMods->value();
    size[2]= pGraph->value();
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
  char type=P_SYST;

  if( pselT->isChecked())
   type='T';
  else if( pselG->isChecked())
           type=P_INV_TITR;
       else if( pselR->isChecked())
                type='R';
          else if( pselP->isChecked())
                type=P_PVT;
                else  if( pselS->isChecked())
                           type=P_SYST;
                    else  if( pselL->isChecked())
                                type=P_LIP;
  return type;
}

void   ProcessWizard::getFlags( char flgs[24] )
{
  char type=getType();

  switch( type )
   {
    case 'T': strncpy( flgs, "0++-+--++-+-", 12);
        break;
    case P_INV_TITR: strncpy( flgs, "0++-+--+--++", 12);
        break;
    case 'R': strncpy( flgs, "0++-++-+--+-", 12);
        break;
    case P_SYST: strncpy( flgs, "0++-+-----+-", 12);
        break;
    case P_LIP: strncpy( flgs, "0++-+-----+-", 12);
       break;
    case P_PVT:
    default:  memcpy( flgs, "0*----------", 12 );
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

   if( c_PsEqn->isChecked() ) //setted before, not switch off
       flgs[10] = '+';

   if( c_PsSY->isChecked() ) //setted before, not switch off
       flgs[4] = '+';
    else flgs[4] = '-';

//Putting flags, using sizes  is out of function
}

void
ProcessWizard::help()
{
   pVisorImp->OpenHelp( PROCESS_HTML, 0, this/*, true*/ );
}

//=============================================================================
// calcScript for diferent types of Processes

void ProcessWizard::defineWindow(char type)
{
  int jj;

  page1Changed = false;
  if( type == curType )
    return;
  curType = type;

  // define object pages
    pgData.Clear();
    switch(type)
    {
     case P_PVT:
            pgData.Add( new pagesSetupData("Xa", o_w_xf));
            break;
     case P_SYST:
            pgData.Add( new pagesSetupData("xa_", o_syxea));
            pgData.Add( new pagesSetupData("xd_", o_syxed));
            pgData.Add( new pagesSetupData("bi_", o_sybi));
            pgData.Add( new pagesSetupData("my",o_wd_ym ));
            pgData.Add( new pagesSetupData("bXs", o_wo_bfc));
           break;
    case P_LIP:
           pgData.Add( new pagesSetupData("xd_", o_syxed));
           pgData.Add( new pagesSetupData("lga",o_wd_yla ));
           pgData.Add( new pagesSetupData("m_t", o_wd_icm));
          break;
    case P_INV_TITR:
          pgData.Add( new pagesSetupData("xa_", o_syxea));
          pgData.Add( new pagesSetupData("my",o_wd_ym ));
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
         sub1->setText("No script");
         sub2->setText("User defined script");
         sub3->setText("PT phase diagram");
         if( c_PsEqn->isChecked() )
           sub2->setChecked(true); // to left an old script
         else
           sub1->setChecked(true); // to left an old script

         pLsts[0]->setDisabled(true);
         listObj->setDisabled(true);
       }
       break;
   case P_SYST:
       {
         sub1->setText("iNu linear");
         sub2->setText("Kd output");
         sub3->setText("ipXi logarithmic");
         QObject::connect( pLsts[0], SIGNAL(itemSelectionChanged()),
                           this, SLOT(CmSetMode()));
         QObject::connect( pLsts[1], SIGNAL(itemSelectionChanged()),
                           this, SLOT(CmSetMode()));
         QObject::connect( pLsts[2], SIGNAL(itemSelectionChanged()),
                           this, SLOT(CmSetMode()));
       }
       break;
   case P_LIP:
       {
         sub1->setText("Classic Lippmann diagram");
         sub2->setText("Variant with total dissolved concentrations");
         sub3->hide();
         QObject::connect( pLsts[0], SIGNAL(itemSelectionChanged()),
                           this, SLOT(CmSetMode()));
        }
         break;
   case P_INV_TITR:
         {
           sub1->setText("pH diagram");
           sub2->setText("Sorption isotherms at constant pH");
           sub3->hide();
           QObject::connect( pLsts[0], SIGNAL(itemSelectionChanged()),
                             this, SLOT(CmSetMode()));
          }
          break;

   default: break;
   }

}

int  ProcessWizard::getNPV( char type, int subtype)             // get number of points
{
  int i1, i2, ret = pPoints->value();

  if( !page1Changed && !tItersChanged )
          return ret;

  switch(type)
   {
   case P_PVT: i1 = getNPoints( 2 ); // nP
             i2 = getNPoints( 3 ); // nT
             if( subtype == 2 )
               ret = i1*12;
             else
             { if(i1==1)
                 ret = i2;
               else if(i2==1)
                      ret = i1;
                    else
                      ret = min( i1, i2);
              }
            break;
   case P_SYST:
             if( subtype == 2 )
               ret = getNPoints( 6 ); // iPxi
             else
               ret = getNPoints( 7 ); // iNu
            break;
   case P_LIP:
            ret = getNPoints( 6 ); // iPxi
            break;
   case P_INV_TITR:
            if( subtype == 0 )
              ret = getNPoints( 8 ); // ipH
            if( subtype == 1 )
              ret = getNPoints( 9 ); // ipe
           break;

   default: break;
   }
  return ret;
}

void  ProcessWizard::setCalcScript( char type, int subtype )   // get process script
{
  int ii;
  QString ret = textEquat1->toPlainText();
  QStringList lst;

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
      if( subtype == 2 )
        ret = QString("$ Calculation of number of TC points\n"
                      "I =: floor( (iTC[1]-iTC[0])/iTC[2] )+1; \n"
                      "$ Calculation of current P value from J index\n"
                      "cP =: iP[0]+ floor( J/I )*iP[2]; vP[J] =: cP; \n"
                      "$ Calculation of current T value from J index \n"
                      "cTC =: iTC[0] + mod( J, I ) * iTC[2];\n"
                      "cT =: cTC+273.15; vT[J] =: cT; \n"
                      "$ End of script");
       break;
//-------------------------------------------------------------------------
  case P_SYST:
          c_PsEqn->setChecked(true);
          if( subtype == 0 )
          {
              ret = QString("modC[J] =: cNu;\n");
              lst = getSelected( pLsts[0] );
              for(ii=0; ii<lst.count();ii++)
              {
                  lst[ii] = lst[ii].trimmed();
                  ret  += QString("xa_[{%1}] =: cNu * 1.;\n").arg( lst[ii]);
              }
              lst = getSelected( pLsts[1] );
              for(ii=0; ii<lst.count();ii++)
              {
                  lst[ii] = lst[ii].trimmed();
                  ret  += QString("xd_[{%1}] =: cNu * 1.;\n").arg( lst[ii]);
              }
              lst = getSelected( pLsts[2] );
              for(ii=0; ii<lst.count();ii++)
              {
                  lst[ii] = lst[ii].trimmed();
                  ret  += QString("bi_[{%1}] =: cNu * 1.;\n").arg( lst[ii]);
              }
          }
         if( subtype == 1 )
         {
           QString BL = "BL", CL = "CL";
           lst = getSelected( pLsts[1] );
           if( lst.count() > 0 )
             BL = lst[0].trimmed();
           if( lst.count() > 1 )
             CL = lst[1].trimmed();
           ret = QString("xd_[{%1}] =: cNu;\n"
                         "xd_[{%2}] =: 1-cNu;\n").arg(BL, CL);
         }
         if( subtype == 2 )
         {
           QString Trace = "Trace", Host = "Host";
           lst = getSelected( pLsts[0] );
           if( lst.count() > 0 )
             Trace = lst[0].trimmed();
           if( lst.count() > 1 )
             Host = lst[1].trimmed();
           ret = QString("cNu =: 1/cXi;\n"
                         "xa_[{%1}] =: cNu;\n"
                         "xa_[{%2}] =: 1-cNu;\n"
                         "modC[J] =: cXi;\n").arg(Trace, Host);
         }
     break;
     //-------------------------------------------------------------------------
   case P_LIP:
     c_PsEqn->setChecked(true);
     if( subtype == 0 || subtype == 1 )
     {
      QString EM0 = "EM0", EM1 = "EM1";
      lst = getSelected( pLsts[0] );
      if( lst.count() > 0 )
        EM0 = lst[0].trimmed();
      if( lst.count() > 1 )
        EM1 = lst[1].trimmed();
      ret = QString("if(cXi < 0.5) begin\n"
                    "  xd_[{%1}] =: 1-cXi; xd_[{%2}] =: cXi;\n"
                    "  modC[J] =: cXi;\n"
                    " end \n"
                    "if( cXi >= 0.5 & cXi <= 2 ) begin\n"
                    "   xd_[{%1}] =: 0.5; xd_[{%2}] =: 0.5; \n"
                    "   modC[J] =: 0.5;\n"
                    "  end \n"
                    " if(cXi > 2) begin \n"
                    "  xd_[{%1}] =: 1/cXi; xd_[{%2}] =: 1-1/cXi; \n"
                    "  modC[J] =: 1/cXi;\n"
                    " end  \n").arg(EM0, EM1);
      }
      break;
  //-------------------------------------------------------------------------
   case P_INV_TITR:
     c_PsEqn->setChecked(true);
     if( subtype == 0 || subtype == 1 )
     {
        QString Asid = "Asid", Base = "Base", TraceM="TraceM";
       lst = getSelected( pLsts[0] );
       if( lst.count() > 0 )
         Asid = lst[0].trimmed();
       if( lst.count() > 1 )
         Base = lst[1].trimmed();
       ret = QString("$ pH sequence of inv. titrations\n"
                     "if ( Next=1) begin \n"
                     "  cNu =: cpH-pH; end \n"
                     "if (Next=2) begin \n"
                     "  xa_[{%2}] =: ((cEh < 0)? 1e-9: cEh);\n"
                     "  xa_[{%1}] =: ((0-cEh < 0)? 1e-9: 0-cEh);\n"
                     "  modC[J] =: cEh; \n").arg(Asid, Base);
       if( lst.count() > 2 )
       {
         TraceM = lst[2].trimmed();
         ret += QString( "xa_[{%1}] =: 10^cpe; \n").arg(TraceM);
       }
       ret += QString( " end \n"
                     "$ modC[J]: acid or base added\n");
     }

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

  switch(type)
  {
    case P_PVT:
      if( subtype == 2 )
      {
          c_PvT->setChecked(true);
          c_PvP->setChecked(true);
          lst = getSelected( pLsts[0] );
          if( lst.count() < 1 )
           return;
          ret = QString("$ Plotting calculated grid as T,P diagram \n"
                        "xp[J] =: vT[J];\n"
                        "$ Ordinates: Presence of the phase in equilibrium\n");
          for(ii=0; ii<lst.count();ii++)
          {
              lst[ii] = lst[ii].trimmed();
              ret  += QString("yp[J][%1] =: (Xa[{%2}]>1e-8? vP[J]/10: empty());\n").arg(
                    ii).arg( lst[ii]);
          }
      }
      break;
  //-------------------------------------------------------------------------
  case P_SYST:
         if( subtype == 1 )
         {
           QString BL, CL;
           lst = getSelected( pLsts[1] );
           if( lst.count() < 2 )
             return;
           BL = lst[0].trimmed();
           CL = lst[1].trimmed();

           QString b_ion, c_ion;
           lst = getSelected( pLsts[3] );
           if( lst.count() < 2 )
             return;
           b_ion = lst[0].trimmed();
           c_ion = lst[1].trimmed();

           ret = QString(
            "$ Abscissa - mole fraction of BL in solid solution\n"
            "xp[J] =: Wxx[{%1}];\n"
            "$ log(Kd(C)) in kgH2O\n"
            "yp[J][0] =: lg( Wxx[{%2}] / my[{%4}] );\n"
            "$ log(Kd(B)) in kgH2O\n"
            "yp[J][1] =: lg( Wxx[{%1}] / my[{%3}] );\n"
            "$ log(D), dimensionless\n"
            "$ yp[J][2] =: lg( Wxx[{%1}] / my[{%3}] / (Wxx[{%2}] / my[{%4}]) );\n"
            "yp[J][2] =: yp[J][1] - yp[J][0]; \n"
            "$ Done\n").arg(BL, CL, b_ion, c_ion);
         }
         if( subtype == 2 )
         {
           QString TraceE1, HostE1;
           lst = getSelected( pLsts[4] );
           if( lst.count() < 2 )
             return;
           TraceE1 = lst[0].trimmed();
           HostE1 = lst[1].trimmed();

           ret = QString(
            "$ Abscissa - log total molality of Trace element\n"
            "xp[J] =: lgm_t[{%1}];\n"
            "$ lg(x Trace end member)\n"
            "yp[J][0] =: lg( bXs[{%1}]/( bXs[{%1}]+ bXs[{%2}]) );\n"
            "$ lg(kd(TraceEl))\n"
            "yp[J][1] =: lg( bXs[{%1}]/( bXs[{%1}]+ bXs[{%2}])/m_t[{%1}] );\n"
            "$ lg(D) host-trace \n"
            "yp[J][2] =: yp[J][1]\n"
            "    - lg( bXs[{%2}]/( bXs[{%1}]+ bXs[{%2}])/m_t[{%2}] );\n").arg(TraceE1, HostE1);
         }
     break;
  //-------------------------------------------------------------------------
   case P_LIP:
     if( subtype == 0 )
     {
       QString EM1;
       lst = getSelected( pLsts[0] );
       if( lst.count() < 2 )
         return;
       EM1 = lst[1].trimmed();

       QString ComIon, EMion0, EMion1;
       lst = getSelected( pLsts[1] );
       if( lst.count() < 3 )
         return;
       ComIon = lst[0].trimmed();
       EMion0 = lst[1].trimmed();
       EMion1 = lst[2].trimmed();

       ret = QString(
        "$ Abscissa - log10 of Lippmann's total solubility product \n"
        "xp[J] =: lga[{%2}] + lg( 10^lga[{%3}] +  10^lga[{%4}] );\n"
        "$ Solidus \n"
        "yp[J][0] =: Wxx[{%1}]; \n"
        "$ Solutus \n"
        "yp[J][1] =: 10^lga[{%4}] / ( 10^lga[{%3}] + 10^lga[{%4}] );\n").arg(EM1, ComIon, EMion0, EMion1);
     }
     if( subtype == 2 )
     {
         QString EM1;
         lst = getSelected( pLsts[0] );
         if( lst.count() < 2 )
           return;
         EM1 = lst[1].trimmed();

         QString ComIC, EM0IC, EM1IC;
         lst = getSelected( pLsts[2] );
         if( lst.count() < 3 )
           return;
         ComIC = lst[0].trimmed();
         EM0IC = lst[1].trimmed();
         EM1IC = lst[2].trimmed();

         ret = QString(
          "$ Abscissa - total dissolved elements product \n"
          "xp[J] =: lg( m_t[{%2}] ) + lg( m_t[{%3}] +  m_t[{%4}] ); \n"
          "$ Solidus \n"
          "yp[J][0] =: Wxx[{%1}];  \n"
          "$ Solutus\n"
          "yp[J][1] =: m_t[{%4}] / ( m_t[{%3}] + m_t[{%4}] ); \n"
          "$ Done\n").arg(EM1, ComIC, EM0IC, EM1IC);
     }
     break;
  //-------------------------------------------------------------------------
  case P_INV_TITR:
        if( subtype == 1 )
        {
          QString M_ion, M_c1, M_c2;
          lst = getSelected( pLsts[1] );
          if( lst.count() <3  )
           return;
          M_ion = lst[0].trimmed();
          M_c1 = lst[1].trimmed();
          M_c2 = lst[2].trimmed();

          ret = QString(
          "xp[J]=: lg(my[{%1}] + my[{%2}] + my[{%3}]);\n"
          "$ ... more aqueous complexes of M can be accounted for \n"
          " yp[J][0]=: (x[{%2}]> 0? \n"
          "        lg(x[{%2}]/SorbentMass) : empty() ); \n"
          " yp[J][1]=: (x[{%3}]> 0? \n"
          "        lg(x[{%3}]/SorbentMass) : empty() ); \n"
          "$ ... other sorbed species of M can be considered \n"
          "$ log total M sorbed \n"
          "yp[J][2] =: lg((x[{%1}]+x[{%2}])/SorbentMass); \n"
          "$ log Kd \n"
          "yp[J][3] =: yp[J][2] - xp[J];\n").arg(M_ion, M_c1, M_c2 );
        }
     default: break;
     }

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
    case P_INV_TITR:
    case P_LIP:
    case P_SYST: page1Changed = true;
                 setCalcScript( type, subtype );
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
  int jj, nO;
  QString str;
  QListWidgetItem* item1;
  QWidget* page1;
  QHBoxLayout* horizontalLayout1;
  QListWidget* lstIndexes1;
  TCStringArray lst;

  // clear old pages
    if( pageLists->count() > 0)
    { for(jj=pageLists->count()-1; jj>0; jj--)
      {
        pageLists->removeWidget( pageLists->widget(jj) );
      }
    }
    pLsts.clear();
    listObj->clear();

   // init new pages
   for(uint ii=0; ii<pgData.GetCount();ii++)
    {
      nO = pgData[ii].nObj;
      TProfil::pm->getNamesList( nO, lst);

      str = QString("%1").arg( pgData[ii].pageName.c_str());
      item1 = new QListWidgetItem( str,  listObj);

      // add page
      page1 = new QWidget();
      horizontalLayout1 = new QHBoxLayout(page1);
      lstIndexes1 = new QListWidget(page1);
      lstIndexes1->setWrapping( true );
      lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
      horizontalLayout1->addWidget(lstIndexes1);
      pageLists->addWidget(page1);

      // insert items to list of indexes
       for(  jj=0; jj<lst.GetCount(); jj++ )
          {
             item1 = new QListWidgetItem( lst[jj].c_str(), lstIndexes1);
          }
       pLsts.append(lstIndexes1);
    }
}

QStringList  ProcessWizard::getSelected( QListWidget *lst )
{
    QStringList ret;
    QListWidgetItem*  ndx;

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

     if( until < from )
     {
         gstring str = tIters->horizontalHeaderItem( col )->text().toLatin1().data();
         vfMessage(this, str.c_str(), "Illegal interval fo values: from > until");
         return nP;
     }

     if( step <= 1e-9 )
       nP = 1;
      else
       nP  = (int)((until-from)/step)+1;

     return nP;
 }

//==============================================================================

equatSetupData eqPr( "xp", "yp", "J", "J" );

// work with lists
void ProcessWizard::resetPageList()
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
              RT_PROCES, pgData, scalarsList, outScript.c_str()  );
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
        const QStyleOptionViewItem &option,
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
