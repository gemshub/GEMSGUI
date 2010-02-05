//-------------------------------------------------------------------
// $Id: InputSystemDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of InputSystemDialog class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
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

const char *SYSTEM_HTML = "gems_bcc";

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qvariant.h>
#include <QMenu>
#include <QAbstractScrollArea>
#include <QClipboard>

#include "InputSystemDialog.h"
#include "CalcDialog.h"
#include "visor_w.h"
#include "m_param.h"
#include "service.h"

void InputSystemDialog::languageChange()
{
    retranslateUi(this);
}


InputSystemDialog::InputSystemDialog( QWidget* parent, const char* pkey,
      TIArray<windowSetupData>& awnData, TIArray<tableSetupData>& atbData ):
        QDialog( parent )
{
    uint ii, jj;
    QListWidgetItem* item1;
    QWidget* page1;
    QHBoxLayout* horizontalLayout1;
    QListWidget* lstIndexes1;

    setupUi(this);

    // define key of record
    QString str= windowTitle();
            str += pkey;
            setWindowTitle( str );

    // define lists pages
    for(  ii=0; ii<awnData.GetCount(); ii++ )
    {
        wnData.Add( new windowSetupData(awnData[ii]));
        // insert to list
        str = QString("%1 (%2)").arg(
            awnData[ii].pageName.c_str(), aObj[awnData[ii].nObj].GetKeywd());
        item1 = new QListWidgetItem( str,  keywdList);

        // add page
        page1 = new QWidget();
        page1->setObjectName(awnData[ii].pageName.c_str());
        horizontalLayout1 = new QHBoxLayout(page1);
        lstIndexes1 = new QListWidget(page1);
        lstIndexes1->setWrapping( true );
        lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
        //QObject::connect( lstIndexes1->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
        //   this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );
        horizontalLayout1->addWidget(lstIndexes1);
        winStac->addWidget(page1);

        // insert items to list of indexes
        TCStringArray lst;
        TProfil::pm->getNamesList( awnData[ii].nObj, lst);
        for(  jj=0; jj<lst.GetCount(); jj++ )
        {
          item1 = new QListWidgetItem( lst[jj].c_str(), lstIndexes1);
          char sw = aObj[ awnData[ii].nSwitch].GetString(jj,0)[0];
          if( sw == S_OFF)
             str = QString("%1 - ").arg(jj);
          else
             str = QString("%1 + ").arg(jj);

          if( aObj[awnData[ii].nObj].GetIndexationCode() == 'J' )
              str += QString(" in phase: %1 ").arg(
                   TProfil::pm->PhNameforDC( jj, true ).c_str());

          item1->setToolTip( str );
         }
        pLists.append(lstIndexes1);
    }

    // define current page
    curPage = 0;
    keywdList->setCurrentItem(0);
    keywdList->item(0)->setSelected(true);
    changePage( curPage );

    // define table
    recTable->setColumnWidth( 0, 50 );
    recTable->setColumnWidth( 1, 80 );
    recTable->setColumnWidth( 2, 70 );
    recTable->setColumnWidth( 3, 40 );
    TSystemDelegate * deleg = new TSystemDelegate( this );
    recTable->setItemDelegate(deleg);
    // setup from atbData
    for(  ii=0; ii<atbData.GetCount(); ii++ )
    {
        jj = atbData[ii].nIdx;
        pLists[atbData[ii].nWin]->item(jj)->setSelected(true);
        atbData[ii].ndxName =
            pLists[atbData[ii].nWin]->item(jj)->text().toLatin1().data();
        tableInsertRow( atbData[ii] );
    }

    // define commands
    for(int ii=0; ii<pLists.count(); ii++ )
    {   QObject::connect( pLists[ii]->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
           this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );
    }

    QObject::connect( recTable, SIGNAL( cellChanged( int, int )  ),
                      this, SLOT( tablEdited( int, int ) ));
    QObject::connect( keywdList, SIGNAL(currentRowChanged ( int  ) ),
                      this, SLOT(changePage( int ) ));
    QObject::connect( pPrint, SIGNAL(clicked()), this, SLOT(CmPrint()));
    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( pOK, SIGNAL(clicked()), this, SLOT(accept()));

    recTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( recTable, SIGNAL(customContextMenuRequested(QPoint)),
         this, SLOT(slotPopupContextMenu(QPoint)));

}


