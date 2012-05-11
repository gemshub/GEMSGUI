//-------------------------------------------------------------------
// $Id: RTparmWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of RTparmWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
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

#include <cmath>
#include <qspinbox.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qvariant.h>

#include "RTparmWizard.h"
#include "GemsMainWindow.h"
#include "v_mod.h"
#include "service.h"
#include "num_methods.h"

void RTparmWizard::languageChange()
{
    retranslateUi(this);
}

void RTparmWizard::CmBack()
{
    int ndx = stackedWidget->currentIndex();

    if( ndx == 2 )
    {
      if( pMode->currentIndex() != 0 )
        ndx--;
    }

    stackedWidget->setCurrentIndex ( ndx-1 );
    resetNextButton();
    resetBackButton();
}

void RTparmWizard::CmNext()
{
    int ndx = stackedWidget->currentIndex();

    if( ndx == 0 )
    {
      if( pMode->currentIndex() != 0 )
        ndx++;
      else
        initPTTable();
    }

    int nLines = pageScript->getScriptLinesNum();
    if( ndx == 1 && nLines > 0)
        pdimY->setValue( nLines );

    stackedWidget->setCurrentIndex ( ndx+1 );
    resetNextButton();
    resetBackButton();
}

void 	RTparmWizard::resetNextButton()
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

void 	RTparmWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}

