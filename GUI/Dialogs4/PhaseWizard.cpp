//-------------------------------------------------------------------
// $Id: PhaseWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of PhaseWizard class
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

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qvariant.h>

#include "PhaseWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void PhaseWizard::languageChange()
{
    retranslateUi(this);
}

void
PhaseWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
PhaseWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	PhaseWizard::resetNextButton()
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

void 	PhaseWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}

PhaseWizard::PhaseWizard( const char* pkey, char flgs[13], int size[6],
                            double r2, QWidget* parent):
        QDialog( parent )
{
    //setFinishEnabled( WizardPage3, true);
    setupUi(this);
    gstring str1= "GEM-Selektor Phase Setup:  ";
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
    ii = ModelCode->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	ModelCode->setCurrentIndex(ii);
 
    ii = AC_modeCode->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	AC_modeCode->setCurrentIndex(ii);
  
    ii = DcEqModeCode->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	DcEqModeCode->setCurrentIndex(ii);

  ii = PhEqModeCode->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	PhEqModeCode->setCurrentIndex(ii);

   ii = DcEqLinkMode->findText(QChar(flgs[4]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	DcEqLinkMode->setCurrentIndex(ii);

   ii = SCM_code->findText(QChar(flgs[5]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	SCM_code->setCurrentIndex(ii);

    ii = pPhPs_0->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
     if( ii >= 0  )
         pPhPs_0->setCurrentIndex(ii);

 //Page 2
    spinBox_dc_sc_M->setValue(size[3]);
    spinBox_ph_px_M->setValue(size[4]);
    spinBox_ph_cf_N->setValue(size[1]);
    spinBox_ph_cf_M->setValue(size[2]);

    if( flgs[5] != 'N' )
    {
      checkBoxSorption->setChecked( true );
      if( size[5] == 0 ) size[5] = 1;
      if( r2 == 0. ) r2 = 1.;
    }
    else checkBoxSorption->setChecked( false );

    if( flgs[11] == '+' )
     checkBoxSorption->setChecked(true);
    else
     checkBoxSorption->setChecked(false);


    spinBox_SurTypes_N->setValue(size[5]);
    QString str;
    lineEdit_surfArea->setValidator( new QDoubleValidator( lineEdit_surfArea ));
    lineEdit_surfArea->setText( str.setNum(r2) );

//Page3
    spinBoxSDrefLnk->setValue(size[0]);
}


PhaseWizard::~PhaseWizard()
{}

void   PhaseWizard::getSizes( int size[6] )
{
    size[0]= spinBoxSDrefLnk->value();
    size[1]= spinBox_ph_cf_N->value();
    size[2]= spinBox_ph_cf_M->value();
    size[3]= spinBox_dc_sc_M->value();
    size[4]= spinBox_ph_px_M->value();
    size[5]= spinBox_SurTypes_N->value();

    if( SCM_code->currentText()[0] != QChar('N') )
      if( size[5] == 0 ) size[5] = 1;

}

double   PhaseWizard::getR2()
{
 double r=lineEdit_surfArea->text().toDouble();

 if( SCM_code->currentText()[0] != QChar('N') )
      if( r == 0. ) r = 1.;
  return r;
}


void PhaseWizard::getFlags( char flgs[13] )
{
  QString str;

     str = ModelCode->currentText();
  flgs[0] = str[0].toLatin1();
     str = AC_modeCode->currentText();
  flgs[3] = str[0].toLatin1();
     str = DcEqModeCode->currentText();
  flgs[1] = str[0].toLatin1();
     str = PhEqModeCode->currentText();
  flgs[2] = str[0].toLatin1();
     str = DcEqLinkMode->currentText();
  flgs[4] = str[0].toLatin1();
     str = SCM_code->currentText();
  flgs[5] = str[0].toLatin1();
  // flgs[6] getting from key
  str = pPhPs_0->currentText();
  flgs[6] = str[0].toLatin1();

  if( spinBox_ph_cf_N->value() > 0 &&
      spinBox_ph_cf_M->value() > 0 )
    flgs[7] = '+';
  else flgs[7] = '-';

  if( spinBox_dc_sc_M->value() > 0 /*&&
      spinBox_ph_px_M->value() > 0*/ )
    flgs[8] = '+';
  else flgs[8] = '-';

//  if( checkBoxSorption->isChecked() )
//       flgs[11] = '+';
//  else flgs[11] = '-';
  //if( flgs[5] != 'N' )
  //     flgs[11] = '+';
  if( flgs[0] == 'A' )
       flgs[11] = '+';
  else flgs[11] = '-';

  // Flags for scripts
 flgs[9] = '-';
 flgs[10] = '-';
 switch( flgs[3] )
  {
   case 'I':
   case 'S':
   default:  break;
   case 'N':  if( flgs[2] != 'N')
                flgs[9] = '+';
              if( flgs[1] != 'N')
                flgs[10] = '+';
             break;
  }
  // testing task
  if( flgs[10] != '+' )
     flgs[4] = 'N';

  if( getDCC->isChecked())
      flgs[12] = '+';
  else
      flgs[12] = '-';

}


void
PhaseWizard::help()
{
  pVisorImp->OpenHelp( GM_PHASE_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//--------------------- End of PhaseWizard.cpp ---------------------------