InputSystemDialog::~InputSystemDialog()
{}

void InputSystemDialog::getTable( TIArray<tableSetupData>& tab ) const
{
  tab.Clear();
  for( uint ii=0; ii< tbData.GetCount(); ii++ )
    tab.Add( new tableSetupData(tbData[ii]) );

}

void InputSystemDialog::changePage( int nPage )
{
    curPage = nPage;
    winStac->setCurrentIndex ( nPage );
    lDesc->setText( aObj[wnData[nPage].nObj].GetDescription(0,0).c_str());
}

void InputSystemDialog::changeTable(const QItemSelection & selected, const QItemSelection & deselected)
{
   QModelIndex  ndx;
   int row;

  // added selected
  foreach( ndx,  selected.indexes()  )
  {
    gstring stt = ndx.data(Qt::DisplayRole).toString().toLatin1().data();
    tableInsertRow( wnData[curPage].nObj, ndx.row(), stt.c_str() );
  }
  // delete deselected
  foreach( ndx,  deselected.indexes()  )
  {
    row = tableFindRow( wnData[curPage].nObj, ndx.row());
    if( row >= 0)
       tableDeleteRow( row );
  }
}

void InputSystemDialog::tablEdited( int row, int column )
{
    switch( column )
    {
     case 2:
        tbData[row].val =  recTable->item(row, column)->data(Qt::EditRole).toDouble();
        break;
     case 3:
        tbData[row].unit = recTable->item(row, column)->text().toLatin1().data()[0];
        break;
     default:
         break;
         }
}

void InputSystemDialog::CmPrint()
{
    try
    {
      gstring filename = "InputSystem.txt";
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

      /*recTable->selectAll();
      QString clipText = createHeader();
      clipText += createString();
      f << clipText.toLatin1().data();*/

      char buf[200];

      sprintf( buf, "%-10.9s", recTable->horizontalHeaderItem(0)->text().toLatin1().data() );
      f << buf;
      sprintf( buf, "%-20.19s", recTable->horizontalHeaderItem(1)->text().toLatin1().data() );
      f << buf;
      sprintf( buf, "%-14.11s", recTable->horizontalHeaderItem(2)->text().toLatin1().data() );
      f << buf;
      sprintf( buf, "%-6.6s", recTable->horizontalHeaderItem(3)->text().toLatin1().data() );
      f << buf << endl;

      for(uint  ii=0; ii<tbData.GetCount(); ii++ )
      {
          sprintf( buf, "%-10.9s", tbData[ii].objName.c_str() );
          f << buf;
          sprintf( buf, "%-20.19s", tbData[ii].ndxName.c_str() );
          f << buf;
          sprintf( buf, "%-14.4lg", tbData[ii].val );
          f << buf;
          sprintf( buf, "%c", tbData[ii].unit );
          f << buf << endl;
      }
    }
    catch( TError& xcpt )
    {
      vfMessage(this, xcpt.title, xcpt.mess);
    }
}

void
InputSystemDialog::help()
{
  pVisorImp->OpenHelp( SYSTEM_HTML, 0, this/*, true*/ );
}

int InputSystemDialog::tableFindRow( int nO, int ndx)
{
  int nRow = -1;
  for(uint ii=0; ii<tbData.GetCount(); ii++ )
  {
   if(tbData[ii].nObj == nO && tbData[ii].nIdx == ndx )
      { nRow = ii; break; }
  }
  return nRow;
}

/*int InputSystemDialog::tableChangeRow( int row, tableSetupData& d)
{
    tbData[row].nWin = d.nWin;
    tbData[row].nObj = d.nWin;

    QTableWidgetItem *_item3 = recTable->item(row, 2);
    tbData[row].val = d.val;
    _item3->setText( QString::number (  tbData[row].val, 'g', 6 ));

    QTableWidgetItem *_item4 = recTable->item(row, 3);
    tbData[row].unit = d.unit;
    _item4->setText(QString("%1").arg(tbData[row].unit));

    return row;
}
*/

