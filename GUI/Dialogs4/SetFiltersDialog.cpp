//-------------------------------------------------------------------
// $Id: SetFiltersDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SetFiltersDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
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

#include "ui_SetFiltersDialog4.h"
#include "SetFiltersDialog.h"
#include "GemsMainWindow.h"
#include "service.h"


SetFiltersDialog::SetFiltersDialog(QWidget* win,
                                   elmFilesConfData *elm_data,
                                   setFiltersData *set_data,
                                   const char * /*prfName*/,
                                   const char* /*caption*/):
    QDialog( win ),
    ui(new Ui::SetFiltersDialogData),
    el_data(elm_data),
    data(set_data)
{
    ui->setupUi(this);

    // functionaliti moved to Elements dialog Phase/DC filters
    ui->cbPHcopyL->hide();
    ui->cbPHcopyN->hide();
    ui->cbPHcopyA->hide();
    ui->cbPHcopyY->hide();

    el_data->changed = false;
    ui->tvIClistP->setFont( pVisorImp->getCellFont() );
    ui->tvIClistD->setFont( pVisorImp->getCellFont() );
    ui->lvDefDBfiles->setFont( pVisorImp->getCellFont() );

    ui->lvDefDBfiles->setSelectionMode(QAbstractItemView::MultiSelection);    //setMultiSelection(true);
    ui->lvDefDBfiles->setSortingEnabled( false );                            //setSorting(-1);
    ui->lvDefDBfiles->setRootIsDecorated(true);
    ui->lvDefDBfiles->setAllColumnsShowFocus(true);
    

    QTreeWidgetItem* pdb = new QTreeWidgetItem( ui->lvDefDBfiles );
    pdb->setText(0,"Database");
    pkern = new QTreeWidgetItem( pdb );
    pkern->setText(0, "Default" );
    
    pdb->setExpanded( true ); //setOpen(true);
    pkern->setExpanded( true ); //setOpen(true);

    QTreeWidgetItem* pMod;
    QTreeWidgetItem* pFile;
    
    uint ii=0;//el_data->flKeywds.GetCount()-1;

    for(size_t i=0; i< el_data->flCnt.size(); i++ )
    {
        pMod = new QTreeWidgetItem( pkern );
        pMod->setText(0, rt[i]->GetKeywd());

        for(int jj=0; jj<el_data->flCnt[i]; jj++ )
        {
            pFile = new QTreeWidgetItem( pMod );
            pFile->setText(0, el_data->flKeywds[ii].c_str() );
            pFile->setText(1, el_data->flNames[ii].c_str()  );
            ii++;
        }
    }
    ui->lvDefDBfiles->expandToDepth( 1);

    setData();

    // signals and slots connections
    connect( ui->pbHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( ui->pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( ui->pbReset, SIGNAL( clicked() ), this, SLOT( CmReset() ) );
    connect( ui->pbApply, SIGNAL( clicked() ), this, SLOT( CmApply() ) );
    connect( ui->pbOk, SIGNAL( clicked() ), this, SLOT( CmOk() ) );
    connect( ui->lvDefDBfiles, SIGNAL( itemSelectionChanged() ),
             this, SLOT( CmChangeFilesCfg() ) );
}

SetFiltersDialog::~SetFiltersDialog()
{
    delete ui;
}

void SetFiltersDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_SETFILT_HTML );
}

void SetFiltersDialog::CmChangeFilesCfg()
{
    el_data->changed = true;
}

void SetFiltersDialog::CmReset()
{
    setData();
    update();
}


void SetFiltersDialog::CmOk()
{
    CmApply();
    accept();
}


void SetFiltersDialog::CmApply()
{
    getData();
}

void SetFiltersDialog::setData()
{
    // set IComp
    ui->cbICsearch->setEditText( tr(data->ic_d.from_templ.c_str()));
    ui->cbICreplace->setEditText( tr(data->ic_d.to_templ.c_str()));

    QString txt = tr("");
    for( size_t ii=0; ii<data->ic_d.oldIComps.size(); ii++ )
    {
        txt += tr( data->ic_d.oldIComps[ii].c_str() );
        txt += tr("\n");
    }
    ui->tvIClistP->setText( txt );
    txt = tr("");
    for( size_t ii=0; ii<data->ic_d.newIComps.size(); ii++ )
    {
        txt += tr( data->ic_d.newIComps[ii].c_str() );
        txt += tr("\n");
    }
    ui->tvIClistD->setText( txt );

    // set DComp
    ui->cbDCsearch->setEditText( tr(data->dc_d.from_templ.c_str()));
    ui->cbDCreplace->setEditText( tr(data->dc_d.to_templ.c_str()));

    ui->mleDCfscript->setText( tr(data->dc_d.f_script.c_str()) );

    // set ReacDC
    ui->cbREsearch->setEditText( tr(data->rd_d.from_templ.c_str()));
    ui->cbREreplace->setEditText( tr(data->rd_d.to_templ.c_str()));

    ui->mleREfscript->setText( tr(data->dc_d.f_script.c_str()) );

    // set Phases
    ui->cbPHsearch->setEditText( tr(data->ph_d.from_templ.c_str()));
    ui->cbPHreplace->setEditText( tr(data->ph_d.to_templ.c_str()));

    //ui->cbPHcopyL->setChecked( data->ph_d.flags[PHcopyL_] );
    ui->cbPHcopyF->setChecked( data->ph_d.flags[PHcopyF_] );
    ui->cbPHcopyD->setChecked( data->ph_d.flags[PHcopyD_] );
    //ui->cbPHcopyN->setChecked( data->ph_d.flags[PHcopyN_] );
    //ui->cbPHcopyA->setChecked( data->ph_d.flags[PHcopyA_] );
    //ui->cbPHcopyY->setChecked( data->ph_d.flags[PHcopyY_] );

    // set Compos
    ui->cbPCOsearch->setEditText( tr(data->cm_d.from_templ.c_str()));
    ui->cbPCOreplace->setEditText( tr(data->cm_d.to_templ.c_str()));

    ui->mlePCOfscript->setText( tr(data->cm_d.f_script.c_str()) );

    // set files list
    //( RT_SDATA, RT_CONST, X, X, RT_ICOMP, RT_DCOMP, RT_COMPOS,
    //   RT_REACDC, X, RT_PHASE )
    setFiles();
    el_data->changed = false;

}


