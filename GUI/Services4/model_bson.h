//-------------------------------------------------------------------
// $Id: model_bson.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TBsonModel, TBsonDelegate and TBsonView,
//
// Copyright (C) 2015 S.Dmytriyeva
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

#ifndef _model_bson_h
#define _model_bson_h


#include "model_w.h"
#include "v_json.h"

#include <QAbstractItemModel>
#include <QTreeView>

class QObject;
class QPainter;

//===========================================
// TBsonModel class
//===========================================

class BsonLine
{
public:

    BsonLine( int ndx, const QString& akey, int atipe, const QString& avalue );
    ~BsonLine();

    int ndx;
    QString keyname; // name of field
    int type;        // type of field
    QString value;   // value of field

    BsonLine *parent;
    QList<BsonLine *> children;
};


/*!
  \ class TBsonModel
  \ class for represents the data set and is responsible for fetchin
  \ the data is neaded for viewing and for writing back any changes.
  \ Reading/writing data from/to bson object
  
*/
class TBsonModel: public QAbstractItemModel
{
	Q_OBJECT

    QList<FieldInfo>	flds;

    QStringList hdData;
    bson *bsonData;
    BsonLine* rootNode;

    void setupModelData();
    QString getDescription( const QModelIndex& index ) const;
    QString getValue( const QModelIndex& index ) const;
    void childChanged(const QModelIndex& index, const QVariant& value, char old_data);

 public:
	  
  TBsonModel( const QList<FieldInfo> aFlds, bson *abson,
              const QStringList& aHeaderData,   QObject* parent = 0 );
  ~TBsonModel();
	 
  QModelIndex index(int row, int column, const QModelIndex& parent) const;
  QModelIndex parent(const QModelIndex& child) const;
  int rowCount ( const QModelIndex & parent ) const;     //ok	
  int columnCount ( const QModelIndex & parent  ) const; // ok
  QVariant data ( const QModelIndex & index, int role ) const;
  bool setData ( const QModelIndex & index, const QVariant & value, int role );
  QVariant headerData ( int section, Qt::Orientation orientation, int role ) const; 
  Qt::ItemFlags flags ( const QModelIndex & index ) const;
	 
  BsonLine* lineFromIndex(const QModelIndex& index) const;
  // return plase object defined by Model
  ePlaceMode getObjectPlace();

};


/*!
  \ class TBsonDelegate
  \ individual items in views are rendered and edited using delegates
*/
class TBsonDelegate: public QItemDelegate
{
	Q_OBJECT

public:
	
     TBsonDelegate( QObject * parent = 0 );
	 QWidget *createEditor(QWidget *parent,
	                       const QStyleOptionViewItem &option,
	                       const QModelIndex &index) const;
//	 void setEditorData(QWidget *editor, const QModelIndex &index) const;
//	 void setModelData(QWidget *editor, QAbstractItemModel *model,
//	                      const QModelIndex &index) const;
};

/*!
  \ class TBsonView implements a tree view of bson structure
  \ that displays items from a model. This class is used to 
  \ provide standard tree views that were previously provided by 
  \ the QTree class, but using the more flexible approach 
  \ provided by Qt's model/view architecture. 
*/
class TBsonView: public QTreeView
{
	Q_OBJECT
	
    QList<FieldInfo>	flds;

	void keyPressEvent(QKeyEvent* e);
	QString createString();
    QString createHeader();
	 
 protected slots:
    void slotPopupContextMenu(const QPoint& pos);
    void changeCurrent( int section );

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
     TBsonView( const QList<FieldInfo> aFlds,QWidget * parent = 0 );
     void printJson( fstream& ff );
     // return current size of object defined by Model
     void getObjectSize( int& rowSize, int& colSize );
};


#endif   // _model_tree_h