int InputSystemDialog::tableInsertRow( tableSetupData& d)
{
    tbData.Add( new tableSetupData( d ));

    int row = recTable->rowCount();
    tableShowRow( row );

    return row;
}

int InputSystemDialog::tableInsertRow( int nO, int ndx, const char * andName)
{
    tbData.Add( new tableSetupData( curPage, nO, aObj[nO].GetKeywd(),
         ndx, andName, wnData[curPage].defVal, wnData[curPage].defUnit ));

    int row = recTable->rowCount();
    tableShowRow( row );

    return row;
}

void InputSystemDialog::tableShowRow( int row )
{
    recTable->insertRow(row);

    QTableWidgetItem *_item1 = new QTableWidgetItem();
    _item1->setText(tbData[row].objName.c_str());
    _item1->setFlags(_item1->flags() & ~Qt::ItemIsEditable);
    recTable->setItem(row, 0, _item1);

    QTableWidgetItem *_item2 = new QTableWidgetItem();
    _item2->setText(tbData[row].ndxName.c_str());
    _item2->setFlags(_item2->flags() & ~Qt::ItemIsEditable);
    recTable->setItem(row, 1, _item2);

    QTableWidgetItem *_item3 = new QTableWidgetItem();
    _item3->setText( QString::number (  tbData[row].val, 'g', 6 ));
    _item3->setFlags(_item3->flags() | Qt::ItemIsEditable);
    recTable->setItem(row, 2, _item3);

    QTableWidgetItem *_item4 = new QTableWidgetItem();
    _item4->setText(QString("%1").arg(tbData[row].unit));
    if(wnData[tbData[row].nWin].nOunit >=0 )
       _item4->setFlags(_item4->flags() | Qt::ItemIsEditable);
    else
      _item4->setFlags(_item4->flags() & ~Qt::ItemIsEditable);
    recTable->setItem(row, 3, _item4);
}

int InputSystemDialog::tableDeleteRow( int nRow )
{
   tbData.Remove(nRow);
   recTable->removeRow(nRow);
   return tbData.GetCount();
}

// copy data
void InputSystemDialog::slotPopupContextMenu(const QPoint &pos)
{
   QMenu *menu = new QMenu(recTable/*this*/);

   QAction *act =  new QAction(tr("&Select all"), this);
   // act->setShortcut(tr("Ctrl+A"));
    act->setStatusTip(tr("Select all"));
           connect(act, SIGNAL(triggered()), this, SLOT(SelectAll()));
     menu->addAction(act);


     if( recTable->currentColumn() == 2 )
         {
            menu->addSeparator();
            act =  new QAction(tr("Calculator"), this);
            act->setShortcut(tr("F8"));
            act->setStatusTip(tr("Use Calculator for specified cells"));
            connect(act, SIGNAL(triggered()), this, SLOT(CmCalc()));
            menu->addAction(act);
         }

     menu->addSeparator();

     act =  new QAction(tr("&Copy"), this);
     //act->setShortcut(tr("Ctrl+C"));
     act->setStatusTip(tr("Copy selected data"));
           connect(act, SIGNAL(triggered()), this, SLOT(CopyData()));
     menu->addAction(act);

     act =  new QAction(tr("Copy &Header and Cells"), this);
     //act->setShortcut(tr("Ctrl+H"));
     act->setStatusTip(tr("Copy selected header&cells"));
           connect(act, SIGNAL(triggered()), this, SLOT(CopyDataHeader()));
     menu->addAction(act);

     menu->addSeparator();

     act =  new QAction(tr("&Delete rows"), this);
     //act->setShortcut(tr("Ctrl+H"));
     act->setStatusTip(tr("Delete selected rows"));
           connect(act, SIGNAL(triggered()), this, SLOT(DeleteRows()));
     menu->addAction(act);

     menu->exec( recTable->viewport()->mapToGlobal(pos) );
     delete menu;
}

void InputSystemDialog::SelectAll()
{
     recTable->selectAll();
}

