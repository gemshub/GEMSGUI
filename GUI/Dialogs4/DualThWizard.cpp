//-------------------------------------------------------------------
// $Id: RTparmWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of DualThWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <qspinbox.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qvariant.h>

#include "DualThWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void DualThWizard::languageChange()
{
    retranslateUi(this);
}

void
DualThWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
DualThWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	DualThWizard::resetNextButton()
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

void 	DualThWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


DualThWizard::DualThWizard( const char* pkey, char flgs[20], int size[8],
                            QWidget* parent):
        QDialog( parent )
{
    int ii;

    // setFinishEnabled( WizardPage2, true);
    setupUi(this);
    string str1= "GEM-Selektor DualTh Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

// page1
    ii = p_Mode->findText(QChar(flgs[12]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_Mode->setCurrentIndex(ii);

    ii = p_St->findText(QChar(flgs[13]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_St->setCurrentIndex(ii);

    ii = p_IPf->findText(QChar(flgs[17]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_IPf->setCurrentIndex(ii);

    ii = p_IPu->findText(QChar(flgs[18], Qt::MatchStartsWith|Qt::MatchCaseSensitive));
    if( ii >= 0  )
    	p_IPu->setCurrentIndex(ii);

    ii = p_LSF->findText(QChar(flgs[19]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_LSF->setCurrentIndex(ii);

// page2

    pnQ->setValue(size[0]);
    pnM->setValue(size[2]);
    pnP->setValue(size[7]);
    pLb->setValue(size[1]);

    ii = pPunE->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunE->setCurrentIndex(ii);
    ii = pPunV->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunV->setCurrentIndex(ii);
    ii = pPunP->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunP->setCurrentIndex(ii);
     ii = pPunT->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunT->setCurrentIndex(ii);

// Page 3

    if( flgs[5] != '-' )
      c_ICb->setChecked( true );
    else c_ICb->setChecked( false );
    if( flgs[6] != '-' )
      c_ICn->setChecked( true );
    else c_ICn->setChecked( false );
    if( flgs[7] != '-' )
      c_AUb->setChecked( true );
    else c_AUb->setChecked( false );
    if( flgs[11] != '-' )
      c_TPI->setChecked( true );
    else c_TPI->setChecked( false );
    if( flgs[9] != '-' )
      c_Chi->setChecked( true );
    else c_Chi->setChecked( false );
    if( flgs[10] != '-' )
      c_Gam->setChecked( true );
    else c_Gam->setChecked( false );
    if( flgs[4] != '-' )
      c_Coul->setChecked( true );
    else c_Coul->setChecked( false );
    if( flgs[16] != '-' )
      c_SYd->setChecked( true );
    else c_SYd->setChecked( false );

//Page 4
   p_Nsd->setValue(size[3]);
}


DualThWizard::~DualThWizard()
{}

void   DualThWizard::getSizes( int size[8] )
{
// Nqpn, Nqpg, Nb internal
    size[0]= pnQ->value();
    size[1]= pLb->value();
    size[2]= pnM->value();
    size[3]= p_Nsd->value();
    size[7]= pnP->value();
}

void DualThWizard::getFlags( char flgs[20] )
{

// Page 1
  QString str;

  str = p_Mode->currentText();
  flgs[12] = str[0].toLatin1();
  str = p_St->currentText();
  flgs[13] = str[0].toLatin1();
  str = p_IPf->currentText();
  flgs[17] = str[0].toLatin1();
  str = p_IPu->currentText();
  flgs[18] = str[0].toLatin1();
  str = p_LSF->currentText();
  flgs[19] = str[0].toLatin1();

// Page 2 disabled flags

//Page 3
   if( c_ICb->isChecked() )
       flgs[5] = '+';
    else flgs[5] = '-';

   if( c_ICn->isChecked() )
       flgs[6] = '+';
    else flgs[6] = '-';

   if( c_AUb->isChecked() )
       flgs[7] = '+';
    else flgs[7] = '-';

   if( c_TPI->isChecked() )
       flgs[11] = '+';
    else flgs[11] = '-';

   if( c_Chi->isChecked() )
       flgs[9] = '+';
    else flgs[9] = '-';

   if( c_Gam->isChecked() )
       flgs[10] = '+';
    else flgs[10] = '-';

   if( c_Coul->isChecked() )
       flgs[4] = '+';
    else flgs[4] = '-';

   if( c_SYd->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';

// page 4
  if( p_Nsd->value() > 0  )
       flgs[8] = '+';
  else flgs[8] = '-';
}

void
DualThWizard::help()
{
  pVisorImp->OpenHelp( GM_DUALTH_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//--------------------- End of DualThWizard.cpp ---------------------------
