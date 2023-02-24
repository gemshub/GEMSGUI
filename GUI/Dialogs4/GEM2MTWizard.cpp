//-------------------------------------------------------------------
// $Id: GEM2MTWizard.cpp 496 2005-03-04 22:58:13Z sveta $
//
// Implementation of GEM2MTWizard class
//
// Copyright (C) 2005-2007  S.Dmytriyeva
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

#include "ui_GEM2MTWizard4.h"
#include "GEM2MTWizard.h"
#include "GemsMainWindow.h"
#include "GEMS3K/num_methods.h"
#include "GEMS3K/io_template.h"
#include "m_gem2mt.h"

extern io_formats::outField DataBR_fields[58];

void GEM2MTWizard::CmBack()
{
    bool chk =  ui->pselS->isChecked()|| ui->pselF->isChecked()|| ui->pselB->isChecked();  // F mode
    int ndx =  ui->stackedWidget->currentIndex();

    if( ndx == graphic_script)
    {
        auto nLines = pageScript->getScriptLinesNum();
        if( nLines > 0)
            ui->pnYS->setValue( nLines );
    }

    ndx--;

    if( ndx == T_P_lookup )
    {  if(!ui->checkArrays->isChecked()|| !ui->pTPInput->isEnabled())
            ndx--; // T P lists
    }

    if( ndx == vtk_format && ! ui->c_PsVTK->isChecked() )
        ndx--;
    if( ndx == graphic_script && ! ui->c_PvMSg->isChecked() )
        ndx--;
    if( ndx == auto_script && ! ui->c_PvMSt->isChecked() )
        ndx--;
    if( ndx == fluxes_transport && !chk )
        ndx--;

    ui->stackedWidget->setCurrentIndex ( ndx );
    resetNextButton();
    resetBackButton();
}

void GEM2MTWizard::CmNext()
{
    bool chk =  ui->pselS->isChecked() || ui->pselF->isChecked() || ui->pselB->isChecked();  // S, F or B mode
    int ndx =  ui->stackedWidget->currentIndex();

    if( ndx == mode_RMT )
    {
        for( int ii=7; ii<14; ii++)
        {
            if( chk )
                GroupBox1->button(ii)->setChecked(false);
            GroupBox1->button(ii)->setEnabled( !chk );
        }
        ui->chFlux->setEnabled( chk );
        ui->chFlux->setChecked( chk );
        setTScript( chk );
        if( chk )
        {
            ui->c_PvMSt->setChecked( true );
            ui->c_PvMSg->setChecked( true );
            ui->chRMass->setChecked( true );
        }
    }

    if( ndx == fluxes_transport )
    {
        if(  ui->pselS->isChecked() ||  ui->pselF->isChecked() ) // ||  ui->pselB->isChecked() )
            ui->pnFD->setValue(  ui->pnC->value() +  ui->pnSFD->value() );
    }

    if( ndx == graphic_script )
    {
        auto nLines = pageScript->getScriptLinesNum();
        if( nLines > 0)
            ui->pnYS->setValue( nLines );
    }

    ndx++;

    if( ndx == fluxes_transport )
    {
        if( !chk )
            ndx++;
        else
        {
            if(  ui->pselS->isChecked() ||  ui->pselF->isChecked() )
            {
                ui->pnPG->setValue( 1 );
                ui->pnFD->setValue(  ui->pnC->value() +  ui->pnSFD->value() );
                //         ui->pnSFD->setValue( 0 );
            }
            ui->pnPG->setEnabled( !(  ui->pselS->isChecked() ||  ui->pselF->isChecked() ) );
            ui->pnFD->setEnabled( !(  ui->pselS->isChecked() ||  ui->pselF->isChecked() ) );
            // ui->pnSFD->setEnabled( !(  ui->pselS->isChecked() || ui->pselF->isChecked() ) && ui->pnSFD->value() > 0 );
        }
    }

    if( ndx == auto_script && ! ui->c_PvMSt->isChecked() )
        ndx++;

    if( ndx == graphic_script && ! ui->c_PvMSg->isChecked() )
        ndx++;

    if( ndx == vtk_format && ! ui->c_PsVTK->isChecked() )
        ndx++;

    if( ndx == gems3k_exchange )
    {
        if(  ui->pselS->isChecked()||  ui->pselF->isChecked()  )
        {
            ui->checkArrays->setChecked(true);
            ui->chInterp->setChecked(true);
            ui->pPPoints->setValue(  ui->pnC->value() );
            ui->pTPoints->setValue(  ui->pnC->value() );
            ui->pTPInput->setEnabled(false);
            ui->pTPlookup->setEnabled(false);
            //ndx++;
            //ndx++; // T P lists
        }
        else
        {
            ui->pTPInput->setEnabled(true);
            ui->pTPlookup->setEnabled(true);
        }
    }

    if(   ndx == T_P_lookup )
    {
        if( !ui->checkArrays->isChecked() || !ui->pTPInput->isEnabled() )
        {
            // internal setup arrays
            // setupPTArrays();
            ndx++;
        }
        else
        {
            definePArray();
            defineTArray();
            initPTable();
            initTTable();
            showPTTable();
        }
    }

    ui->stackedWidget->setCurrentIndex ( ndx );
    resetNextButton();
    resetBackButton();
}

void 	GEM2MTWizard::resetNextButton()
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

void 	GEM2MTWizard::resetBackButton()
{
    ui->pBack->setEnabled(  ui->stackedWidget->currentIndex() > 0 );
}


