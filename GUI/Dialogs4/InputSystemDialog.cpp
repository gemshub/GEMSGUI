//-------------------------------------------------------------------
// $Id: InputSystemDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of InputSystemDialog class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
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
#include <QHeaderView>

#include "InputSystemDialog.h"
#include "GemsMainWindow.h"
#include "CalcDialog.h"
#include "m_param.h"
#include "service.h"

void InputSystemDialog::languageChange()
{
    retranslateUi(this);
}


InputSystemDialog::InputSystemDialog( QWidget* parent, const char* pkey,
      TIArray<windowSetupData>& awnData, TIArray<tableSetupData>& atbData,
      TIArray<pagesSetupData>& scalarsList ):
        QDialog( parent )
{
    size_t ii, jj;
    QListWidgetItem* item1;
    QWidget* page1;
    QHBoxLayout* horizontalLayout1;
    QListWidget* lstIndexes1;

    setupUi(this);

    recTable = new TRecipeTable(groupBox_3, this);
    gridLayout->addWidget(recTable, 1, 2, 2, 1);


    // define key of record
    QString str= windowTitle();
            str += pkey;
            setWindowTitle( str );

    pTname->setText( QString(aObj[o_ssname].GetStringEmpty(0,0).c_str()));
    // define lists pages
    for(  ii=0; ii<awnData.GetCount()-1; ii++ )  // -1 all exept static window
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
        lstIndexes1->setResizeMode(QListView::Adjust);
        lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
        //QObject::connect( lstIndexes1->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
        //   this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );
        horizontalLayout1->addWidget(lstIndexes1);
        winStac->addWidget(page1);

        // insert items to list of indexes
        TCStringArray lst;
        TProfil::pm->getNamesList( awnData[ii].nObj, lst);
        for(  jj=0; jj<lst.size(); jj++ )
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

    // add lists static
    //wnData.Add( new windowSetupData( "Static", -1, 0, -1, -1, 0.,'_' ) );
    ii = wnData.GetCount();
    wnData.Add( new windowSetupData(awnData[ii]));
    // insert to list
    str = QString("%1").arg(  awnData[ii].pageName.c_str());
    item1 = new QListWidgetItem( str,  keywdList);

    // add page
    page1 = new QWidget();
    page1->setObjectName(str);
    horizontalLayout1 = new QHBoxLayout(page1);
    lstIndexes1 = new QListWidget(page1);
    lstIndexes1->setWrapping( true );
    lstIndexes1->setResizeMode(QListView::Adjust);
    lstIndexes1->setSelectionMode(QAbstractItemView::MultiSelection);
    horizontalLayout1->addWidget(lstIndexes1);
    winStac->addWidget(page1);

    // insert items to list of indexes
    for(  jj=0; jj<scalarsList.GetCount(); jj++ )
    {
      stData.Add( new pagesSetupData(scalarsList[jj]));
      item1 = new QListWidgetItem( scalarsList[jj].pageName.c_str(),  lstIndexes1);
      int nJ = scalarsList[jj].ndx;
      if(nJ<0) nJ = 0;
      auto nO = scalarsList[jj].nObj;
      if( nO<0)
          continue;
      if( aObj[nO].GetM() > 1 )
       item1->setToolTip( aObj[nO].GetDescription(0,nJ).c_str() );
      else
       item1->setToolTip( aObj[nO].GetDescription(nJ,0).c_str() );
    }
    pLists.append(lstIndexes1);

    // define table
    recTable->setColumnWidth( 0, 70 );
    recTable->setColumnWidth( 1, 90 );
    recTable->setColumnWidth( 2, 100 );
    //recTable->setColumnWidth( 3, 50 );
    TSystemDelegate * deleg = new TSystemDelegate( this );
    recTable->setItemDelegate(deleg);
    // setup from atbData
    for(  ii=0; ii<atbData.GetCount(); ii++ )
    {
        auto jj = atbData[ii].nIdx;
        auto iWin = atbData[ii].nWin;
        auto nO = atbData[ii].nObj;
        if( iWin  >= wnData.GetCount()-1 ) // static list
          jj = max(0,staticFindRow( nO, jj ));
        pLists[iWin]->item(jj)->setSelected(true);
        atbData[ii].ndxName =
            pLists[iWin]->item(jj)->text().toStdString();
        tableInsertRow( atbData[ii] );
    }

    // define current page
    curPage = 0;
    keywdList->setCurrentItem(nullptr);
    keywdList->item(0)->setSelected(true);
    changePage( curPage );

    //for(int ii=0; ii<pLists.count(); ii++ )
    //{   QObject::connect( pLists[ii]->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
    //       this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );
    //}

    QObject::connect( recTable, SIGNAL( cellChanged( int, int )  ),
                      this, SLOT( tablEdited( int, int ) ));
    QObject::connect( keywdList, SIGNAL(currentRowChanged ( int  ) ),
                      this, SLOT(changePage( int ) ));
    QObject::connect( pPrint, SIGNAL(clicked()), this, SLOT(CmPrint()));
    QObject::connect( pHelp, SIGNAL(clicked()), this, SLOT(help()));
    QObject::connect( pCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect( pOK, SIGNAL(clicked()), this, SLOT(accept()));

//    recTable->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect( recTable, SIGNAL(customContextMenuRequested(QPoint)),
//         this, SLOT(slotPopupContextMenu(QPoint)));

}


InputSystemDialog::~InputSystemDialog()
{}

void InputSystemDialog::getTable( TIArray<tableSetupData>& tab ) const
{
  string txt = pTname->text().toStdString();

  if( txt == emptiness /*|| txt == short_emptiness*/ )
  aObj[o_ssname].SetString( S_EMPTY, 0, 0 );
  else
  aObj[o_ssname].SetString( txt.c_str(), 0, 0 );

  tab.Clear();
  for( uint ii=0; ii< tbData.GetCount(); ii++ )
    tab.Add( new tableSetupData(tbData[ii]) );

}

void InputSystemDialog::changePage( int nPage_ )
{
    uint nPage = nPage_;
    QObject::disconnect( pLists[curPage]->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
              this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );

    curPage = nPage;

    QObject::connect( pLists[curPage]->selectionModel(), SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection& ) ),
              this, SLOT( changeTable( const QItemSelection&, const QItemSelection& )) );

    winStac->setCurrentIndex ( nPage );
    if( nPage < wnData.GetCount()-1 )
      lDesc->setText( aObj[wnData[nPage].nObj].GetDescription(0,0).c_str());
    else
      lDesc->setText( "Other system recipe inputs (masses, volumes, T & P intervals)");
}

