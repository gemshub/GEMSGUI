//-------------------------------------------------------------------
// $Id: model_bson.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TBsonModel, TBsonDelegate and TBsonView,
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

#include <QtGui>

#include "model_bson.h"
#include "CalcDialog.h"
#include "GemsMainWindow.h"
#include "service.h"
#include "ipm2format.h"

BsonLine::BsonLine( int andx, const QString& akey, int atype, const QString& avalue):
  ndx( andx), keyname( akey), type( atype ), value( avalue)
{
    parent = 0;
}

BsonLine::~BsonLine()
{
    qDeleteAll(children);
}

void bson_to_list( const char *data, int datatype, BsonLine* parent );

void bson_to_list( const char *data, int datatype, BsonLine* parent )
{
    bson_iterator i;
    QString key;
    QString value;
    int ndx=0;

    bson_iterator_from_buffer(&i, data);
    while (bson_iterator_next(&i))
    {
       bson_type t = bson_iterator_type(&i);
       if (t == 0)
          break;
       key = bson_iterator_key(&i);

       switch (t)
       {
         // impotant datatypes
         case BSON_NULL:
              value = "null";
             break;
         case BSON_BOOL:
              value =( bson_iterator_bool(&i) ? "true" : "false");
              break;
         case BSON_INT:
              value = QString( "%1").arg( bson_iterator_int(&i));
              break;
         case BSON_LONG:
              value = QString( "%1").arg( (uint64_t) bson_iterator_long(&i));
              break;
         case BSON_DOUBLE:
              //value = QString( "%1").arg( bson_iterator_double(&i));
              value.setNum( bson_iterator_double(&i), 'g', 10 );
              break;
         case BSON_STRING:
              value = QString( "%1").arg( bson_iterator_string(&i));
              break;
         // main constructions
         case BSON_OBJECT:
         case BSON_ARRAY:
              value = "";
              break;
         default:
              Error("bson_to_list" ,"can't print type " );
       }

       BsonLine *line = new BsonLine(ndx++, key, t , value );
       line->parent = parent;
       parent->children.append( line );

       switch (t)
       {
          // main constructions
          case BSON_OBJECT:
                // !! next level
               bson_to_list( bson_iterator_value(&i), BSON_OBJECT, line );
                 break;
          case BSON_ARRAY:
               // !! next level
               bson_to_list( bson_iterator_value(&i), BSON_ARRAY, line );
               break;
       }

    }
}

void list_to_bson(BsonLine *line, bson* bsonobj );

void list_to_bson_full(BsonLine *headline, bson* bsonobj )
{
    bson_destroy(bsonobj );
    bson_init( bsonobj );
    for(int ii=0; ii< headline->children.size(); ii++ )
      list_to_bson( headline->children[ii],  bsonobj );
    bson_finish( bsonobj );
}

void list_to_bson(BsonLine *line, bson* bsonobj )
{
    int ii;

    switch (line->type)
    {
      // impotant datatypes
      case BSON_NULL:
           bson_append_null(bsonobj, line->keyname.toUtf8().data());
           break;
      case BSON_BOOL:
           bson_append_bool(bsonobj, line->keyname.toUtf8().data(),
              ( line->value == "true"? true : false));
           break;
      case BSON_INT:
      case BSON_LONG:
      case BSON_DOUBLE:
           bson_append_double(bsonobj, line->keyname.toUtf8().data(), line->value.toDouble() );
           break;
      case BSON_STRING:
           bson_append_string(bsonobj, line->keyname.toUtf8().data(), line->value.toUtf8().data() );
           break;
      // main constructions
      case BSON_OBJECT:
           bson_append_start_object(bsonobj, line->keyname.toUtf8().data());
           for( ii=0; ii< line->children.size(); ii++ )
             list_to_bson( line->children[ii],  bsonobj );
           bson_append_finish_object( bsonobj );
           break;
    case BSON_ARRAY:
        bson_append_start_array(bsonobj, line->keyname.toUtf8().data());
        for( ii=0; ii< line->children.size(); ii++ )
          list_to_bson( line->children[ii],  bsonobj );
        bson_append_finish_array( bsonobj );
        break;
      default:
           Error("list_to_bson" ,"can't print type " );
    }

}