GEM2MTWizard::GEM2MTWizard( const char* pkey, char flgs[32],
int size[20], double Tai[4], double Pai[4],double Tau[3],
const char *acalcScript, const char *aoutScript,
const char* aXname, const char* aYname,
TCIntArray vtk1, TCIntArray vtk2, QWidget* parent):
    QDialog( parent ),
    ui(new Ui::GEM2MTWizardData),
    calcScript(acalcScript), outScript(aoutScript), pageScript(nullptr)
{

    PTable = nullptr;
    TTable = nullptr;
    //    setFinishEnabled( WizardPage2, true);
    ui->setupUi(this);
    string str1= "GEM-Selektor GEM2MT Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );

    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

    //Page1
    if( flgs[18] == 'T' )
        ui->pselT->setChecked( true );
    else  if( flgs[18] == 'B' )
        ui->pselB->setChecked( true );
    else  if( flgs[18] == 'A' )
        ui->pselA->setChecked( true );
    else if( flgs[18] == 'C' )
        ui->pselC->setChecked( true );
    else if( flgs[18] == 'D' )
        ui->pselD->setChecked( true );
    else if( flgs[18] == 'W' )
        ui->pselW->setChecked( true );
    else if( flgs[18] == 'F' )
        ui->pselF->setChecked( true );
    else
        ui->pselS->setChecked( true );
    //Page2

    ui->pnC->setValue(size[0]);
    ui->pnIV->setValue(size[1]);
    ui->pntM->setValue(size[2]);
    ui->pLb->setValue(size[9]);
    ui->pTauFrom->setValue(Tau[0]);
    ui->pTaustep->setValue(Tau[2]);

    if( flgs[4] != '-' )
        ui->c_PvICi->setChecked( true );
    else  ui->c_PvICi->setChecked( false );
    if( flgs[21] != '-' )
        ui->c_PsSYd->setChecked( true );
    else  ui->c_PsSYd->setChecked( false );
    if( flgs[22] != '-' )
        ui->c_PsSIA->setChecked( true );
    else  ui->c_PsSIA->setChecked( false );
    if( flgs[6] != '-' )
        ui->c_PvMSt->setChecked( true );
    else  ui->c_PvMSt->setChecked( false );
    if( flgs[17] != '-' )
        ui->c_PvMSc->setChecked( true );
    else  ui->c_PvMSc->setChecked( false );
    if( flgs[7] != '-' )
        ui->c_PvMSg->setChecked( true );
    else  ui->c_PvMSg->setChecked( false );
    if( flgs[28] != '-' )
        ui->c_PsMO->setChecked( true );
    else  ui->c_PsMO->setChecked( false );
    if( flgs[29] != '-' )
        ui->c_PsVTK->setChecked( true );
    else  ui->c_PsVTK->setChecked( false );
    if( flgs[31] != '-' )
        ui->c_PsSmode->setChecked( true );
    else  ui->c_PsSmode->setChecked( false );

    QObject::connect(  ui->c_PvMSt, SIGNAL(toggled(bool)), this, SLOT(setTScript(bool)) );

    //Page 3

    if( flgs[15] != '-' )
        ui->chSelect->setChecked( true );
    else ui->chSelect->setChecked( false );

    if( flgs[23] == 'j' )
        ui->rbJson->setChecked( true );
    else if( flgs[23] == 'b' || flgs[23] == '-' )
        ui->rbBinary->setChecked( true );
    else if( flgs[23] == 'f' )
        ui->rbFunJson->setChecked( true );
    else if( flgs[23] == 'o' )
        ui->rbFunKeyValue->setChecked( true );
    else
        ui->rbKeyVal->setChecked( true );

    if( flgs[24] != '-' )
        ui->chAll->setChecked( true );
    else ui->chAll->setChecked( false );
    if( flgs[25] != '-' )
        ui->chComment->setChecked( true );
    else ui->chComment->setChecked( false );

    ui->pPfrom->setValue(Pai[0]);
    ui->pPuntil->setValue(Pai[1]);
    ui->pPstep->setValue(Pai[2]);
    ui->pPtol->setValue(Pai[3]);

    ui->pTfrom->setValue(Tai[0]);
    ui->pTuntil->setValue(Tai[1]);
    ui->pTstep->setValue(Tai[2]);
    ui->pTtol->setValue(Tai[3]);


    if( flgs[26] != '-' )
        ui->checkArrays->setChecked( false );
    else ui->checkArrays->setChecked( true );
    arrayChecked(flgs[26] == '-');

    if( flgs[27] != '-' )
        ui->chInterp->setChecked( true );
    else ui->chInterp->setChecked( false );

    ui->pPPoints->setValue(size[12]);
    ui->pTPoints->setValue(size[13]);

    QObject::connect( ui->checkArrays, SIGNAL( toggled(bool) ), this, SLOT( arrayChecked(bool) ) );

    //Page 4

    ui->pnPG->setValue(size[3]);
    ui->pnFD->setValue(size[4]);
    ui->pnSFD->setValue(size[5]);


    switch( flgs[30] )
    {
    case '3': ui->moveGas->setChecked( true );
        [[fallthrough]];
    case '1': ui->moveAq->setChecked( true );
        break;
    case '2': ui->moveGas->setChecked( true );
        break;
    case '4': ui->moveSolid->setChecked( true );
        break;
    default: break;
    }

    QObject::connect( ui->moveGas, SIGNAL(toggled(bool)), this, SLOT(UnCheckSolid(bool)) );
    QObject::connect( ui->moveAq, SIGNAL(toggled(bool)), this, SLOT(UnCheckSolid(bool)) );
    QObject::connect( ui->moveSolid, SIGNAL(toggled(bool)), this, SLOT(UnCheckAqGas(bool)) );

    //Page 5
    ui->pqpt->setValue(size[17]);

    GroupBox1 = new QButtonGroup( ui->gbScript1 );
    GroupBox1->setExclusive(false);
    GroupBox1->addButton( ui->chP, 0 );
    GroupBox1->addButton( ui->chT, 1 );
    GroupBox1->addButton( ui->chV, 2 );
    GroupBox1->addButton( ui->chDiCp0, 3 );
    GroupBox1->addButton( ui->chDiCp1, 4 );
    GroupBox1->addButton( ui->chFlux, 5 );
    GroupBox1->addButton( ui->chRMass, 6 );
    GroupBox1->addButton( ui->chHydP0, 7 );
    GroupBox1->addButton( ui->chHydP1, 8 );
    GroupBox1->addButton( ui->chHydP2, 9 );
    GroupBox1->addButton( ui->chHydP3, 10 );
    GroupBox1->addButton( ui->chHydP4, 11 );
    GroupBox1->addButton( ui->chHydP5, 12 );
    GroupBox1->addButton( ui->chHydP6, 13 );
    connect( GroupBox1, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(ScriptChange(QAbstractButton*)) );

    if( flgs[6] =='+' )
        ui->pScript_t->setPlainText(calcScript.c_str());

    //Page 6

    ui->pnYS->setValue(size[6]);
    ui->pnE->setValue(size[7]);
    ui->pYE->setValue(size[8]);
    ui->pqpg->setValue(size[11]);
    resetPageList(aXname, aYname);


    //Page 7

    if( flgs[16] != '-' )
        ui->c_PvnVTK->setChecked( true );
    else  ui->c_PvnVTK->setChecked( false );
    // ui->pnVTK->setValue(size[18]);

    ui->listStatic->setWrapping( true );
    ui->listStatic->setSelectionMode(QAbstractItemView::MultiSelection);

    // define lists pages
    resetVTKList();

    // define commands
    QObject::connect( ui->keywdList, SIGNAL(currentRowChanged(int)),
                      this, SLOT(changePage(int)));

    // define List from record
    setVTK(vtk1, vtk2);
    disableVTK();
    QObject::connect(  ui->c_PvnVTK, SIGNAL(clicked()), this, SLOT(disableVTK()));


    //Page8

    ui->pnPTypes->setValue(size[15]);
    ui->pnProps->setValue(size[16]);

    if( flgs[12] != '-' )
        ui->c_PvGrid->setChecked( true );
    else
        ui->c_PvGrid->setChecked( false );

    if( flgs[13] != '-' )
        ui->c_PsDDc->setChecked( true );
    else  ui->c_PsDDc->setChecked( false );
    if( flgs[14] != '-' )
        ui->c_PsDIc->setChecked( true );
    else  ui->c_PsDIc->setChecked( false );
    ui->spinBox18->setValue(size[14]);
    ui->pnEl->setValue(size[10]);

}