void InputSystemDialog::changeTable(const QItemSelection & selected, const QItemSelection & deselected)
{
   QModelIndex  ndx;
   int row, nO, ndx_row;

  // added selected
  foreach( ndx,  selected.indexes()  )
  {
    string stt = ndx.data(Qt::DisplayRole).toString().toStdString();
    nO = wnData[curPage].nObj;
    ndx_row = ndx.row();
    if( nO<0 || curPage >= wnData.GetCount()-1 )
    { nO = stData[ndx_row].nObj;
      ndx_row =  stData[ndx_row].ndx;
    }
    tableInsertRow( nO, ndx_row, stt.c_str() );
  }
  // delete deselected
  foreach( ndx,  deselected.indexes()  )
  {
    nO = wnData[curPage].nObj;
    ndx_row = ndx.row();
    if( nO<0 || curPage >= wnData.GetCount()-1 )
    { nO = stData[ndx_row].nObj;
      ndx_row =  stData[ndx_row].ndx;
    }
    row = tableFindRow( nO, ndx_row );
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
        tbData[row].unit = recTable->item(row, column)->text().toStdString()[0];
        break;
     default:
         break;
         }
}

void InputSystemDialog::CmPrint()
{
    try
    {
      std::string filename = "InputSystem.txt";
      if( vfChooseFileSave(this, filename,
          "Put file name for printing" ) == false )
               return;
      fstream f(filename.c_str(), ios::out);
      ErrorIf( !f.good() , filename.c_str(), "Fileopen error");

      /*recTable->selectAll();
      QString clipText = createHeader();
      clipText += createString();
      f << clipText.toStdString();*/

      char buf[200];

      sprintf( buf, "%-10.9s", recTable->horizontalHeaderItem(0)->text().toStdString() );
      f << buf;
      sprintf( buf, "%-20.19s", recTable->horizontalHeaderItem(1)->text().toStdString() );
      f << buf;
      sprintf( buf, "%-14.11s", recTable->horizontalHeaderItem(2)->text().toStdString() );
      f << buf;
      sprintf( buf, "%-6.6s", recTable->horizontalHeaderItem(3)->text().toStdString() );
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
  pVisorImp->OpenHelp( GEMS_RECIPE_HTML );
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

int InputSystemDialog::staticFindRow( int nO, int ndx)
{
  int nRow = -1;
  for(uint ii=0; ii<stData.GetCount(); ii++ )
  {
   if(stData[ii].nObj == nO && stData[ii].ndx == ndx )
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

void InputSystemDialog::deleteRows( int f_from, int r_to )
{
  int jj, nO, ndx;
  uint iWin;
  for( int row = f_from; row >= r_to; row-- )
   {
      ndx = jj = tbData[row].nIdx;
      iWin = tbData[row].nWin;
      nO = tbData[row].nObj;

      if( iWin  >= wnData.GetCount()-1 ) // static list
        jj = max(0,staticFindRow( nO, jj ));

      pLists[iWin]->item(jj)->setSelected(false);
      if( iWin != curPage )
      {
        int rw = tableFindRow( nO, ndx );
        if( rw >= 0)
         tableDeleteRow( rw );
      }
   }
}


//=================================================================


TRecipeTable::TRecipeTable( QWidget* parent,InputSystemDialog* aparentDlg  ):
    QTableWidget(parent), parentDlg(aparentDlg)
{
    setColumnCount(4);
    QTableWidgetItem *_qitem = new QTableWidgetItem("Property");
    setHorizontalHeaderItem(0, _qitem);
    _qitem = new QTableWidgetItem("Name");
    setHorizontalHeaderItem(1, _qitem);
    _qitem = new QTableWidgetItem("Quantity");
    setHorizontalHeaderItem(2, _qitem);
    _qitem = new QTableWidgetItem("Units");
    setHorizontalHeaderItem(3, _qitem);

    QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy4);
    setMinimumSize(QSize(360, 0));
    horizontalHeader()->setDefaultSectionSize(100);
    horizontalHeader()->setMinimumSectionSize(25);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setDefaultSectionSize(25);
    verticalHeader()->setStretchLastSection(false);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)),
         this, SLOT(slotPopupContextMenu(QPoint)));

}

