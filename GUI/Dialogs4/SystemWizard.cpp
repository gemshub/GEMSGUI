//-------------------------------------------------------------------
// $Id: SystemWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SystemWizard class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
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

#include "ui_SystemWizard4.h"
#include "SystemWizard.h"
#include "GemsMainWindow.h"
#include "service.h"


void SystemWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void SystemWizard::CmNext()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	SystemWizard::resetNextButton()
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

void 	SystemWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}

SystemWizard::SystemWizard( const char* pkey, char flgs[40],
string name, string comment, string EQkey,  QWidget* parent):
    QDialog( parent ),
    ui(new Ui::SystemWizardData)
{
    //setFinishEnabled( WizardPage3, true);
    ui->setupUi(this);
    string str1= "GEM-Selektor System Setup:  ";
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
    ui->pName->setText(name.c_str());
    ui->pComment->setText(comment.c_str());

    if( flgs[15] != '-' || flgs[17] != '-')
        ui->pPLim->setChecked( true );
    else ui->pPLim->setChecked( false );

    if( flgs[0] != '-' )
        ui->pPE->setChecked( true );
    else ui->pPE->setChecked( false );

    if( flgs[1] != '-' )
        ui->pPV->setChecked( true );
    else ui->pPV->setChecked( false );

    if( flgs[9] != '-' )
        ui->pPBeq->setChecked( true );
    else ui->pPBeq->setChecked( false );

    if( flgs[24] != '-' )
        ui->pPAalp->setChecked( true );
    else ui->pPAalp->setChecked( false );

    if( flgs[25] != '-' )
        ui->pPSigm->setChecked( true );
    else ui->pPSigm->setChecked( false );

    if( flgs[26] != '-' )
        ui->pXr0h0->setChecked( true );
    else ui->pXr0h0->setChecked( false );

    if( flgs[27] != '-' )
        ui->pPXepsC->setChecked( true );
    else ui->pPXepsC->setChecked( false );

    if( flgs[28] != '-' )
        ui->pPNfsp->setChecked( true );
    else ui->pPNfsp->setChecked( false );

    if( flgs[34] != '-' )
        ui->pPParc->setChecked( true );
    else ui->pPParc->setChecked( false );

    if( flgs[35] != '-' )
        ui->pPDelB->setChecked( true );
    else ui->pPDelB->setChecked( false );

    if( flgs[36] != '-' )
        ui->pPXlam->setChecked( true );
    else ui->pPXlam->setChecked( false );


    // Page 2
    ui->pKey->setText(EQkey.c_str());
    if( flgs[13] != '-' )
        ui->pPbPh->setChecked( true );
    else ui->pPbPh->setChecked( false );

    /* if( flgs[18] == '+' )
            r1->setChecked( true );
    else if( flgs[18] == '-' )
            r2->setChecked( true );
         else
            r3->setChecked( true );
   */
}

SystemWizard::~SystemWizard()
{
    delete ui;
}

string SystemWizard::getName(  )
{
    return ui->pName->text().toStdString();
}

string SystemWizard::getComment(  )
{
    return ui->pComment->text().toStdString();
}

string SystemWizard::getEQkey(  )
{
    return ui->pKey->text().toStdString();
}


void SystemWizard::getFlags( char flgs[40] )
{
    // Page 2
    if( ui->pPbPh->isChecked() )
        flgs[13] = '+';
    else flgs[13] = '-';

    //if( r1->isChecked() )
    //  flgs[18] = '+';
    //else  if( r2->isChecked() )
    //      flgs[18] = '-';
    //  else
    flgs[18] = '*';

    // Page 1

    if( ui->pPLim->isChecked() )
        flgs[15] = flgs[17] = '+';
    else flgs[15] = flgs[17] = '-';

    if( ui->pPE->isChecked() )
        flgs[0] = '+';
    else flgs[0] = '-';

    if( ui->pPV->isChecked() )
        flgs[1] = '+';
    else flgs[1] = '-';

    if( ui->pPBeq->isChecked() )
        flgs[9] = '+';
    else flgs[9] = '-';

    if( ui->pPAalp->isChecked() )
        flgs[24] = '+';
    else flgs[24] = '-';

    if( ui->pPSigm->isChecked() )
        flgs[25] = '+';
    else flgs[25] = '-';

    if( ui->pXr0h0->isChecked() )
        flgs[26] = '+';
    else flgs[26] = '-';

    if( ui->pPXepsC->isChecked() )
        flgs[27] = '+';
    else flgs[27] = '-';

    if( ui->pPNfsp->isChecked() )
        flgs[28] = '+';
    else flgs[28] = '-';

    if( ui->pPParc->isChecked() )
        flgs[34] = '+';
    else flgs[34] = '-';

    if( ui->pPDelB->isChecked() )
        flgs[35] = '+';
    else flgs[35] = '-';

    if( ui->pPXlam->isChecked() )
        flgs[36] = '+';
    else flgs[36] = '-';

    if( ui->pPask->isChecked() )
        flgs[37] = '+';
    else flgs[37] = '-';
}

void SystemWizard::help()
{
    pVisorImp->OpenHelp( GEMS_SYSTEM_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

//--------------------- End of SystemWizard.cpp ---------------------------
