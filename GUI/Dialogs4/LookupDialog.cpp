//-------------------------------------------------------------------
// $Id: LookupDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of LookupDialog class
//
// Copyright (C) 2009  S.Dmytriyeva
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

#include <QDoubleSpinBox>

#include "ui_LookupDialog4.h"
#include "LookupDialog.h"
#include "GemsMainWindow.h"
#include "service.h"
#include "v_mod.h"
#include "GEMS3K/num_methods.h"
#include "GEMS3K/gems3k_impex.h"

#define Inherited LookupDialogData

LookupDialog::LookupDialog(QWidget* parent):
    QDialog( parent),
    ui(new Ui::LookupDialogData)
{
    ui->setupUi(this);
    switch(GEMS3KGenerator::default_type_f)
    {
    case GEMS3KGenerator::f_binary:
        ui->rbBinary->setChecked( true ); break;
    case GEMS3KGenerator::f_json:
        ui->rbJson->setChecked( true ); break;
    case GEMS3KGenerator::f_thermofun:
        ui->rbFunJson->setChecked( true ); break;
    case GEMS3KGenerator::f_kv_thermofun:
        ui->rbFunKeyValue->setChecked( true ); break;
    default:
    case GEMS3KGenerator::f_key_value:
        ui->rbKeyVal->setChecked( true ); break;
    }

    PTable = nullptr;
    TTable = nullptr;
    ui->buttonBack->hide();
    QObject::connect( ui->buttonOK, SIGNAL( clicked() ), this, SLOT( CmNext() ) );
    QObject::connect( ui->buttonBack, SIGNAL( clicked() ), this, SLOT( CmBack() ) );
    QObject::connect( ui->buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    QObject::connect( ui->buttonHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    QObject::connect( ui->checkArrays, SIGNAL( toggled(bool) ), this, SLOT( arrayChecked(bool) ) );
}

LookupDialog::~LookupDialog()
{
    delete ui;
}

void LookupDialog::CmNext()
{
    if( ui->stackW->currentIndex() == 1 )
    {
        accept();
    } else if(!ui->checkArrays->isChecked())
    {
        // internal setup arrays
        setupPTArrays();
        accept();
    }
    else
    {
        definePArray();
        defineTArray();
        initPTable();
        initTTable();
        ui->buttonBack->show();
        ui->stackW->setCurrentIndex(1);
    }
}

void LookupDialog::CmBack()
{
    if( ui->stackW->currentIndex() == 1 )
    {
        ui->buttonBack->hide();
        ui->stackW->setCurrentIndex(0);
    }
}


void LookupDialog::arrayChecked( bool check )
{
    ui->pPstep->setEnabled ( !check );
    ui->pTstep->setEnabled ( !check );
    ui->pPPoints->setEnabled ( check );
    ui->pTPoints->setEnabled ( check );
    ui->chInterp->setEnabled ( check );
}

void LookupDialog::definePArray()
{
    int nPs, nPi, nP, ii;
    double  cP;
    double Pai[4];
    double *arP;

    nPs = ui->pPPoints->value();
    getPdata( Pai );
    if( nPs == 1 )
    {
        //      getPdata( Pai );
        nPs = nPi = getNpoints( Pai );
        //      ui->pPPoints->textFromValue( nPi );
        ui->pPPoints->setValue( nPi );
    }
    nP = nPs;
    //init P array
    double stepP = getStep( Pai, nP );
    cP = Pai[0];
    arP = static_cast<double*>(aObj[ o_w_pval]->Alloc( nP, 1, D_));
    if( PTable == nullptr)
        for( ii=0; ii<nP; ii++ )
        {
            arP[ii] = cP;
            cP += stepP;
        }
}

void LookupDialog::initPTable()
{
    // init table
    TObjectModel* model;
    //TObjectTable* PTable;
    QList<FieldInfo>	aFlds;

    if( PTable )
    {   ui->gridLayout_4->removeWidget(PTable);
        delete PTable; //????
    }

    aFlds.clear();
    aFlds.append(FieldInfo( o_w_pval, ftFloat, 15, false, First, eYes, stIO, 20, 1));

    if(ui->chInterp->isChecked())
        aFlds.append(FieldInfo( o_w_tval, ftFloat, 15, false, Tied, eYes, stIO, 20, 1));
    model = new TObjectModel( aFlds, this );

    PTable =  new TObjectTable( aFlds, this );
    TObjectDelegate *deleg = new TObjectDelegate( PTable, this);
    PTable->setItemDelegate(deleg);
    PTable->setModel(model);

    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    PTable->setSizePolicy(sizePolicy1);

    int rowSize =0, colSize=0;
    PTable->getObjectSize(rowSize, colSize);
    PTable->setMaximumSize(QSize(colSize, 16777215));

    if(!ui->chInterp->isChecked())
        ui->gridLayout_4->addWidget(PTable, 1, 0, 1, 1);
    else
        ui->gridLayout_4->addWidget(PTable, 1, 0, 1, 2);

}

void LookupDialog::defineTArray()
{
    int nT, nTs, nTi, ii;
    double cT;
    double Tai[4];
    double  *arT;

    getTdata( Tai );
    if(ui->chInterp->isChecked())
    {
        nT = ui->pPPoints->value();
        //          getTdata( Tai );
    }
    else {
        nTs = ui->pTPoints->value();
        if( nTs == 1 )
        {
            //          getTdata( Tai );
            nTs = nTi = getNpoints( Tai );
            //          ui->pTPoints->textFromValue( nTi );
            ui->pTPoints->setValue( nTi );
        }
        nT = nTs;
    }
    //init T array
    double stepT = getStep( Tai, nT );
    cT = Tai[0];
    arT = static_cast<double*>(aObj[ o_w_tval]->Alloc( nT, 1, D_));
    if( PTable == nullptr)
        for( ii=0; ii<nT; ii++ )
        {
            arT[ii] = cT;
            cT += stepT;
        }

}

void LookupDialog::initTTable()
{
    // init table
    TObjectModel* model;
    //TObjectTable* fieldTable;
    QList<FieldInfo>	aFlds;

    if( TTable )
    {   ui->gridLayout_4->removeWidget(TTable);
        delete TTable; //????
        TTable = nullptr;
    }

    if(ui->chInterp->isChecked())  // only one table
        return;

    aFlds.clear();
    aFlds.append(FieldInfo( o_w_tval, ftFloat, 15, false, First, eYes, stIO, 20, 1));
    model = new TObjectModel( aFlds, this );

    TTable =  new TObjectTable( aFlds, this );
    TObjectDelegate *deleg = new TObjectDelegate( TTable, this);
    TTable->setItemDelegate(deleg);
    TTable->setModel(model);

    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    TTable->setSizePolicy(sizePolicy1);

    int rowSize =0, colSize=0;
    TTable->getObjectSize(rowSize, colSize);
    TTable->setMaximumSize(QSize(colSize, 16777215));

    ui->gridLayout_4->addWidget(TTable, 1, 1, 1, 1);

}

void LookupDialog::objectChanged()
{

}

void LookupDialog::setupPTArrays()
{
    int nT, nP, ii;
    double cT, cP;
    double Pai[4], Tai[4];
    double  *arT, *arP;

    //init P array
    getPdata( Pai );
    nP = getNpoints( Pai );
    arP = static_cast<double*>( aObj[ o_w_pval]->Alloc( nP, 1, D_) );
    cP = Pai[START_];
    for( ii=0; ii<nP; ii++ )
    {
        arP[ii] = cP;
        cP+= Pai[2];
    }

    //init T array
    getTdata( Tai );
    nT = getNpoints( Tai );
    arT = static_cast<double*>(aObj[ o_w_tval]->Alloc( nT, 1, D_));
    cT = Tai[START_];
    for( ii=0; ii<nT; ii++ )
    {
        arT[ii] = cT;
        cT+= Tai[2];
    }

}

void   LookupDialog::setTdata( double Tai[4] )
{
    //pTfrom->textFromValue( Tai[0] );
    ui->pTfrom->setValue( Tai[0] );
    //pTuntil->textFromValue( Tai[1] );
    ui->pTuntil->setValue( Tai[1] );
}

void   LookupDialog::setPdata( double Pai[4] )
{
    //ui->pPfrom->textFromValue( Pai[0] );
    ui->pPfrom->setValue( Pai[0] );
    //ui->pPuntil->textFromValue( Pai[1] );
    ui->pPuntil->setValue( Pai[1] );
}

void   LookupDialog::getTdata( double Tai[4] )
{
    //   nT = pTN->value();
    Tai[0] = ui->pTfrom->value();
    Tai[1] = ui->pTuntil->value();
    if( Tai[1] < Tai[0] )
    {
        Tai[1] = Tai[0];
        Tai[0] = ui->pTuntil->value();
    }
    Tai[2] = ui->pTstep->value();
    Tai[3] = ui->pTtol->value();
}

void   LookupDialog::getPdata( double Pai[4] )
{
    //   nP = pPN->value();
    Pai[0] = ui->pPfrom->value();
    Pai[1] = ui->pPuntil->value();
    if( Pai[1] < Pai[0] )
    {
        Pai[1] = Pai[0];
        Pai[0] = ui->pPuntil->value();
    }
    Pai[2] = ui->pPstep->value();
    Pai[3] = ui->pPtol->value();
}

void   LookupDialog::getFlags( char flgs[6] )
{
    if( ui->chSelect->isChecked() )
        flgs[0] = '+';
    else flgs[0] = '-';

    if( ui->rbJson->isChecked() )
        flgs[1] = 'j';
    else if( ui->rbBinary->isChecked() )
        flgs[1] = 'b';
    else if( ui->rbFunJson->isChecked() )
        flgs[1] = 'f';
    else if( ui->rbFunKeyValue->isChecked() )
        flgs[1] = 'o';
    else
        flgs[1] = 't';

    if( ui->chAll->isChecked() )
        flgs[2] = '+';
    else flgs[2] = '-';

    if( ui->chComment->isChecked() )
        flgs[3] = '+';
    else flgs[3] = '-';

    if( ui->chInterp->isChecked() )
        flgs[4] = '+';
    else flgs[4] = '-';

    if( ui->chNoNodearray->isChecked() )
        flgs[5] = '+';
    else flgs[5] = '-';
}

void LookupDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_WRIPMF_HTML );
}


//--------------------- End of SettingsDialog.cpp ---------------------------

