//-------------------------------------------------------------------
// Id: gems/vizor/dlg/HLinpDialog.cpp  version 2.0.0    2001
//
// Implementation of HLinpDialog class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

const char *GEMS_LINP_HTML = "gm_syseq";

#include "HLinpDialog.h"
#include "v_object.h"
#include "visor_w.h"
#include "v_mod.h"
#include "service.h"
#include "units.h"
#include "m_param.h"


#include <stdio.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qheader.h>
/*
int objOnList[20] = { //phase
     o_musf, -1, o_muphc, o_sypcl,
     o_syxpun, o_syphm, -1, o_syyof,
                   //dcomp&readc
     o_musm, -1, o_mudcc, o_sydcl,
     o_syxdun, o_syxed, -1, o_sygex,
     o_syrsc, o_sydll, o_sydul, o_syrlc
 };
*/

int fldSymbols[20] = { //phase
     -1, -1, -1, 1,
     16, -1, 4, -1,
                   //dcomp&readc
     -1, -1, -1, 1,
     16, -1, 4, -1,
     23, -1, -1, 22
 };


MLineEdit::MLineEdit( QWidget * parent, const char * name ):
  QLineEdit( parent, name ), it(0), col(-1)
{
}

void
MLineEdit::focusOutEvent(QFocusEvent* e)
{
    getData();
    QLineEdit::focusOutEvent(e);
    hide();
}

void MLineEdit::setData( QListViewItem *ait, int acol)
{
 it = ait;
 col = acol;
 if( !it )
   return;
 if( it->text( 1 ).isEmpty() ) //dcomp&reacdc
  type =  fldSymbols[col+8];
 else
  type =  fldSymbols[col];      //phase
  if( type == -1 )
     setMaxLength( 20 );
  else
     setMaxLength( 2 );
}

const gstring emptiness("---");

void MLineEdit::getData()
{
 if( it == 0 || col <= 2 )
   return;
 gstring ss = (const char*)text();
 ss.strip();
 if( type == -1  ) // double
 {
   if( ss == S_EMPTY || ss == emptiness )
   {
      it->setText( col, S_EMPTY );
   }
   else
   {
    double dd;
    vstr sv(20);
    if( sscanf(ss.c_str(), "%lg%s", &dd, &sv.p ) != 1 )
         vfMessage(topLevelWidget(), ss.c_str(), "Sorry! Wrong value typed!" );
    else
      it->setText( col, ss.c_str() );
   }
  }
  else // spec simbol
  {
   if( ss == S_EMPTY || ss == emptiness )
   {
      it->setText( col, S_EMPTY );
   }
   else
   {
    gstring Vals(aUnits[type].getVals(0));
    size_t ind1 = Vals.find(ss);
    if( ind1 == gstring::npos )
      vfMessage(topLevelWidget(), ss.c_str(), "Sorry! Wrong value typed!" );
    else
      it->setText( col, ss.c_str() );
   }   
  }
}

//----------------------------------------------------------

HLinpDialog* HLinpDialog::pDia = 0;

HLinpDialog::HLinpDialog( QWidget* parent,  const char* name )
    : HLinpDialogData( parent, name )
{
    colEdit = 0;
    pDia = this;
    ListView1->setSorting(-1);
    // ListView1->setRootIsDecorated(true);
    ListView1->setAllColumnsShowFocus(false);

    connect( ListView1, SIGNAL(
     rightButtonPressed( QListViewItem *, const QPoint &, int )  ),
     this, SLOT( CmEdit( QListViewItem *, const QPoint &, int ) ) );

    loadList();
}

// System Actions

void
HLinpDialog::Update()
{
    loadList();
    ListView1->update();
}

HLinpDialog::~HLinpDialog()
{
}

void
HLinpDialog::closeEvent(QCloseEvent* ev)
{
    pDia = 0;
    QWidget::closeEvent(ev);
}

