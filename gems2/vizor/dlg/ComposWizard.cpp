//-------------------------------------------------------------------
// $$
//
// Implementation of ComposWizard class
//
// Copyright (C) 2005  S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

const char *COMPOS_HTML = "compos_setup";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>

#include "ComposWizard.h"
#include "visor_w.h"

#define Inherited ComposWizardData
ComposWizard::ComposWizard( const char* pkey, char flgs[6], int size[2],
                            double r2, QWidget* parent):
        Inherited( parent, 0, true )
{
    gstring str1= "Setup to Create Compos:  ";
            str1 += pkey;
    setCaption( str1.c_str() );

    setFinishEnabled( WizardPage2, true);
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

    for(int ii=0; ii < (comboBox1->count()); ii++)
    {
      if(comboBox1->text(ii)[0] == QChar(flgs[5]) )
      {  comboBox1->setCurrentItem(ii);
         break;
      }
    }
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
  flgs[5] = str[0].latin1();

}


void
ComposWizard::help()
{
  pVisorImp->OpenHelp( COMPOS_HTML, 0, this/*, true*/ );
}

//--------------------- End of ComposWizard.cpp ---------------------------
