//-------------------------------------------------------------------
// $Id$
//
// Implementation of SetFiltersDialog class
//
// Copyright (C) 2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
const char *GEMS_SFD_HTML = "setfilters";

#define Inherited SetFiltersDialogData


#include <qlabel.h>

#include "SetFiltersDialog.h"
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qtextview.h>
#include <qcheckbox.h>
#include <qlistview.h>

#include "visor_w.h"


SetFiltersDialog::SetFiltersDialog(QWidget* win,
           elmFilesConfData *elm_data,
           setFiltersData *set_data,
           const char * /*prfName*/,
           const char* /*caption*/):
        Inherited( win, 0, true /* false = modeless */ ),
        el_data(elm_data),
        data(set_data)
{
    el_data->changed = false;
    tvIClistP->setFont( pVisorImp->getCellFont() );
    tvIClistD->setFont( pVisorImp->getCellFont() );
    lvDefDBfiles->setFont( pVisorImp->getCellFont() );

    lvDefDBfiles->setMultiSelection(true);
    lvDefDBfiles->setSorting(-1);
    lvDefDBfiles->setRootIsDecorated(true);
    lvDefDBfiles->setAllColumnsShowFocus(true);

    QListViewItem* pdb = new QListViewItem( lvDefDBfiles, "Database" );
    pkern = new QListViewItem( pdb, "Default" );
    pdb->setOpen(true);
    pkern->setOpen(true);

    QListViewItem* pMod;
    QListViewItem* pFile;
    uint ii=el_data->flKeywds.GetCount()-1;

    for(int i=el_data->flCnt.GetCount()-1; i>=0; i--)
    {
       pMod = new QListViewItem( pkern, rt[i].GetKeywd() );

       for(int jj=0; jj<el_data->flCnt[i]; jj++ )
       {
          pFile = new QListViewItem( pMod,
              el_data->flKeywds[ii].c_str(), el_data->flNames[ii].c_str() );
          ii--;
       }
    }
    setData();

    // signals and slots connections
    connect( pbHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( pbReset, SIGNAL( clicked() ), this, SLOT( CmReset() ) );
    connect( pbApply, SIGNAL( clicked() ), this, SLOT( CmApply() ) );
    connect( pbOk, SIGNAL( clicked() ), this, SLOT( CmOk() ) );
    connect( lvDefDBfiles, SIGNAL( selectionChanged() ),
         this, SLOT( CmChangeFilesCfg() ) );
}

SetFiltersDialog::~SetFiltersDialog()
{}


void
SetFiltersDialog::CmHelp()
{
    pVisorImp->OpenHelp( GEMS_SFD_HTML, 0, this, true );
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
    for( uint ii=0; ii<data->ic_d.oldIComps.GetCount(); ii++ )
    {
      txt += tr( data->ic_d.oldIComps[ii].c_str() );
      txt += tr("\n");
    }
    tvIClistP->setText( txt );
    txt = tr("");
    for( uint ii=0; ii<data->ic_d.newIComps.GetCount(); ii++ )
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

    cbPHcopyL->setChecked( data->ph_d.flags[PHcopyL_] );
    cbPHcopyF->setChecked( data->ph_d.flags[PHcopyF_] );
    cbPHcopyD->setChecked( data->ph_d.flags[PHcopyD_] );
    cbPHcopyN->setChecked( data->ph_d.flags[PHcopyN_] );
    cbPHcopyA->setChecked( data->ph_d.flags[PHcopyA_] );
    cbPHcopyY->setChecked( data->ph_d.flags[PHcopyY_] );

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
    data->ic_d.from_templ = (const char*)cbICsearch->currentText();
    data->ic_d.to_templ = (const char*)cbICreplace->currentText();
    // only show
    //tvIClistP->setText( txt );
    //tvIClistD->setText( txt );

  // set DComp
    data->dc_d.from_templ = (const char*)cbDCsearch->currentText();
    data->dc_d.to_templ = (const char*)cbDCreplace->currentText();
    data->dc_d.f_script = (const char*)mleDCfscript->text();

  // set ReacDC
    data->rd_d.from_templ = (const char*)cbREsearch->currentText();
    data->rd_d.to_templ = (const char*)cbREreplace->currentText();
    data->rd_d.f_script = (const char*)mleREfscript->text();

  // set Phases
    data->ph_d.from_templ = (const char*)cbPHsearch->currentText();
    data->ph_d.to_templ = (const char*)cbPHreplace->currentText();

    data->ph_d.flags[PHcopyL_] = cbPHcopyL->isChecked();
    data->ph_d.flags[PHcopyF_] = cbPHcopyF->isChecked();
    data->ph_d.flags[PHcopyD_] = cbPHcopyD->isChecked();
    data->ph_d.flags[PHcopyN_] = cbPHcopyN->isChecked();
    data->ph_d.flags[PHcopyA_] = cbPHcopyA->isChecked();
    data->ph_d.flags[PHcopyY_] = cbPHcopyY->isChecked();

   // set Compos
    data->cm_d.from_templ = (const char*)cbPCOsearch->currentText();
    data->cm_d.to_templ = (const char*)cbPCOreplace->currentText();
    data->cm_d.f_script = (const char*)mlePCOfscript->text();

  // get files list (or reopen files)
  //( RT_SDATA, RT_CONST, X, X, RT_ICOMP, RT_DCOMP, RT_COMPOS,
  //   RT_REACDC, X, RT_PHASE )
  getFiles();

}

void SetFiltersDialog::getFiles()
{


    if( el_data->changed == false )
      return;

    uint cnt = 0;
    QListViewItem* pMod1 = pkern->firstChild();
    QListViewItem* pFile1;
    el_data->selKeywds.Clear();
    el_data->selCnt.Clear();

    while( pMod1  )
    {
      cnt = 0;
      pFile1 = pMod1->firstChild();
      while( pFile1 )
      {
       if( pFile1->isSelected() )
       {
         const char *col =pFile1->text( 0 );
         cnt++;
         el_data->selKeywds.Add( col );
       }
       pFile1 = pFile1->nextSibling();
      }
      el_data->selCnt.Add( cnt );
      pMod1 = pMod1->nextSibling();
    }
    el_data->changed = true;
}

void SetFiltersDialog::setFiles()
{
    // show selections
    bool isSel;
    int jj =0, ik =0;

    QListViewItem* pMod1 = pkern->firstChild();
    QListViewItem* pFile1;

    while( pMod1  )
    {
      pFile1 = pMod1->firstChild();
      while( pFile1 )
      {
        const char *col =pFile1->text( 0 );
        isSel = false;
        for( int ii=0; ii<el_data->selCnt[ik]; ii++ )
         if( el_data->selKeywds[jj+ii].find( col ) != gstring::npos )
         {
          isSel = true;
          break;
         }
        pFile1->setSelected( isSel );
        pFile1 = pFile1->nextSibling();
      }
      jj += el_data->selCnt[ik];
      ik++;
      pMod1 = pMod1->nextSibling();
    }
}

// --------------------- End SetFiltersDialog.cpp -------------------------