RTparmWizard::RTparmWizard( const char* pkey, char flgs[10], int size[7],
          float val[6],  const char *acalcScript, const char* aXname, const char* aYname,
          QWidget* parent):
    QDialog( parent ), calcScript(acalcScript), pageScript(0)
{
    int ii;

   if( flgs[0] == SRC_DCOMP )
      isDC = true;
   else
      isDC = false;


    setupUi(this);
    gstring str1= "GEM-Selektor RTparm Setup:  ";
            str1 += pkey;
    setWindowTitle( str1.c_str() );
    resetNextButton();
    resetBackButton();


// page1
    ii = min( size[2],pMode->count()-1 );
    pMode->setCurrentIndex(ii);
    pTfrom->setValue(val[0]);
    pTuntil->setValue(val[1]);
    pTstep->setValue(val[2]);
    pPfrom->setValue(val[3]);
    pPuntil->setValue(val[4]);
    pPstep->setValue(val[5]);

    ii = pPtun->findText(QChar(flgs[7]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        pPtun->setCurrentIndex(ii);
    ii = pPun->findText(QChar(flgs[8]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        pPun->setCurrentIndex(ii);
    if( flgs[6] == 'T' )
    {
     butT->setChecked(true);
     butP->setChecked(false);
    }
    else
    {
        butT->setChecked(false);
        butP->setChecked(true);
    }

    pNP->setValue(size[0]);
    pNT->setValue(size[1]);

//Page 2 equations
     resetPageList(aXname, aYname);


// Page 3
    pdimY->setValue(size[6]);
    pECol->setValue(size[5]);
    pELine->setValue(size[4]);

   ii = pWhat->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
       pWhat->setCurrentIndex(ii);

   ii = pPunE->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
       pPunE->setCurrentIndex(ii);

   ii = pPunV->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
       pPunV->setCurrentIndex(ii);

   ii = pPunP->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
       pPunP->setCurrentIndex(ii);
   ii = pPunT->findText(QChar(flgs[4]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
       pPunT->setCurrentIndex(ii);

   //Page 4
      spinBox18->setValue(size[3]);

   // commands
   stackedWidget->setCurrentIndex (0);
   PairwiseChecked();

   QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
   QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
   QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));

   QObject::connect( pTfrom, SIGNAL(valueChanged( double  )), this, SLOT(TChange()));
   QObject::connect( pTuntil, SIGNAL(valueChanged( double  )), this, SLOT(TChange()));
   QObject::connect( pTstep, SIGNAL(valueChanged( double  )), this, SLOT(TChange()));
   QObject::connect( pPfrom, SIGNAL(valueChanged( double  )), this, SLOT(PChange()));
   QObject::connect( pPuntil, SIGNAL(valueChanged( double  )), this, SLOT(PChange()));
   QObject::connect( pPstep, SIGNAL(valueChanged( double  )), this, SLOT(PChange()));
   QObject::connect( pMode, SIGNAL(currentIndexChanged( int ) ), this, SLOT(PairwiseChecked()));

}


RTparmWizard::~RTparmWizard()
{}

void   RTparmWizard::getSizes( int size[7] )
{
    size[0]= pNP->value();
    size[1]= pNT->value();
    size[6]= pdimY->value();
    size[5]= pECol->value();
    size[4]= pELine->value();
    size[3]= spinBox18->value();

    size[2] = pMode->currentIndex();
}

void RTparmWizard::getFlags( char flgs[6], gstring& xName )
{
// Page 1 - not return
    QString str = pPtun->currentText();
    flgs[7] = str[0].toLatin1();

    str = pPun->currentText();
    flgs[8] = str[0].toLatin1();

    if(butP->isChecked())
    {  flgs[6] = 'P';
       xName = pPun->currentText().toLatin1().data();
    }
    else
    {  flgs[6] = 'T';
       xName = pPtun->currentText().toLatin1().data();
    }

// Page 2
  if( pECol->value() > 0 && pELine->value() > 0 )
       flgs[5] = '+';
  else flgs[5] = '-';
}

void   RTparmWizard::getFloat( float val[6] )
{
    val[0] = pTfrom->text().toDouble();
    val[1] = pTuntil->text().toDouble();
    val[2] = pTstep->text().toDouble();
    val[3] = pPfrom->text().toDouble();
    val[4] = pPuntil->text().toDouble();
    val[5] = pPstep->text().toDouble();
}


void RTparmWizard::help()
{
  pVisorImp->OpenHelp( GM_RTPARM_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}


void RTparmWizard::TChange()
{
    double Tai[4];
    int nT;
    Tai[0] = pTfrom->text().toDouble();
    Tai[1] = pTuntil->text().toDouble();
    Tai[2] = pTstep->text().toDouble();
    nT = getNpoints( Tai );
    pNT->setValue(nT);
}

void RTparmWizard::PChange()
{
  double Pai[4];
  int nP;
  Pai[0] = pPfrom->text().toDouble();
  Pai[1] = pPuntil->text().toDouble();
  Pai[2] = pPstep->text().toDouble();
  nP = getNpoints( Pai );
  pNP->setValue(nP);
}


void RTparmWizard::PairwiseChecked()
{
  bool modeNoPair = ( pMode->currentIndex() != 0);

  pTfrom->setEnabled ( modeNoPair );
  pTuntil->setEnabled ( modeNoPair );
  pTstep->setEnabled ( modeNoPair );
  pPfrom->setEnabled ( modeNoPair );
  pPuntil->setEnabled ( modeNoPair );
  pPstep->setEnabled ( modeNoPair );

  pNP->setEnabled ( !modeNoPair );
  pNT->setEnabled ( !modeNoPair );


}


void RTparmWizard::definePTArray()
{
   int nPT;
   double *arP, *arT;

   nPT = max( pNP->value(), pNT->value());
   arP = (double *) aObj[ o_rpxp].Alloc( nPT, 1, D_);
   arT = (double *) aObj[ o_rpxt].Alloc( nPT, 1, D_);
}

void RTparmWizard::initPTTable()
 {
    // init table
   TObjectModel* model;
   TObjectTable* fieldTable;
   QList<FieldInfo>	aFlds;

   definePTArray();

   aFlds.clear();
   aFlds.append(FieldInfo( o_rpxp, ftFloat, 15, false, First, eYes, stIO, 20, 1));
   aFlds.append(FieldInfo( o_rpxt, ftFloat, 15, false, Tied, eYes, stIO, 20, 1));

   model = new TObjectModel( aFlds, this );
   fieldTable =  new TObjectTable( aFlds, this );
   TObjectDelegate *deleg = new TObjectDelegate( fieldTable, this);
   fieldTable->setItemDelegate(deleg);
   fieldTable->setModel(model);

   QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
   sizePolicy1.setHorizontalStretch(0);
   sizePolicy1.setVerticalStretch(0);
   fieldTable->setSizePolicy(sizePolicy1);

   int rowSize =0, colSize=0;
   fieldTable->getObjectSize(rowSize, colSize);
   fieldTable->setMaximumSize(QSize(colSize, 16777215));

   gridLayout_4->addWidget(fieldTable, 1, 0, 1, 2);

}



//==============================================================================

equatSetupData eq( "", "yF", "jTP", "" );
equatSetupData eqT( "xT", "yF", "jTP", "twTC" );
equatSetupData eqP( "xP", "yF", "jTP", "twP" );

// work with lists
void RTparmWizard::resetPageList(const char* aXname, const char* aYname)
{

    TIArray<pagesSetupData> scalarsList;
    TIArray<pagesSetupData> pgData;

    GetListsnRT( -1, pgData,  scalarsList );
    GetListsnRT( RT_RTPARM, pgData,  scalarsList );
/*
   if( isDC )
   {
       GetListsnRT( RT_DCOMP,  pgData,  scalarsList );
   }
   else
   {
       GetListsnRT( RT_REACDC, pgData,  scalarsList );
   }
*/
   pageScript = new EquatSetup( page_3, eq,
             RT_PROCES, pgData, scalarsList, calcScript.c_str(), aXname, aYname  );
    verticalLayout_5->addWidget(pageScript);

}

//--------------------- End of RTparmWizard.cpp ---------------------------
