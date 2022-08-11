//-------------------------------------------------------------------
// $Id: RTparmWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of RTparmWizard class
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

#include "ui_RTparmWizard4.h"
#include "RTparmWizard.h"
#include "GemsMainWindow.h"
#include "v_mod.h"
#include "service.h"
#include "GEMS3K/num_methods.h"


void RTparmWizard::CmBack()
{
    int ndx = ui->stackedWidget->currentIndex();

    if( ndx == 2 )
    {
        if( ui->pMode->currentIndex() != 0 )
            ndx--;
    }

    ui->stackedWidget->setCurrentIndex ( ndx-1 );
    resetNextButton();
    resetBackButton();
}

void RTparmWizard::CmNext()
{
    int ndx = ui->stackedWidget->currentIndex();

    if( ndx == 0 )
    {
        if( ui->pMode->currentIndex() != 0 )
            ndx++;
        else
            initPTTable();
    }

    auto nLines = pageScript->getScriptLinesNum();
    if( ndx == 2 && nLines > 0)
        ui->pdimY->setValue( nLines );

    ui->stackedWidget->setCurrentIndex ( ndx+1 );
    resetNextButton();
    resetBackButton();
}

void 	RTparmWizard::resetNextButton()
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

void 	RTparmWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}

RTparmWizard::RTparmWizard( const char* pkey, char flgs[10], int size[7],
double val[6],  const char *acalcScript, const char* aXname, const char* aYname,
QWidget* parent):
    QDialog( parent ),
    ui(new Ui::RTparmWizardData),
    calcScript(acalcScript), pageScript(nullptr)
{
    int ii;

    if( flgs[0] == SRC_DCOMP )
        isDC = true;
    else
        isDC = false;


    ui->setupUi(this);
    string str1= "GEM-Selektor RTparm Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );
    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();


    // page1
    ii = min( size[2],ui->pMode->count()-1 );
    ui->pMode->setCurrentIndex(ii);
    ui->pTfrom->setValue(val[0]);
    ui->pTuntil->setValue(val[1]);
    ui->pTstep->setValue(val[2]);
    ui->pPfrom->setValue(val[3]);
    ui->pPuntil->setValue(val[4]);
    ui->pPstep->setValue(val[5]);

    ii = ui->pPtun->findText(QChar(flgs[7]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPtun->setCurrentIndex(ii);
    ii = ui->pPun->findText(QChar(flgs[8]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPun->setCurrentIndex(ii);
    if( flgs[6] == 'T' )
    {
        ui->butT->setChecked(true);
        ui->butP->setChecked(false);
    }
    else
    {
        ui->butT->setChecked(false);
        ui->butP->setChecked(true);
    }

    ui->pNP->setValue(size[0]);
    ui->pNT->setValue(size[1]);

    //Page 2 equations
    resetPageList(aXname, aYname);


    // Page 3
    ui->pdimY->setValue(size[6]);
    ui->pECol->setValue(size[5]);
    ui->pELine->setValue(size[4]);

    ii = ui->pWhat->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pWhat->setCurrentIndex(ii);

    ii = ui->pPunE->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunE->setCurrentIndex(ii);

    ii = ui->pPunV->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunV->setCurrentIndex(ii);

    ii = ui->pPunP->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunP->setCurrentIndex(ii);
    ii = ui->pPunT->findText(QChar(flgs[4]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunT->setCurrentIndex(ii);

    //Page 4
    ui->spinBox18->setValue(size[3]);

    // commands
    ui->stackedWidget->setCurrentIndex (0);
    PairwiseChecked();

    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( ui->butT, SIGNAL(clicked(bool)),  ui->butP, SLOT(toggle()));
    QObject::connect( ui->butP, SIGNAL(clicked(bool)),  ui->butT, SLOT(toggle()));
    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));

    QObject::connect( ui->pTfrom, SIGNAL(valueChanged(double)), this, SLOT(TChange()));
    QObject::connect( ui->pTuntil, SIGNAL(valueChanged(double)), this, SLOT(TChange()));
    QObject::connect( ui->pTstep, SIGNAL(valueChanged(double)), this, SLOT(TChange()));
    QObject::connect( ui->pPfrom, SIGNAL(valueChanged(double)), this, SLOT(PChange()));
    QObject::connect( ui->pPuntil, SIGNAL(valueChanged(double)), this, SLOT(PChange()));
    QObject::connect( ui->pPstep, SIGNAL(valueChanged(double)), this, SLOT(PChange()));
    QObject::connect( ui->pMode, SIGNAL(currentIndexChanged(int) ), this, SLOT(PairwiseChecked()));

}


RTparmWizard::~RTparmWizard()
{
    delete ui;
}

void   RTparmWizard::getSizes( int size[7] )
{
    size[0]= ui->pNP->value();
    size[1]= ui->pNT->value();
    size[6]= ui->pdimY->value();
    size[5]= ui->pECol->value();
    size[4]= ui->pELine->value();
    size[3]= ui->spinBox18->value();

    size[2] = ui->pMode->currentIndex();
}

void RTparmWizard::getFlags( char flgs[6], string& xName )
{
    // Page 1 - not return
    QString str = ui->pPtun->currentText();
    flgs[7] = str[0].toLatin1();

    str = ui->pPun->currentText();
    flgs[8] = str[0].toLatin1();

    if(ui->butP->isChecked())
    {  flgs[6] = 'P';
        xName = ui->pPun->currentText().toStdString();
    }
    else
    {  flgs[6] = 'T';
        xName = ui->pPtun->currentText().toStdString();
    }

    // Page 2
    if( ui->pECol->value() > 0 && ui->pELine->value() > 0 )
        flgs[5] = '+';
    else flgs[5] = '-';
}

void   RTparmWizard::getFloat( double val[6] )
{
    val[0] = ui->pTfrom->value();
    val[1] = ui->pTuntil->value();
    val[2] = ui->pTstep->value();
    val[3] = ui->pPfrom->value();
    val[4] = ui->pPuntil->value();
    val[5] = ui->pPstep->value();
}


void RTparmWizard::help()
{
    pVisorImp->OpenHelp( GM_RTPARM_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}


void RTparmWizard::TChange()
{
    double Tai[4];
    Tai[0] = ui->pTfrom->value();
    Tai[1] = ui->pTuntil->value();
    Tai[2] = ui->pTstep->value();
    auto nT = getNpoints( Tai );
    ui->pNT->setValue(nT);
}

void RTparmWizard::PChange()
{
    double Pai[4];
    Pai[0] = ui->pPfrom->value();
    Pai[1] = ui->pPuntil->value();
    Pai[2] = ui->pPstep->value();
    auto nP = getNpoints( Pai );
    ui->pNP->setValue(nP);
}


void RTparmWizard::PairwiseChecked()
{
    bool modeNoPair = ( ui->pMode->currentIndex() != 0);

    ui->pTfrom->setEnabled ( modeNoPair );
    ui->pTuntil->setEnabled ( modeNoPair );
    ui->pTstep->setEnabled ( modeNoPair );
    ui->pPfrom->setEnabled ( modeNoPair );
    ui->pPuntil->setEnabled ( modeNoPair );
    ui->pPstep->setEnabled ( modeNoPair );

    ui->pNP->setEnabled ( !modeNoPair );
    ui->pNT->setEnabled ( !modeNoPair );
}


void RTparmWizard::definePTArray()
{
    int nPT;
    //double *arP, *arT;

    nPT = max( ui->pNP->value(), ui->pNT->value());
    /*arP = (double *)*/ aObj[ o_rpxp]->Alloc( nPT, 1, D_);
    /*arT = (double *)*/ aObj[ o_rpxt]->Alloc( nPT, 1, D_);
}

void RTparmWizard::initPTTable()
{
    // init table
    TObjectModel* model;
    TObjectTable* fieldTable;
    QList<FieldInfo>	aFlds;

    definePTArray();

    aFlds.clear();
    aFlds.append(FieldInfo( o_rpxp, ftFloat, 15, false, First, eYes, stIO, 20, 1));
    aFlds.append(FieldInfo( o_rpxt, ftFloat, 15, false, Tied, eYes, stIO, 20, 1));

    model = new TObjectModel( aFlds, this );
    fieldTable =  new TObjectTable( aFlds, this );
    TObjectDelegate *deleg = new TObjectDelegate( fieldTable, this);
    fieldTable->setItemDelegate(deleg);
    fieldTable->setModel(model);

    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    fieldTable->setSizePolicy(sizePolicy1);

    int rowSize =0, colSize=0;
    fieldTable->getObjectSize(rowSize, colSize);
    fieldTable->setMaximumSize(QSize(colSize, 16777215));

    ui->gridLayout_4->addWidget(fieldTable, 1, 0, 1, 2);

}



//==============================================================================

static equatSetupData eq( "", "yF", "jTP", "" );
//static equatSetupData eqT( "xT", "yF", "jTP", "twTC" );
//static equatSetupData eqP( "xP", "yF", "jTP", "twP" );

// work with lists
void RTparmWizard::resetPageList(const char* aXname, const char* aYname)
{

    std::vector<pagesSetupData> scalarsList;
    std::vector<pagesSetupData> pgData;

    GetListsnRT( -1, pgData,  scalarsList );
    GetListsnRT( RT_RTPARM, pgData,  scalarsList );
    /*
   if( isDC )
   {
       GetListsnRT( RT_DCOMP,  pgData,  scalarsList );
   }
   else
   {
       GetListsnRT( RT_REACDC, pgData,  scalarsList );
   }
*/
    pageScript = new EquatSetup( ui->page_3, eq,
                                 RT_PROCES, pgData, scalarsList, calcScript.c_str(), aXname, aYname  );
    ui->verticalLayout_5->addWidget(pageScript);

}

//--------------------- End of RTparmWizard.cpp ---------------------------
