//-------------------------------------------------------------------
// $Id: GEM2MTWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of UnSpaceWizard class
//
// Copyright (C) 2005-2008  S.Dmytriyeva
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

#include "ui_UnSpaceWizard4.h"
#include "UnSpaceWizard.h"
#include "GemsMainWindow.h"
#include "service.h"

void UnSpaceWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void UnSpaceWizard::CmNext()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void UnSpaceWizard::resetNextButton()
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

void UnSpaceWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}


UnSpaceWizard::UnSpaceWizard( const char* pkey, char flgs[38],
int size[10], QWidget* parent):
    QDialog( parent ),
    ui(new Ui::UnSpaceWizardData)
{
    //setFinishEnabled( WizardPage_3, true);
    ui->setupUi(this);
    string str1= "GEM-Selektor UnSpace Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );

    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();
    
    //Page1
    int ii = ui->p_Mode->findText(QChar(flgs[27]), Qt::MatchStartsWith|Qt::MatchCaseSensitive); //Pa_OF
    if( ii >= 0  )
        ui->p_Mode->setCurrentIndex(ii);

    ii = ui->p_St->findText(QChar(flgs[28]), Qt::MatchStartsWith|Qt::MatchCaseSensitive); //Pa_Crit
    if( ii >= 0  )
        ui->p_St->setCurrentIndex(ii);

    ui->pnQ->setValue(size[0]);     // Q
    ui->pnM->setValue(size[1]);     // quan_lev
    ui->pnP->setValue(size[2]);     // nGB
    ui->pLb->setValue(size[3]);     // nGN
    ui->pLb_2->setValue(size[4]);   // nGR

    // Page 2
    ii = ui->p_PsUnInt->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_PsUnInt->setCurrentIndex(ii);

    ii = ui->p_PsUnInB->findText(QChar(flgs[7]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_PsUnInB->setCurrentIndex(ii);

    ii = ui->p_PsUnFltI->findText(QChar(flgs[8]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_PsUnFltI->setCurrentIndex(ii);

    ii = ui->p_PsUnFltD->findText(QChar(flgs[9]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_PsUnFltD->setCurrentIndex(ii);

    if( flgs[10] != '-' )
        ui->c_PsGen1->setChecked( true );
    else ui->c_PsGen1->setChecked( false );
    if( flgs[11] != '-' )
        ui->c_PsGen2->setChecked( true );
    else ui->c_PsGen2->setChecked( false );
    if( flgs[12] != '-' )
        ui->c_PsGen3->setChecked( true );
    else ui->c_PsGen3->setChecked( false );
    if( flgs[13] != '-' )
        ui->c_PsGen4->setChecked( true );
    else ui->c_PsGen4->setChecked( false );
    if( flgs[14] != '-' )
        ui->c_PsGen5->setChecked( true );
    else ui->c_PsGen5->setChecked( false );
    if( flgs[15] != '-' )
        ui->c_PsGen6->setChecked( true );
    else ui->c_PsGen6->setChecked( false );
    if( flgs[16] != '-' )
        ui->c_PsGen7->setChecked( true );
    else ui->c_PsGen7->setChecked( false );

    if( flgs[20] != '-' )
        ui->c_Pa_f1->setChecked( true );
    else ui->c_Pa_f1->setChecked( false );
    if( flgs[21] != '-' )
        ui->c_Pa_f2->setChecked( true );
    else ui->c_Pa_f2->setChecked( false );
    if( flgs[22] != '-' )
        ui->c_Pa_f3->setChecked( true );
    else ui->c_Pa_f3->setChecked( false );
    if( flgs[23] != '-' )
        ui->c_Pa_f4->setChecked( true );
    else ui->c_Pa_f4->setChecked( false );
    if( flgs[24] != '-' )
        ui->c_Pa_f5->setChecked( true );
    else ui->c_Pa_f5->setChecked( false );
    if( flgs[25] != '-' )
        ui->c_Pa_f6->setChecked( true );
    else ui->c_Pa_f6->setChecked( false );

    // Page 3
    ui->p_Pa_Adapt->setValue((flgs[26]-'0'));

    if( flgs[18] != '-' )
        ui->c_PsEqn->setChecked( true );
    else ui->c_PsEqn->setChecked( false );
    if( flgs[19] != '-' )
        ui->c_PsGraph->setChecked( true );
    else ui->c_PsGraph->setChecked( false );
    if( flgs[29] != '-' )
        ui->c_Pa_Zcp->setChecked( true );
    else ui->c_Pa_Zcp->setChecked( false );
    if( flgs[36] != '-' )
        ui->c_PvSi->setChecked( true );
    else ui->c_PvSi->setChecked( false );
    if( flgs[17] != '-' )
        ui->c_PsSy->setChecked( true );
    else ui->c_PsSy->setChecked( false );

    if( flgs[32] != '-' )
        ui->c_PvGs->setChecked( true );
    else ui->c_PvGs->setChecked( false );
    if( flgs[34] != '-' )
        ui->c_PvVs->setChecked( true );
    else ui->c_PvVs->setChecked( false );
    if( flgs[35] != '-' )
        ui->c_PvPgam->setChecked( true );
    else ui->c_PvPgam->setChecked( false );
    if( flgs[30] != '-' )
        ui->c_PvPOM->setChecked( true );
    else ui->c_PvPOM->setChecked( false );
    if( flgs[33] != '-' )
        ui->c_PvSs->setChecked( true );
    else ui->c_PvSs->setChecked( false );
    if( flgs[31] != '-' )
        ui->c_PvPOR->setChecked( true );
    else ui->c_PvPOR->setChecked( false );

    // Page 4
    ui->p_Nsd->setValue(size[5]);  // Nsd
    ui->pGraph->setValue(size[6]); // dimXY[1] must be 3
    ui->pELine->setValue(size[7]); // dimEF[0] any
    ui->pECol->setValue(size[8]);  // dimEF[1] must be 5

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

}


UnSpaceWizard::~UnSpaceWizard()
{}

void   UnSpaceWizard::getSizes( int size[10] )
{
    size[0] = ui->pnQ->value();     // Q
    size[1] = ui->pnM->value();     // quan_lev
    size[2] = ui->pnP->value();     // nGB
    size[3] = ui->pLb->value();     // nGN
    size[4] = ui->pLb_2->value();   // nGR
    size[5] = ui->p_Nsd->value();  // Nsd
    size[6] = ui->pGraph->value(); // dimXY[1] must be 3
    size[7] = ui->pELine->value(); // dimEF[0] any
    size[8] = ui->pECol->value();  // dimEF[1] must be 5
}


void   UnSpaceWizard::getFlags( char flgs[38] )
{
    QString str;

    //Page1
    str = ui->p_Mode->currentText();
    flgs[27] = str[0].toLatin1();
    str = ui->p_St->currentText();
    flgs[28] = str[0].toLatin1();

    // Page 2
    str = ui->p_PsUnInt->currentText();
    flgs[6] = str[0].toLatin1();
    str = ui->p_PsUnInB->currentText();
    flgs[7] = str[0].toLatin1();
    str = ui->p_PsUnFltI->currentText();
    flgs[8] = str[0].toLatin1();
    str = ui->p_PsUnFltD->currentText();
    flgs[9] = str[0].toLatin1();

    if( ui->c_PsGen1->isChecked() )
        flgs[10] = '+';
    else flgs[10] = '-';
    if( ui->c_PsGen2->isChecked() )
        flgs[11] = '+';
    else flgs[11] = '-';
    if( ui->c_PsGen3->isChecked() )
        flgs[12] = '+';
    else flgs[12] = '-';
    if( ui->c_PsGen4->isChecked() )
        flgs[13] = '+';
    else flgs[13] = '-';
    if( ui->c_PsGen5->isChecked() )
        flgs[14] = '+';
    else flgs[14] = '-';
    if( ui->c_PsGen6->isChecked() )
        flgs[15] = '+';
    else flgs[15] = '-';
    if( ui->c_PsGen7->isChecked() )
        flgs[16] = '+';
    else flgs[16] = '-';

    if( ui->c_Pa_f1->isChecked() )
        flgs[20] = '+';
    else flgs[20] = '-';
    if( ui->c_Pa_f2->isChecked() )
        flgs[21] = '+';
    else flgs[21] = '-';
    if( ui->c_Pa_f3->isChecked() )
        flgs[22] = '+';
    else flgs[22] = '-';
    if( ui->c_Pa_f4->isChecked() )
        flgs[23] = '+';
    else flgs[23] = '-';
    if( ui->c_Pa_f5->isChecked() )
        flgs[24] = '+';
    else flgs[24] = '-';
    if( ui->c_Pa_f6->isChecked() )
        flgs[25] = '+';
    else flgs[25] = '-';

    // Page 3
    flgs[26] = static_cast<char>(ui->p_Pa_Adapt->value())+'0';
    // ui->p_Pa_Adapt->setValue((int)(flgs[26]-'0'));

    if( ui->c_PsEqn->isChecked() )
        flgs[18] = '+';
    else flgs[18] = '-';
    if( ui->c_PsGraph->isChecked() )
        flgs[19] = '+';
    else flgs[19] = '-';
    if( ui->c_Pa_Zcp->isChecked() )
        flgs[29] = '+';
    else flgs[29] = '-';
    if( ui->c_PvSi->isChecked() )
        flgs[36] = '+';
    else flgs[36] = '-';
    if( ui->c_PsSy->isChecked() )
        flgs[17] = '+';
    else flgs[17] = '-';

    if( ui->c_PvGs->isChecked() )
        flgs[32] = '+';
    else flgs[32] = '-';
    if( ui->c_PvVs->isChecked() )
        flgs[34] = '+';
    else flgs[34] = '-';
    if( ui->c_PvPgam->isChecked() )
        flgs[35] = '+';
    else flgs[35] = '-';
    if( ui->c_PvPOM->isChecked() )
        flgs[30] = '+';
    else flgs[20] = '-';
    if( ui->c_PvSs->isChecked() )
        flgs[33] = '+';
    else flgs[33] = '-';
    if( ui->c_PvPOR->isChecked() )
        flgs[31] = '+';
    else flgs[31] = '-';

    // Page 4
    str = ui->pPunE->currentText();
    flgs[0] = str[0].toLatin1();
    str = ui->pPunV->currentText();
    flgs[1] = str[0].toLatin1();
    str = ui->pPunP->currentText();
    flgs[2] = str[0].toLatin1();
    str = ui->pPunT->currentText();
    flgs[3] = str[0].toLatin1();

    // flags 0,1, 37 not used or internal
    flgs[4] = '0';
    flgs[5] = '0';
}

void UnSpaceWizard::help()
{
    pVisorImp->OpenHelp( GM_UNSPACE_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

//--------------------- End of UnSpaceWizard.cpp ---------------------------
