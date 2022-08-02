//-------------------------------------------------------------------
// $Id: GtDemoWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GtDemoWizard class
//
// Copyright (C) 2005-2021 S.Dmytriyeva
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
#include "KeyFilter.h"
#include "m_param.h"
#include "m_proces.h"
#include "service.h"

void GtDemoWizard::CmBack()
{
    int ndx = ui->stackedWidget->currentIndex();
    if( ndx == 2 && curentRT <= RT_SYSEQ)
        ndx--;
    ui->stackedWidget->setCurrentIndex( ndx-1 );
    resetNextButton();
    resetBackButton();
}

void GtDemoWizard::CmNext()
{
    int ndx = ui->stackedWidget->currentIndex();
    if( ndx == 0)
    {
        curentRT = get_nRT();
        if( curentRT > RT_SYSEQ )
            reset_process_list( curentRT );
        else
            ndx++;
    }

    switch( ndx)
    {
    case 1: // define keys window
        reset_keys_list( curentRT );
        break;
    case 2: // define graph window
        reset_page_list( curentRT );
        break;
    case 3:
    {
        auto nLines = pageScript->getScriptLinesNum();
        if(nLines > 0 )
        {
            ui->pGraphY->setValue( nLines );
            ui->pGraphX->setValue( pageScript->getAbscissaNum() );
        }
    }
        break;
    }
    ui->stackedWidget->setCurrentIndex( ndx+1 );
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


GtDemoWizard::GtDemoWizard( const string& pkey, char flgs[16], int size[8], const string& ascript,
                            const string& proc_key, const string& aXname, const string& aYname,
                            TCStringArray& keys, QWidget* parent):
    QDialog( parent ),
    ui(new Ui::GtDemoWizardData),
    script(ascript),
    pageScript(nullptr),
    process_old_selection(proc_key),
    sel_old_selection(keys)
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
    
    // Page1
    switch(size[0]) //nRT - chain index
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

    // Page2
    // Page3
    sel_keys_table = new KeysTableProxy(ui->page_5);
    alloc_keys_model(get_nRT());
    ui->gridLayout_5->addWidget(sel_keys_table, 4, 0, 1, 5);

    selSelectionChanged( ui->checkFull->isChecked() );
    QObject::connect( ui->bSelectAll, SIGNAL(clicked()), sel_keys_table, SLOT(selectAll()));
    QObject::connect( ui->bClearAll, SIGNAL(clicked()), sel_keys_table, SLOT(clearSelection()));
    QObject::connect( ui->bFilter, SIGNAL(clicked()), this, SLOT(selChangeFilter()));
    QObject::connect( ui->checkFull, SIGNAL(stateChanged(int)), this, SLOT(selSelectionChanged(int)));

    // Page4
    reset_page_list(size[0], aXname.c_str(), aYname.c_str());

    // Page5
    ui->pGraphY->setValue(size[6]);
    ui->pGraphX->setValue(size[7]);
    ui->pNwc->setValue(size[2]);
    ui->pNqp->setValue(size[3]);
    ui->pELine->setValue(size[4]);
    ui->pECol->setValue(size[5]);

    // Page6
    ui->spinBox18->setValue(size[1]);
    if( flgs[13] != '-' )
        ui->calcSys->setChecked( true );
    else ui->calcSys->setChecked( false );

    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
}


GtDemoWizard::~GtDemoWizard()
{
    delete ui;
}

void GtDemoWizard::getFlags(char flgs[16])
{
    if( ui->calcSys->isChecked() )
        flgs[13] = '+';
    else flgs[13] = '-';
}

void   GtDemoWizard::getSizes( int size[8], std::string& prKey, TCStringArray& keys )
{
    prKey = get_process_key();
    keys = sel_keys_table->allSelectedKeys();

    size[0] = get_nRT();
    size[1]= ui->spinBox18->value();
    size[2]= ui->pNwc->value();
    size[3]= ui->pNqp->value();
    size[4]= ui->pELine->value();
    size[5]= ui->pECol->value();
    size[6]= ui->pGraphY->value();
    size[7]= ui->pGraphX->value();
}

std::string   GtDemoWizard::get_process_key()
{
    auto prKey = process_old_selection;
    if( process_keys_table )
    {
        auto allselpr = process_keys_table->allSelectedKeys();
        if( !allselpr.empty() )
          prKey = allselpr[0];
    }
    return  prKey;
}


int GtDemoWizard::get_nRT()
{
    int newRT;

    if( ui->psST->isChecked() )
        newRT = RT_SYSEQ;
    else if( ui->psPE->isChecked() )
        newRT = RT_PROCES;
    else if( ui->psPB->isChecked() )
        newRT = RT_UNSPACE;
    else if( ui->psIC->isChecked() )
        newRT = RT_ICOMP;
    else if( ui->psDC->isChecked() )
        newRT = RT_DCOMP;
    else if( ui->psBC->isChecked() )
        newRT = RT_COMPOS;
    else if( ui->psRE->isChecked() )
        newRT = RT_REACDC;
    else if( ui->psRP->isChecked() )
        newRT = RT_RTPARM;
    else if( ui->psPH->isChecked() )
        newRT = RT_PHASE;
    else  if( ui->psUT->isChecked() )
        newRT = RT_DUALTH;
    else
        newRT = RT_GEM2MT;

    return newRT;
}