void SetFiltersDialog::getData()
{
    // get IComp
    data->ic_d.from_templ = ui->cbICsearch->currentText().toStdString();
    data->ic_d.to_templ = ui->cbICreplace->currentText().toStdString();
    // only show
    //ui->tvIClistP->setText( txt );
    //ui->tvIClistD->setText( txt );

    // set DComp
    data->dc_d.from_templ = ui->cbDCsearch->currentText().toStdString();
    data->dc_d.to_templ = ui->cbDCreplace->currentText().toStdString();
    data->dc_d.f_script = ui->mleDCfscript->toPlainText().toStdString();

    // set ReacDC
    data->rd_d.from_templ = ui->cbREsearch->currentText().toStdString();
    data->rd_d.to_templ = ui->cbREreplace->currentText().toStdString();
    data->rd_d.f_script = ui->mleREfscript->toPlainText().toStdString();

    // set Phases
    data->ph_d.from_templ = ui->cbPHsearch->currentText().toStdString();
    data->ph_d.to_templ = ui->cbPHreplace->currentText().toStdString();

    //data->ph_d.flags[PHcopyL_] = ui->cbPHcopyL->isChecked();
    data->ph_d.flags[PHcopyF_] = ui->cbPHcopyF->isChecked();
    data->ph_d.flags[PHcopyD_] = ui->cbPHcopyD->isChecked();
    //data->ph_d.flags[PHcopyN_] = ui->cbPHcopyN->isChecked();
    //data->ph_d.flags[PHcopyA_] = ui->cbPHcopyA->isChecked();
    //data->ph_d.flags[PHcopyY_] = ui->cbPHcopyY->isChecked();

    // set Compos
    data->cm_d.from_templ = ui->cbPCOsearch->currentText().toStdString();
    data->cm_d.to_templ = ui->cbPCOreplace->currentText().toStdString();
    data->cm_d.f_script = ui->mlePCOfscript->toPlainText().toStdString();

    // get files list (or reopen files)
    //( RT_SDATA, RT_CONST, X, X, RT_ICOMP, RT_DCOMP, RT_COMPOS,
    //   RT_REACDC, X, RT_PHASE )
    getFiles();

}

void SetFiltersDialog::getFiles()
{
    if( el_data->changed == false )
        return;

    int ii, jj, nF, cnt = 0;
    int npMod1 = pkern->childCount();
    QTreeWidgetItem* pMod;
    QTreeWidgetItem* pFile;
    
    el_data->selKeywds.clear();
    el_data->selCnt.clear();

    for( ii=0; ii< npMod1; ii++ )
    {
        cnt = 0;

        pMod = pkern->child(ii);
        nF = pMod->childCount();
        for( jj = 0; jj< nF; jj++ )
        {
            pFile = pMod->child( jj );
            if( pFile->isSelected() )
            {
                string col =pFile->text( 0 ).toStdString();
                cnt++;
                el_data->selKeywds.push_back( col );
            }
        }
        el_data->selCnt.push_back( cnt );
    }

    el_data->changed = true;
}

void SetFiltersDialog::setFiles()
{
    // show selections
    bool isSel;
    int jj =0, ik;

    int ii, jk, nF;
    auto npMod1 = pkern->childCount();
    QTreeWidgetItem* pMod;
    QTreeWidgetItem* pFile;

    for( ik=0; ik< npMod1; ik++ )
    {
        pMod = pkern->child(ik);
        nF = pMod->childCount();
        for( jk = 0; jk< nF; jk++ )
        {
            pFile = pMod->child( jk );
            string col =pFile->text( 0 ).toStdString();
            isSel = false;
            for(  ii=0; ii<el_data->selCnt[ik]; ii++ )
                if( el_data->selKeywds[jj+ii].find( col ) != string::npos )
                {
                    isSel = true;
                    break;
                }
            pFile->setSelected( isSel );
        }
        jj += el_data->selCnt[ik];
    }

}

// --------------------- End SetFiltersDialog.cpp -------------------------

