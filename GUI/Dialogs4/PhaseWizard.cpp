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

PhaseWizard::PhaseWizard( const char* pkey, char flgs[33], int size[22],
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
    ii = kin_t0->findText(QChar(flgs[27]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
      if( ii >= 0  )
          kin_t0->setCurrentIndex(ii);
    ii = kin_t1->findText(QChar(flgs[28]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
        if( ii >= 0  )
            kin_t1->setCurrentIndex(ii);

    PsDiS->setChecked(flgs[21] == '+');

    nlPh->setValue(size[6]);
    nEIl->setValue(size[14]);
    nlPc->setValue(size[7]);
    nEIp->setValue(size[15]);
    ndqf->setValue(size[8]);
    nCDc->setValue(size[16]);
    nrcp->setValue(size[9]);

//Page4
    ii = kin_t2->findText(QChar(flgs[29]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        kin_t2->setCurrentIndex(ii);
    ii = kin_t3->findText(QChar(flgs[30]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        kin_t3->setCurrentIndex(ii);
    ii = kin_t4->findText(QChar(flgs[31]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        kin_t4->setCurrentIndex(ii);
    ii = kin_t5->findText(QChar(flgs[32]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
        if( ii >= 0  )
            kin_t5->setCurrentIndex(ii);

    nFaces->setValue(size[10]);
    nIsoC->setValue(size[18]);
    nReg->setValue(size[11]);
    nIsoS->setValue(size[19]);
    nrpC->setValue(size[12]);
    mDe->setValue(size[20]);
    numpC->setValue(size[13]);

//Page5
    spinBoxSDrefLnk->setValue(size[0]);
}


PhaseWizard::~PhaseWizard()
{}

void   PhaseWizard::getSizes( int size[22] )
{
    size[0]= spinBoxSDrefLnk->value();
    size[1]= spinBox_ph_cf_N->value();
    size[2]= spinBox_ph_cf_M->value();
    size[3]= spinBox_dc_sc_M->value();
    size[4]= spinBox_ph_px_M->value();
    size[5]= spinBox_SurTypes_N->value();

    if( SCM_code->currentText()[0] != QChar('N') )
      if( size[5] == 0 ) size[5] = 1;

    size[6]= nlPh->value();
    size[14]= nEIl->value();
    size[7]= nlPc->value();
    size[15]= nEIp->value();
    size[8]= ndqf->value();
    size[16]= nCDc->value();
    size[9]= nrcp->value();
    size[10]= nFaces->value();
    size[18]= nIsoC->value();
    size[11]= nReg->value();
    size[19]= nIsoS->value();
    size[12]= nrpC->value();
    size[20]= mDe->value();
    size[13]= numpC->value();
}

double   PhaseWizard::getR2()
{
 double r=lineEdit_surfArea->text().toDouble();

 if( SCM_code->currentText()[0] != QChar('N') )
      if( r == 0. ) r = 1.;
  return r;
}


void PhaseWizard::getFlags( char flgs[33] )
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

  str = kin_t0->currentText();
  flgs[27] = str[0].toLatin1();
  str = kin_t1->currentText();
  flgs[28] = str[0].toLatin1();
  str = kin_t2->currentText();
  flgs[29] = str[0].toLatin1();
  str = kin_t3->currentText();
  flgs[30] = str[0].toLatin1();
  str = kin_t4->currentText();
  flgs[31] = str[0].toLatin1();
  str = kin_t5->currentText();
  flgs[32] = str[0].toLatin1();

  if( ndqf->value() > 0  )
      flgs[13] = '+';
   else flgs[13] = '-';
  if( nrcp->value() > 0  )
      flgs[14] = '+';
   else flgs[14] = '-';
  if( nlPh->value() > 0 )
      flgs[15] = '+';
   else flgs[15] = '-';
  if( nEIl->value() > 0 )
      flgs[17] = '+';
   else flgs[17] = '-';
  if( nCDc->value() > 0 )
      flgs[18] = '+';
   else flgs[18] = '-';
  if( nIsoC->value() > 0 )
      flgs[19] = '+';
   else flgs[19] = '-';
  if( nIsoS->value() > 0 )
      flgs[20] = '+';
   else flgs[20] = '-';
  if( PsDiS->isChecked() )
      flgs[21] = '+';
   else flgs[21] = '-';
  if( nFaces->value() > 0 && nReg->value() > 0 )
      flgs[23] = '+';
   else flgs[23] = '-';
  if( numpC->value() > 0 )
      flgs[24] = '+';
   else flgs[24] = '-';

}




void
PhaseWizard::help()
{
  pVisorImp->OpenHelp( GM_PHASE_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//--------------------- End of PhaseWizard.cpp ---------------------------