void TRecipeTable::keyPressEvent(QKeyEvent* e)
{
    if ( e->modifiers() & Qt::ControlModifier )
    {
        switch ( e->key() )
        {
          case Qt::Key_A:
            SelectAll();
            return;
          case Qt::Key_C:
            CopyData();
            return;
          case Qt::Key_H:
            CopyDataHeader();
            return;
          case Qt::Key_D:
            DeleteRows();
            return;
          case Qt::Key_V:
            PasteData();
            return;
        }
    }
    switch( e->key() )
    {
      case Qt::Key_F1:
            CmHelp();
            return;
      case Qt::Key_F8:
            CmCalc();
              return;
     }
    QTableWidget::keyPressEvent(e);
}



// copy data
void TRecipeTable::slotPopupContextMenu(const QPoint &pos)
{
   QMenu *menu = new QMenu(this);

   QAction* act =  new QAction(tr("&Help"), this);
   act->setIcon(QIcon(":/menu/Icons/ShowHelpWindowIcon.png"));
   act->setShortcut(tr("F1"));
   act->setStatusTip(tr("Help to the specified cell"));
   connect(act, SIGNAL(triggered()), this, SLOT(CmHelp()));
   menu->addAction(act);

   act =  new QAction(tr("Select &all"), this);
   act->setShortcut(tr("Ctrl+A"));
   act->setStatusTip(tr("Select all"));
           connect(act, SIGNAL(triggered()), this, SLOT(SelectAll()));
   menu->addAction(act);

   if( currentColumn() == 2 )
         {
            menu->addSeparator();
            act =  new QAction(tr("Calculator"), this);
            act->setShortcut(tr("F8"));
            act->setStatusTip(tr("Use Calculator for specified cells"));
            connect(act, SIGNAL(triggered()), this, SLOT(CmCalc()));
            menu->addAction(act);

            act =  new QAction(tr("Paste"), this);
            act->setShortcut(tr("Ctrl+V"));
            act->setStatusTip(tr("Paste to selected cell"));
            connect(act, SIGNAL(triggered()), this, SLOT(PasteData()));
            menu->addAction(act);
         }
     menu->addSeparator();

     act =  new QAction(tr("&Copy"), this);
     act->setShortcut(tr("Ctrl+C"));
     act->setStatusTip(tr("Copy selected data"));
           connect(act, SIGNAL(triggered()), this, SLOT(CopyData()));
     menu->addAction(act);

     act =  new QAction(tr("Copy &Header and Cells"), this);
     act->setShortcut(tr("Ctrl+H"));
     act->setStatusTip(tr("Copy selected header&cells"));
           connect(act, SIGNAL(triggered()), this, SLOT(CopyDataHeader()));
     menu->addAction(act);

     menu->addSeparator();

     act =  new QAction(tr("&Delete rows"), this);
     act->setShortcut(tr("Ctrl+D"));
     act->setStatusTip(tr("Delete selected rows"));
           connect(act, SIGNAL(triggered()), this, SLOT(DeleteRows()));
     menu->addAction(act);

     menu->exec( viewport()->mapToGlobal(pos) );
     delete menu;
}

