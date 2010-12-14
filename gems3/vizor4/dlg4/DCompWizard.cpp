//-------------------------------------------------------------------
// $Id: DCompWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of DCompWizard class
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

const char *DCOMP_HTML = "gm_dcomp";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qvariant.h>


#include "DCompWizard.h"
#include "visor_w.h"

void DCompWizard::languageChange()
{
    retranslateUi(this);
}

void
DCompWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
DCompWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	DCompWizard::resetNextButton()
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

void 	DCompWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


DCompWizard::DCompWizard( const char* pkey, char flgs[15], int size[4],
                            QWidget* parent):
      QDialog( parent )
{

    setupUi(this);
    gstring str1= "GEM-Selektor DComp Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();


//Page 1
    int ii;
    ii = p_pct_1->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_pct_1->setCurrentIndex(ii);
    ii = p_pct_2->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_pct_2->setCurrentIndex(ii);
   ii = p_pct_3->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
	   p_pct_3->setCurrentIndex(ii);
   ii = pPdcC->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
	   pPdcC->setCurrentIndex(ii);
//Page 2
    pNeCp->setValue(size[0]);
    pNft->setValue(size[1]);
    pNemp->setValue(size[3]);
    pPdcVT->setChecked( flgs[14] == '+' );

    ii = pPunE->findText(QChar(flgs[7]),Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunE->setCurrentIndex(ii);

    ii = pPunV->findText(QChar(flgs[8]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunV->setCurrentIndex(ii);

    ii = pPunP->findText(QChar(flgs[9]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunP->setCurrentIndex(ii);
    ii = pPunT->findText(QChar(flgs[10]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunT->setCurrentIndex(ii);

//Page3
    spinBoxSDrefLnk->setValue(size[2]);
}


DCompWizard::~DCompWizard()
{}

void   DCompWizard::getSizes( int size[4] )
{
    size[0]= pNeCp->value();
    size[1]= pNft->value();
    size[2]= spinBoxSDrefLnk->value();
    size[3]= pNemp->value();

}

void DCompWizard::getFlags( char flgs[15] )
{
  QString str;

     str = p_pct_1->currentText();
  flgs[0] = str[0].toLatin1();
     str = p_pct_2->currentText();
  flgs[1] = str[0].toLatin1();
     str = p_pct_3->currentText();
  flgs[2] = str[0].toLatin1();
// 3, 4, 5, reserved
     str = pPdcC->currentText();
  flgs[6] = str[0].toLatin1();
// 7,8,9,10 not changed
  if( pNeCp->value() > 0  )
    flgs[11] = '+';
  else
    flgs[11] = '-';

  if( pNft->value() > 0  )
    flgs[12] = '+';
  else
    flgs[12] = '-';

  if( flgs[0] == 'H'  )
    flgs[13] = '+';
  else
    flgs[13] = '-';
// check
  if( pPdcVT->isChecked() )
    flgs[14] = '+';
  else
    flgs[14] = '-';


}


void
DCompWizard::help()
{
  pVisorImp->OpenHelp( DCOMP_HTML, 0 );
}

//--------------------- End of DCompWizard.cpp ---------------------------