void GEM2MTWizard::arrayChecked( bool check )
{
    ui->pPstep->setEnabled ( !check );
    ui->pTstep->setEnabled ( !check );
    ui->pPPoints->setEnabled ( check );
    ui->pTPoints->setEnabled ( check );
    ui->chInterp->setEnabled ( check );
    if(!check )
        ui->chInterp->setChecked(false);
}

void GEM2MTWizard::UnCheckSolid( bool check )
{
    if( check )
        ui->moveSolid->setChecked( false );
}

void GEM2MTWizard::UnCheckAqGas( bool check )
{
    if( check )
    {
        ui->moveAq->setChecked( false );
        ui->moveGas->setChecked( false );
    }
}


GEM2MTWizard::~GEM2MTWizard()
{
    delete ui;
}

void   GEM2MTWizard::getSizes( int size[20] )
{

    //Page 2
    size[0]=  ui->pnC->value();
    size[1]=  ui->pnIV->value();
    size[2]=  ui->pntM->value();
    size[9]= ui->pLb->value();
    //Page 3
    size[12]=  ui->pPPoints->value();
    size[13]= ui->pTPoints->value();

    if(!ui->checkArrays->isChecked())
    {
        // internal setup arrays (iterators )
        double Pai[4], Tai[4];
        getPdata( Pai );
        size[12] = getNpoints( Pai );
        getTdata( Tai );
        size[13] = getNpoints( Tai );
    }
    else
    {
        size[12]=  ui->pPPoints->value();
        size[13]= ui->pTPoints->value();
    }

    if( ui->chInterp->isChecked() )
    {
        size[13] = size[12];
    }
    //Page 4
    size[3]=  ui->pnPG->value();
    size[4]=  ui->pnFD->value();
    size[5]=  ui->pnSFD->value();
    //Page 5
    size[17]= ui->pqpt->value();
    //Page 6
    size[6]=  ui->pnYS->value();
    size[7]=  ui->pnE->value();
    size[8]= ui->pYE->value();
    size[11]= ui->pqpg->value();
    //Page 7
    //size[18]=  ui->pnVTK->value();
    //Page 8
    size[15]=  ui->pnPTypes->value();
    size[16]=  ui->pnProps->value();
    size[10]=  ui->pnEl->value();
    size[14]= ui->spinBox18->value();
}

void   GEM2MTWizard::getTdata( double Tai[4] )
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

