//-------------------------------------------------------------------
// $Id: RTparmWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ProjectWizard class
//
// Copyright (C) 2009  S.Dmytriyeva, D.Kulik
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

const char *PROJECT_HTML = "gm_project";

#include <qcheckbox.h>

#include "ProjectWizard.h"
#include "visor_w.h"

void ProjectWizard::languageChange()
{
    retranslateUi(this);
}

void ProjectWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void ProjectWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	ProjectWizard::resetNextButton()
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

void 	ProjectWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}

ProjectWizard::ProjectWizard( const char* pkey, char flgs[38], QWidget* parent):
    QDialog( parent )
{
    int ii;

    //setFinishEnabled( WizardPage3, true);
    setupUi(this);
    gstring str1= "GEM-Selektor Project Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

// page1
    // flgs 0-9 MSpmv, 10-29 TPptv, 30-33  TPun, 34-37 TPsv
        
        if( flgs[11] != '-' )    // TPptv[1] for all allocated 
        	pTPptv_un->setChecked( true );
        else pTPptv_un->setChecked( false );

        if( flgs[12] != '-' )    // TPptv[2] 
        	pTPptv_2->setChecked( true );
        else pTPptv_2->setChecked( false );
        
        if( flgs[14] != '-' )    // TPptv[4] 
        	pTPptv_4->setChecked( true );
        else pTPptv_4->setChecked( false );
        
        if( flgs[16] != '-' )    // TPptv[6] 
        	pTPptv_6->setChecked( true );
        else pTPptv_6->setChecked( false );

        if( flgs[20] != '-' )    // TPptv[10] 
        	pTPptv_10->setChecked( true );
        else pTPptv_10->setChecked( false );
        if( flgs[21] != '-' )    // TPptv[11] 
        	pTPptv_11->setChecked( true );
        else pTPptv_11->setChecked( false );
        if( flgs[22] != '-' )    // TPptv[12] 
        	pTPptv_12->setChecked( true );
        else pTPptv_12->setChecked( false );
        if( flgs[23] != '-' )    // TPptv[13] 
        	pTPptv_13->setChecked( true );
        else pTPptv_13->setChecked( false );
        if( flgs[24] != '-' )    // TPptv[14] 
        	pTPptv_14->setChecked( true );
        else pTPptv_14->setChecked( false );
        
        if( flgs[4] != '-' )    // MSpmv[4] 
        	pMSpmv4->setChecked( true );
        else pMSpmv4->setChecked( false );
        if( flgs[6] != '-' )    // MSpmv[6] 
        	pMSpmv6->setChecked( true );
        else pMSpmv6->setChecked( false );
    
// Page 2   - 30-33  TPun
    
    ii = pPunE->findText(QChar(flgs[30]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunE->setCurrentIndex(ii);

    ii = pPunV->findText(QChar(flgs[31]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunV->setCurrentIndex(ii);

    ii = pPunP->findText(QChar(flgs[32]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunP->setCurrentIndex(ii);
    ii = pPunT->findText(QChar(flgs[33]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	pPunT->setCurrentIndex(ii);

}

ProjectWizard::~ProjectWizard()
{}

void ProjectWizard::getFlags( char flgs[38] )
{
// Page 1 
// flgs 0-9 MSpmv, 10-29 TPptv, 30-33  TPun, 34-37 TPsv
	if( pTPptv_un->isChecked() ) // for all uncertains TPpvt[1]&TPpvt[9]
	  flgs[11] = flgs[19] = '+';
    else flgs[11] = flgs[19]= '-';

	if( pTPptv_2->isChecked() ) // TPpvt[2] & TPpvt[3]
	{	  flgs[12] = '+';
	    if( pTPptv_un->isChecked() ) // for all uncertains
	        flgs[13]  = '+';
	}
    else flgs[12] = flgs[13]= '-';
	
	if( pTPptv_4->isChecked() ) // TPpvt[4] & TPpvt[5]
	{	  flgs[14] = '+';
	    if( pTPptv_un->isChecked() ) // for all uncertains
	        flgs[15]  = '+';
	}
    else flgs[14] = flgs[15]= '-';

	if( pTPptv_6->isChecked() ) // TPpvt[6] & TPpvt[7]
	{	  flgs[16] = '+';
	    if( pTPptv_un->isChecked() ) // for all uncertains
	        flgs[17]  = '+';
	}
    else flgs[16] = flgs[17]= '-';

	if( pTPptv_10->isChecked() ) // TPpvt[10]
	  flgs[20]  = '+';
    else flgs[20] = '-';
	if( pTPptv_11->isChecked() ) // TPpvt[11]
	  flgs[21]  = '+';
    else flgs[21] = '-';
	if( pTPptv_12->isChecked() ) // TPpvt[12]
	  flgs[22]  = '+';
    else flgs[22] = '-';
	if( pTPptv_13->isChecked() ) // TPpvt[13]
	  flgs[23]  = '+';
    else flgs[23] = '-';
	if( pTPptv_14->isChecked() ) // TPpvt[14]
	  flgs[24]  = '+';
    else flgs[24] = '-';

	if( pMSpmv4->isChecked() ) // MSpmv[4]
	  flgs[4]  = '+';
    else flgs[4] = '-';
	if( pMSpmv6->isChecked() ) // MSpmv[6]
	  flgs[6]  = '+';
    else flgs[6] = '-';
// Page 2 not used	
}

void
ProjectWizard::help()
{
  pVisorImp->OpenHelp( PROJECT_HTML, 0, this/*, true*/ );
}

//--------------------- End of ProjectWizard.cpp ---------------------------
