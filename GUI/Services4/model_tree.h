//-------------------------------------------------------------------
// $Id: model_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TTreeModel, TTreeDelegate and TTreeView,
//
// Copyright (C) 2009 S.Dmytriyeva
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

#ifndef _model_tree_h
#define _model_tree_h


#include "model_w.h"

#include <QAbstractItemModel>
#include <QTreeView>

class QObject;
class QPainter;

//===========================================
// TTreeModel class
//===========================================

class TreeLine
{
public:

	TreeLine(int aiPh, int aiDC, int aiDCPh);
	~TreeLine();

	int iPh; // index of phase
	int iDC; // index of DComp/React in full list
	int iDC_Ph; // index of DComp/React in phaes list
	char UGval;
	
	TreeLine *parent;
    QList<TreeLine *> children;
};


/*!
  \ class TTreeModel
  \ class for represents the data set and is responsible for fetchin
  \ the data is neaded for viewing and for writing back any changes.
  \ Reading/writing data from/to TObject and TObjList classes
  
*/
class TTreeModel: public QAbstractItemModel
{
	Q_OBJECT
	
  QList<FieldInfo>	fldsPh;
  QList<FieldInfo>	fldsDC;
  QStringList hdData;
  
  TreeLine* rootNode;

  TreeLine* lineFromIndex(const QModelIndex& index) const;
  QString getDescription(  int nO, int N ) const;

  void setGOcorr(const QModelIndex& index, QString& value, int nO, int iN );
  QString getGOcorr( const QModelIndex& index, int nO, int iN ) const;
  QString getObjValue( const QModelIndex& index, int nO, int iN ) const;

 void childChanged(const QModelIndex& index, const QVariant& value, char old_data);
public:
	  
  TTreeModel( const QList<FieldInfo> aFldsPh, const QList<FieldInfo> aFldsDC,
		      const QStringList& aHeaderData,   QObject* parent = 0 );
  ~TTreeModel();
	 
  QModelIndex index(int row, int column, const QModelIndex& parent) const;
  QModelIndex parent(const QModelIndex& child) const;
  int rowCount ( const QModelIndex & parent ) const;     //ok	
  int columnCount ( const QModelIndex & parent  ) const; // ok
  QVariant data ( const QModelIndex & index, int role ) const;
  bool setData ( const QModelIndex & index, const QVariant & value, int role );
  QVariant headerData ( int section, Qt::Orientation orientation, int role ) const; 
  Qt::ItemFlags flags ( const QModelIndex & index ) const;
	 
  void setupModelData();

  FieldInfo getInfo( const QModelIndex& index, int& iN, int& iM, 
			 Selection* sel=0, int to_calc = 0) const
  {
    TreeLine *parentItem = lineFromIndex( index );
	iM = 0;   
	if( parentItem->iDC == -1 ) // row for phase
	{
		iN = parentItem->iPh;
		return fldsPh[min(index.column(), fldsPh.count())];
	} else
        {	iN = parentItem->iDC;
   		    return fldsDC[index.column()];
        }	
  }
 
};


/*!
  \ class TTreeDelegate
  \ individual items in views are rendered and edited using delegates
*/
class TTreeDelegate: public QItemDelegate
{
	Q_OBJECT

public:
	
	 TTreeDelegate( QObject * parent = 0 );
	 QWidget *createEditor(QWidget *parent,
	                       const QStyleOptionViewItem &option,
	                       const QModelIndex &index) const;
	 void setEditorData(QWidget *editor, const QModelIndex &index) const;
	 void setModelData(QWidget *editor, QAbstractItemModel *model,
	                      const QModelIndex &index) const;
};

/*!
  \ class TTreeView implements a tree view 
  \ that displays items from a model. This class is used to 
  \ provide standard tree views that were previously provided by 
  \ the QTree class, but using the more flexible approach 
  \ provided by Qt's model/view architecture. 
*/
class TTreeView: public QTreeView
{
	Q_OBJECT
	
	void keyPressEvent(QKeyEvent* e);
	QString createString();
    QString createHeader();
	 
 protected slots:
    void slotPopupContextMenu(const QPoint& pos);

 public slots:
    void CmHelp();
    void CmCalc();
    void SelectRow();
    void SelectColumn();
    void SelectAll();
    void SelectGroup();
    void CopyData();
    void CopyDataHeader();

    
 public:
	 TTreeView( QWidget * parent = 0 );
	 void printList( fstream& ff );
     void resetList()
	 {
    	 ((TTreeModel *)(model() ))->setupModelData();
     }

};


#endif   // _model_tree_h