void GtDemoWizard::help()
{
    pVisorImp->OpenHelp( GM_GTDEMO_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

//==============================================================================

equatSetupData eqd( "x0", "y0", "jR", "jR", true );


// work with lists
void GtDemoWizard::reset_page_list( int newRT,const char* aXname, const char* aYname )
{

    std::vector<pagesSetupData> scalarsList;
    std::vector<pagesSetupData> pgData;

    if( equatRT == newRT )
        return;

    equatRT = newRT;

    if( equatRT >= RT_SYSEQ ) // read first record in DB
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
        GetListsnRT( equatRT, pgData,  scalarsList );
    }
    else
    {
        GetListsnRT( equatRT, pgData,  scalarsList );
    }

    GetListsnRT( RT_GTDEMO, pgData,  scalarsList );

    if( pageScript )
        pageScript->resetPageList( newRT, pgData, scalarsList );
    else
    {
        pageScript = new EquatSetup( ui->page_3, eqd, equatRT, pgData, scalarsList,
                                     script.c_str(), aXname, aYname  );
        ui->verticalLayout_2->addWidget(pageScript);
    }
}

void GtDemoWizard::reset_process_list(int newRT)
{
    if( newRT != RT_PROCES || process_keys_model != nullptr ) // init only once
      return;

    QString filter = "*";
    TCStringArray old_selection;

    if( !process_old_selection.empty() )
    {
      if( process_old_selection.find_first_of('*') != std::string::npos )
         filter = process_old_selection.c_str();
      else
         old_selection.push_back(process_old_selection);
    }

    ui->checkPrFull->setChecked( old_selection.empty() );
    ui->linePrFilter->setText(filter);
    process_keys_table = new KeysTableProxy(ui->page_6);
    process_keys_table->updateSelectionMode(QAbstractItemView::SingleSelection);
    process_keys_model = new KeyModel( this, RT_PROCES );
    process_keys_model->setSelection(old_selection);
    process_keys_table->setModel(process_keys_model);
    ui->gridLayout_4->addWidget(process_keys_table, 5, 0, 1, 3);

    processSelectionChanged( ui->checkPrFull->isChecked() );
    QObject::connect( ui->bPrFilter, SIGNAL(clicked()), this, SLOT(processChangeFilter()));
    QObject::connect( ui->checkPrFull, SIGNAL(stateChanged(int)), this, SLOT(processSelectionChanged(int)));
}

void GtDemoWizard::reset_keys_list(int newRT)
{
    if( newRT == keysRT )
        return;
    sel_old_selection.clear();
    delete sel_keys_model;
    alloc_keys_model(newRT);
}

void GtDemoWizard::alloc_keys_model(int cRT)
{
    keysRT = cRT;
    if( cRT > RT_PROCES )
        return;   // not implemented

    if( cRT == RT_PROCES )
    {
        TCStringArray aRklist;

        auto process_key = get_process_key();
        try
        {
            const char *key_p = nullptr;
            if( !process_key.empty() && process_key.find_first_of('*') == std::string::npos )
                key_p= process_key.c_str();
            TProcess::pm->RecordLoadinProfile(key_p);
            process_old_selection = char_array_to_string( rt[RT_PROCES]->PackKey(), MAXRKEYLEN );
        }
        catch( TError& xcpt )
        {
            vfMessage(window(), xcpt.title, xcpt.mess);
        }
        for(int i=0; i<aObj[o_pestl]->GetN(); i++ )
            aRklist.push_back( aObj[o_pestl]->GetString( i, 0 ));
        sel_keys_model = new FixedKeyModel( this, RT_SYSEQ, aRklist );
    }
    else
    {
        sel_keys_model = new KeyModel( this, cRT );
    }
    sel_keys_model->setSelection(sel_old_selection);
    // Headers from nRT ????
    sel_keys_table->setModel(sel_keys_model);
    ui->checkFull->setChecked( sel_old_selection.empty() );
    ui->lineFilter->setText("*");
}


void GtDemoWizard::selChangeFilter()
{
    string keyFilter = ui->lineFilter->text().toStdString();
    string str_name = "Template for record key";
    KeyFilter dbFilter(this, keysRT, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->lineFilter->setText(keyFilter.c_str());
        sel_keys_table->setFilter( this, keyFilter.c_str());
    }
}

void GtDemoWizard::selSelectionChanged(int state)
{
    sel_keys_table->setFullSelection( this, state);
    ui->bSelectAll->setEnabled(state);
    ui->bClearAll->setEnabled(state);
    ui->bFilter->setEnabled(state&&keysRT<=RT_SYSEQ);
}

void GtDemoWizard::processChangeFilter()
{
    string keyFilter = ui->lineFilter->text().toStdString();
    string str_name = "Template for process record key";
    KeyFilter dbFilter(this, RT_PROCES, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->linePrFilter->setText(keyFilter.c_str());
        process_keys_table->setFilter( this, keyFilter.c_str());
    }
}

void GtDemoWizard::processSelectionChanged(int state)
{
    process_keys_table->setFullSelection( this, state);
    ui->bPrFilter->setEnabled(state);
}


//--------------------- End of GtDemoWizard.cpp ---------------------------
