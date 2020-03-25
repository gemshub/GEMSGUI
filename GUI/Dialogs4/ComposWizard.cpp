//-------------------------------------------------------------------
// $Id: ComposWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ComposWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva, D.Kulik
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


#include "ComposWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void ComposWizard::languageChange()
{
    retranslateUi(this);
}

void
ComposWizard::CmBack()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void
ComposWizard::CmNext()
{
	stackedWidget->setCurrentIndex ( stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	ComposWizard::resetNextButton()
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

void 	ComposWizard::resetBackButton()
{
	pBack->setEnabled( stackedWidget->currentIndex() > 0 );
}


ComposWizard::ComposWizard( const char* pkey, char flgs[6], int size[2],
                            double r2, QWidget* parent):
    QDialog( parent )
{
    setupUi(this);
    gstring str1= "GEM-Selektor Compos Setup:  ";
            str1 += pkey;
            setWindowTitle( str1.c_str() );
    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();
    
    
// set up startup sizes
    spinBox11->setValue(size[0]);
    spinBox18->setValue(size[1]);

// set up startup cheks
    if( flgs[0] == '+' || flgs[0] == '*' )
      checkBox2->setChecked( true );
    else checkBox2->setChecked( false );

    if( flgs[1] == '+' || flgs[1] == '*' )
      checkBox2_2->setChecked( true );
    else checkBox2_2->setChecked( false );

    if( flgs[4] == '+' || flgs[4] == '*' )
      checkBox8->setChecked( true );
    else checkBox8->setChecked( false );

// Ctext calculation

    if( flgs[3] == '+' || flgs[3] == '*' )
      checkBox2_2_2->setChecked( true );
    else checkBox2_2_2->setChecked( false );

    QString str;
    lineEdit1->setValidator( new QDoubleValidator( lineEdit1 ) );
    lineEdit1->setText( str.setNum(r2) );

    int ii = comboBox1->findText(QChar(flgs[5]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
    	comboBox1->setCurrentIndex(ii);
}


ComposWizard::~ComposWizard()
{}

void   ComposWizard::getSizes( int size[2] )
{
    size[0]= spinBox11->value();
    size[1]= spinBox18->value();
}

double   ComposWizard::getR2()
{
 double r=lineEdit1->text().toDouble();

 if( r == 0. && checkBox2_2_2->isChecked() )
       r = 1.;
  return r;
}


void ComposWizard::getFlags( char flgs[6] )
{
  if( checkBox2->isChecked() )
       flgs[0] = '+';
  else flgs[0] = '-';

  if( checkBox2_2->isChecked() )
       flgs[1] = '+';
  else flgs[1] = '-';

  if( checkBox2_2_2->isChecked() )
       flgs[3] = '+';
  else flgs[3] = '-';

  if( spinBox11->value() > 0 )
       flgs[2] = '+';
  else flgs[2] = '-';

  if( checkBox8->isChecked() )
       flgs[4] = '+';
  else flgs[4] = '-';

  QString str = comboBox1->currentText();
  flgs[5] = str[0].toLatin1();

}


void
ComposWizard::help()
{
  pVisorImp->OpenHelp( GM_COMPOS_WZ_HTML, WZSTEP, stackedWidget->currentIndex()+1 );
}

//--------------------- End of ComposWizard.cpp ---------------------------
