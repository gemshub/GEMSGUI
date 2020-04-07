//-------------------------------------------------------------------
// $Id: LookupDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of LookupDialog class
//
// Copyright (C) 2009  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <qvalidator.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <QDoubleSpinBox>

#include "LookupDialog.h"
#include "GemsMainWindow.h"
#include "service.h"
#include "v_mod.h"
#include "num_methods.h"
//#include "model_w.h"

#define Inherited LookupDialogData

LookupDialog::LookupDialog(QWidget* parent):
	QDialog( parent)
{
	setupUi(this);

    PTable = nullptr;
    TTable = nullptr;
    buttonBack->hide();
    QObject::connect( buttonOK, SIGNAL( clicked() ), this, SLOT( CmNext() ) );
    QObject::connect( buttonBack, SIGNAL( clicked() ), this, SLOT( CmBack() ) );
    QObject::connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	QObject::connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    QObject::connect( checkArrays, SIGNAL( toggled(bool) ), this, SLOT( arrayChecked(bool) ) );

}

void LookupDialog::languageChange()
{
    retranslateUi(this);
}


LookupDialog::~LookupDialog()
{}


void LookupDialog::CmNext()
{
  if( stackW->currentIndex() == 1 )
    {
       accept();
    } else if(!checkArrays->isChecked())
      {
        // internal setup arrays
        setupPTArrays();
        accept();
      }
     else
     {
       definePArray();
       defineTArray();
       initPTable();
       initTTable();
       buttonBack->show();
       stackW->setCurrentIndex(1);
     }
}

void LookupDialog::CmBack()
{
  if( stackW->currentIndex() == 1 )
    {
       buttonBack->hide();
       stackW->setCurrentIndex(0);
     }
}


void LookupDialog::arrayChecked( bool check )
{
  pPstep->setEnabled ( !check );
  pTstep->setEnabled ( !check );
  pPPoints->setEnabled ( check );
  pTPoints->setEnabled ( check );
  chInterp->setEnabled ( check );
}

void LookupDialog::definePArray()
{
   int nPs, nPi, nP, ii;
   double  cP;
   double Pai[4];
   double *arP;

   nPs = pPPoints->value();
   getPdata( Pai );
   if( nPs == 1 )
   {
//      getPdata( Pai );
      nPs = nPi = getNpoints( Pai );
//      pPPoints->textFromValue( nPi );
      pPPoints->setValue( nPi );
   }
   nP = nPs;
  //init P array
   double stepP = getStep( Pai, nP );
   cP = Pai[0];
   arP = static_cast<double*>(aObj[ o_w_pval].Alloc( nP, 1, D_));
   if( PTable == nullptr)
    for( ii=0; ii<nP; ii++ )
    {
     arP[ii] = cP;
     cP += stepP;
    }
}

void LookupDialog::initPTable()
 {
    // init table
   TObjectModel* model;
   //TObjectTable* PTable;
   QList<FieldInfo>	aFlds;

   if( PTable )
   {   gridLayout_4->removeWidget(PTable);
       delete PTable; //????
   }

   aFlds.clear();
   aFlds.append(FieldInfo( o_w_pval, ftFloat, 15, false, First, eYes, stIO, 20, 1));

   if(chInterp->isChecked())
       aFlds.append(FieldInfo( o_w_tval, ftFloat, 15, false, Tied, eYes, stIO, 20, 1));
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

   if(!chInterp->isChecked())
     gridLayout_4->addWidget(PTable, 1, 0, 1, 1);
   else
     gridLayout_4->addWidget(PTable, 1, 0, 1, 2);

}

void LookupDialog::defineTArray()
{
   int nT, nTs, nTi, ii;
   double cT;
   double Tai[4];
   double  *arT;

   getTdata( Tai );
   if(chInterp->isChecked())
   {
       nT = pPPoints->value();
//          getTdata( Tai );
   }
   else {
       nTs = pTPoints->value();
       if( nTs == 1 )
       {
//          getTdata( Tai );
          nTs = nTi = getNpoints( Tai );
//          pTPoints->textFromValue( nTi );
          pTPoints->setValue( nTi );
       }
       nT = nTs;
   }
   //init T array
   double stepT = getStep( Tai, nT );
   cT = Tai[0];
   arT = static_cast<double*>(aObj[ o_w_tval].Alloc( nT, 1, D_));
   if( PTable == nullptr)
    for( ii=0; ii<nT; ii++ )
    {
      arT[ii] = cT;
      cT += stepT;
    }

}

void LookupDialog::initTTable()
 {
   // init table
   TObjectModel* model;
   //TObjectTable* fieldTable;
   QList<FieldInfo>	aFlds;

   if( TTable )
   {   gridLayout_4->removeWidget(TTable);
       delete TTable; //????
       TTable = nullptr;
   }

   if(chInterp->isChecked())  // only one table
           return;

   aFlds.clear();
   aFlds.append(FieldInfo( o_w_tval, ftFloat, 15, false, First, eYes, stIO, 20, 1));
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

   gridLayout_4->addWidget(TTable, 1, 1, 1, 1);

}

void LookupDialog::objectChanged()
{

}

void LookupDialog::setupPTArrays()
{
   int nT, nP, ii;
   double cT, cP;
   double Pai[4], Tai[4];
   double  *arT, *arP;

   //init P array
   getPdata( Pai );
   nP = getNpoints( Pai );
   arP = static_cast<double*>( aObj[ o_w_pval].Alloc( nP, 1, D_) );
   cP = Pai[START_];
   for( ii=0; ii<nP; ii++ )
   {
     arP[ii] = cP;
     cP+= Pai[2];
   }

   //init T array
   getTdata( Tai );
   nT = getNpoints( Tai );
   arT = static_cast<double*>(aObj[ o_w_tval].Alloc( nT, 1, D_));
   cT = Tai[START_];
   for( ii=0; ii<nT; ii++ )
   {
     arT[ii] = cT;
     cT+= Tai[2];
   }

}

void   LookupDialog::setTdata( double Tai[4] )
{
   pTfrom->textFromValue( Tai[0] );
   pTfrom->setValue( Tai[0] );
   pTuntil->textFromValue( Tai[1] );
   pTuntil->setValue( Tai[1] );
}

void   LookupDialog::setPdata( double Pai[4] )
{
    pPfrom->textFromValue( Pai[0] );
    pPfrom->setValue( Pai[0] );
    pPuntil->textFromValue( Pai[1] );
    pPuntil->setValue( Pai[1] );
}

void   LookupDialog::getTdata( double Tai[4] )
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

void   LookupDialog::getPdata( double Pai[4] )
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

void   LookupDialog::getFlags( char flgs[6] )
{
   if( chSelect->isChecked() )
       flgs[0] = '+';
    else flgs[0] = '-';

   if( chMode->isChecked() )
       flgs[1] = '+';
    else flgs[1] = '-';
   
   if( chAll->isChecked() )
       flgs[2] = '+';
    else flgs[2] = '-';
   
   if( chComment->isChecked() )
       flgs[3] = '+';
    else flgs[3] = '-';

    if( chInterp->isChecked() )
        flgs[4] = '+';
     else flgs[4] = '-';

    if( chNoNodearray->isChecked() )
        flgs[5] = '+';
     else flgs[5] = '-';
}

void LookupDialog::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_WRIPMF_HTML );
}


//--------------------- End of SettingsDialog.cpp ---------------------------

