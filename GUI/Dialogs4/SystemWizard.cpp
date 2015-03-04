//-------------------------------------------------------------------
// $Id: SystemWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SystemWizard class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
// Uses  string class (C) A.Rysin 1999
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

#include "SystemWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void SystemWizard::languageChange()
{
    retranslateUi(this);
}

void SystemWizard::CmBack()
{
    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void SystemWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	SystemWizard::resetNextButton()
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

void 	SystemWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}

SystemWizard::SystemWizard( const char* pkey, char flgs[40],
    string name, string comment, string EQkey,  QWidget* parent):
    QDialog( parent )
{

    //setFinishEnabled( WizardPage3, true);
    setupUi(this);
    string str1= "GEM-Selektor System Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

// page1
    pName->setText(name.c_str());
    pComment->setText(comment.c_str());

    if( flgs[15] != '-' || flgs[17] != '-')
            pPLim->setChecked( true );
    else pPLim->setChecked( false );

    if( flgs[0] != '-' )
            pPE->setChecked( true );
    else pPE->setChecked( false );

    if( flgs[1] != '-' )
            pPV->setChecked( true );
    else pPV->setChecked( false );

    if( flgs[9] != '-' )
            pPBeq->setChecked( true );
    else pPBeq->setChecked( false );

    if( flgs[24] != '-' )
            pPAalp->setChecked( true );
    else pPAalp->setChecked( false );

    if( flgs[25] != '-' )
            pPSigm->setChecked( true );
    else pPSigm->setChecked( false );

    if( flgs[26] != '-' )
            pXr0h0->setChecked( true );
    else pXr0h0->setChecked( false );

    if( flgs[27] != '-' )
            pPXepsC->setChecked( true );
    else pPXepsC->setChecked( false );

    if( flgs[28] != '-' )
            pPNfsp->setChecked( true );
    else pPNfsp->setChecked( false );

    if( flgs[34] != '-' )
            pPParc->setChecked( true );
    else pPParc->setChecked( false );

    if( flgs[35] != '-' )
            pPDelB->setChecked( true );
    else pPDelB->setChecked( false );

    if( flgs[36] != '-' )
            pPXlam->setChecked( true );
    else pPXlam->setChecked( false );


// Page 2
    pKey->setText(EQkey.c_str());
    if( flgs[13] != '-' )
            pPbPh->setChecked( true );
    else pPbPh->setChecked( false );

   /* if( flgs[18] == '+' )
            r1->setChecked( true );
    else if( flgs[18] == '-' )
            r2->setChecked( true );
         else
            r3->setChecked( true );
   */
}

SystemWizard::~SystemWizard()
{}

string SystemWizard::getName(  )
{
  return string(pName->text().toUtf8().data());
}

string SystemWizard::getComment(  )
{
  return string(pComment->text().toUtf8().data());
}

string SystemWizard::getEQkey(  )
{
  return string(pKey->text().toUtf8().data());
}


void SystemWizard::getFlags( char flgs[40] )
{
// Page 2
    if( pPbPh->isChecked() )
          flgs[13] = '+';
    else flgs[13] = '-';

    //if( r1->isChecked() )
    //  flgs[18] = '+';
    //else  if( r2->isChecked() )
    //      flgs[18] = '-';
    //  else
          flgs[18] = '*';

// Page 1

    if( pPLim->isChecked() )
          flgs[15] = flgs[17] = '+';
    else flgs[15] = flgs[17] = '-';

    if( pPE->isChecked() )
          flgs[0] = '+';
    else flgs[0] = '-';

    if( pPV->isChecked() )
          flgs[1] = '+';
    else flgs[1] = '-';

    if( pPBeq->isChecked() )
          flgs[9] = '+';
    else flgs[9] = '-';

    if( pPAalp->isChecked() )
          flgs[24] = '+';
    else flgs[24] = '-';

    if( pPSigm->isChecked() )
          flgs[25] = '+';
    else flgs[25] = '-';

    if( pXr0h0->isChecked() )
          flgs[26] = '+';
    else flgs[26] = '-';

    if( pPXepsC->isChecked() )
          flgs[27] = '+';
    else flgs[27] = '-';

    if( pPNfsp->isChecked() )
          flgs[28] = '+';
    else flgs[28] = '-';

    if( pPParc->isChecked() )
          flgs[34] = '+';
    else flgs[34] = '-';

    if( pPDelB->isChecked() )
          flgs[35] = '+';
    else flgs[35] = '-';

    if( pPXlam->isChecked() )
          flgs[36] = '+';
    else flgs[36] = '-';

    if( pPask->isChecked() )
          flgs[37] = '+';
    else flgs[37] = '-';
}

void
SystemWizard::help()
{
  pVisorImp->OpenHelp( GEMS_SYSTEM_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//--------------------- End of SystemWizard.cpp ---------------------------