void   GEM2MTWizard::getPdata( double Pai[4] )
{
    //   nP = ui->pPN->value();
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

void   GEM2MTWizard::getTaudata( double Tau[3] )
{
    Tau[0] = ui->pTauFrom->value();
    Tau[2] = ui->pTaustep->value();
    Tau[1] = Tau[2]* ui->pntM->value()+Tau[0];
}

string GEM2MTWizard::getCalcScript() const
{
    string res= ui->pScript_t->toPlainText().toStdString();
    return res;
}


void   GEM2MTWizard::getFlags( char flgs[32] )
{
    char type='S';

    if(  ui->pselT->isChecked())
        type='T';
    else if(  ui->pselB->isChecked())
        type='B';
    else if(  ui->pselA->isChecked())
        type='A';
    else if(  ui->pselC->isChecked())
        type='C';
    else if(  ui->pselD->isChecked())
        type='D';
    else  if(  ui->pselS->isChecked())
        type='S';
    else  if(  ui->pselW->isChecked())
        type='W';
    else  if(  ui->pselF->isChecked())
        type='F';
    flgs[18] = type;

    // Page 2

    if( ui->pLb->value() > 0  )
        flgs[5] = '+';
    else flgs[5] = '-';
    if(  ui->c_PvICi->isChecked() )
        flgs[4] = '+';
    else flgs[4] = '-';
    if(  ui->c_PsSYd->isChecked() )
        flgs[21] = '+';
    else flgs[21] = '-';
    if(  ui->c_PsSIA->isChecked() )
        flgs[22] = '+';
    else flgs[22] = '-';
    if(  ui->c_PvMSt->isChecked() )
        flgs[6] = '+';
    else flgs[6] = '-';
    if(  ui->c_PvMSc->isChecked() )
        flgs[17] = '+';
    else flgs[17] = '-';
    if(  ui->c_PvMSg->isChecked() )
        flgs[7] = '+';
    else flgs[7] = '-';
    if(  ui->c_PsMO->isChecked() )
        flgs[28] = '+';
    else flgs[28] = '-';
    if(  ui->c_PsVTK->isChecked() )
        flgs[29] = '+';
    else flgs[29] = '-';
    if(  ui->c_PsSmode->isChecked() )
        flgs[31] = '+';
    else flgs[31] = '-';

    //Page 3
    if( ui->chSelect->isChecked() )
        flgs[15] = '+';
    else flgs[15] = '-';

    if( ui->rbJson->isChecked() )
        flgs[23] = 'j';
    else if( ui->rbBinary->isChecked() )
        flgs[23] = 'b';
    else if( ui->rbFunJson->isChecked() )
        flgs[23] = 'f';
    else if( ui->rbFunKeyValue->isChecked() )
        flgs[23] = 'o';
    else
        flgs[23] = 't';

    if( ui->chAll->isChecked() )
        flgs[24] = '+';
    else flgs[24] = '-';
    if( ui->chComment->isChecked() )
        flgs[25] = '+';
    else flgs[25] = '-';

    if( ui->checkArrays->isChecked() )
        flgs[26] = '-';
    else flgs[26] = '+';
    if( ui->chInterp->isChecked() )
        flgs[27] = '+';
    else flgs[27] = '-';

    // Page 4
    if(  ui->pnPG->value() > 0  )
        flgs[9] = '+';
    else flgs[9] = '-';
    if(  ui->pnFD->value() > 0  )
        flgs[10] = '+';
    else flgs[10] = '-';
    if(  ui->pnSFD->value() > 0  )
        flgs[11] = '+';
    else flgs[11] = '-';

    if( ui->moveGas->isChecked() )
        flgs[30] = '2';
    if( ui->moveAq->isChecked())
    { if( ui->moveGas->isChecked() )
            flgs[30] = '3';
        else
            flgs[30] = '1';
    }
    if( ui->moveSolid->isChecked() )
        flgs[30] = '4';

    //Page 6
    if(  ui->pnE->value() > 0  && ui->pYE->value() >0  )
        flgs[8] = '+';
    else flgs[8] = '-';


    //Page 7

    if(  ui->c_PvnVTK->isChecked() )
        flgs[16] = '+';
    else flgs[16] = '-';

    //Page8

    if(  ui->c_PvGrid->isChecked() )
        flgs[12] = '+';
    else flgs[12] = '-';

    if(  ui->c_PsDDc->isChecked() )
        flgs[13] = '+';
    else flgs[13] = '-';
    if(  ui->c_PsDIc->isChecked() )
        flgs[14] = '+';
    else flgs[14] = '-';


    // flags 0,1,2,3, 19, 20, 30, 31 not used or internal
}

void GEM2MTWizard::help()
{
    pVisorImp->OpenHelp( GM_GEM2MT_WZ_HTML, WZSTEP,  ui->stackedWidget->currentIndex()+1 );
}

//==============================================================================

void GEM2MTWizard::setTScript( bool c_PvMSt_checked )
{
    if(  c_PvMSt_checked && calcScript.empty() )
        ScriptChange(nullptr);
}

// This function assembles the nodes and fluxes initialization and modification script
//   Fixed by DK at CSM on May 6, 2019
//
void GEM2MTWizard::ScriptChange(QAbstractButton*)
{
    QString ret;// = textEquat1->toPlainText();

    ret += "if( ct=0 ) begin \n"
           "$ Initialization part at zero (time) step\n"
           " if( (nC > 0) & (qc < nC) ) begin \n"
           "$  Initial setup of cells (nodes, boxes)\n";

    if( ui->chDiCp0->isChecked() ) // Initial system variant assignment to nodes
    {
        ret +=  "$  Assign different fluid and rock composition indices to nodes (assuming fluid=0 and rock=1)\n";
        if(  ui->pselA->isChecked() ||  ui->pselC->isChecked() )
        {   // for A and C mode, we need two first nodes as Cauchy sources
            ret += "$   for A or C mode, nodes 0 and 1 contain the initial fluid\n"
            "  DiCp[qc][0] =: ( (qc=0 | qc=1 ) ? 0 : ( nIV > 1 ?  1 : 0 ) );\n";
        }
        else { // for other modes, one Cauchy source is sufficient
            ret += "$   Node 0 contains initial fluid, for more nodes change as: ( qc=0 | qc=XX )?...\n"
                   "  DiCp[qc][0] =: ( qc=0 ? 0 : ( nIV > 1 ?  1 : 0 ) );\n";
        }
    }

    if( ui->chDiCp1->isChecked() ) // Initial assignment of node type
    {
        if(  ui->pselA->isChecked() ||  ui->pselC->isChecked() )
        {   // for A and C mode, we need two first nodes as Cauchy sources
            ret += "$   for A or C mode, nodes 0 and 1 are set to a constant-flux source\n"
                  "  DiCp[qc][1] =: ( (qc=0 | qc=1) ? 3 : 0 ); \n";
        }
        else if(  ui->pselB->isChecked() ||  ui->pselF->isChecked() )
        {
            ret += "$   for B or F mode, node 0 is set to a constant-flux source\n"
                   "  DiCp[qc][1] =: ( qc=0 ? 3: 0 ); \n";
            //  ret += "  DiCp[qc][1] =: ( qc=0 ? ( nSFD=1? 0: 3 ): 0 ); \n";
        }
        else {// One Cauchy source is sufficient
            ret += "$   Node 0 is set as source, for more sources change as: ( qc=0 | qc=XX )?...\n"
                   "  DiCp[qc][1] =: ( qc=0 ? 3 : 0 ); \n";
        }

        if(  ui->pselW->isChecked() ) // Random-walk sink fix
        {
            ret +=  "$   Other nodes normal, the last two set as constant source and sink\n"
                "  DiCp[qc][1] =: ( qc<nC-2 ? DiCp[qc][1]: (3));\n"
                "  DiCp[qc][1] =: (  qc<nC-1 ? DiCp[qc][1]: (-3));\n";
        }
        else {
            ret +=  "$   Other nodes normal, the last one is set as a constant-flux sink\n"
                "  DiCp[qc][1] =: (  qc<nC-1 ? DiCp[qc][1]: (-3));\n";
        }
    }

    if( ui->chP->isChecked() )   // Initial node pressure, bar (for GEM)
    { ret +=    "$  Set initial node pressures (change 0 in qc*0 to set a gradient)\n"
                "   StaP[qc][0] =: PTVi[(DiCp[qc][0])][0] + qc*0;\n";
        if(  ui->pselS->isChecked() ||  ui->pselF->isChecked() )
            ret += "   Pval[qc] =: StaP[qc][0];\n";
    }

    if( ui->chT->isChecked() )   // Initial node temperature, C (for GEM)
    {  ret +=   "$  Set initial node temperatures (change 0 in qc*0 to set a gradient)\n"
                "   StaP[qc][1] =: PTVi[(DiCp[qc][0])][1] + qc*0;\n";
        if(  ui->pselS->isChecked() ||  ui->pselF->isChecked() )
            ret += "   Tval[qc] =: StaP[qc][1];\n";
    }
    if( ui->chV->isChecked() )  // Volume constraint for GEM (usually 0) - change to AMR on water vapor?
        ret += "   StaP[qc][2] =: PTVi[(DiCp[qc][0])][2];\n";

    if( ui->chRMass->isChecked() ) // Initial (reactive) mass of the node
        ret += "   StaP[qc][3] =: PTVi[(DiCp[qc][0])][3];\n";

    if( ui->chHydP0->isChecked() ) // Initial total volume of the node, m3
        ret +=  "$  Initial total volume of the node, m3 (for porosity)\n"
             "   HydP[qc][0] =: ADpar[1];\n";

    if( ui->chHydP1->isChecked() ) // Initial advection velocity
        ret +=  "$  Initial advection velocity, m/s\n"
             "   HydP[qc][1] =: ADpar[2];\n";

    if( ui->chHydP2->isChecked() )  // Initial effective porosity
        ret +=  "$  Initial effective porosity\n"
             "   HydP[qc][2] =: ADpar[3];\n";

    if( ui->chHydP3->isChecked() )  // initial effective permeability
        ret +=  "$  Initial effective permeability\n"
             "   HydP[qc][3] =: ADpar[4];\n";

    if( ui->chHydP4->isChecked() )   // Initial specific longitudinal dispersivity
        ret +=  "$  Initial specific longitudinal dispersivity\n"
             "   HydP[qc][4] =: ADpar[5];\n";

    if( ui->chHydP5->isChecked() )   // Initial general diffusivity
        ret +=  "$  Initial general diffusivity\n"
             "   HydP[qc][5] =: ADpar[6];\n";

    if( ui->chHydP6->isChecked() )  // Initial tortuosity factor
        ret +=  "$  Initial tortuosity factor\n"
             "   HydP[qc][6] =: ADpar[7];\n";

    ret +=  " end \n"
            "$ end of initialization of cells (nodes, boxes)\n";

    if( ui->chFlux->isChecked() )
    {
        ret +=  "   if( (nFD > 0) & (qf < nFD) ) begin \n"
                "$   initialisation of tables for properties of fluxes\n"
                "     if( qf = qc ) begin\n"
                "$    Setting the chain of unidirectional fluxes connecting boxes\n";
        if(  ui->pselS->isChecked() )
        {
            ret +=  "$     flux from node/box\n"
                    "      FDLi[qf][0] =: ( qf=0? 0: FDLi[qf-1][1] ); \n"
                    "$     flux to node/box\n"
                    "      FDLi[qf][1] =: ( (qf<(nC-1))? (qf+1): (-1)); \n"
                    "$     flux order zero (constant mass per step)\n"
                    "      FDLf[qf][0] =: 0;\n"
                    "$     flux rate 1 (the whole fluid mass)\n"
                    "      FDLf[qf][1] =: 1;\n";
        }
        if(  ui->pselF->isChecked() )
        {
            ret +=  "$     flux from node/box\n"
                    "      FDLi[qf][0] =: ( qf=0? 0: FDLi[qf-1][1] ); \n"
                    "$     flux to node/box\n"
                    "      FDLi[qf][1] =: ( (qf<(nC-1))? (qf+1): (-1)); \n"
                    "$     flux order 1 (proportional to source MPG mass)\n"
                    "      FDLf[qf][0] =: ( qf=0? 0: 1);\n"
                    "$     flux rate constant\n"
                    "      FDLf[qf][1] =: ( qf=0? 1: 0.1);\n";
        }
        if(  ui->pselB->isChecked() )
        {
            ret +=  "$     flux from node/box\n"
                    "      FDLi[qf][0] =: ( qf=0? 0: FDLi[qf-1][1] ); \n"
                    "$     flux to node/box\n"
                    "      FDLi[qf][1] =: ( (qf<(nC-1))? (qf+1): (-1)); \n"
                    "$     flux order 1 (proportional to source MPG mass)\n"
                    "      FDLf[qf][0] =: ( qf=0? 0: 1);\n"
                    "$     flux rate constant\n"
                    "      FDLf[qf][1] =: ( qf=0? 1: 0.1);\n";
        }
        ret += "  end\n"
               "$ end of setting a chain of 1-dir fluxes connecting boxes\n";

        ret += "     if( qf > nC-1 ) begin\n"
                "$    additional fluxes (elemental remo/prod or arbitrary normal fluxes)\n"
                "$    enter index 0 as MPG name in FDLmp[qf] column\n"
                "$    group for first elemental removal or production row in BSF table\n"
                "       if( qf = nC ) begin\n"
                "         FDLi[qf][0] =: 30;\n"
                "         FDLi[qf][1] =: (-1);\n"
                "$    flux order (0, 1 or 3), change as desired\n"
                "         FDLf[qf][0] =: 0;\n"
                "$    flux rate (constant): >0: removal; <0: production. Change as desired\n"
                "         FDLf[qf][1] =: 0.001;\n"
                "         end\n"
                "$    end of group - add below groups for other remo/prod rows in BSF table\n";

        ret += "$    for the second group, enter index 1 as MPG name in FDLmp[qf] column\n"
                "$        if( qf = nC+1 ) begin\n"
                "$         .......\n"
                "     end\n"
                "$    For arbitrary normal MPG fluxes between boxes, add groups like above\n"
                "$     in setting the chain of fluxes\n"
                "$    end of additional fluxes\n";

        ret+=  "  end\n"
                "$ end of initialization of fluxes\n";
    }
    ret += "end\n"
           "$ end of initialization at ct=0\n"
           "$\n$else\n"
           "$ optional modifications at (time) step ct > 0\n"
           "$ begin \n"
           "$  add operators here \n"
           "$ ... \n"
           "$ end of optional modifications at (time) steps ct > 0\n"
           "$ end \n";

    ui->pScript_t->setPlainText(ret);
}

//==============================================================================

static equatSetupData eqMT( "xt", "yt", "qc", "qc" );

// work with lists
void GEM2MTWizard::resetPageList(const char* aXname, const char* aYname)
{

    std::vector<pagesSetupData> scalarsList;
    std::vector<pagesSetupData> pgData1;

    GetListsnRT( -2, pgData1,  scalarsList );
    GetListsnRT( RT_GEM2MT, pgData1,  scalarsList );

    pageScript = new EquatSetup( ui->page_6, eqMT,
                                 RT_GEM2MT, pgData1, scalarsList, outScript.c_str(), aXname, aYname  );
    ui->verticalLayout_9->addWidget(pageScript);


}

//==============================================================================

// work with lists
void GEM2MTWizard::resetVTKList()
{
    uint jj;
    int nO;

    std::vector<pagesSetupData> scalarsList;
    std::vector<pagesSetupData> scalarsList2;
    std::vector<pagesSetupData> wnData;

    for(int ii=0; ii<38; ii++ )
        scalarsList.push_back( pagesSetupData(DataBR_fields[ii].name.c_str(), ii));
    GetListsnRT( -2, wnData,  scalarsList2 );

    pNdx.push_back(f_bIC);
    pNdx.push_back(f_uIC);
    pNdx.push_back(f_xDC);
    pNdx.push_back(f_gam);
    pNdx.push_back(f_xPH);
    pNdx.push_back(f_mPS);
    pNdx.push_back(f_vPS);
    pNdx.push_back(f_xPA);
    pNdx.push_back(f_bSP);
    pNdx.push_back(f_aPH);
    pNdx.push_back(f_rMB);
    pNdx.push_back(f_dll);
    pNdx.push_back(f_dul);
    pNdx.push_back(f_amrl);
    pNdx.push_back(f_amru);
    pNdx.push_back(f_mPH);
    pNdx.push_back(f_vPH);
    pNdx.push_back(f_m_t);
    pNdx.push_back(f_con);
    pNdx.push_back(f_mju);
    pNdx.push_back(f_lga);


    pNdx.push_back(f_bPS);

    QString str;
    QListWidgetItem* item1;
    QWidget* page1;
    QHBoxLayout* horizontalLayout1;
    QListWidget* lstIndexes1;

    // delete old data
    cPage = 0;
    pgData.push_back( pagesSetupData("Scalars", -1));
    pLists.append( ui->listStatic  );

    ui->listStatic->clear();
    for(size_t  ii=0; ii<scalarsList.size(); ii++ )
    {
        stData.push_back( pagesSetupData(scalarsList[ii]));
        item1 = new QListWidgetItem( scalarsList[ii].pageName.c_str(),  ui->listStatic);
        item1->setToolTip( DataBR_fields[ii].comment.c_str() );
    }


    // init new pages
    for(size_t ii=0; ii<wnData.size(); ii++ )
    {
        nO = wnData[ii].nObj;

        TCStringArray lst;
        TProfil::pm->getNamesList( nO, lst);
        if( lst.size() < 1 )  // undefined indexation
            continue;

        pgData.push_back( pagesSetupData(wnData[ii]));
        str = QString("%1").arg( wnData[ii].pageName.c_str());
        item1 = new QListWidgetItem( str,  ui->keywdList);

        // add page
        page1 = new QWidget();
        horizontalLayout1 = new QHBoxLayout(page1);
        lstIndexes1 = new QListWidget(page1);
        lstIndexes1->setWrapping( true );
        lstIndexes1->setResizeMode(QListView::Adjust);
        lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
        horizontalLayout1->addWidget(lstIndexes1);
        ui->winStac->addWidget(page1);

        // insert items to list of indexes
        for(  jj=0; jj<lst.size(); jj++ )
        {
            item1 = new QListWidgetItem( lst[jj].c_str(), lstIndexes1);
        }
        pLists.append(lstIndexes1);
    }

    // define current page
    cPage = 0;
    ui->keywdList->setCurrentItem(nullptr);
    ui->keywdList->item(0)->setSelected(true);
    //changePage( cPage );

    for(int  ii=0; ii<pLists.count(); ii++ ) // ui->listStatic added as first
    {    QObject::connect( pLists[ii]->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
                           this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );
    }

}

void GEM2MTWizard::changePage( int nPage )
{
    if(nPage < 0 )
        return;
    cPage = nPage;
    ui->winStac->setCurrentIndex ( nPage );
    if( cPage <= 0 )
        ui->lDesc->setText( "Static Objects List ");
    else
        ui->lDesc->setText( aObj[pgData[nPage].nObj]->GetDescription(0,0).c_str());
}

void GEM2MTWizard::changeTable(const QItemSelection & selected, const QItemSelection & deselected)
{
    QModelIndex  ndx;
    int row;

    // added selected
    foreach( ndx,  selected.indexes()  )
    {
        string stt = ndx.data(Qt::DisplayRole).toString().toStdString();
        tableInsertRow( pgData[cPage].nObj, ndx.row(), stt.c_str() );
    }
    // delete deselected
    foreach( ndx,  deselected.indexes()  )
    {
        row = tableFindRow( pgData[cPage].nObj, ndx.row());
        if( row >= 0)
            tableDeleteRow( row );
    }
}

// internal functions
int GEM2MTWizard::tableFindRow( int nO, int ndx)
{
    int nRow = -1;
    for(size_t ii=0; ii<scriptData.size(); ii++ )
    {
        if(scriptData[ii].nObj == nO && scriptData[ii].nIdx == ndx )
        { nRow = ii; break; }
    }
    return nRow;
}

void GEM2MTWizard::tableInsertRow( int nO, int ndx, const char * andName )
{
    if(cPage == 0)
    {  scriptData.push_back(  scriptSetupData( cPage, nO, andName,
                                               ndx, andName, "" ));
    }
    else
    {
        scriptData.push_back(  scriptSetupData( cPage, nO, aObj[nO]->GetKeywd(),
                                                ndx, andName, "" ));
    }
    listUpdate();
}


void GEM2MTWizard::tableDeleteRow( int nRow )
{
    scriptData.erase(scriptData.begin()+nRow);
    listUpdate();
}

void GEM2MTWizard::disableVTK()
{
    ui->groupBox_3->setEnabled(  ui->c_PvnVTK->isChecked() );
}

void GEM2MTWizard::listUpdate()
{
    QString buf;

    ui->listVTK->clear();
    for(size_t ii=0; ii<scriptData.size(); ii++ )
    {
        if( scriptData[ii].nObj >= 0 )
            buf = QString("%1(%2)").arg(
                        pgData[scriptData[ii].nWin].pageName.c_str()/*scriptData[ii].objName.c_str()*/,
                    scriptData[ii].ndxName.c_str() );
        else
            buf = QString("%1").arg( scriptData[ii].ndxName.c_str() );

        ui->listVTK->addItem(buf);
    }
    ui->listVTK->setCurrentRow(0);
}

inline int GEM2MTWizard::findVTKarr( int vtk1  )
{
    //if( vtk1 == f_bPS )
    // return po strokam

    for( size_t ii=0; ii< pNdx.size(); ii++ )
        if(  pNdx[ii] == vtk1 )
            return ii+1;

    return -1;
}

void GEM2MTWizard::setVTK( TCIntArray vtk1, TCIntArray vtk2  )
{

    scriptData.clear();

    for(size_t ii=0; ii<vtk1.size(); ii++ )
    {
        int cPage1;
        int ndx;

        if( vtk1[ii] < f_bIC )
        {
            cPage1 = 0;
            ndx = vtk1[ii];
        }
        else
        {
            cPage1 =  findVTKarr( vtk1[ii] );
            ndx = vtk2[ii];

            if(vtk1[ii] == f_bPS) // bPS
            { cPage1 +=  vtk2[ii]/TMulti::sm->GetPM()->N;
                ndx = vtk2[ii]%TMulti::sm->GetPM()->N;
                cPage1 = min<int>(cPage1, pLists.count()-1);
            }
        }

        if( cPage1 >=0 && ndx >=0 )
        {       ui->keywdList->setCurrentItem(ui->keywdList->item(cPage1 ));
            pLists[cPage1]->item(ndx)->setSelected(true);
        }
    }
}

void GEM2MTWizard::getVTK( TCIntArray& vtk1, TCIntArray& vtk2  )
{
    uint page_;
    int vt_1;
    vtk1.clear();
    vtk2.clear();
    if( ! ui->c_PvnVTK->isChecked() )
        return; // select all

    for(size_t ii=0; ii<scriptData.size(); ii++ )
    {
        if( scriptData[ii].nObj >= 0 )
        {
            page_ = scriptData[ii].nWin;
            if(page_ >  pNdx.size() )
                vt_1 = f_bPS;
            else vt_1 =  pNdx[page_-1];


            if( vt_1 == f_bPS ) //bPs
            {
                vtk1.push_back( vt_1 );
                vtk2.push_back(scriptData[ii].nIdx + pgData[page_].ndx*TMulti::sm->GetPM()->N);
            }
            else
            {
                vtk1.push_back( vt_1 );
                vtk2.push_back(scriptData[ii].nIdx);
            }
        }
        else
        {
            vtk1.push_back( scriptData[ii].nIdx );
            vtk2.push_back( 0 );
        }
    }

}

void GEM2MTWizard::setupPTArrays()
{
    int nT, nP, ii;
    double cT, cP;
    double Pai[4], Tai[4];
    double  *arT, *arP;

    //init P array
    getPdata( Pai );
    nP = getNpoints( Pai );
    arP = static_cast<double *>(aObj[ o_mtpval]->Alloc( nP, 1, D_));
    cP = Pai[START_];
    for( ii=0; ii<nP; ii++ )
    {
        arP[ii] = cP;
        cP+= Pai[2];
    }

    //init T array
    getTdata( Tai );
    nT = getNpoints( Tai );
    arT = static_cast<double *>( aObj[ o_mtpval]->Alloc( nT, 1, D_) );
    cT = Tai[START_];
    for( ii=0; ii<nT; ii++ )
    {
        arT[ii] = cT;
        cT+= Tai[2];
    }

}

void GEM2MTWizard::definePArray()
{
    int nPs, nP, ii;
    double Pai[4];
    double *arP;

    nP =  ui->pPPoints->value();
    //init P array
    nPs = aObj[ o_mtpval]->GetN();
    arP = static_cast<double *>(aObj[ o_mtpval]->Alloc( nP, 1, D_));

    if( nPs==1 && nP>1 )
    {
        double stepP = getStep( Pai, nP );
        double cP = Pai[0];
        for( ii=0; ii<nP; ii++ )
        {
            arP[ii] = cP;
            cP += stepP;
        }
    }
}

void GEM2MTWizard::initPTable()
{
    // init table
    TObjectModel* model;
    QList<FieldInfo>	aFlds;

    if( PTable )
    {   ui->gridLayout_14->removeWidget(PTable);
        delete PTable; //????
    }

    aFlds.clear();
    aFlds.append(FieldInfo( o_mtpval, ftFloat, 15, false, First, eYes, stIO, 20, 1));

    if(ui->chInterp->isChecked())
        aFlds.append(FieldInfo( o_mttval, ftFloat, 15, false, Tied, eYes, stIO, 20, 1));
    model = new TObjectModel( aFlds, this );

    PTable =  new TObjectTable( aFlds, this );
    TObjectDelegate *deleg = new TObjectDelegate( PTable, this);
    PTable->setItemDelegate(deleg);
    PTable->setModel(model);

    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
    //sizePolicy1.setHorizontalStretch(0);
    //sizePolicy1.setVerticalStretch(0);
    PTable->setSizePolicy(sizePolicy1);

    int rowSize =0, colSize=0;
    PTable->getObjectSize(rowSize, colSize);
    PTable->setMaximumSize(QSize(colSize, 16777215));

    //   if(!chInterp->isChecked())
    //     formLayout->setWidget(3, QFormLayout::LabelRole,PTable/*, 3, 0, 1, 1*/);
    //   else
    //     formLayout->setWidget(3, QFormLayout::SpanningRole,PTable/*, 3, 0, 1, 2*/);

}

void GEM2MTWizard::defineTArray()
{
    int nT, nTs, ii;
    double Tai[4];
    double  *arT;

    getTdata( Tai );

    if(ui->chInterp->isChecked())
        nT =  ui->pPPoints->value();
    else
        nT = ui->pTPoints->value();

    //init T array
    nTs =aObj[ o_mttval]->GetN();
    arT = static_cast<double *>(aObj[ o_mttval]->Alloc( nT, 1, D_));

    if( nTs == 1 && nT >1 )
    {
        double stepT = getStep( Tai, nT );
        double cT = Tai[0];
        for( ii=0; ii<nT; ii++ )
        {
            arT[ii] = cT;
            cT += stepT;
        }
    }

}

void GEM2MTWizard::initTTable()
{
    // init table
    TObjectModel* model;
    QList<FieldInfo>	aFlds;

    if( TTable )
    {   ui->gridLayout_14->removeWidget(TTable);
        delete TTable; //????
        TTable = nullptr;
    }

    if(ui->chInterp->isChecked())  // only one table
        return;

    aFlds.clear();
    aFlds.append(FieldInfo( o_mttval, ftFloat, 15, false, First, eYes, stIO, 20, 1));
    model = new TObjectModel( aFlds, this );

    TTable =  new TObjectTable( aFlds, this );
    TObjectDelegate *deleg = new TObjectDelegate( TTable, this);
    TTable->setItemDelegate(deleg);
    TTable->setModel(model);

    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
    //sizePolicy1.setHorizontalStretch(0);
    //sizePolicy1.setVerticalStretch(0);
    TTable->setSizePolicy(sizePolicy1);

    int rowSize =0, colSize=0;
    TTable->getObjectSize(rowSize, colSize);
    TTable->setMaximumSize(QSize(colSize, 16777215));

    //formLayout->setWidget(3, QFormLayout::FieldRole,TTable/*, 3, 1, 1, 1*/);
}

void GEM2MTWizard::showPTTable()
{
    if(!ui->chInterp->isChecked())
    {
        ui->gridLayout_14->addWidget( PTable, 3,0,1,1 );
        ui->gridLayout_14->addWidget( TTable, 3,1,1,1 );
        ui->label_26->setText(QApplication::translate("GEM2MTWizardData", "Pressure P, bar", nullptr));
        ui->label_27->setText(QApplication::translate("GEM2MTWizardData", "Temperature T, C", nullptr));
    }
    else
    {
        ui->gridLayout_14->addWidget(PTable, 3,0,1,2 );
        ui->label_26->setText(QApplication::translate("GEM2MTWizardData", "          Pressure P, bar        Temperature T, C ", nullptr));
        ui->label_27->setText(QApplication::translate("GEM2MTWizardData", "", nullptr));
    }
}

void GEM2MTWizard::objectChanged()
{

}

//--------------------- End of GEM2MTWizard.cpp ---------------------------

