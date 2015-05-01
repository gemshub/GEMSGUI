//-------------------------------------------------------------------
// $Id: SetFiltersDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of SetFiltersDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QLabel>

#include "SetFiltersDialog.h"
#include "GemsMainWindow.h"
#include "service.h"


SetFiltersDialog::SetFiltersDialog(QWidget* win,
           elmFilesConfData *elm_data,
           setFiltersData *set_data,
           const char * /*prfName*/,
           const char* /*caption*/):
       QDialog( win ),
        el_data(elm_data),
        data(set_data)
{
	
	setupUi(this);

    // functionaliti moved to Elements dialog Phase/DC filters
    cbPHcopyL->hide();
    cbPHcopyN->hide();
    cbPHcopyA->hide();
    cbPHcopyY->hide();


    el_data->changed = false;
    tvIClistP->setFont( pVisorImp->getCellFont() );
    tvIClistD->setFont( pVisorImp->getCellFont() );
    lvDefDBfiles->setFont( pVisorImp->getCellFont() );

    lvDefDBfiles->setSelectionMode(QAbstractItemView::MultiSelection);    //setMultiSelection(true);
    lvDefDBfiles->setSortingEnabled( false );                            //setSorting(-1);
    lvDefDBfiles->setRootIsDecorated(true);
    lvDefDBfiles->setAllColumnsShowFocus(true);
    

    QTreeWidgetItem* pdb = new QTreeWidgetItem( lvDefDBfiles );
    pdb->setText(0,"Database");
    pkern = new QTreeWidgetItem( pdb );
    pkern->setText(0, "Default" );
    
    pdb->setExpanded( true ); //setOpen(true);
    pkern->setExpanded( true ); //setOpen(true);

    QTreeWidgetItem* pMod;
    QTreeWidgetItem* pFile;
    
    uint ii=0;//el_data->flKeywds.GetCount()-1;

    for(int i=0; i< el_data->flCnt.size(); i++ )
    {
        pMod = new QTreeWidgetItem( pkern );
        pMod->setText(0, rt[i].GetKeywd());

       for(int jj=0; jj<el_data->flCnt[i]; jj++ )
       {
           pFile = new QTreeWidgetItem( pMod );
           pFile->setText(0, el_data->flKeywds[ii].c_str() );
           pFile->setText(1, el_data->flNames[ii].c_str()  );
           ii++;
       }
    }
   lvDefDBfiles->expandToDepth( 1);

    setData();

    // signals and slots connections
    connect( pbHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pbReset, SIGNAL( clicked() ), this, SLOT( CmReset() ) );
    connect( pbApply, SIGNAL( clicked() ), this, SLOT( CmApply() ) );
    connect( pbOk, SIGNAL( clicked() ), this, SLOT( CmOk() ) );
    connect( lvDefDBfiles, SIGNAL( itemSelectionChanged() ),
         this, SLOT( CmChangeFilesCfg() ) );
}

SetFiltersDialog::~SetFiltersDialog()
{}

void SetFiltersDialog::languageChange()
{
    retranslateUi(this);
}


void
SetFiltersDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_SETFILT_HTML );
}

void
SetFiltersDialog::CmChangeFilesCfg()
{
  el_data->changed = true;
}

void
SetFiltersDialog::CmReset()
{
    setData();
    update();
}


void
SetFiltersDialog::CmOk()
{
   CmApply();
   accept();
}


void
SetFiltersDialog::CmApply()
{
   getData();
}

