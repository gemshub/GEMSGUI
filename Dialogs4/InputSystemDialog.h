//-------------------------------------------------------------------
// $Id: InputSystemDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of InputSystemDialog class
//
// Copyright (C) 2010  S.Dmytriyeva, D.Kulik
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef InputSystemDialog_included
#define InputSystemDialog_included

#include <QDialog>
#include "ui_InputSystemDialog4.h"
#include "filters_data.h"
#include "EquatSetupWidget.h"

/*!
  \ class TSystemDelegate
  \ individual items in views are rendered and edited using delegates
*/
class TSystemDelegate: public QItemDelegate
{
        Q_OBJECT

public:

         TSystemDelegate( QObject * parent = 0 );
         QWidget *createEditor(QWidget *parent,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;
         void setEditorData(QWidget *editor, const QModelIndex &index) const;
         void setModelData(QWidget *editor, QAbstractItemModel *model,
                             const QModelIndex &index) const;
};


class InputSystemDialog : public QDialog, public Ui::InputSystemDialogData
{

   Q_OBJECT

   TIArray<windowSetupData> wnData;
   TIArray<tableSetupData>  tbData;
   TIArray<pagesSetupData> stData;
   uint curPage;

   QList<QListWidget *> pLists;

   // internal functions
   int tableFindRow( int nO, int ndx );
   //int tableChangeRow( int row, tableSetupData& d);
   int tableInsertRow( tableSetupData& d);
   int tableInsertRow( int nO, int ndx, const char * andName);
   int tableDeleteRow( int row );
   void tableShowRow( int row );
   // copy data
   QString createString();
   QString createHeader();
   // working with static list
   int staticFindRow( int nO, int ndx);


public:

    InputSystemDialog( QWidget* parent, const char* pkey,
       TIArray<windowSetupData>& wnData, TIArray<tableSetupData>& tbData,
       TIArray<pagesSetupData>& scalarsList );
    virtual ~InputSystemDialog();


   int getObjTable( int row ) const
       { return tbData[row].nObj; }
   int getUnitsTable( int row ) const
       { return wnData[ tbData[row].nWin].nOunit; }
   int getListTable( int row ) const
   { return wnData[ tbData[row].nWin ].unitLine; }

   void getTable( TIArray<tableSetupData>& tab ) const;

protected slots:
    virtual void languageChange();
    void slotPopupContextMenu(const QPoint &pos);
    void SelectAll();
    void CopyData();
    void CopyDataHeader();
    void DeleteRows();

public slots:
    void changePage( int nPage );
    void tablEdited( int row, int column );
    void changeTable( const QItemSelection & selected, const QItemSelection & deselected );
    void CmPrint();
    void CmCalc();
    void help();
};

#endif // InputSystemDialog_included