//--------------------------------------------------------------------------------------
//  class TBsonModel
//  class for represents the data set and is responsible for fetchin
//  the data is neaded for viewing and for writing back any changes.
//  Reading/writing data from/to TObject and TObjList classes
//---------------------------------------------------------------------------------------
TBsonModel::TBsonModel( const QList<FieldInfo> aFlds, bson *abson, const QStringList& aHeaderData,   QObject* parent ):
        QAbstractItemModel(parent), hdData( aHeaderData)
{
  flds = aFlds;
  bsonData = abson;
  rootNode = 0;
  setupModelData();
  
  connect( this, SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& )),
  			parent,  SLOT(objectChanged()) );	 
}

TBsonModel::~TBsonModel()
{
  if(rootNode )
     delete rootNode;
}

BsonLine *TBsonModel::lineFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<BsonLine *>(index.internalPointer());
    } else {
        return rootNode;
    }
}


void TBsonModel::setupModelData()
{

#if QT_VERSION >= 0x050000
    beginResetModel();
    if( rootNode )
      delete rootNode;

    rootNode = new BsonLine(0, "start", BSON_OBJECT , "" );
    bson_to_list( bsonData->data, BSON_OBJECT, rootNode );
    endResetModel();
#else
    if( rootNode )
      delete rootNode;

    rootNode = new BsonLine(0, "start", BSON_OBJECT , "" );
    bson_to_list( bsonData->data, BSON_OBJECT, rootNode );
    reset();
#endif

}

QModelIndex TBsonModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!rootNode)
        return QModelIndex();
    BsonLine *parentItem = lineFromIndex( parent );
    return createIndex(row, column, parentItem->children[row]);
}

QModelIndex TBsonModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();
 
    BsonLine *childItem = lineFromIndex(child);
    BsonLine *parentItem = childItem->parent;
    if (parentItem == rootNode )
        return QModelIndex();
    return createIndex(parentItem->ndx, 0, parentItem);
}


int TBsonModel::rowCount( const QModelIndex& parent ) const
{
   if (!rootNode)
       return 0;
  if (parent.column() > 0)
      return 0;
  BsonLine *parentItem = lineFromIndex( parent );
  return parentItem->children.count();
}	

int TBsonModel::columnCount( const QModelIndex& parent ) const
{
  return 3;
}	

Qt::ItemFlags TBsonModel::flags( const QModelIndex& index ) const
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  BsonLine *item = lineFromIndex( index );
  if( index.column() == 1 && (item->type != BSON_OBJECT && item->type != BSON_ARRAY ) )
   {  flags |= Qt::ItemIsEditable;
             return flags;
   }
  else
     return (flags & ~Qt::ItemIsEditable);
}

QVariant TBsonModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
 if( role == Qt::DisplayRole  && orientation == Qt::Horizontal )
	   return hdData[section];
  return QVariant();
}

QString TBsonModel::getDescription( const QModelIndex& index ) const
{
    BsonLine *item = lineFromIndex( index );
    bool ok;
    item->keyname.toInt(&ok,10);
    if( ok && item->parent->type ==  BSON_ARRAY )
      item = item->parent;
    // !!!  Used only for one tipe object,  must be changed in future
    TRWArrays fldsDesk( fo_sMod, MULTI_fields );
    int ii = fldsDesk.findFld(item->keyname.toUtf8().data());
    if( ii>=0 )
      return QString( fldsDesk.getComment( ii ).c_str());
    return item->keyname; // may be string from io2format array
}

QVariant TBsonModel::data( const QModelIndex& index, int role ) const
{
   if(!index.isValid())
	 return QVariant();	

   switch( role )
   { case Qt::DisplayRole:
     case Qt::EditRole:
            {  BsonLine *item = lineFromIndex( index );
               if( index.column()== 0 )
                   return item->keyname;
               else
                   if( index.column()== 1 )
                       return item->value;
                   else
                       return getDescription( index );
           }
      case Qt::ToolTipRole:
      case Qt::StatusTipRole:
              return  getDescription(index );
      default: break;
   }

   return QVariant();
}

bool TBsonModel::setData( const QModelIndex& index, const QVariant& value, int role)
{
	if( index.isValid() && ( role == Qt::EditRole ) )
	{
      if(  index.column()== 1)
		  {	 
             BsonLine *line =lineFromIndex(index);
             line->value = QVariant(value).toString();
             list_to_bson_full( rootNode, bsonData );
             // !!!  Used only for one tipe object, must be changed in future
             TProfil::pm->fromBsonObject( (bson *)aObj[flds[0].nO].GetPtr() );
		  }	 
	 return true;
	} 
	return false;
}

