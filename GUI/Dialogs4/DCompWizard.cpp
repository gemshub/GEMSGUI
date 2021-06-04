//-------------------------------------------------------------------
// $Id: DCompWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of DCompWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva, D.Kulik
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

#include "ui_DCompWizard4.h"
#include "DCompWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void DCompWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex (  ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}


void DCompWizard::CmNext()
{
    ui->stackedWidget->setCurrentIndex (  ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}


void DCompWizard::resetNextButton()
{
    if(  ui->stackedWidget->currentIndex() ==  ui->stackedWidget->count() - 1 )
    {
        ui->pNext->disconnect();
        connect(  ui->pNext, SIGNAL(clicked()), this, SLOT(accept()) );
        ui->pNext->setText("&Finish");
    }
    else
    {
        ui->pNext->disconnect();
        connect(  ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()) );
        ui->pNext->setText("&Next>");
    }
}


void DCompWizard::resetBackButton()
{
    ui->pBack->setEnabled(  ui->stackedWidget->currentIndex() > 0 );
}


DCompWizard::DCompWizard( const char* pkey, char flgs[15], int size[4],
QWidget* parent):
    QDialog( parent ),
    ui(new Ui::DCompWizardData)
{

    ui->setupUi(this);
    string str1= "GEM-Selektor DComp Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );

    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

    //Page 1
    int ii;
    ii = ui->p_pct_1->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_1->setCurrentIndex(ii);
    ii = ui->p_pct_2->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_2->setCurrentIndex(ii);
    ii = ui->p_pct_3->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_3->setCurrentIndex(ii);
    ii = ui->p_pct_4->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_4->setCurrentIndex(ii);
    ii =  ui->pPdcC->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPdcC->setCurrentIndex(ii);

    //Page 2
    ui->pNeCp->setValue(size[0]);
    ui->pNft->setValue(size[1]);
    ui->pNemp->setValue(size[3]);
    ui->pPdcVT->setChecked( flgs[14] == '+' );

    ii = ui->pPunE->findText(QChar(flgs[7]),Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunE->setCurrentIndex(ii);
    ii = ui->pPunV->findText(QChar(flgs[8]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunV->setCurrentIndex(ii);
    ii = ui->pPunP->findText(QChar(flgs[9]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunP->setCurrentIndex(ii);
    ii = ui->pPunT->findText(QChar(flgs[10]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunT->setCurrentIndex(ii);

    //Page3
    ui->spinBoxSDrefLnk->setValue(size[2]);

    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));

}


DCompWizard::~DCompWizard()
{
    delete ui;
}


void   DCompWizard::getSizes( int size[4] )
{
    size[0]= ui->pNeCp->value();
    size[1]= ui->pNft->value();
    size[2]= ui->spinBoxSDrefLnk->value();
    size[3]= ui->pNemp->value();

}


void DCompWizard::getFlags( char flgs[15] )
{
    QString str;

    str = ui->p_pct_1->currentText();
    flgs[0] = str[0].toLatin1();
    str = ui->p_pct_2->currentText();
    flgs[1] = str[0].toLatin1();
    str = ui->p_pct_3->currentText();
    flgs[2] = str[0].toLatin1();
    str = ui->p_pct_4->currentText();
    flgs[3] = str[0].toLatin1();
    // 4, 5 (reserved)
    str = ui->pPdcC->currentText();
    flgs[6] = str[0].toLatin1();
    // 7,8,9,10 not changed
    if( ui->pNeCp->value() > 0  )
        flgs[11] = '+';
    else
        flgs[11] = '-';

    if( ui->pNft->value() > 0  )
        flgs[12] = '+';
    else
        flgs[12] = '-';

    if( flgs[0] == 'H'  )
        flgs[13] = '+';
    else
        flgs[13] = '-';
    // check
    if( ui->pPdcVT->isChecked() )
        flgs[14] = '+';
    else
        flgs[14] = '-';
}


void DCompWizard::help()
{
    pVisorImp->OpenHelp( GM_DCOMP_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}


//--------------------- End of DCompWizard.cpp ---------------------------
