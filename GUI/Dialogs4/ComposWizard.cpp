//-------------------------------------------------------------------
// $Id: ComposWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ComposWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva, D.Kulik
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

#include "ui_ComposWizard4.h"
#include "ComposWizard.h"
#include "GemsMainWindow.h"
#include "service.h"


void ComposWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void ComposWizard::CmNext()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void ComposWizard::resetNextButton()
{
    if( ui->stackedWidget->currentIndex() == ui->stackedWidget->count() - 1 )
    {
        ui->pNext->disconnect();
        connect( ui->pNext, SIGNAL(clicked()), this, SLOT(accept()) );
        ui->pNext->setText("&Finish");
    }
    else
    {
        ui->pNext->disconnect();
        connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()) );
        ui->pNext->setText("&Next>");
    }
}

void ComposWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}


ComposWizard::ComposWizard( const char* pkey, char flgs[6], int size[2],
double r2, QWidget* parent):
    QDialog( parent ),
    ui(new Ui::ComposWizardData)
{
    ui->setupUi(this);
    string str1= "GEM-Selektor Compos Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );
    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();
    
    // set up startup sizes
    ui->spinBox11->setValue(size[0]);
    ui->spinBox18->setValue(size[1]);

    // set up startup cheks
    if( flgs[0] == '+' || flgs[0] == '*' )
        ui->checkBox2->setChecked( true );
    else ui->checkBox2->setChecked( false );

    if( flgs[1] == '+' || flgs[1] == '*' )
        ui->checkBox2_2->setChecked( true );
    else ui->checkBox2_2->setChecked( false );

    if( flgs[4] == '+' || flgs[4] == '*' )
        ui->checkBox8->setChecked( true );
    else ui->checkBox8->setChecked( false );

    // Ctext calculation

    if( flgs[3] == '+' || flgs[3] == '*' )
        ui->checkBox2_2_2->setChecked( true );
    else ui->checkBox2_2_2->setChecked( false );

    QString str;
    ui->lineEdit1->setValidator( new QDoubleValidator( ui->lineEdit1 ) );
    ui->lineEdit1->setText( str.setNum(r2) );

    int ii = ui->comboBox1->findText(QChar(flgs[5]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->comboBox1->setCurrentIndex(ii);

    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


ComposWizard::~ComposWizard()
{
    delete ui;
}

void   ComposWizard::getSizes( int size[2] )
{
    size[0]= ui->spinBox11->value();
    size[1]= ui->spinBox18->value();
}

double   ComposWizard::getR2()
{
    double r=ui->lineEdit1->text().toDouble();

    if( approximatelyZero(r) && ui->checkBox2_2_2->isChecked() )
        r = 1.;
    return r;
}


void ComposWizard::getFlags( char flgs[6] )
{
    if( ui->checkBox2->isChecked() )
        flgs[0] = '+';
    else flgs[0] = '-';

    if( ui->checkBox2_2->isChecked() )
        flgs[1] = '+';
    else flgs[1] = '-';

    if( ui->checkBox2_2_2->isChecked() )
        flgs[3] = '+';
    else flgs[3] = '-';

    if( ui->spinBox11->value() > 0 )
        flgs[2] = '+';
    else flgs[2] = '-';

    if( ui->checkBox8->isChecked() )
        flgs[4] = '+';
    else flgs[4] = '-';

    QString str = ui->comboBox1->currentText();
    flgs[5] = str[0].toLatin1();

}


void ComposWizard::help()
{
    pVisorImp->OpenHelp( GM_COMPOS_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

//--------------------- End of ComposWizard.cpp ---------------------------
