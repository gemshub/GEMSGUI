//-------------------------------------------------------------------
// $Id: ProcessWizard.cpp 968 2007-12-13 13:23:32Z gems $                   
//
// Implementation of ProcessWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
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

const char *PROCESS_HTML = "gm_proces";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qvariant.h>

#include "ProcessWizard.h"
#include "gstring.h"
#include "visor_w.h"
#include "m_param.h"


void ProcessWizard::languageChange()
{
    retranslateUi(this);
}

void
ProcessWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
ProcessWizard::CmNext()
{
    int ndx = stackedWidget->currentIndex();
    int nLines = pageScript->getScriptLinesNum();
    if( ndx == 1 && nLines > 0)
        pGraph->setValue( nLines );

    stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	ProcessWizard::resetNextButton()
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

void 	ProcessWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


ProcessWizard::ProcessWizard( const char* pkey, char flgs[24], int size[6],
            const char *acalcScript, const char *aoutScript, QWidget* parent):
        QDialog( parent ), calcScript(acalcScript), outScript(aoutScript), pageScript(0)
{
    //setFinishEnabled( WizardPage2, true);
    setupUi(this);
    gstring str1= "GEM-Selektor Process Setup:  ";
        str1 += pkey;
        setWindowTitle( str1.c_str() );

    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();


 //Page1
   if( flgs[8] != '-' )
       pselT->setChecked( true );
   else  if( flgs[11] != '-' )
            pselG->setChecked( true );
          else  if( flgs[5] != '-' )
                  pselR->setChecked( true );
                else if( flgs[2] != '-' )
                       pselS->setChecked( true );
                     else
                       pselP->setChecked( true );
   if( !pselP->isChecked() )
     flgs[10] = '+';

//Page2
   resetPageList();

//Page3
    pPoints->setValue(size[0]);
    pMods->setValue(size[1]);
    pGraph->setValue(size[2]);
    pELine->setValue(size[3]);
    pECol->setValue(size[4]);

    if( flgs[12] != '-' )
      c_PvTm->setChecked( true );
    else c_PvTm->setChecked( false );

    if( flgs[13] != '-' )
      c_PvNV->setChecked( true );
    else c_PvNV->setChecked( false );

    if( flgs[17] != '-' )
      c_PvTau->setChecked( true );
    else c_PvTau->setChecked( false );

    if( flgs[14] != '-' )
      c_PvP->setChecked( true );
    else c_PvP->setChecked( false );

    if( flgs[15] != '-' )
      c_PvT->setChecked( true );
    else c_PvT->setChecked( false );

    if( flgs[16] != '-' )
      c_PvV->setChecked( true );
    else c_PvV->setChecked( false );

    if( flgs[18] != '-' )
      c_PvpXi->setChecked( true );
    else c_PvpXi->setChecked( false );

    if( flgs[19]!= '-' )
      c_PvNu->setChecked( true );
    else c_PvNu->setChecked( false );

    if( flgs[20] != '-' )
      c_PvKin->setChecked( true );
    else c_PvKin->setChecked( false );

//Page4
    if( flgs[22] != '-' )
      c_PvR1->setChecked( true );
    else c_PvR1->setChecked( false );

    if( flgs[0] >= '5' )
      pStepwise->setChecked( true );
    else pStepwise->setChecked( false );

    if( flgs[3] != '-' )
      c_PsRT->setChecked( true );
    else c_PsRT->setChecked( false );

    if( flgs[10] != '-' )
      c_PsEqn->setChecked( true );
    else c_PsEqn->setChecked( false );

    if( flgs[4] != '-' )
      c_PsSY->setChecked( true );
    else c_PsSY->setChecked( false );

//Page5
    spinBox18->setValue(size[5]);

}


ProcessWizard::~ProcessWizard()
{}

void   ProcessWizard::getSizes( int size[6] )
{

    size[0]= pPoints->value();
    size[1]= pMods->value();
    size[2]= pGraph->value();
    size[3]= pELine->value();
    size[4]= pECol->value();
    size[5]= spinBox18->value();
}


void   ProcessWizard::getFlags( char flgs[24] )
{
  char type='S';

  if( pselT->isChecked())
   type='T';
  else if( pselG->isChecked())
           type='G';
       else if( pselR->isChecked())
                type='R';
          else if( pselP->isChecked())
                type='P';
                else  if( pselS->isChecked())
                           type='S';
                    else  if( pselL->isChecked())
                                type='L';
   switch( type )
   {
    case 'T': strncpy( flgs, "0++-+--++-+-", 12);
        break;
    case 'G': strncpy( flgs, "0++-+--+--++", 12);
        break;
    case 'R': strncpy( flgs, "0++-++-+--+-", 12);
        break;
    case 'S': strncpy( flgs, "0++-+-----+-", 12);
        break;
    case 'L': strncpy( flgs, "0++-+-----+-", 12);
       break;
    case 'P':
    default:  memcpy( flgs, "0*----------", 12 );
   }
// Putting other flags
   if( c_PvTm->isChecked() )
       flgs[12] = '+';
    else flgs[12] = '-';

   if( c_PvNV->isChecked() )
       flgs[13] = '+';
    else flgs[13] = '-';

   if( c_PvTau->isChecked() )
       flgs[17] = '+';
    else flgs[17] = '-';

   if( c_PvP->isChecked() )
       flgs[14] = '+';
    else flgs[14] = '-';

   if( c_PvT->isChecked() )
       flgs[15] = '+';
    else flgs[15] = '-';

   if( c_PvV->isChecked() )
       flgs[16] = '+';
    else flgs[16] = '-';

   if( c_PvpXi->isChecked() )
       flgs[18] = '+';
    else flgs[18] = '-';

   if( c_PvNu->isChecked() )
       flgs[19] = '+';
    else flgs[19] = '-';

   if( c_PvKin->isChecked() )
       flgs[20] = '+';
    else flgs[20] = '-';

//Page 3
   if( c_PvR1->isChecked() )
       flgs[22] = '+';
    else flgs[22] = '-';

   if( pStepwise->isChecked() )
       flgs[0] = '5';
    else flgs[0] = '0';

   if( c_PsRT->isChecked() )
       flgs[3] = '+';

   if( c_PsEqn->isChecked() ) //setted before, not switch off
       flgs[10] = '+';

   if( c_PsSY->isChecked() ) //setted before, not switch off
       flgs[4] = '+';
    else flgs[4] = '-';

//Putting flags, using sizes  is out of function
}

void
ProcessWizard::help()
{
   pVisorImp->OpenHelp( PROCESS_HTML, 0, this/*, true*/ );
}

//==============================================================================

equatSetupData eqPr( "xp", "yp", "J", "J" );

// work with lists
void ProcessWizard::resetPageList()
{

    TIArray<pagesSetupData> scalarsList;
    TIArray<pagesSetupData> pgData;

    if(  rt[RT_SYSEQ].GetStatus() != ONEF_) // read first record in DB
        {
            TCStringArray aRklist;
            TCIntArray anRk;
            int Nr = rt[RT_SYSEQ].GetKeyList( ALLKEY, aRklist, anRk );
            if( Nr > 0 )
              TProfil::pm->loadSystat( aRklist[0].c_str() );
        }

    GetListsnRT( MD_MULTI, pgData,  scalarsList );
    GetListsnRT( MD_SYSTEM, pgData,  scalarsList );
    GetListsnRT( MD_MTPARM, pgData,  scalarsList );
    GetListsnRT( MD_RMULTS, pgData,  scalarsList );
    GetListsnRT( RT_PROCES, pgData,  scalarsList );

//    if( pageScript )
//      pageScript->resetPageList( newRT, pgData, scalarsList );
//    else
    {
      pageScript = new EquatSetup( page_5, eqPr,
              RT_PROCES, pgData, scalarsList, outScript.c_str()  );
      verticalLayout->addWidget(pageScript);
    }

}


//--------------------- End of ProcessWizard.cpp ---------------------------