void SetFiltersDialog::setData()
{
  // set IComp
    cbICsearch->setEditText( tr(data->ic_d.from_templ.c_str()));
    cbICreplace->setEditText( tr(data->ic_d.to_templ.c_str()));

    QString txt = tr("");
    for( uint ii=0; ii<data->ic_d.oldIComps.size(); ii++ )
    {
      txt += tr( data->ic_d.oldIComps[ii].c_str() );
      txt += tr("\n");
    }
    tvIClistP->setText( txt );
    txt = tr("");
    for( uint ii=0; ii<data->ic_d.newIComps.size(); ii++ )
    {
      txt += tr( data->ic_d.newIComps[ii].c_str() );
      txt += tr("\n");
    }
    tvIClistD->setText( txt );

  // set DComp
    cbDCsearch->setEditText( tr(data->dc_d.from_templ.c_str()));
    cbDCreplace->setEditText( tr(data->dc_d.to_templ.c_str()));

    mleDCfscript->setText( tr(data->dc_d.f_script.c_str()) );

  // set ReacDC
    cbREsearch->setEditText( tr(data->rd_d.from_templ.c_str()));
    cbREreplace->setEditText( tr(data->rd_d.to_templ.c_str()));

    mleREfscript->setText( tr(data->dc_d.f_script.c_str()) );

  // set Phases
    cbPHsearch->setEditText( tr(data->ph_d.from_templ.c_str()));
    cbPHreplace->setEditText( tr(data->ph_d.to_templ.c_str()));

    //cbPHcopyL->setChecked( data->ph_d.flags[PHcopyL_] );
    cbPHcopyF->setChecked( data->ph_d.flags[PHcopyF_] );
    cbPHcopyD->setChecked( data->ph_d.flags[PHcopyD_] );
    //cbPHcopyN->setChecked( data->ph_d.flags[PHcopyN_] );
    //cbPHcopyA->setChecked( data->ph_d.flags[PHcopyA_] );
    //cbPHcopyY->setChecked( data->ph_d.flags[PHcopyY_] );

  // set Compos
    cbPCOsearch->setEditText( tr(data->cm_d.from_templ.c_str()));
    cbPCOreplace->setEditText( tr(data->cm_d.to_templ.c_str()));

    mlePCOfscript->setText( tr(data->cm_d.f_script.c_str()) );

  // set files list
  //( RT_SDATA, RT_CONST, X, X, RT_ICOMP, RT_DCOMP, RT_COMPOS,
  //   RT_REACDC, X, RT_PHASE )
  setFiles();
  el_data->changed = false;

}


void SetFiltersDialog::getData()
{

    // get IComp
    data->ic_d.from_templ = (const char*)cbICsearch->currentText().toUtf8().data();
    data->ic_d.to_templ = (const char*)cbICreplace->currentText().toUtf8().data();
    // only show
    //tvIClistP->setText( txt );
    //tvIClistD->setText( txt );

  // set DComp
    data->dc_d.from_templ = (const char*)cbDCsearch->currentText().toUtf8().data();
    data->dc_d.to_templ = (const char*)cbDCreplace->currentText().toUtf8().data();
    data->dc_d.f_script = (const char*)mleDCfscript->toPlainText().toUtf8().data();

  // set ReacDC
    data->rd_d.from_templ = (const char*)cbREsearch->currentText().toUtf8().data();
    data->rd_d.to_templ = (const char*)cbREreplace->currentText().toUtf8().data();
    data->rd_d.f_script = (const char*)mleREfscript->toPlainText().toUtf8().data();

  // set Phases
    data->ph_d.from_templ = (const char*)cbPHsearch->currentText().toUtf8().data();
    data->ph_d.to_templ = (const char*)cbPHreplace->currentText().toUtf8().data();

    //data->ph_d.flags[PHcopyL_] = cbPHcopyL->isChecked();
    data->ph_d.flags[PHcopyF_] = cbPHcopyF->isChecked();
    data->ph_d.flags[PHcopyD_] = cbPHcopyD->isChecked();
    //data->ph_d.flags[PHcopyN_] = cbPHcopyN->isChecked();
    //data->ph_d.flags[PHcopyA_] = cbPHcopyA->isChecked();
    //data->ph_d.flags[PHcopyY_] = cbPHcopyY->isChecked();

   // set Compos
    data->cm_d.from_templ = (const char*)cbPCOsearch->currentText().toUtf8().data();
    data->cm_d.to_templ = (const char*)cbPCOreplace->currentText().toUtf8().data();
    data->cm_d.f_script = (const char*)mlePCOfscript->toPlainText().toUtf8().data();

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
         string col =pFile->text( 0 ).toUtf8().data();
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
    int npMod1 = pkern->childCount();
    QTreeWidgetItem* pMod;
    QTreeWidgetItem* pFile;

    for( ik=0; ik< npMod1; ik++ )
    {
      pMod = pkern->child(ik);
      nF = pMod->childCount();
      for( jk = 0; jk< nF; jk++ )
      {
        pFile = pMod->child( jk );
        string col =pFile->text( 0 ).toUtf8().data();
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

