//-------------------------------------------------------------------
// $Id: KeyProfile.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyProfile class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#include "ui_KeyProfile4.h"
#include "KeyProfile.h"
#include "GemsMainWindow.h"
#include "visor.h"
#include "service.h"

KeyProfile::KeyProfile( QWidget* win, uint irt, const char* caption):
    QDialog( win),
    ui(new Ui::KeyProfileData),
    iRt(irt), newKey(false)
{
    ui->setupUi(this);

    ui->pList->setFont( pVisorImp->getCellFont() );

    TCIntArray    temp;
    TCStringArray keyList;

    setWindowTitle( caption );
    if( pVisor->getElemPrMode() )
    {   ui->rbNewPrMode->setChecked( true );
        ui->rbOldPrMode->setChecked( false );
    }
    else
    {   ui->rbNewPrMode->setChecked( false );
        ui->rbOldPrMode->setChecked( true );
    }

    //    string s = "Select Project record";
    //    pLabel->setText(s.c_str());

    auto n = rt[irt]->GetKeyList( "*", keyList, temp);
    for( size_t ii=0; ii<n; ii++ )
        ui->pList->addItem(keyList[ii].c_str());
    ui->pList->setCurrentRow(0);
    ui->pList->item(0)->setSelected(true);

    // signals and slots connections
    QObject::connect( ui->pNewProfBtn, SIGNAL(clicked()), this, SLOT(CmNew()));
    QObject::connect( ui->pGO_OKButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect( ui->pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( ui->pHelpButton, SIGNAL(clicked()), this, SLOT(CmHelp()));
    QObject::connect( ui->pRe_runIA, SIGNAL( clicked() ), this, SLOT( CmReturnIA() ) );
    QObject::connect( ui->pRe_runSmart, SIGNAL( clicked() ), this, SLOT( CmReturnSmart() ) );
    QObject::connect( ui->pList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
    //     QObject::connect(ui->pList, SIGNAL(returnPressed(QListWidgetItem *)), this, SLOT(accept()));
}

KeyProfile::~KeyProfile()
{
    delete ui;
}

string KeyProfile::getKey() const
{
    pVisor->setElemPrMode(ui->rbNewPrMode->isChecked());
    if( newKey == true )
        return ALLKEY;
    int sel = ui->pList->currentRow();
    if( sel != -1 )
        return ui->pList->item(sel)->text().toStdString();

    return string();
}

void KeyProfile::CmReturnIA()
{
    if( ui->pRe_runIA->isChecked() )
        ui->pRe_runSmart->setChecked(false);
}

void KeyProfile::CmReturnSmart()
{
    if( ui->pRe_runSmart->isChecked() )
        ui->pRe_runIA->setChecked(false);
}

void KeyProfile::CmNew()
{
    newKey = true;
    accept();
}

void KeyProfile::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_MPROJ_HTML );
}

bool KeyProfile::getAqGasState() const
{
    if( newKey == true )
        return true;    // new record => selection aqueous and gas phases

    if( ui->pAqGas->isChecked() )
        return false;
    else return true; // new selection aqueous and gas phases
}

int KeyProfile::getMakeDump() const
{
    if( newKey == true )
        return 0;    // new record => no dump

    if( ui->pRe_runIA->isChecked() )
        return 1;  // Re-run and save all sysEq in auto IA mode

    if( ui->pRe_runSmart->isChecked() )
        return 2;  // Re-run and save all sysEq in smart
    
    return 0;
}

bool KeyProfile::getFilesState() const
{
    if( newKey == true )
        return false;    // new record => add all files

    if( ui->pFiles->isChecked() )
        return true;
    else return false; // add files from project directory

}

bool KeyProfile::getRemakeState() const
{
    if( ui->CheckBox4->isChecked() )
        return true;   // remake flags
    else return false;

}

string KeyProfile::getTemplateKey() const
{
    if( newKey == true && ui->pTemplate->isChecked() )
    {
        int sel = ui->pList->currentRow();
        if( sel != -1 )
            return ui->pList->item(sel)->text().toStdString();
        Error( "New Modelling Project", "No template record selected");
    }
    return string();
}

bool KeyProfile::getGEMSExport() const
{
    return ui->checkGEMS3k->isChecked();
}

bool KeyProfile::getGEMSExportMode() const
{
    return ui->checkBrief->isChecked();
}

// --------------------- End KeyProfile.cpp -------------------------