QString InputSystemDialog::createString()
{
  QString cText;
  QString clipText;
  QList<QTableWidgetSelectionRange> rec = recTable->selectedRanges();
  int row, col;

  for( row = rec[0].topRow(); row <= rec[0].bottomRow(); row++ )
   {
       bool frst = true;
       for( col = rec[0].leftColumn(); col <= rec[0].rightColumn(); col++ )
       {
           if( !frst )
               clipText += "\t";
           frst = false;
           cText = recTable->item(row, col)->text();
           if( cText == emptiness.c_str() )
                  cText = "  ";//"\r";
            clipText += cText;
       }
       if( !frst )
           clipText += "\n";
   }
      return clipText;
}

QString InputSystemDialog::createHeader()
{
  QString cText;
  QString clipText;
  QList<QTableWidgetSelectionRange> rec = recTable->selectedRanges();
  int col;

   bool frst = true;
   for( col = rec[0].leftColumn(); col <= rec[0].rightColumn(); col++ )
   {
           if( !frst )
               clipText += "\t";
           frst = false;
           cText = recTable->horizontalHeaderItem(col)->text();
           clipText += cText;
    }
   return clipText;
}

 void InputSystemDialog::CopyData()
 {
    QString clipText = createString();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
 }

 void InputSystemDialog::CopyDataHeader()
 {
      QString clipText = createHeader();
      clipText += "\n";
      clipText += createString();
      QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
  }


 void InputSystemDialog::DeleteRows()
 {
   QList<QTableWidgetSelectionRange> rec = recTable->selectedRanges();

   for( int row = rec[0].bottomRow(); row >= rec[0].topRow(); row-- )
    {
       int jj = tbData[row].nIdx;
       //int iWin = tbData[row].nWin;
       int nO = tbData[row].nObj;
       pLists[tbData[row].nWin]->item(jj)->setSelected(false);
       // if( iWin != curWin )
       //  tableDeleteRow( row );
       int rw = tableFindRow( nO, jj);
       if( rw >= 0)
          tableDeleteRow( rw );
    }
}

 void InputSystemDialog::CmCalc()
 {
   QList<QTableWidgetSelectionRange> rec = recTable->selectedRanges();

   if( rec.count() < 1 )
      return;

   if(  recTable->currentColumn() == 2 )
    {
      CalcDialog calc( this, -1 );
      if( calc.exec() )
       {
          for(int row = rec[0].topRow(); row <= rec[0].bottomRow(); row++ )
           {
              double val = tbData[row].val;
              val = calc.fun( val );
              tbData[row].val = val;
              recTable->item(row,2)->setText( QString::number(  tbData[row].val, 'g', 6 ));
           }
       }
    }

 }


//-------------------------------------------------------------------------------------
// TTreeDelegate -  individuak items in views are rendered and edited using delegates
//-------------------------------------------------------------------------------------

TSystemDelegate::TSystemDelegate( QObject * parent ):
        QItemDelegate( parent )
{
}

// Editing System table
QWidget *TSystemDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
   int nObj= o_syxea, nUnit= o_syxdun, nList=5;
   InputSystemDialog *dlg = dynamic_cast<InputSystemDialog*>(parent->window());

   if( dlg )
    {
        nObj = dlg->getObjTable( index.row() );
        nUnit = dlg->getUnitsTable( index.row() );
        nList = dlg->getListTable( index.row() );
    }

   switch( index.column() )
        {
          case 3:
           {
             FieldInfo fld = FieldInfo( nUnit, ftCheckBox, nList, false, NextP, eYes, stIO, 1, 1);
             TCellCheck* editor =  new TCellCheck( fld, 0, 0, parent);
             return editor;
           }
        case 2:
           {
             FieldInfo fld = FieldInfo( nObj, ftFloat, 20, false, NextP, eYes, stIO, 1, 1);
             TCellInput* editor =  new TCellInput( fld, 0, 0, parent);
             return editor;
           }
        default:
                return QAbstractItemDelegate::createEditor( parent, option,  index );
        }
}

void TSystemDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
        TCell *cellEdit = dynamic_cast<TCell*>(editor);
        if( cellEdit)
        {
            cellEdit->setData( index.data(Qt::EditRole).toString());
        }
}

void TSystemDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
        TCell *cellEdit = dynamic_cast<TCell*>(editor);
        if( cellEdit)
        {
          model->setData(index, cellEdit->getData(),  Qt::EditRole);
        }
 }

//--------------------- End of InputSystemDialog.cpp ---------------------------