void TRecipeTable::SelectAll()
{
     selectAll();
}

QString TRecipeTable::createString()
{
  QString cText;
  QString clipText;
  QList<QTableWidgetSelectionRange> rec = selectedRanges();
  int row, col;

  for( row = rec[0].topRow(); row <= rec[0].bottomRow(); row++ )
   {
       bool frst = true;
       for( col = rec[0].leftColumn(); col <= rec[0].rightColumn(); col++ )
       {
           if( !frst )
               clipText += splitCol;
           frst = false;
           cText = item(row, col)->text();
           if( cText == emptiness.c_str() )
                  cText = "  ";//"\r";
            clipText += cText;
       }
       if( !frst )
           clipText += splitRow;
   }
      return clipText;
}

QString TRecipeTable::createHeader()
{
  QString cText;
  QString clipText;
  QList<QTableWidgetSelectionRange> rec = selectedRanges();
  int col;

   bool frst = true;
   for( col = rec[0].leftColumn(); col <= rec[0].rightColumn(); col++ )
   {
           if( !frst )
               clipText += splitCol;
           frst = false;
           cText = horizontalHeaderItem(col)->text();
           clipText += cText;
    }
   return clipText;
}

 void TRecipeTable::CopyData()
 {
    QString clipText = createString();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
 }

 void TRecipeTable::CopyDataHeader()
 {
      QString clipText = createHeader();
      clipText += splitRow;
      clipText += createString();
      QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
  }


 void TRecipeTable::DeleteRows()
 {
   QList<QTableWidgetSelectionRange> rec = selectedRanges();
   int r_from = rec[0].bottomRow();
   int r_to = rec[0].topRow();
   if( r_from < r_to)
   {
      r_from = r_to;
      r_to = rec[0].bottomRow();
   }
   parentDlg->deleteRows(r_from, r_to);
 }

 void TRecipeTable::CmCalc()
 {
   QList<QTableWidgetSelectionRange> rec = selectedRanges();

   if( rec.count() < 1 )
      return;

   if(  currentColumn() == 2 )
    {
      CalcDialog calc( this, -1 );
      if( calc.exec() )
       {
          for(int row = rec[0].topRow(); row <= rec[0].bottomRow(); row++ )
           {
              double val =  parentDlg->getTableVal(row);
              QString rval = calc.fun( val );
              item(row,2)->setText( rval);
              parentDlg->setTableVal(row, item(row, 2)->data(Qt::EditRole).toDouble());
           }
       }
    }

 }

 void TRecipeTable::CmHelp()
 {
   pVisorImp->OpenHelp( GEMS_RECIPE_HTML );
 }

 void TRecipeTable::PasteData()
 {
     if(  currentColumn() == 2 )
      {
          QString rval = QApplication::clipboard()->text(QClipboard::Clipboard);
          double val = rval.toDouble();
          rval = QString::number(val,'g',16);
          int row = currentRow();
          if( row >= 0  )
          {  item(row,2)->setText(  rval );
             parentDlg->setTableVal(row, item(row, 2)->data(Qt::EditRole).toDouble());
          }
     }
 }

//-------------------------------------------------------------------------------------
// TTreeDelegate -  individual items in views are rendered and edited using delegates
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
        size_t row = static_cast<size_t>(index.row());
        nObj = dlg->getObjTable( row );
        nUnit = dlg->getUnitsTable( row );
        nList = dlg->getListTable( row );
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
