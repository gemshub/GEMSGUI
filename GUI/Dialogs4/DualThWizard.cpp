//-------------------------------------------------------------------
// $Id: RTparmWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of DualThWizard class
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

#include "ui_DualThWizard4.h"
#include "DualThWizard.h"
#include "GemsMainWindow.h"
#include "service.h"


void DualThWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void DualThWizard::CmNext()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void DualThWizard::resetNextButton()
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

void DualThWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}


DualThWizard::DualThWizard( const char* pkey, char flgs[20], int size[8],
QWidget* parent):
    QDialog( parent ),
    ui(new Ui::DualThWizardData)
{
    int ii;

    // setFinishEnabled( WizardPage2, true);
    ui->setupUi(this);
    string str1= "GEM-Selektor DualTh Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );

    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

    // page1
    ii = ui->p_Mode->findText(QChar(flgs[12]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_Mode->setCurrentIndex(ii);

    ii = ui->p_St->findText(QChar(flgs[13]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_St->setCurrentIndex(ii);

    ii = ui->p_IPf->findText(QChar(flgs[17]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_IPf->setCurrentIndex(ii);

    ii = ui->p_IPu->findText(QChar(flgs[18]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_IPu->setCurrentIndex(ii);

    ii = ui->p_LSF->findText(QChar(flgs[19]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_LSF->setCurrentIndex(ii);

    // page2

    ui->pnQ->setValue(size[0]);
    ui->pnM->setValue(size[2]);
    ui->pnP->setValue(size[7]);
    ui->pLb->setValue(size[1]);

    ii = ui->pPunE->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunE->setCurrentIndex(ii);
    ii = ui->pPunV->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunV->setCurrentIndex(ii);
    ii = ui->pPunP->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunP->setCurrentIndex(ii);
    ii = ui->pPunT->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunT->setCurrentIndex(ii);

    // Page 3

    if( flgs[5] != '-' )
        ui->c_ICb->setChecked( true );
    else ui->c_ICb->setChecked( false );
    if( flgs[6] != '-' )
        ui->c_ICn->setChecked( true );
    else ui->c_ICn->setChecked( false );
    if( flgs[7] != '-' )
        ui->c_AUb->setChecked( true );
    else ui->c_AUb->setChecked( false );
    if( flgs[11] != '-' )
        ui->c_TPI->setChecked( true );
    else ui->c_TPI->setChecked( false );
    if( flgs[9] != '-' )
        ui->c_Chi->setChecked( true );
    else ui->c_Chi->setChecked( false );
    if( flgs[10] != '-' )
        ui->c_Gam->setChecked( true );
    else ui->c_Gam->setChecked( false );
    if( flgs[4] != '-' )
        ui->c_Coul->setChecked( true );
    else ui->c_Coul->setChecked( false );
    if( flgs[16] != '-' )
        ui->c_SYd->setChecked( true );
    else ui->c_SYd->setChecked( false );

    //Page 4
    ui->p_Nsd->setValue(size[3]);

    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    QObject::connect(ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));

}


DualThWizard::~DualThWizard()
{
    delete ui;
}

void   DualThWizard::getSizes( int size[8] )
{
    // Nqpn, Nqpg, Nb internal
    size[0]= ui->pnQ->value();
    size[1]= ui->pLb->value();
    size[2]= ui->pnM->value();
    size[3]= ui->p_Nsd->value();
    size[7]= ui->pnP->value();
}

void DualThWizard::getFlags( char flgs[20] )
{
    // Page 1
    QString str;

    str = ui->p_Mode->currentText();
    flgs[12] = str[0].toLatin1();
    str = ui->p_St->currentText();
    flgs[13] = str[0].toLatin1();
    str = ui->p_IPf->currentText();
    flgs[17] = str[0].toLatin1();
    str = ui->p_IPu->currentText();
    flgs[18] = str[0].toLatin1();
    str = ui->p_LSF->currentText();
    flgs[19] = str[0].toLatin1();

    // Page 2 disabled flags

    //Page 3
    if( ui->c_ICb->isChecked() )
        flgs[5] = '+';
    else flgs[5] = '-';

    if( ui->c_ICn->isChecked() )
        flgs[6] = '+';
    else flgs[6] = '-';

    if( ui->c_AUb->isChecked() )
        flgs[7] = '+';
    else flgs[7] = '-';

    if( ui->c_TPI->isChecked() )
        flgs[11] = '+';
    else flgs[11] = '-';

    if( ui->c_Chi->isChecked() )
        flgs[9] = '+';
    else flgs[9] = '-';

    if( ui->c_Gam->isChecked() )
        flgs[10] = '+';
    else flgs[10] = '-';

    if( ui->c_Coul->isChecked() )
        flgs[4] = '+';
    else flgs[4] = '-';

    if( ui->c_SYd->isChecked() )
        flgs[16] = '+';
    else flgs[16] = '-';

    // page 4
    if( ui->p_Nsd->value() > 0  )
        flgs[8] = '+';
    else flgs[8] = '-';
}

void DualThWizard::help()
{
    pVisorImp->OpenHelp( GM_DUALTH_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

//--------------------- End of DualThWizard.cpp ---------------------------
