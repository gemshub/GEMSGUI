//-------------------------------------------------------------------
// $Id: ProjectWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ProjectWizard class
//
// Copyright (C) 2009  S.Dmytriyeva, D.Kulik
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

#include <QComboBox>

#include "ui_ProjectWizard4.h"
#include "ProjectWizard.h"
#include "GemsMainWindow.h"
#include "service.h"


void ProjectWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void ProjectWizard::CmNext()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	ProjectWizard::resetNextButton()
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

void 	ProjectWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}

ProjectWizard::ProjectWizard( const char* pkey, char flgs[38], QWidget* parent):
    QDialog( parent ),
    ui(new Ui::ProjectWizardData)
{
    int ii;

    //setFinishEnabled( WizardPage3, true);
    ui->setupUi(this);
    string str1= "GEM-Selektor Project Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );

    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

    // page1
    // flgs 0-9 MSpmv, 10-29 TPptv, 30-33  TPun, 34-37 TPsv

    if( flgs[11] != '-' )    // TPptv[1] for all allocated
        ui->pTPptv_un->setChecked( true );
    else ui->pTPptv_un->setChecked( false );

    if( flgs[12] != '-' )    // TPptv[2]
        ui->pTPptv_2->setChecked( true );
    else ui->pTPptv_2->setChecked( false );

    if( flgs[14] != '-' )    // TPptv[4]
        ui->pTPptv_4->setChecked( true );
    else ui->pTPptv_4->setChecked( false );

    if( flgs[16] != '-' )    // TPptv[6]
        ui->pTPptv_6->setChecked( true );
    else ui->pTPptv_6->setChecked( false );

    if( flgs[20] != '-' )    // TPptv[10]
        ui->pTPptv_10->setChecked( true );
    else ui->pTPptv_10->setChecked( false );
    if( flgs[21] != '-' )    // TPptv[11]
        ui->pTPptv_11->setChecked( true );
    else ui->pTPptv_11->setChecked( false );
    if( flgs[22] != '-' )    // TPptv[12]
        ui->pTPptv_12->setChecked( true );
    else ui->pTPptv_12->setChecked( false );
    if( flgs[23] != '-' )    // TPptv[13]
        ui->pTPptv_13->setChecked( true );
    else ui->pTPptv_13->setChecked( false );
    if( flgs[24] != '-' )    // TPptv[14]
        ui->pTPptv_14->setChecked( true );
    else ui->pTPptv_14->setChecked( false );

    if( flgs[4] != '-' )    // MSpmv[4]
        ui->pMSpmv4->setChecked( true );
    else ui->pMSpmv4->setChecked( false );
    if( flgs[6] != '-' )    // MSpmv[6]
        ui->pMSpmv6->setChecked( true );
    else ui->pMSpmv6->setChecked( false );
    
    // Page 2   - 30-33  TPun
    
    ii = ui->pPunE->findText(QChar(flgs[30]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunE->setCurrentIndex(ii);

    ii = ui->pPunV->findText(QChar(flgs[31]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunV->setCurrentIndex(ii);

    ii = ui->pPunP->findText(QChar(flgs[32]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunP->setCurrentIndex(ii);
    ii = ui->pPunT->findText(QChar(flgs[33]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunT->setCurrentIndex(ii);

}

ProjectWizard::~ProjectWizard()
{
    delete ui;
}

int ProjectWizard::get_Settings()
{
    return ui->pTaskType->currentIndex();
}

void ProjectWizard::getFlags( char flgs[38] )
{
    // Page 1
    // flgs 0-9 MSpmv, 10-29 TPptv, 30-33  TPun, 34-37 TPsv
    if( ui->pTPptv_un->isChecked() ) // for all uncertains TPpvt[1]&TPpvt[9]
        flgs[11] = flgs[19] = '+';
    else flgs[11] = flgs[19]= '-';

    if( ui->pTPptv_2->isChecked() ) // TPpvt[2] & TPpvt[3]
    {	  flgs[12] = '+';
        if( ui->pTPptv_un->isChecked() ) // for all uncertains
            flgs[13]  = '+';
    }
    else flgs[12] = flgs[13]= '-';

    if( ui->pTPptv_4->isChecked() ) // TPpvt[4] & TPpvt[5]
    {	  flgs[14] = '+';
        if( ui->pTPptv_un->isChecked() ) // for all uncertains
            flgs[15]  = '+';
    }
    else flgs[14] = flgs[15]= '-';

    if( ui->pTPptv_6->isChecked() ) // TPpvt[6] & TPpvt[7]
    {	  flgs[16] = '+';
        if( ui->pTPptv_un->isChecked() ) // for all uncertains
            flgs[17]  = '+';
    }
    else flgs[16] = flgs[17]= '-';

    if( ui->pTPptv_10->isChecked() ) // TPpvt[10]
        flgs[20]  = '+';
    else flgs[20] = '-';
    if( ui->pTPptv_11->isChecked() ) // TPpvt[11]
        flgs[21]  = '+';
    else flgs[21] = '-';
    if( ui->pTPptv_12->isChecked() ) // TPpvt[12]
        flgs[22]  = '+';
    else flgs[22] = '-';
    if( ui->pTPptv_13->isChecked() ) // TPpvt[13]
        flgs[23]  = '+';
    else flgs[23] = '-';
    if( ui->pTPptv_14->isChecked() ) // TPpvt[14]
        flgs[24]  = '+';
    else flgs[24] = '-';

    if( ui->pMSpmv4->isChecked() ) // MSpmv[4]
        flgs[4]  = '+';
    else flgs[4] = '-';
    if( ui->pMSpmv6->isChecked() ) // MSpmv[6]
        flgs[6]  = '+';
    else flgs[6] = '-';
    // Page 2 not used
}

void ProjectWizard::help()
{
    pVisorImp->OpenHelp( GM_PROJECT_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

//--------------------- End of ProjectWizard.cpp ---------------------------