void HLinpDialog::CmEdit( QListViewItem *it,
       const QPoint &, int col)
{
  if( it == 0 || col < 3  )
    return;
  QString data = it->text( col );
  if( data.isEmpty() )
    return;
  if( colEdit==0)
    colEdit = new MLineEdit(this);
  QRect rec;
  rec = ListView1->itemRect( it );
  rec.setLeft( rec.left()+ ListView1->header()->sectionPos(col )+
               ListView1->frameWidth() + ListView1->x() );
  rec.setTop( rec.top() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y() );
  rec.setBottom( rec.bottom() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y() );
  rec.setWidth( ListView1->columnWidth( col ) );

  colEdit->setData( it, col );
  colEdit->setGeometry( rec );
  colEdit->setText( data );
  colEdit->setFocus();
  colEdit->show();
/*  connect( colEdit, SIGNAL( returnPressed () ),
     this, SLOT( CmSaveEdit() ));
*/
  update();
}

/*
void HLinpDialog::CmSaveEdit()
{
  colEdit->getData();
  colEdit->hide();
  update();
}
*/

void HLinpDialog::CmCancel()
{
  close();
}


void HLinpDialog::CmHelp()
{
   pVisorImp->OpenHelp( GEMS_LINP_HTML );
}

void HLinpDialog::CmPrint()
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

void HLinpDialog::CmUndo()
{
   loadList();
   ListView1->update();
}


void HLinpDialog::CmBCC()
{
   saveList();
   TProfil::pm->CalcBcc(); // Calc bulk composition
   loadList();
   ListView1->update();
}


void HLinpDialog::CmOK()
{
   saveList();
   TProfil::pm->CalcBcc(); // Calc bulk composition
   close();
}


void HLinpDialog::loadList()
{

   ListView1->clear();
   if( colEdit )
    colEdit->setData( 0, -1 );
   item = new QListViewItem( ListView1, tr( "System" ) );
   QListViewItem * phase;
   QListViewItem * elem;
   short *l1_    = (short *)aObj[ o_mul1 ].GetPtr();
   vstr buf(20);
   int jj, je;
    jj = je = aObj[o_musm].GetN()-1;


   for( int ii=aObj[o_musf].GetN()-1; ii>=0 ; ii--)
   {
     sprintf( buf, " %d", l1_[ii] );
     phase = new QListViewItem( item,
     gstring( aObj[o_musf].GetStringEmpty(ii), 12, 16).c_str(), buf.p,
     aObj[o_muphc].GetStringEmpty(ii).c_str(),
     aObj[o_sypcl].GetStringEmpty(ii).c_str(),
     aObj[o_syxpun].GetStringEmpty(ii).c_str(),
     aObj[o_syphm].GetStringEmpty(ii).c_str(),
     "J" /*aObj[].GetStringEmpty(ii).c_str()*/,
     aObj[o_syyof].GetStringEmpty(ii).c_str() );
    je = je - l1_[ii];
        for( ; jj>je ; jj--)
        {
             elem = new QListViewItem( phase,
                gstring( aObj[o_musm].GetStringEmpty(jj), 12, 16).c_str(),"",
                aObj[o_mudcc].GetStringEmpty(jj).c_str(),
                aObj[o_sydcl].GetStringEmpty(jj).c_str(),
                aObj[o_syxdun].GetStringEmpty(jj).c_str(),
                aObj[o_syxed].GetStringEmpty(jj).c_str(),
                "J", //aObj[].GetStringEmpty(jj).c_str(),
                aObj[o_sygex].GetStringEmpty(jj).c_str());
                elem->setText( 8, aObj[o_syrsc].GetStringEmpty(jj).c_str());
                elem->setText( 9, aObj[o_sydll].GetStringEmpty(jj).c_str());
                elem->setText( 10, aObj[o_sydul].GetStringEmpty(jj).c_str());
                elem->setText( 11, aObj[o_syrlc].GetStringEmpty(jj).c_str() );
        }
    }
    item->setOpen(true);
    ListView1->setFocus();
}

