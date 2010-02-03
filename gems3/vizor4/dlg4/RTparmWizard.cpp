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

#include <qspinbox.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qvariant.h>

#include "RTparmWizard.h"
#include "visor_w.h"

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
                            QWidget* parent):
    QDialog( parent )
{
    int ii;
    gstring str1= "GEM-Selektor RTparm Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    //setFinishEnabled( WizardPage3, true);
    setupUi(this);
    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

// page1
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

 // Page 2
    pNP->setValue(size[0]);
    pNT->setValue(size[1]);
    pdimY->setValue(size[6]);
    pECol->setValue(size[5]);
    pELine->setValue(size[4]);
    ii = min( size[2],pMode->count()-1 );
    pMode->setCurrentIndex(ii);

//Page 3
   spinBox18->setValue(size[3]);

    ii = pPabs->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPabs->setCurrentIndex(ii);
    ii = pPtun->findText(QChar(flgs[7]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPtun->setCurrentIndex(ii);

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

void RTparmWizard::getFlags( char flgs[6] )
{
// Page 1 - not return
// Page 2
  if( pECol->value() > 0 && pELine->value() > 0 )
       flgs[5] = '+';
  else flgs[5] = '-';
//Page 3
  QString str = pPabs->currentText();
  flgs[6] = str[0].toLatin1();
  str = pPtun->currentText();
  flgs[7] = str[0].toLatin1();
}

void
RTparmWizard::help()
{
  pVisorImp->OpenHelp( RTPARM_HTML, 0, this/*, true*/ );
}

//--------------------- End of RTparmWizard.cpp ---------------------------
