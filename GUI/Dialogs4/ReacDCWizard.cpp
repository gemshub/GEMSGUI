//-------------------------------------------------------------------
// $Id: ReacDCWizard.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of ReacDCWizard class
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

#include <QHeaderView>

#include "ui_ReacDCWizard4.h"
#include "ReacDCWizard.h"
#include "GemsMainWindow.h"
#include "KeyFilter.h"
#include "service.h"
#include "v_mod.h"


void ReacDCWizard::CmBack()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()-1 );
    resetNextButton();
    resetBackButton();
}

void ReacDCWizard::CmNext()
{
    ui->stackedWidget->setCurrentIndex ( ui->stackedWidget->currentIndex()+1 );
    resetNextButton();
    resetBackButton();
}

void ReacDCWizard::resetNextButton()
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

void 	ReacDCWizard::resetBackButton()
{
    ui->pBack->setEnabled( ui->stackedWidget->currentIndex() > 0 );
}


ReacDCWizard::ReacDCWizard( const char* pkey, char flgs[12], int size[4],
const TCStringArray& sel, QWidget* parent):
    QDialog( parent ),
    ui(new Ui::ReacDCWizardData), old_selection(sel)
{
    ui->setupUi(this);
    string str1= "GEM-Selektor ReacDC Setup:  ";
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
    ii = ui->p_pct_1->findText(QChar(flgs[0]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_1->setCurrentIndex(ii);
    ii = ui->p_pct_2->findText(QChar(flgs[1]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_2->setCurrentIndex(ii);
    ii = ui->p_pct_3->findText(QChar(flgs[2]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_3->setCurrentIndex(ii);
    ii = ui->p_pct_4->findText(QChar(flgs[3]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->p_pct_4->setCurrentIndex(ii);
    ii = ui->pPreC->findText(QChar(flgs[6]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPreC->setCurrentIndex(ii);

    //Page 2
    ui->checkFull->setChecked( old_selection.empty() );
    ui->lineFilter->setText("*");
    keys_model = new RDKeyModel( this, old_selection  );
    QStringList headers_list;
    headers_list <<  "Source" <<  "Aggr.State" <<  "Group" <<
                     "Name" <<   "Comment";
    keys_model->setHeaders(headers_list);
    tableKeys = new KeysTableProxy(ui->page_3);
    tableKeys->setModel(keys_model);
    ui->gridLayout->addWidget(tableKeys, 5, 0, 1, 5);

    selectionChanged( ui->checkFull->isChecked() );
    QObject::connect( ui->bSelectAll, SIGNAL(clicked()), tableKeys, SLOT(selectAll()));
    QObject::connect( ui->bClearAll, SIGNAL(clicked()), tableKeys, SLOT(clearSelection()));
    QObject::connect( ui->bFilter, SIGNAL(clicked()), this, SLOT(ChangeFilter()));
    QObject::connect( ui->checkFull, SIGNAL(stateChanged(int)), this, SLOT(selectionChanged(int)));

    //Page 3
    ui->pnDC->setValue(size[0]);
    ui->pnTp->setValue(size[1]);
    ui->pnPp->setValue(size[2]);

    ii = ui->pPunE->findText(QChar(flgs[7]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunE->setCurrentIndex(ii);

    ii = ui->pPunV->findText(QChar(flgs[8]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunV->setCurrentIndex(ii);

    ii = ui->pPunP->findText(QChar(flgs[9]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunP->setCurrentIndex(ii);
    ii = ui->pPunT->findText(QChar(flgs[10]), Qt::MatchStartsWith|Qt::MatchCaseSensitive);
    if( ii >= 0  )
        ui->pPunT->setCurrentIndex(ii);

    //Page4
    ui->spinBoxSDrefLnk->setValue(size[3]);
}

ReacDCWizard::~ReacDCWizard()
{
    delete tableKeys;
    delete ui;
}

void   ReacDCWizard::getSizes( int size[4] )
{
    size[0]= ui->pnDC->value();
    size[1]= ui->pnTp->value();
    size[2]= ui->pnPp->value();
    size[3]= ui->spinBoxSDrefLnk->value();
}

void ReacDCWizard::getFlags( char flgs[12] )
{
    QString str;

    str = ui->p_pct_1->currentText();
    flgs[0] = str[0].toLatin1();
    str = ui->p_pct_2->currentText();
    flgs[1] = str[0].toLatin1();
    str = ui->p_pct_3->currentText();
    flgs[2] = str[0].toLatin1();
    str = ui->p_pct_4->currentText();
    flgs[3] = str[0].toLatin1();
    // 3, 4, 5, reserved
    str = ui->pPreC->currentText();
    flgs[6] = str[0].toLatin1();
    // 7,8,9,10,11 not changed
}

TCStringArray ReacDCWizard::allSelectedKeys() const
{
    return tableKeys->allSelectedKeys();
}

void ReacDCWizard::help()
{
    pVisorImp->OpenHelp( GM_REACDC_WZ_HTML, WZSTEP, ui->stackedWidget->currentIndex()+1 );
}

void ReacDCWizard::ChangeFilter()
{
    string keyFilter = ui->lineFilter->text().toStdString();
    string str_name = "Template for ";
    str_name +=  rt[RT_REACDC]->GetKeywd();
    str_name +=  "&";
    str_name +=  rt[RT_DCOMP]->GetKeywd();
    str_name += " record key";
    KeyFilter dbFilter(this, RT_DCOMP, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->lineFilter->setText(keyFilter.c_str());
        tableKeys->setFilter( this, keyFilter.c_str());
    }
}

void ReacDCWizard::selectionChanged(int state)
{
    tableKeys->setFullSelection( this, state);
    ui->bSelectAll->setEnabled(state);
    ui->bClearAll->setEnabled(state);
    ui->bFilter->setEnabled(state);
}

//--------------------- End of ReacDCWizard.cpp ---------------------------
