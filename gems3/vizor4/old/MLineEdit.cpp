//-------------------------------------------------------------------
// $Id: NewSystemDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of NewSystemDialog class
//
// Copyright (C) 1996-2002 S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
const char *GEMS_SYS_HTML = "gems_sys";
const char *GEMS_BCC_HTML = "gems_bcc";
const char *GEMS_IPM_HTML = "gems_ipm";
const char *GEMS_HOWTO_HTML = "ge_howto";

#include <math.h>
#include <q3toolbar.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qapplication.h>
#include <q3header.h>
#include <qtabwidget.h>


#include "NewSystemDialog.h"
#include "ProgressDialog.h"
#include "v_mod.h"
#include "service.h"
#include "visor.h"
#include "visor_w.h"
#include "m_param.h"
#include "m_syseq.h"
#include "units.h"
#include "gdatastream.h"


//---------------------------------------------------------------
//-------------  HLinpDialog  -------------------------------------

void NewSystemDialog::CmEdit( Q3ListViewItem *it,
       const QPoint &, int col)
{
  if( it == 0 || col < 3  )
    return;

  QString data = it->text( col );
  if( data.isEmpty() )
    return;

  if( colEdit==0 )
    colEdit = new MLineEdit(ListView1/*this*/);

  QRect rec = ListView1->itemRect( it );
/*
  rec.setLeft( rec.left()+ ListView1->header()->sectionPos(col )+
               ListView1->frameWidth() + ListView1->x() );
  rec.setTop( rec.top() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y()+
              menu->height()+toolBar->height() );
  rec.setBottom( rec.bottom() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y()+
              menu->height()+toolBar->height() );
  rec.setWidth( ListView1->columnWidth( col ) );
*/
  rec.setLeft( rec.left()+ ListView1->header()->sectionPos(col )+
               ListView1->frameWidth());
  rec.setTop( rec.top() + ListView1->frameWidth()+
              ListView1->header()->height() );
  rec.setBottom( rec.bottom() + ListView1->frameWidth()+
              ListView1->header()->height() );
  rec.setWidth( ListView1->columnWidth( col ) );

  colEdit->setData( it, col );
  colEdit->setGeometry( rec );
  colEdit->setText( data );
  colEdit->setFocus();
  colEdit->show();
  connect( colEdit, SIGNAL( returnPressed () ),
     colEdit, SLOT( CmAccept() ));

  update();
}


//-------------  HLinpDialog  -------------------------------------

void NewSystemDialog::CmEdit( Q3ListViewItem *it,
       const QPoint &, int col)
{
  if( it == 0 || col < 3  )
    return;

  QString data = it->text( col );
  if( data.isEmpty() )
    return;

  if( colEdit==0 )
    colEdit = new MLineEdit(ListView1/*this*/);

  QRect rec = ListView1->itemRect( it );
/*
  rec.setLeft( rec.left()+ ListView1->header()->sectionPos(col )+
               ListView1->frameWidth() + ListView1->x() );
  rec.setTop( rec.top() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y()+
              menu->height()+toolBar->height() );
  rec.setBottom( rec.bottom() + ListView1->frameWidth()+
              ListView1->header()->height()+ ListView1->y()+
              menu->height()+toolBar->height() );
  rec.setWidth( ListView1->columnWidth( col ) );
*/
  rec.setLeft( rec.left()+ ListView1->header()->sectionPos(col )+
               ListView1->frameWidth());
  rec.setTop( rec.top() + ListView1->frameWidth()+
              ListView1->header()->height() );
  rec.setBottom( rec.bottom() + ListView1->frameWidth()+
              ListView1->header()->height() );
  rec.setWidth( ListView1->columnWidth( col ) );

  colEdit->setData( it, col );
  colEdit->setGeometry( rec );
  colEdit->setText( data );
  colEdit->setFocus();
  colEdit->show();
  connect( colEdit, SIGNAL( returnPressed () ),
     colEdit, SLOT( CmAccept() ));

  update();
}



//---------  MLineEdit  ----------------------------------

int fldSymbols[20] = { //phase
     -1, -1, -1, 1,
     5/*16*/, -1, 7, -1,
                   //dcomp&readc
     -1, -1, -1, 1,
     5/*16*/, -1, 7, -1,
     4/*23*/, -1, -1, 3/*22*/
 };


MLineEdit::MLineEdit( QWidget * parent, const char * name ):
  QLineEdit( parent, name ), it(0), col(-1)
{

   connect( parent, SIGNAL( contentsMoving ( int, int) ),
   this, SLOT( CmHide( int, int) ) );

}

void MLineEdit::CmHide( int , int  )
{
    CmAccept();
}

void MLineEdit::CmAccept()
{
    getData();
    hide();
}

void
MLineEdit::focusOutEvent(QFocusEvent* e)
{
    getData();
    hide();
    QLineEdit::focusOutEvent(e);
//    hide();
}

void MLineEdit::setData( Q3ListViewItem *ait, int acol)
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
  {   setMaxLength( 2 );
      Vals = aUnits[type].getVals(0);
  }
}

const gstring emptiness("---");

void MLineEdit::getData()
{
 if( it == 0 || col <= 2 )
   return;

 gstring old_data = (const char*)it->text( col );

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
    if( sscanf(ss.c_str(), "%lg%s", &dd, sv.p ) != 1 )
    {
        setText( old_data.c_str());
        vfMessage(topLevelWidget(), ss.c_str(), "Sorry! Wrong value typed!" );
    }
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
//    Vals = aUnits[type].getVals(0);
    size_t ind1 = Vals.find(ss);
    if( ind1 == gstring::npos )
    {
       setText( old_data.c_str());
       vfMessage(topLevelWidget(), ss.c_str(), "Sorry! Wrong value typed!" );
    }
    else
      it->setText( col, ss.c_str() );
   }
  }
  if( col == 3 ) // + or -
  {
    Q3ListViewItem* pp = it->firstChild();
    while( pp )
    {
      if( ss[0] == '-' ||
         ( ( ss[0] == '+' || ss[0] == '*' ) && old_data[0] == '-' ))
      pp->setText( col, ss.c_str() );
      pp = pp->nextSibling();
    }
  }
}

void
MLineEdit::mousePressEvent(QMouseEvent *e)
{
    if( e->button() != RightButton || type < 0 )
    {
        QLineEdit::mousePressEvent(e);
        return;
    }
    Q3PopupMenu* menu = new Q3PopupMenu;
    CHECK_PTR(menu);

    for(uint ii=0; ii<Vals.length(); ii++)
    {
        gstring s(Vals, ii, 1);
        int id = menu->insertItem(s.c_str(), ii);
        menu->setItemEnabled(id, true);
    }
    connect(menu, SIGNAL(activated(int)),
            this, SLOT(SetIndex(int)));
    menu->exec(e->globalPos());
}

void
MLineEdit::SetIndex(int ii)
{
    if( ii>=(int)Vals.length() )
        return;

    gstring val(Vals, ii, 1);
    setText(val.c_str());
    getData();
    //it->setText( col, val.c_str() );
}


//-------- End of file NewSystemDialog.cpp ----------------------------


