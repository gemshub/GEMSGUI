//-------------------------------------------------------------------
// $Id: GEM2MTWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of UnSpaceWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva
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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qvariant.h>

#include "UnSpaceWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void UnSpaceWizard::languageChange()
{
    retranslateUi(this);
}

void
UnSpaceWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
UnSpaceWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	UnSpaceWizard::resetNextButton()
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

void 	UnSpaceWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


UnSpaceWizard::UnSpaceWizard( const char* pkey, char flgs[38],
                              int size[10], QWidget* parent):
       QDialog( parent )
{
    //setFinishEnabled( WizardPage_3, true);
    setupUi(this);
    string str1= "GEM-Selektor UnSpace Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

    
//Page1
    int ii = p_Mode->findText(QChar(flgs[27]), Qt::MatchStartsWith|Qt::MatchCaseSensitive); //Pa_OF
    if( ii >= 0  )
    	p_Mode->setCurrentIndex(ii);
 
    ii = p_St->findText(QChar(flgs[28]), Qt::MatchStartsWith|Qt::MatchCaseSensitive); //Pa_Crit
    if( ii >= 0  )
    	p_St->setCurrentIndex(ii); 	

    pnQ->setValue(size[0]);     // Q
    pnM->setValue(size[1]);     // quan_lev
    pnP->setValue(size[2]);     // nGB
    pLb->setValue(size[3]);     // nGN
    pLb_2->setValue(size[4]);   // nGR

// Page 2
    ii = p_PsUnInt->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_PsUnInt->setCurrentIndex(ii); 	

    ii = p_PsUnInB->findText(QChar(flgs[7]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_PsUnInB->setCurrentIndex(ii); 	

    ii = p_PsUnFltI->findText(QChar(flgs[8]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_PsUnFltI->setCurrentIndex(ii); 	

    ii = p_PsUnFltD->findText(QChar(flgs[9]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	p_PsUnFltD->setCurrentIndex(ii); 	
 
    if( flgs[10] != '-' )
      c_PsGen1->setChecked( true );
    else c_PsGen1->setChecked( false );
    if( flgs[11] != '-' )
      c_PsGen2->setChecked( true );
    else c_PsGen2->setChecked( false );
    if( flgs[12] != '-' )
      c_PsGen3->setChecked( true );
    else c_PsGen3->setChecked( false );
    if( flgs[13] != '-' )
      c_PsGen4->setChecked( true );
    else c_PsGen4->setChecked( false );
    if( flgs[14] != '-' )
      c_PsGen5->setChecked( true );
    else c_PsGen5->setChecked( false );
    if( flgs[15] != '-' )
      c_PsGen6->setChecked( true );
    else c_PsGen6->setChecked( false );
    if( flgs[16] != '-' )
      c_PsGen7->setChecked( true );
    else c_PsGen7->setChecked( false );

    if( flgs[20] != '-' )
      c_Pa_f1->setChecked( true );
    else c_Pa_f1->setChecked( false );
    if( flgs[21] != '-' )
      c_Pa_f2->setChecked( true );
    else c_Pa_f2->setChecked( false );
    if( flgs[22] != '-' )
      c_Pa_f3->setChecked( true );
    else c_Pa_f3->setChecked( false );
    if( flgs[23] != '-' )
      c_Pa_f4->setChecked( true );
    else c_Pa_f4->setChecked( false );
    if( flgs[24] != '-' )
      c_Pa_f5->setChecked( true );
    else c_Pa_f5->setChecked( false );
    if( flgs[25] != '-' )
      c_Pa_f6->setChecked( true );
    else c_Pa_f6->setChecked( false );

// Page 3
    p_Pa_Adapt->setValue((int)(flgs[26]-'0'));

    if( flgs[18] != '-' )
      c_PsEqn->setChecked( true );
    else c_PsEqn->setChecked( false );
    if( flgs[19] != '-' )
      c_PsGraph->setChecked( true );
    else c_PsGraph->setChecked( false );
    if( flgs[29] != '-' )
      c_Pa_Zcp->setChecked( true );
    else c_Pa_Zcp->setChecked( false );
    if( flgs[36] != '-' )
      c_PvSi->setChecked( true );
    else c_PvSi->setChecked( false );
    if( flgs[17] != '-' )
      c_PsSy->setChecked( true );
    else c_PsSy->setChecked( false );

    if( flgs[32] != '-' )
      c_PvGs->setChecked( true );
    else c_PvGs->setChecked( false );
    if( flgs[34] != '-' )
      c_PvVs->setChecked( true );
    else c_PvVs->setChecked( false );
    if( flgs[35] != '-' )
      c_PvPgam->setChecked( true );
    else c_PvPgam->setChecked( false );
    if( flgs[30] != '-' )
      c_PvPOM->setChecked( true );
    else c_PvPOM->setChecked( false );
    if( flgs[33] != '-' )
      c_PvSs->setChecked( true );
    else c_PvSs->setChecked( false );
    if( flgs[31] != '-' )
      c_PvPOR->setChecked( true );
    else c_PvPOR->setChecked( false );

// Page 4
    p_Nsd->setValue(size[5]);  // Nsd
    pGraph->setValue(size[6]); // dimXY[1] must be 3
    pELine->setValue(size[7]); // dimEF[0] any
    pECol->setValue(size[8]);  // dimEF[1] must be 5

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

}


UnSpaceWizard::~UnSpaceWizard()
{}

void   UnSpaceWizard::getSizes( int size[10] )
{

    size[0] = pnQ->value();     // Q
    size[1] = pnM->value();     // quan_lev
    size[2] = pnP->value();     // nGB
    size[3] = pLb->value();     // nGN
    size[4] = pLb_2->value();   // nGR
    size[5] = p_Nsd->value();  // Nsd
    size[6] = pGraph->value(); // dimXY[1] must be 3
    size[7] = pELine->value(); // dimEF[0] any
    size[8] = pECol->value();  // dimEF[1] must be 5
}


void   UnSpaceWizard::getFlags( char flgs[38] )
{
  QString str;

//Page1
   str = p_Mode->currentText();
   flgs[27] = str[0].toLatin1();
   str = p_St->currentText();
   flgs[28] = str[0].toLatin1();

// Page 2
   str = p_PsUnInt->currentText();
   flgs[6] = str[0].toLatin1();
   str = p_PsUnInB->currentText();
   flgs[7] = str[0].toLatin1();
   str = p_PsUnFltI->currentText();
   flgs[8] = str[0].toLatin1();
   str = p_PsUnFltD->currentText();
   flgs[9] = str[0].toLatin1();

   if( c_PsGen1->isChecked() )
       flgs[10] = '+';
    else flgs[10] = '-';
   if( c_PsGen2->isChecked() )
       flgs[11] = '+';
    else flgs[11] = '-';
   if( c_PsGen3->isChecked() )
       flgs[12] = '+';
    else flgs[12] = '-';
   if( c_PsGen4->isChecked() )
       flgs[13] = '+';
    else flgs[13] = '-';
   if( c_PsGen5->isChecked() )
       flgs[14] = '+';
    else flgs[14] = '-';
   if( c_PsGen6->isChecked() )
       flgs[15] = '+';
    else flgs[15] = '-';
   if( c_PsGen7->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';

   if( c_Pa_f1->isChecked() )
       flgs[20] = '+';
    else flgs[20] = '-';
   if( c_Pa_f2->isChecked() )
       flgs[21] = '+';
    else flgs[21] = '-';
   if( c_Pa_f3->isChecked() )
       flgs[22] = '+';
    else flgs[22] = '-';
   if( c_Pa_f4->isChecked() )
       flgs[23] = '+';
    else flgs[23] = '-';
   if( c_Pa_f5->isChecked() )
       flgs[24] = '+';
    else flgs[24] = '-';
   if( c_Pa_f6->isChecked() )
       flgs[25] = '+';
    else flgs[25] = '-';

// Page 3
   flgs[26] = (char)(p_Pa_Adapt->value())+'0';
    p_Pa_Adapt->setValue((int)(flgs[26]-'0'));

   if( c_PsEqn->isChecked() )
       flgs[18] = '+';
    else flgs[18] = '-';
   if( c_PsGraph->isChecked() )
       flgs[19] = '+';
    else flgs[19] = '-';
   if( c_Pa_Zcp->isChecked() )
       flgs[29] = '+';
    else flgs[29] = '-';
   if( c_PvSi->isChecked() )
       flgs[36] = '+';
    else flgs[36] = '-';
   if( c_PsSy->isChecked() )
       flgs[17] = '+';
    else flgs[17] = '-';

   if( c_PvGs->isChecked() )
       flgs[32] = '+';
    else flgs[32] = '-';
   if( c_PvVs->isChecked() )
       flgs[34] = '+';
    else flgs[34] = '-';
   if( c_PvPgam->isChecked() )
       flgs[35] = '+';
    else flgs[35] = '-';
   if( c_PvPOM->isChecked() )
       flgs[30] = '+';
    else flgs[20] = '-';
   if( c_PvSs->isChecked() )
       flgs[33] = '+';
    else flgs[33] = '-';
   if( c_PvPOR->isChecked() )
       flgs[31] = '+';
    else flgs[31] = '-';

// Page 4
   str = pPunE->currentText();
   flgs[0] = str[0].toLatin1();
   str = pPunV->currentText();
   flgs[1] = str[0].toLatin1();
   str = pPunP->currentText();
   flgs[2] = str[0].toLatin1();
   str = pPunT->currentText();
   flgs[3] = str[0].toLatin1();

// flags 0,1, 37 not used or internal
   flgs[4] = '0';
   flgs[5] = '0';
}

void
UnSpaceWizard::help()
{
  pVisorImp->OpenHelp( GM_UNSPACE_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//--------------------- End of UnSpaceWizard.cpp ---------------------------
