//-------------------------------------------------------------------
// $Id: GtDemoWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GtDemoWizard class
//
// Copyright (C) 2005-2008 S.Dmytriyeva
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


#include "ui_GtDemoWizard4.h"
#include "GtDemoWizard.h"
#include "GemsMainWindow.h"
#include "m_param.h"
#include "service.h"

void GtDemoWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void GtDemoWizard::CmNext()
{
    int ndx = ui->stackedWidget->currentIndex();
    auto nLines = pageScript->getScriptLinesNum();
    if( ndx == 1 && nLines > 0)
    {
        ui->pGraphY->setValue( nLines );
        ui->pGraphX->setValue( pageScript->getAbscissaNum() );
    }

    if( ndx == 0 ) // define graph window
    {
        CmChangePage2(0);
    }

    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void 	GtDemoWizard::resetNextButton()
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

void GtDemoWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}


GtDemoWizard::GtDemoWizard( const char* pkey, int size[8], const char *ascript,
const char *proc_key, const char* aXname, const char* aYname, QWidget* parent):
    QDialog( parent ),
    ui(new Ui::GtDemoWizardData),
    script(ascript), pageScript(nullptr)
{
    ui->setupUi(this);

    string str1= "GEM-Selektor GtDemo Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );

    allButtons = new QButtonGroup( ui->groupBox_7 );
    allButtons->addButton(ui->psIC, 0);
    allButtons->addButton(ui->psDC, 1);
    allButtons->addButton(ui->psBC, 2);
    allButtons->addButton(ui->psRE, 3);
    allButtons->addButton(ui->psRP, 4);
    allButtons->addButton(ui->psPH, 5);
    allButtons->addButton(ui->psST, 6);
    allButtons->addButton(ui->psPE, 7);
    allButtons->addButton(ui->psPB, 8);
    allButtons->addButton(ui->psUT, 9);
    allButtons->addButton(ui->psTR, 10);

    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();
    
    //Page1
    nRT = -1;// size[0];
    switch( size[0]) //nRT - chain index
    {
    case RT_ICOMP:  ui->psIC->setChecked( true ); break;
    case RT_DCOMP:  ui->psDC->setChecked( true ); break;
    case RT_COMPOS: ui->psBC->setChecked( true ); break;
    case RT_REACDC: ui->psRE->setChecked( true ); break;
    case RT_RTPARM: ui->psRP->setChecked( true ); break;
    case RT_PHASE:  ui->psPH->setChecked( true ); break;
    default:
    case RT_SYSEQ:  ui->psST->setChecked( true ); break;
    case RT_PROCES: ui->psPE->setChecked( true ); break;
    case RT_UNSPACE: ui->psPB->setChecked( true ); break;
    case RT_DUALTH: ui->psUT->setChecked( true ); break;
    case RT_GEM2MT: ui->psTR->setChecked( true ); break;
    }
    //Page 2
    resetPageList(size[0], aXname, aYname);

    //Page3
    ui->pGraphY->setValue(size[6]);
    ui->pGraphX->setValue(size[7]);
    ui->pNwc->setValue(size[2]);
    ui->pNqp->setValue(size[3]);
    ui->pELine->setValue(size[4]);
    ui->pECol->setValue(size[5]);
    //Page4
    ui->spinBox18->setValue(size[1]);
    ui->lineEditProcesKey->setText( proc_key );

    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    //QObject::connect( allButtons, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(CmChangePage2(int)));

    //groupBox_7->toggled();

}


GtDemoWizard::~GtDemoWizard()
{
    delete ui;
}

void   GtDemoWizard::getSizes( int size[8] )
{
    size[0] = getnRT();

    size[1]= ui->spinBox18->value();
    size[2]= ui->pNwc->value();
    size[3]= ui->pNqp->value();
    size[4]= ui->pELine->value();
    size[5]= ui->pECol->value();
    size[6]= ui->pGraphY->value();
    size[7]= ui->pGraphX->value();
}

int   GtDemoWizard::getnRT()
{
    int newRT;

    if( ui->psST->isChecked() )
        newRT = RT_SYSEQ;
    else
        if( ui->psPE->isChecked() )
            newRT = RT_PROCES;
        else
            if( ui->psPB->isChecked() )
                newRT = RT_UNSPACE;
            else
                if( ui->psIC->isChecked() )
                    newRT = RT_ICOMP;
                else
                    if( ui->psDC->isChecked() )
                        newRT = RT_DCOMP;
                    else
                        if( ui->psBC->isChecked() )
                            newRT = RT_COMPOS;
                        else
                            if( ui->psRE->isChecked() )
                                newRT = RT_REACDC;
                            else
                                if( ui->psRP->isChecked() )
                                    newRT = RT_RTPARM;
                                else
                                    if( ui->psPH->isChecked() )
                                        newRT = RT_PHASE;
                                    else
                                        if( ui->psUT->isChecked() )
                                            newRT = RT_DUALTH;
                                        else
                                            newRT = RT_GEM2MT;

    return newRT;
}

void GtDemoWizard::CmChangePage2(int)
{
    int	newRT = getnRT();
    resetPageList(  newRT );
}

string GtDemoWizard::getPrKey()
{
    string str = ui->lineEditProcesKey->text().toStdString();
    return str;
}

void GtDemoWizard::help()
{
    pVisorImp->OpenHelp( GM_GTDEMO_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

//==============================================================================

equatSetupData eqd( "x0", "y0", "jR", "jR", true );



// work with lists
void GtDemoWizard::resetPageList( int newRT,const char* aXname, const char* aYname )
{

    std::vector<pagesSetupData> scalarsList;
    std::vector<pagesSetupData> pgData;

    if( nRT == newRT )
        return;

    nRT = newRT;

    if( nRT >= RT_SYSEQ ) // read first record in DB
    {
        if(  rt[RT_SYSEQ]->GetStatus() != ONEF_)
        {
            TCStringArray aRklist;
            TCIntArray anRk;
            auto Nr = rt[RT_SYSEQ]->GetKeyList( ALLKEY, aRklist, anRk );
            if( Nr > 0 )
                TProfil::pm->loadSystat( aRklist[0].c_str() );
        }
        GetListsnRT( MD_MULTI, pgData,  scalarsList );
        GetListsnRT( MD_SYSTEM, pgData,  scalarsList );
        GetListsnRT( MD_MTPARM, pgData,  scalarsList );
        GetListsnRT( MD_RMULTS, pgData,  scalarsList );
        GetListsnRT( nRT, pgData,  scalarsList );
    }
    else
    {
        GetListsnRT( nRT, pgData,  scalarsList );
    }

    GetListsnRT( RT_GTDEMO, pgData,  scalarsList );


    if( pageScript )
        pageScript->resetPageList( newRT, pgData, scalarsList );
    else
    {
        pageScript = new EquatSetup( ui->page_3, eqd, nRT, pgData, scalarsList,
                                     script.c_str(), aXname, aYname  );
        ui->verticalLayout_2->addWidget(pageScript);
    }

}


//--------------------- End of GtDemoWizard.cpp ---------------------------
