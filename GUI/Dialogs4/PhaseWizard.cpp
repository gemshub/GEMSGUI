//-------------------------------------------------------------------
// $Id: PhaseWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of PhaseWizard class
//
// Copyright (C) 2005-2021  S.Dmytriyeva, D.Kulik
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

#include <QHeaderView>

#include "ui_PhaseWizard4.h"
#include "PhaseWizard.h"
#include "GemsMainWindow.h"
#include "KeyFilter.h"
#include "service.h"
#include "v_mod.h"


void PhaseWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void PhaseWizard::CmNext()
{
    int ndx = ui->stackedWidget->currentIndex();

    if( ndx == 0 && ndxInPhPs_0 != ui->pPhPs_0->currentIndex())
    {
        vfMessage(this, "PhaseWizard",
                  "Key of Phase record will be changed");
    }

    if( ndx == ui->stackedWidget->count() - 2)
    {
        auto rd_sel = rd_keys_table->allSelectedKeys();
        if( rd_sel.size() != old_nDC)
            ui->getDCC->setChecked(true);
    }

    ui->stackedWidget->setCurrentIndex ( ndx+1 );
    resetNextButton();
    resetBackButton();
}

void 	PhaseWizard::resetNextButton()
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

void 	PhaseWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}

PhaseWizard::PhaseWizard( const char* pkey, char flgs[37], int size[30],
double r2, const TCStringArray& rd_sel,
const TCStringArray& phase_sel, const TCStringArray& lDCr_sel,
QWidget* parent):
    QDialog( parent ),
    ui(new Ui::PhaseWizardData),
    rd_old_selection(rd_sel), old_nDC(rd_sel.size()),
    phase_old_selection(phase_sel), lDCr_old_selection(lDCr_sel)
{
    //setFinishEnabled( WizardPage3, true);
    ui->setupUi(this);
    string str1= "GEM-Selektor Phase Setup:  ";
    str1 += pkey;
    setWindowTitle( str1.c_str() );

    QObject::connect( ui->pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( ui->pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( ui->pBack, SIGNAL(clicked()), this, SLOT(CmBack()));
    QObject::connect( ui->pNext, SIGNAL(clicked()), this, SLOT(CmNext()));
    ui->stackedWidget->setCurrentIndex (0);
    resetNextButton();
    resetBackButton();

    //Page 1
    int ii;
    ii = ui->ModelCode->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->ModelCode->setCurrentIndex(ii);

    ii = ui->AC_modeCode->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->AC_modeCode->setCurrentIndex(ii);

    ii = ui->DcEqModeCode->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->DcEqModeCode->setCurrentIndex(ii);

    ii = ui->PhEqModeCode->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->PhEqModeCode->setCurrentIndex(ii);

    ii = ui->DcEqLinkMode->findText(QChar(flgs[4]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->DcEqLinkMode->setCurrentIndex(ii);

    ii = ui->SCM_code->findText(QChar(flgs[5]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->SCM_code->setCurrentIndex(ii);

    ii = ui->pPhPs_0->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPhPs_0->setCurrentIndex(ii);
    ndxInPhPs_0 = ui->pPhPs_0->currentIndex();

    //Page 2
    QString filter = "*";
    if( pkey )
        filter = QChar(pkey[0]) + QString( pkey[1] != ':' ? "*" : "") + QString(":*:*:*:");
    ui->checkRDFull->setChecked( rd_old_selection.empty() );
    ui->lineRDFilter->setText(filter);
    rd_keys_model = new RDKeyModel( this, filter.toUtf8().data()  );
    rd_keys_model->setSelection(rd_old_selection);
    QStringList headers_list;
    headers_list <<  "Source" <<  "Aggr.State" <<  "Group" <<
                     "Name" <<   "Comment";
    rd_keys_model->setHeaders(headers_list);
    rd_keys_table = new KeysTableProxy(ui->page_6);
    rd_keys_table->setModel(rd_keys_model);
    //ui->gridLayout->addWidget(rd_keys_table, 5, 0, 1, 5);
    ui->gridLayout_8->addWidget(rd_keys_table, 4, 0, 1, 4);


    rdSelectionChanged( ui->checkRDFull->isChecked() );
    QObject::connect( ui->bRDSelectAll, SIGNAL(clicked()), rd_keys_table, SLOT(selectAll()));
    QObject::connect( ui->bRDClearAll, SIGNAL(clicked()), rd_keys_table, SLOT(clearSelection()));
    QObject::connect( ui->bRDFilter, SIGNAL(clicked()), this, SLOT(rdChangeFilter()));
    QObject::connect( ui->checkRDFull, SIGNAL(stateChanged(int)), this, SLOT(rdSelectionChanged(int)));

    //Page 3
    ui->pnscM->setValue(size[3]);
    ui->pnpxM->setValue(size[4]);
    ui->pNcpN->setValue(size[1]);
    ui->pNcpM->setValue(size[2]);

    ui->ndqf->setValue(size[8]);
    ui->nrcp->setValue(size[9]);
    ui->pncsolv->setValue(size[22]);
    ui->pcsolv->setValue(size[23]);
    ui->pncdicl->setValue(size[24]);
    ui->pndicl->setValue(size[25]);
    ui->pndh->setValue(size[26]);

    // page 4
    ii = ui->kin_t0->findText(QChar(flgs[29]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t0->setCurrentIndex(ii);
    ii = ui->kin_t1->findText(QChar(flgs[30]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t1->setCurrentIndex(ii);

    ui->nEIl->setValue(size[14]);
    ui->nEIp->setValue(size[15]);
    ui->nCDc->setValue(size[16]);
    ui->nSiT->setValue(size[29]);
    ui->nIsoC->setValue(size[18]);
    ui->nIsoS->setValue(size[19]);
    ui->mDe->setValue(size[20]);

    //    if( flgs[5] != 'N' )
    //    {
    //      checkBoxSorption->setChecked( true );
    //      if( size[5] == 0 ) size[5] = 1;
    //      if( r2 == 0. ) r2 = 1.;
    //    }
    //    else checkBoxSorption->setChecked( false );

    if( flgs[11] == '+' )
        ui->checkBoxSorption->setChecked(true);
    else
        ui->checkBoxSorption->setChecked(false);

    ui->spinBox_SurTypes_N->setValue(size[5]);
    QString str;
    ui->lineEdit_surfArea->setValidator( new QDoubleValidator( ui->lineEdit_surfArea ));
    ui->lineEdit_surfArea->setText( str.setNum(r2) );

    // Page 5
    if( flgs[28] == '+' )
        ui->PapCon->setChecked(true);
    else
        ui->PapCon->setChecked(false);

    ui->checkDCrFull->setChecked( lDCr_old_selection.empty() );
    ui->lineDCrFilter->setText(filter);
    lDCr_keys_model = new RDKeyModel( this, filter.toStdString().c_str() );
    lDCr_keys_model->setSelection(lDCr_old_selection);
    headers_list.clear();
    headers_list <<  "Source" <<  "Aggr.State" <<  "Group" <<
                     "Name" <<   "Comment";
    lDCr_keys_model->setHeaders(headers_list);
    lDCr_keys_table = new KeysTableProxy(ui->ldcrSelectBox);
    lDCr_keys_table->setModel(lDCr_keys_model);
    ui->gridLayout_10->addWidget(lDCr_keys_table, 2, 0, 1, 4);


    lDCrSelectionChanged( ui->checkDCrFull->isChecked() );
    ui->ldcrSelectBox->setEnabled(ui->PapCon->isChecked());
    QObject::connect( ui->bDCrSelectAll, SIGNAL(clicked()), lDCr_keys_table, SLOT(selectAll()));
    QObject::connect( ui->bDCrClearAll, SIGNAL(clicked()), lDCr_keys_table, SLOT(clearSelection()));
    QObject::connect( ui->bDCrFilter, SIGNAL(clicked()), this, SLOT(lDCrChangeFilter()));
    QObject::connect( ui->checkDCrFull, SIGNAL(stateChanged(int)), this, SLOT(lDCrSelectionChanged(int)));
    QObject::connect( ui->PapCon, &QCheckBox::stateChanged, [&](int state) {  ui->ldcrSelectBox->setEnabled(state); } );

    // page 6
    ii = ui->kin_t2->findText(QChar(flgs[31]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t2->setCurrentIndex(ii);
    ii = ui->kin_t3->findText(QChar(flgs[32]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t3->setCurrentIndex(ii);
    ii = ui->kin_t4->findText(QChar(flgs[33]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t4->setCurrentIndex(ii);
    ii = ui->kin_t5->findText(QChar(flgs[34]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t5->setCurrentIndex(ii);
    ii = ui->kin_t6->findText(QChar(flgs[35]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t6->setCurrentIndex(ii);
    ii = ui->kin_t7->findText(QChar(flgs[36]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->kin_t7->setCurrentIndex(ii);

    ui->nPRk->setValue(size[10]);
    ui->nSkr->setValue(size[11]);
    ui->nrpC->setValue(size[12]);
    ui->numpC->setValue(size[13]);
    ui->pnaptC->setValue(size[27]);
    ui->pnAscC->setValue(size[28]);

    //Page 7
    if( flgs[15] == '+' )
        ui->PlPhl->setChecked(true);
    else
        ui->PlPhl->setChecked(false);

    ui->checkPhFull->setChecked( phase_old_selection.empty() );
    ui->linePhFilter->setText("*");
    phase_keys_model = new KeyModel( this, RT_PHASE );
    phase_keys_model->setSelection(phase_old_selection);
    headers_list.clear();
    headers_list <<  "Aggr.State" <<  "Group" <<
                     "Name" <<  "Type" <<   "Comment";
    phase_keys_model->setHeaders(headers_list);
    phase_keys_table = new KeysTableProxy(ui->phaseSelectBox);
    phase_keys_table->setModel(phase_keys_model);
    ui->gridLayout_9->addWidget(phase_keys_table, 2, 0, 1, 4);

    phaseSelectionChanged( ui->checkPhFull->isChecked() );
    ui->phaseSelectBox->setEnabled(ui->PlPhl->isChecked());
    QObject::connect( ui->bPhSelectAll, SIGNAL(clicked()), phase_keys_table, SLOT(selectAll()));
    QObject::connect( ui->bPhClearAll, SIGNAL(clicked()), phase_keys_table, SLOT(clearSelection()));
    QObject::connect( ui->bPhFilter, SIGNAL(clicked()), this, SLOT(phaseChangeFilter()));
    QObject::connect( ui->checkPhFull, SIGNAL(stateChanged(int)), this, SLOT(phaseSelectionChanged(int)));
    QObject::connect( ui->PlPhl, &QCheckBox::stateChanged, [&](int state) {  ui->phaseSelectBox->setEnabled(state); } );

    //Page 8

    ui->nlPh->setValue(size[6]);
    ui->nlPc->setValue(size[7]);
    ui->spinBoxSDrefLnk->setValue(size[0]);

    if( flgs[12] == '+' )
        ui->getDCC->setChecked(true);
    else
        ui->getDCC->setChecked(false);
}


PhaseWizard::~PhaseWizard()
{
    delete  rd_keys_table;
    delete  phase_keys_table;
    delete  lDCr_keys_table;
    delete ui;
}

void   PhaseWizard::getSizes( int size[30], TCStringArray& rd_sel,
TCStringArray& phase_sel, TCStringArray& lDCr_sel )
{
    rd_sel = rd_keys_table->allSelectedKeys();
    phase_sel.clear();
    if( ui->PlPhl->isChecked())
        phase_sel = phase_keys_table->allSelectedKeys();
    lDCr_sel.clear();
    if( ui->PapCon->isChecked())
        lDCr_sel = lDCr_keys_table->allSelectedKeys();

    size[0]= ui->spinBoxSDrefLnk->value();
    size[1]= ui->pNcpN->value();
    size[2]= ui->pNcpM->value();
    size[3]= ui->pnscM->value();
    size[4]= ui->pnpxM->value();
    size[5]= ui->spinBox_SurTypes_N->value();

    //    if( ui->SCM_code->currentText()[0] != QChar('N') )
    //      if( size[5] == 0 ) size[5] = 1;

    size[6]= phase_sel.size(); //nlPh->value();
    size[14]= ui->nEIl->value();
    size[7]= ui->nlPc->value();
    size[15]= ui->nEIp->value();
    size[8]= ui->ndqf->value();
    size[16]= ui->nCDc->value();
    size[9]= ui->nrcp->value();
    size[10]= ui->nPRk->value();
    size[19]= ui->nIsoS->value();
    size[12]= ui->nrpC->value();
    size[20]= ui->mDe->value();
    size[13]= ui->numpC->value();
    size[18]= ui->nIsoC->value();
    size[11]= lDCr_sel.size(); //nSkr->value();

    size[22]= ui->pncsolv->value();
    size[23]= ui->pcsolv->value();
    size[24]= ui->pncdicl->value();
    size[25]= ui->pndicl->value();
    size[26]= ui->pndh->value();
    size[29]= ui->nSiT->value();
    size[27]= ui->pnaptC->value();
    size[28]= ui->pnAscC->value();

    // 17, 21 reserved
}

double   PhaseWizard::getR2()
{
    double r=ui->lineEdit_surfArea->text().toDouble();
    QString str = ui->SCM_code->currentText();
    if( str[0] != QChar('N') )
        if( approximatelyZero(r) ) r = 1.;
    return r;
}


void PhaseWizard::getFlags( char flgs[37] )
{
    QString str;

    str = ui->ModelCode->currentText();
    flgs[0] = str[0].toLatin1();
    str = ui->AC_modeCode->currentText();
    flgs[3] = str[0].toLatin1();
    str = ui->DcEqModeCode->currentText();
    flgs[1] = str[0].toLatin1();
    str = ui->PhEqModeCode->currentText();
    flgs[2] = str[0].toLatin1();
    str = ui->DcEqLinkMode->currentText();
    flgs[4] = str[0].toLatin1();
    str = ui->SCM_code->currentText();
    flgs[5] = str[0].toLatin1();
    // flgs[6] getting from key
    str = ui->pPhPs_0->currentText();
    flgs[6] = str[0].toLatin1();

    if( ui->pNcpN->value() > 0 &&
            ui->pNcpM->value() > 0 )
        flgs[7] = '+';
    else flgs[7] = '-';

    if( ui->pnscM->value() > 0 /*&&
                      pnpxM->value() > 0*/ )
        flgs[8] = '+';
    else flgs[8] = '-';

    //  if( checkBoxSorption->isChecked() )
    //       flgs[11] = '+';
    //  else flgs[11] = '-';
    //if( flgs[5] != 'N' )
    //     flgs[11] = '+';
    if( flgs[0] == 'A' )
        flgs[11] = '+';
    else flgs[11] = '-';

    // Flags for scripts
    flgs[9] = '-';
    flgs[10] = '-';
    switch( flgs[3] )
    {
    case 'I':
    case 'S':
    default:  break;
    case 'N':  if( flgs[2] != 'N')
            flgs[9] = '+';
        if( flgs[1] != 'N')
            flgs[10] = '+';
        break;
    }
    // testing task
    if( flgs[10] != '+' )
        flgs[4] = 'N';

    if( ui->getDCC->isChecked())
        flgs[12] = '+';
    else
        flgs[12] = '-';

    str = ui->kin_t0->currentText();
    flgs[29] = str[0].toLatin1();
    str = ui->kin_t1->currentText();
    flgs[30] = str[0].toLatin1();
    str = ui->kin_t2->currentText();
    flgs[31] = str[0].toLatin1();
    str = ui->kin_t3->currentText();
    flgs[32] = str[0].toLatin1();
    str = ui->kin_t4->currentText();
    flgs[33] = str[0].toLatin1();
    str = ui->kin_t5->currentText();
    flgs[34] = str[0].toLatin1();
    str = ui->kin_t6->currentText();
    flgs[35] = str[0].toLatin1();
    str = ui->kin_t7->currentText();
    flgs[36] = str[0].toLatin1();

    if( ui->ndqf->value() > 0  )
        flgs[13] = '+';
    else flgs[13] = '-';
    if( ui->nrcp->value() > 0  )
        flgs[14] = '+';
    else flgs[14] = '-';
    if( ui->PlPhl->isChecked())
        flgs[15] = '+';
    else
        flgs[15] = '-';
    //  if( nlPh->value() > 0 )
    //      flgs[15] = '+';
    //   else flgs[15] = '-';
    if( ui->nEIl->value() > 0 )
        flgs[20] = '+';
    else flgs[20] = '-';
    if( ui->nCDc->value() > 0 )
        flgs[21] = '+';
    else flgs[21] = '-';
    if( ui->nIsoC->value() > 0 )
        flgs[22] = '+';
    else flgs[22] = '-';
    if( ui->nIsoS->value() > 0 )
        flgs[23] = '+';
    else flgs[23] = '-';
    if( ui->nPRk->value() > 0 ) // && nSkr->value() > 0 )
        flgs[26] = '+';
    else flgs[26] = '-';
    if( ui->numpC->value() > 0 )
        flgs[27] = '+';
    else flgs[27] = '-';

    //---------------------------------------------------
    if( ui->pncsolv->value() > 0 && ui->pcsolv->value() > 0 )
        flgs[16] = '+';
    else flgs[16] = '-';
    if( ui->pndicl->value() > 0 && ui->pncdicl->value() > 0 )
        flgs[17] = '+';
    else flgs[17] = '-';
    if( ui->pndh->value() > 0  )
        flgs[18] = '+';
    else flgs[18] = '-';
    if( ui->nSiT->value() > 0  )
        flgs[24] = '+';
    else flgs[24] = '-';

    if( ui->PapCon->isChecked())
        flgs[28] = '+';
    else
        flgs[28] = '-';
    //  if( pnaptC->value() > 0  )
    //      flgs[28] = '+';
    //   else flgs[28] = '-';
    // 19, 25, 36 reserved

}

void PhaseWizard::help()
{
    pVisorImp->OpenHelp( GM_PHASE_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

void PhaseWizard::rdChangeFilter()
{
    string keyFilter = ui->lineRDFilter->text().toStdString();
    string str_name = "Template for ";
    str_name +=  rt[RT_REACDC]->GetKeywd();
    str_name +=  "&";
    str_name +=  rt[RT_DCOMP]->GetKeywd();
    str_name += " record key";
    KeyFilter dbFilter(this, RT_DCOMP, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->lineRDFilter->setText(keyFilter.c_str());
        rd_keys_table->setFilter( this, keyFilter.c_str());
    }
}

void PhaseWizard::rdSelectionChanged(int state)
{
    rd_keys_table->setFullSelection( this, state);
    ui->bRDSelectAll->setEnabled(state);
    ui->bRDClearAll->setEnabled(state);
    //ui->bRDFilter->setEnabled(state);
}

void PhaseWizard::phaseChangeFilter()
{
    string keyFilter = ui->linePhFilter->text().toStdString();
    string str_name = "Template for Phase record key";
    KeyFilter dbFilter(this, RT_PHASE, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->linePhFilter->setText(keyFilter.c_str());
        phase_keys_table->setFilter( this, keyFilter.c_str());
    }
}

void PhaseWizard::phaseSelectionChanged(int state)
{
    phase_keys_table->setFullSelection( this, state);
    ui->bPhSelectAll->setEnabled(state);
    ui->bPhClearAll->setEnabled(state);
    ui->bPhFilter->setEnabled(state);
}

void PhaseWizard::lDCrChangeFilter()
{
    string keyFilter = ui->lineDCrFilter->text().toStdString();
    string str_name = "Template for ";
    str_name +=  rt[RT_REACDC]->GetKeywd();
    str_name +=  "&";
    str_name +=  rt[RT_DCOMP]->GetKeywd();
    str_name += " record key";
    KeyFilter dbFilter(this, RT_DCOMP, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->lineDCrFilter->setText(keyFilter.c_str());
        lDCr_keys_table->setFilter( this, keyFilter.c_str());
    }
}

void PhaseWizard::lDCrSelectionChanged(int state)
{
    lDCr_keys_table->setFullSelection( this, state);
    ui->bDCrSelectAll->setEnabled(state);
    ui->bDCrClearAll->setEnabled(state);
    //ui->bDCrFilter->setEnabled(state);
}


//--------------------- End of PhaseWizard.cpp ---------------------------