// return place of object defined by Model
ePlaceMode TBsonModel::getObjectPlace()
{
  setupModelData();
  return flds[0].place;
}


//-------------------------------------------------------------------------------------
// TBsonDelegate -  individuak items in views are rendered and edited using delegates
//-------------------------------------------------------------------------------------

TBsonDelegate::TBsonDelegate( QObject * parent ):
	QItemDelegate( parent )
{
}

// Editing QTreeView for objects in System page
QWidget *TBsonDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    BsonLine *line =  ((TBsonModel *)(index.model() ))->lineFromIndex(index);
    if( index.column() > 0 )
    {
        switch( line->type  )
        {
        case BSON_BOOL:
                { QComboBox *accessComboBox = new QComboBox(parent);
                  accessComboBox->addItem(tr("false"));
                  accessComboBox->addItem(tr("true"));
                  return accessComboBox;
                 }
        case BSON_INT:
        case BSON_LONG:
        case BSON_DOUBLE:
               { QLineEdit *lineEdit = new QLineEdit(parent);
                 lineEdit->setValidator(new QDoubleValidator(lineEdit));
                 //lineEdit->setValidator(new QDoubleValidator(-999.0,
                 //            999.0, 2, lineEdit));
                 return lineEdit;
               }
        case BSON_NULL:
        case BSON_STRING:
              { QLineEdit *lineEdit = new QLineEdit(parent);
                return lineEdit;
              }
        // main constructions
       case BSON_OBJECT:
       case BSON_ARRAY:
       default:
             Error("TBsonDelegate" ,"can't print type " );

       }
    }	
   return QAbstractItemDelegate::createEditor( parent, option,  index );
}


//-------------------------------------------------------------------------------------
// class TBsonView implements a tree view that displays items from a model to bson.
//-------------------------------------------------------------------------------------

TBsonView::TBsonView( const QList<FieldInfo> aFlds,QWidget * parent ):
     QTreeView( parent ), flds(aFlds)
{
     setFont( pVisorImp->getCellFont() );
     setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
#if QT_VERSION >= 0x050000
     header()->setSectionsClickable(true);
     header()->setSectionResizeMode( /*QHeaderView::ResizeToContents*/QHeaderView::Interactive );

#else
     header()->setResizeMode( /*QHeaderView::ResizeToContents*/QHeaderView::Interactive );

#endif
	    setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
	    setFocusPolicy(Qt::StrongFocus);
	    setTabKeyNavigation( false );
        setContextMenuPolicy(Qt::CustomContextMenu);
        setSelectionMode( QAbstractItemView::/*MultiSelection*/ExtendedSelection );
        setSelectionBehavior( QAbstractItemView::SelectItems );
        setIndentation( 20 );

        connect( this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotPopupContextMenu(QPoint)));
        connect( header(), SIGNAL(sectionClicked(int)),
             this, SLOT(changeCurrent(int)));
}