void HLinpDialog::saveList()
{
   if( colEdit )
    colEdit->getData();

    QListViewItem* pPhase = item->firstChild();
    QListViewItem* pComp;
    const char *col;
    vstr buf(30);
    int ii, nPh =0, nDc =0;

    while( pPhase )
    {
      pComp = pPhase->firstChild();
      for( ii=0; ii<8; ii++)
      {
         col =pPhase->text( ii );
         switch(ii)
         {
              case 0:
              case 1:
              case 2:
              case 6: // no edited
                 break;
              case 3: if( !aObj[o_sypcl].IsNull() )
                       aObj[o_sypcl].SetString( col, nPh );
                      break;
              case 4: if( !aObj[o_syxpun].IsNull() )
                       aObj[o_syxpun].SetString( col, nPh );
                      break;
              case 5: if( !aObj[o_syphm].IsNull() )
                       aObj[o_syphm].SetString( col, nPh );
                      break;
              case 7: if( !aObj[o_syyof].IsNull() )
                       aObj[o_syyof].SetString( col, nPh );
                      break;
           }
      }
      while( pComp )
      {
        for( ii=0; ii<12; ii++)
        {
            col =pComp->text( ii );
            switch(ii)
            {
              case 0:
              case 1:
              case 2:
              case 6: // no edited
                 break;
              case 3: if( !aObj[o_sydcl].IsNull() )
                       aObj[o_sydcl].SetString( col, nDc );
                      break;
              case 4: if( !aObj[o_syxdun].IsNull() )
                       aObj[o_syxdun].SetString( col, nDc );
                      break;
              case 5: if( !aObj[o_syxed].IsNull() )
                       aObj[o_syxed].SetString( col, nDc );
                      break;
              case 7: if( !aObj[o_sygex].IsNull() )
                       aObj[o_sygex].SetString( col, nDc );
                      break;
              case 8: if( !aObj[o_syrsc].IsNull() )
                       aObj[o_syrsc].SetString( col, nDc );
                      break;
              case 9: if( !aObj[o_sydll].IsNull() )
                       aObj[o_sydll].SetString( col, nDc );
                      break;
              case 10: if( !aObj[o_sydul].IsNull() )
                       aObj[o_sydul].SetString( col, nDc );
                      break;
              case 11: if( !aObj[o_syrlc].IsNull() )
                       aObj[o_syrlc].SetString( col, nDc );
                      break;
           }
         }
         nDc++;
         pComp = pComp->nextSibling();
      }
     nPh++;
     pPhase = pPhase->nextSibling();
    }

}


void HLinpDialog::printList( fstream& f )
{
    QListViewItem* pPhase = item->firstChild();
    QListViewItem* pComp;
    const char *col;
    vstr buf(30);

    while( pPhase )
    {
      pComp = pPhase->firstChild();
      for(int ii=0; ii<8; ii++)
      {
         col =pPhase->text( ii );
         switch(ii)
         {
              case 0: sprintf( buf, "%16s", col );
                 break;
              case 1: sprintf( buf, "%3s", col );
                 break;
              case 7:
              case 9:
              case 10: sprintf( buf, "%15s", col );
                 break;
              default: sprintf( buf, "%1s", col );
                  break;
           }
         f<<  buf << " ";
      }
      f<< "\n";

      while( pComp )
      {
         for(int ii=0; ii<12; ii++)
        {
            col =pComp->text( ii );
            switch(ii)
            {
              case 0: sprintf( buf, "%16s", col );
                 break;
              case 1: sprintf( buf, "%3s", col );
                 break;
              case 7:
              case 9:
              case 10: sprintf( buf, "%15s", col );
                 break;
              default: sprintf( buf, "%1s", col );
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
//--------------------- End of HLinpDialog.cpp ---------------------------

