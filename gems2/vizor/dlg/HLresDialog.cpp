//-------------------------------------------------------------------
// $Id$
//
// Implementation of HLresDialog class
//
// Copyright (C) 1996-2001    S.Dmytriyeva
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

const char *GEMS_LRES_HTML = "ge_howto";

#include "HLresDialog.h"
#include "v_object.h"
#include "visor_w.h"
#include "v_mod.h"
#include "service.h"

#include <stdio.h>
#include <qlistview.h>
#include <qpushbutton.h>

HLresDialog* HLresDialog::pDia = 0;

HLresDialog::HLresDialog( QWidget* parent,  const char* name )
    : HLresDialogData( parent, name )
{
    pDia = this;
    ListView1->setSorting(-1);
    // ListView1->setRootIsDecorated(true);
    ListView1->setAllColumnsShowFocus(true);

    loadList();
}

HLresDialog::~HLresDialog()
{
}

void
HLresDialog::closeEvent(QCloseEvent* ev)
{
    pDia = 0;
    QWidget::closeEvent(ev);
}


void HLresDialog::CmClose()
{
  close();
}

void HLresDialog::CmHelp()
{
   pVisorImp->OpenHelp( GEMS_LRES_HTML );
}

void HLresDialog::CmPrint()
{
    // open file to output
 gstring filename;
 if( vfChooseFileSave(this, filename,
    "Put file name for printing" ) == false )
            return;

  fstream f(filename.c_str(), ios::out);
  ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

  printList(f);
}

void HLresDialog::CmRefresh()
{
   loadList();
   ListView1->update();
}


void HLresDialog::loadList()
{

   ListView1->clear();
   item = new QListViewItem( ListView1, tr( "System" ) );
   QListViewItem * phase;
   QListViewItem * elem;
   short *l1_    = (short *)aObj[ o_wi_l1 ].GetPtr();
   vstr buf(20);
   int jj, je;
    jj = je = aObj[o_wd_sm].GetN()-1;


   for( int ii=aObj[o_wd_sf].GetN()-1; ii>=0 ; ii--)
   {
    sprintf( buf, " %d", l1_[ii] );
    phase = new QListViewItem( item,
     aObj[o_wd_sf].GetStringEmpty(ii).c_str(), buf.p,
     aObj[o_wi_phc].GetStringEmpty(ii).c_str(),
     aObj[o_w_xf].GetStringEmpty(ii).c_str() );
    je = je - l1_[ii];
        for( ; jj>je ; jj--)
         switch( aObj[o_wi_dcc].GetStringEmpty(jj)[0])
         {
           case DC_SCP_CONDEN:
           case DC_AQ_SOLVENT: /* mol fractions in solvent */
           case DC_AQ_SOLVCOM:
           case DC_GAS_COMP:
           case DC_GAS_H2O:
           case DC_GAS_CO2:   /* gases */
           case DC_GAS_H2:    /* volume */
           case DC_GAS_N2:
           case DC_SOL_IDEAL:
           case DC_SOL_MINOR:   /* volume */
           case DC_SOL_MAJOR:
           case DC_PEL_CARRIER:
           case DC_SUR_MINAL:
           case DC_SUR_CARRIER: /* sorbent */
             elem = new QListViewItem( phase,
                aObj[o_wd_sm].GetStringEmpty(jj).c_str()," ",
                aObj[o_wi_dcc].GetStringEmpty(jj).c_str(),
                aObj[o_w_x].GetStringEmpty(jj).c_str(),
                aObj[o_wd_yla].GetStringEmpty(jj).c_str(),
                aObj[o_wo_wx].GetStringEmpty(jj).c_str(),
                aObj[o_wo_lngam].GetStringEmpty(jj).c_str() );
            break;
        default:
             elem = new QListViewItem( phase,
                aObj[o_wd_sm].GetStringEmpty(jj).c_str()," ",
                aObj[o_wi_dcc].GetStringEmpty(jj).c_str(),
                aObj[o_w_x].GetStringEmpty(jj).c_str(),
                aObj[o_wd_yla].GetStringEmpty(jj).c_str(),
                aObj[o_wd_ym].GetStringEmpty(jj).c_str(),
                aObj[o_wo_lngam].GetStringEmpty(jj).c_str() );
            break; /* error in DC class code */
        }
    }
    item->setOpen(true);
    ListView1->setFocus();
}


void HLresDialog::printList( fstream& f )
{
    QListViewItem* pPhase = item->firstChild();
    QListViewItem* pComp;
    const char *col;
    vstr buf(30);

    while( pPhase )
    {
      pComp = pPhase->firstChild();
      for(int ii=0; ii<4; ii++)
      {
        col =pPhase->text( ii );
        switch(ii)
        {
         case 0: sprintf( buf, "%16s", col );
                 break;
         case 1:
         case 2: sprintf( buf, "%2s", col );
                 break;
         default: sprintf( buf, "%15s", col );
                  break;
        }
         f<<  buf << " ";
      }
      f<< "\n";

      while( pComp )
      {
         for(int ii=0; ii<7; ii++)
        {
           col =pComp->text( ii );
           switch(ii)
           {
              case 0: sprintf( buf, "%16s", col );
                 break;
              case 1:
              case 2: sprintf( buf, "%2s", col );
                 break;
              default: sprintf( buf, "%15s", col );
                  break;
           }
         f<<  buf << " ";
        }
         f<< "\n";
         pComp = pComp->nextSibling();
      }
      pPhase = pPhase->nextSibling();
    }

}
//--------------------- End of HLresDialog.cpp ---------------------------