void TBsonView::changeCurrent( int section )
{
    if( !currentIndex().isValid() )
    { QModelIndex index = model()->index( 0, section, rootIndex());
      setCurrentIndex(index);
    }
}
// return current size of object defined by Model
void TBsonView::getObjectSize( int& rowSize, int& colSize )
{
       int fullrowSize, fullcolSize;
       int ii=0;

    // flds[ii].fType == ftBson )
    colSize =  wdF(flds[ii].fType, flds[ii].npos, flds[ii].edit)+1;
    rowSize  =  htF(flds[ii].fType, flds[ii].maxN )+1;

   fullcolSize = ( header()->sectionSize(0) *  model()->columnCount() );
   fullrowSize  = ( header()->sectionSize(0) *  model()->rowCount()  );

   if( fullcolSize > colSize )
   {
       rowSize += fontMetrics().height()+1;
   }

   if( fullrowSize > rowSize )
   {	colSize += fontMetrics().width('M')*2+1;
   }

   // for frame
   if( colSize > 0 && rowSize > 0 )
   { colSize +=  4;
     rowSize +=  4;
   }

   int  colHead = model()->headerData( 0,  Qt::Vertical, Qt::SizeHintRole ).toSize().width();
   for( ii=1; ii<model()->rowCount(); ii++ )
           colHead = max( colHead, model()->headerData( ii,  Qt::Vertical, Qt::SizeHintRole ).toSize().width());
   colSize += colHead;

   int  rowHead = model()->headerData( 0,  Qt::Horizontal, Qt::SizeHintRole ).toSize().height();
   for( ii=1; ii<model()->columnCount(); ii++ )
           rowHead = max( rowHead, model()->headerData( ii,  Qt::Horizontal, Qt::SizeHintRole ).toSize().height());
   rowSize += rowHead;
       // 	colSize += verticalHeader()->width();
       //   rowSize += horizontalHeader()->height();

}

 void TBsonView::slotPopupContextMenu(const QPoint &pos)
 {
    QModelIndex index = indexAt( pos );
    TBsonModel* model =(TBsonModel *)index.model();
    BsonLine *line =  model->lineFromIndex(index);

    QMenu *menu = new QMenu(this);
  //   no_menu1 = false;
  
     QAction* act =  new QAction(tr("&Help"), this);
     act->setIcon(QIcon(":/menu/Icons/ShowHelpWindowIcon.png"));
     act->setShortcut(tr("F1"));
     act->setStatusTip(tr("Help for specified cell"));
     connect(act, SIGNAL(triggered()), this, SLOT(CmHelp()));
     menu->addAction(act);
          	    
     if( index.column()==1 && (line->type == BSON_INT || line->type == BSON_LONG ||
                line->type == BSON_DOUBLE )  )
         {
     	    menu->addSeparator();
            act =  new QAction(tr("&Calculator"), this);
            act->setShortcut(tr("F8"));
            act->setStatusTip(tr("Use Calculator for specified cells"));
       	    connect(act, SIGNAL(triggered()), this, SLOT(CmCalc()));
            menu->addAction(act);
         }
       menu->addSeparator();
         
   	   act =  new QAction(tr("Select &row"), this);

        act->setShortcut(tr("Ctrl+R"));
        act->setStatusTip(tr("Select current row"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectRow()));
        menu->addAction(act);
 
        act =  new QAction(tr("Select co&lumn"), this);
        act->setShortcut(tr("Ctrl+L"));
        act->setStatusTip(tr("Select curent column"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectColumn()));
        menu->addAction(act);
       
        act =  new QAction(tr("Select &group"), this);
        act->setShortcut(tr("Ctrl+G"));
        act->setStatusTip(tr("Select current phase"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectGroup()));
        menu->addAction(act);

        act =  new QAction(tr("Select &all"), this);
        act->setShortcut(tr("Ctrl+A"));
        act->setStatusTip(tr("Select all"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(SelectAll()));
        menu->addAction(act);

        menu->addSeparator();

        act =  new QAction(tr("&Copy"), this);
        act->setShortcut(tr("Ctrl+C"));
        act->setStatusTip(tr("Copy selected data"));
   	    connect(act, SIGNAL(triggered()), this, SLOT(CopyData()));
        menu->addAction(act);

        act =  new QAction(tr("Copy wit&h names"), this);
        act->setShortcut(tr("Ctrl+H"));
        act->setStatusTip(tr("Copy selected header&cells"));
  	    connect(act, SIGNAL(triggered()), this, SLOT(CopyDataHeader()));
        menu->addAction(act);

       menu->exec( viewport()->mapToGlobal(pos) );
       delete menu;
 }

  void TBsonView::keyPressEvent(QKeyEvent* e)
 {
	    if ( e->modifiers() & Qt::ControlModifier ) 
	    {
			switch ( e->key() ) 
			{
			  case Qt::Key_R:
			    SelectRow();
			    return;
			  case Qt::Key_L:
			    SelectColumn();
			    return;
			  case Qt::Key_G:
			    SelectGroup();
			    return;
			  case Qt::Key_A:
			    SelectAll();
			    return;
			  case Qt::Key_C:
			    CopyData();
			    return;
			  case Qt::Key_H:
			    CopyDataHeader();
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
 	QTreeView::keyPressEvent(e);
 }
   
  void TBsonView::SelectRow()
  {
    selectionModel()->select( currentIndex(), 
    		QItemSelectionModel::Rows|QItemSelectionModel::Select ); 
  }

  void TBsonView::SelectColumn()
  {
      selectionModel()->select( currentIndex(),
              QItemSelectionModel::Columns|QItemSelectionModel::Select );
  }

  void TBsonView::SelectAll()
  {
     selectAll();
  }

  void TBsonView::SelectGroup()
  {
    QModelIndex parIndex = model()->parent(currentIndex());
    for(int row = 0; row < model()->rowCount( parIndex ); row++ ) 
     {
    	QModelIndex  childIndex = model()->index( row, 0, parIndex);
        selectionModel()->select( childIndex, 
        		QItemSelectionModel::Rows|QItemSelectionModel::Select ); 
     }
 }

  QString TBsonView::createString()
  {
      QString cText;
	  QString clipText;
	  QModelIndexList selIndexes = selectedIndexes();	  

	 int row, rw, col;
	 QModelIndex index, childIndex;
     for( row = 0; row < model()->rowCount( rootIndex() ); row++ ) 
     {
       	 childIndex = model()->index( row, 0, rootIndex());
         bool frst = true;
       	 for( col = 0; col < model()->columnCount( rootIndex() ); col++ ) 
	         {
	            index = model()->index(row, col, rootIndex() );
	          	if( selIndexes.contains( index ) )
	          	{
	      		  if( !frst )
                    clipText += splitCol;
	      		  frst = false;
	      		  cText = model()->data(index).toString();
	      		  if( cText == emptiness.c_str() )
	      			  cText = "  ";//"\r"; 
	      	     clipText += cText;
	          	}
	         }  
       	 if( !frst )
             clipText += splitRow;

	     for (rw = 0; rw < model()->rowCount( childIndex ); rw++ ) 
	     {
	    	 frst = true; 
             for (col = 0; col < model()->columnCount( childIndex ); col++ ) 
	         {
	             index = model()->index(rw, col, childIndex );
		       	 if( selIndexes.contains( index ) )
		         {
		      	   if( !frst )
                      clipText += splitCol;
		      	    frst = false;
		      		cText = model()->data(index).toString();
		      		if( cText == emptiness.c_str() )
		      			  cText = "  ";//"\r"; 
		      	    clipText += cText;
		         }
	        }  
       	   if( !frst )
            clipText += splitRow;
	   }
      
     }
	return clipText;  
  }

  QString TBsonView::createHeader()
  {
    QString cText;
	QString clipText;
    int col;
    bool frst = true;
    for( col = 0; col < model()->columnCount( rootIndex() ); col++ ) 
	{
	    if( selectionModel()->columnIntersectsSelection( col,  rootIndex() ) )
       	{
   		  if( !frst )
            clipText += splitCol;
   		  frst = false;
   		  cText = model()->headerData( col, Qt::Horizontal, Qt::DisplayRole ).toString();
   		  if( cText == emptiness.c_str() )
   			  cText = "  ";//"\r"; 
   	      clipText += cText;
	    }
	 }  
     if( !frst )
        clipText += splitRow;
	return clipText;  
  }

  void TBsonView::CopyData()
  {
 	QString clipText = createString();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
  }

  void TBsonView::CopyDataHeader()
  {
 	QString clipText = createHeader();
	clipText += createString();
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
  }

  void TBsonView::CmCalc()
  {
    QModelIndex index = currentIndex();
    TBsonModel *model = (TBsonModel *)index.model();
    BsonLine *line =  model->lineFromIndex(index);

    if( (line->type == BSON_INT || line->type == BSON_LONG ||
                line->type == BSON_DOUBLE )  )
     {
       CalcDialog calc( topLevelWidget(), -1 );
       if( calc.exec() )
        {
            QString res = calc.fun( index.data(Qt::EditRole).toDouble() );
            model->setData(index, res, Qt::EditRole);
       }
    }
   }

  // Help on F1 pressed on data field
    void TBsonView::CmHelp()
    {
      QModelIndex index = currentIndex();
      BsonLine *line =  ((TBsonModel *)index.model())->lineFromIndex(index);
      bool ok;
      line->keyname.toInt(&ok,10);
      if( ok && line->parent->type ==  BSON_ARRAY )
        line = line->parent;

      string hlp= line->keyname.toUtf8().data();
      cout << hlp.c_str() << endl;
      pVisorImp->OpenHelp( 0, hlp.c_str() );
    }
  
//---------------------- End of file  model_bson.cpp ---------------------------
