//-------------------------------------------------------------------
// $Id: ReacDCWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ReacDCWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
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

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qvariant.h>

#include "ReacDCWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void ReacDCWizard::languageChange()
{
    retranslateUi(this);
}

void
ReacDCWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
ReacDCWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	ReacDCWizard::resetNextButton()
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

void 	ReacDCWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


ReacDCWizard::ReacDCWizard( const char* pkey, char flgs[12], int size[4],
                            QWidget* parent):
        QDialog( parent )
{

    //setFinishEnabled( WizardPage3, true);
    setupUi(this);
    gstring str1= "GEM-Selektor ReacDC Setup:  ";
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
   ii = p_pct_4->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
           p_pct_4->setCurrentIndex(ii);
   ii = pPreC->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
   if( ii >= 0  )
	   pPreC->setCurrentIndex(ii);

//Page 2
    pnDC->setValue(size[0]);
    pnTp->setValue(size[1]);
    pnPp->setValue(size[2]);

    ii = pPunE->findText(QChar(flgs[7]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
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
    spinBoxSDrefLnk->setValue(size[3]);
}


ReacDCWizard::~ReacDCWizard()
{}

void   ReacDCWizard::getSizes( int size[4] )
{
    size[0]= pnDC->value();
    size[1]= pnTp->value();
    size[2]= pnPp->value();
    size[3]= spinBoxSDrefLnk->value();

}

void ReacDCWizard::getFlags( char flgs[12] )
{
  QString str;

     str = p_pct_1->currentText();
  flgs[0] = str[0].toLatin1();
     str = p_pct_2->currentText();
  flgs[1] = str[0].toLatin1();
     str = p_pct_3->currentText();
  flgs[2] = str[0].toLatin1();
     str = p_pct_4->currentText();
  flgs[3] = str[0].toLatin1();
// 3, 4, 5, reserved
     str = pPreC->currentText();
  flgs[6] = str[0].toLatin1();
// 7,8,9,10,11 not changed  
}


void
ReacDCWizard::help()
{
  pVisorImp->OpenHelp( GM_REACDC_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//--------------------- End of ReacDCWizard.cpp ---------------------------
