//-------------------------------------------------------------------
// $Id: RTparmWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of RTparmWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
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

const char *RTPARM_HTML = "gm_rtparm";

#include <math.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qvariant.h>

#include "RTparmWizard.h"
#include "visor_w.h"
#include "v_mod.h"
#include "service.h"

void RTparmWizard::languageChange()
{
    retranslateUi(this);
}

void
RTparmWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
RTparmWizard::CmNext()
{
    int ndx = stackedWidget->currentIndex();

    if( ndx == 0 )
    {
      if( !TChange() )
        return;
      if( !PChange() )
        return;
    }

    int nLines = pageScript->getScriptLinesNum();
    if( ndx == 1 && nLines > 0)
        pdimY->setValue( nLines );

    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
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
          float val[6],  const char *acalcScript,   QWidget* parent):
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

//Page 2 equations
     resetPageList();


// Page 3
    pNP->setValue(size[0]);
    pNT->setValue(size[1]);
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

   QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
   QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
   QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));

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


void
RTparmWizard::help()
{
  pVisorImp->OpenHelp( RTPARM_HTML, 0 );
}


bool RTparmWizard::TChange()
{
  double from, until, step;
  int nT;

  from = pTfrom->text().toDouble();
  until = pTuntil->text().toDouble();
  step = pTstep->text().toDouble();

  if( fabs(step) < 1e-30 )
      nT = 1;
  else
      nT  = (int)((until-from)/step)+1;

   if( nT < 1 || nT > 9999 )
   {
       //pTfrom->setValue(until);
       //pTuntil->setValue(from);
       vfMessage(this, "Temperature", "Wrong number of steps - please, check values in this iterator!");
       return false;
   }

  pNT->setValue(nT);
  return true;
}

bool RTparmWizard::PChange()
{
  double from, until, step;
  int nP;

  from = pPfrom->text().toDouble();
  until = pPuntil->text().toDouble();
  step = pPstep->text().toDouble();

  if( fabs(step) < 1e-30 )
      nP = 1;
  else
      nP  = (int)((until-from)/step)+1;

   if( nP < 1 || nP > 9999 )
   {
       //pTfrom->setValue(until);
       //pPuntil->setValue(from);
       vfMessage(this, "Pressure", "Wrong number of steps - please, check values in this iterator!");
       return false;
   }

  pNP->setValue(nP);
  return true;
}


//==============================================================================

equatSetupData eq( "", "yF", "jTP", "" );
equatSetupData eqT( "xT", "yF", "jTP", "twTC" );
equatSetupData eqP( "xP", "yF", "jTP", "twP" );

// work with lists
void RTparmWizard::resetPageList()
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
             RT_PROCES, pgData, scalarsList, calcScript.c_str()  );
    verticalLayout_5->addWidget(pageScript);

}

//--------------------- End of RTparmWizard.cpp ---------------------------
