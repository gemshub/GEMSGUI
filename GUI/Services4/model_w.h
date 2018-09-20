//-------------------------------------------------------------------
// $Id: model_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TObjectModel, TObjectDelegate, TObjectTable,
// TCell, TCellInput, TCellCheck, TCellText  classes
//
// Copyright (C) 2008 S.Dmytriyeva
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

#ifndef _model_w_h
#define _model_w_h

#include "page_f.h"
#include "GemsMainWindow.h"

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QTableView>

#include <QAbstractItemDelegate>
#include <QFontMetrics>
#include <QModelIndex>
#include <QSize>

class QAbstractItemModel;
class QObject;
class QPainter;

#ifdef __APPLE__
const char  splitRow = '\r';
const char  splitCol = '\t';
#else
const char  splitRow = '\n';
const char  splitCol = '\t';
#endif

const gstring emptiness("---");
//const gstring short_emptiness("---");

struct Selection {
    int N1;
    int N2;
    int M1;
    int M2;

    Selection(int n1, int n2, int m1, int m2 ):
     N1(n1), N2(n2), M1(m1), M2(m2)
     {}

    Selection(const Selection& sel):
     N1(sel.N1), N2(sel.N2), M1(sel.M1), M2(sel.M2)
     {}

};

//inline gstring visualizeEmpty(const gstring& text);
//inline int wdF(eFieldType ft, int npos, eEdit edit );
//inline int htF(eFieldType ft, int ht);
//===========================================
// TObjectModel class
//===========================================

/*!
  \ class TObjectModel
  \ class for represents the data set and is responsible for fetchin
  \ the data is neaded for viewing and for writing back any changes.
  \ Reading/writing data from/to TObject and TObjList classes
  
*/

class TObjectModel: public QAbstractTableModel
{
	Q_OBJECT
	
  QList<FieldInfo>	flds;
	
  // get index flds and object coordinates from current item coordinate
  int getObjFromModel( int row, int col, int& nO, int& iN, int& iM, 
                  Selection* sel=0 ) const;
  
  QString getDescription( TObject* pObj, int N, int M) const;
  int rCount() const;
  int cCount() const;

  public:
	  
	 TObjectModel( const QList<FieldInfo> aFlds, 
	               QObject * parent = 0 );
	 
	 int rowCount ( const QModelIndex & parent ) const;	
	 int columnCount ( const QModelIndex & parent  ) const;
	 QVariant data ( const QModelIndex & index, int role ) const;
	 bool setData ( const QModelIndex & index, const QVariant & value, int role );
	 QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;  
	 Qt::ItemFlags flags ( const QModelIndex & index ) const;
	
	 FieldInfo& getInfo( int row, int col, int& iN, int& iM, 
                         Selection* sel=0 )
	 {
		int nO, ii;
                ii = getObjFromModel( row, col, nO, iN, iM, sel );
                if( ii>=0 )
		   return flds[ii];
                iN = iM = -1;
                return flds[0];
	 }
	 
	 // return plase object defined by Model
	 ePlaceMode getObjectPlace();

};

/*!
  \ class TObjectTable implements a table view
  \ that displays items from a model. This class is used to
  \ provide standard tables that were previously provided by
  \ the QTable class, but using the more flexible approach
  \ provided by Qt's model/view architecture.
*/

class TObjectTable: public QTableView
{
        Q_OBJECT

        QList<FieldInfo>	flds;
        bool vScroll;
        bool hScroll;

        void focusOutEvent( QFocusEvent * event );
        void focusInEvent( QFocusEvent * event );
        void keyPressEvent(QKeyEvent* e);
    //    bool edit( const QModelIndex & index, EditTrigger trigger, QEvent * event );

        void updateStatus( const QModelIndex & current);
        QList<QAction*> itemActions( const QModelIndex & current);

        Selection getSelectionRange( bool paste_ = false );
        QString createString( Selection& sel );
        QString createHeader();
        void pasteIntoArea( Selection& sel, bool transpose);
        void  setFromString(char splitrow, const QString& str,
                Selection sel, bool transpose);// throw(TError);

 protected slots:
    void currentChanged( const QModelIndex& current, const QModelIndex& previous );
    void slotPopupContextMenu(const QPoint& pos);
  //  void closeEditor( QWidget * editor, QAbstractItemDelegate::EndEditHint hint );

 public slots:
    void CmHelp();
    void CmSDRef();
    void CmDComp();
    void CmShowPhaseKey();
    void CmCalc();
    void SelectRow();
    void SelectColumn();
    void SelectAll();
    void SelectObject();
    void CutData();
    void ClearData();
    void CopyData();
    void CopyDataHeader();
    void PasteData();
    void PasteTransposedData();


 public:
         TObjectTable( const QList<FieldInfo> aFlds,
                               QWidget * parent = 0 );

         // return current size of object defined by Model
     void getObjectSize( int& rowSize, int& colSize );
     bool isText() { return( flds[0].fType == ftText );}
};




/*!
  \ class TObjectDelegate
  \ individual items in views are rendered and edited using delegates

*/

class TObjectDelegate: public QItemDelegate
{
	Q_OBJECT
	
   TObjectTable *tabl;

   bool eventFilter(QObject *object, QEvent *event);

 public:
	
   TObjectDelegate( TObjectTable *tabl, QObject * parent = 0 );
   QWidget *createEditor(QWidget *parent,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
	                      const QModelIndex &index) const;
   // QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
   // void updateEditorGeometry( QWidget * editor,
   //         const QStyleOptionViewItem & option, const QModelIndex & index ) const;
// private slots:
//	void commitAndCloseLineEditor();
//	void commitAndCloseCheckEditor();
//	void commitAndCloseTextEditor();

};


//===========================================
// TCell... classes
//===========================================

/*!
  \class TCell
  \brief Abstract base class for elementary cell
  
  Elementary cell could be Input, Multiline Input or CheckBox
*/
class TCell
{
protected:
    
    FieldInfo fld;               // description
    int iN, iM;                  // indexes in object  0 <= iN < rObj.getN() ; 0 <= iM < rObj.getM()
    QWidget* th; 
    bool edited;

public:

    TCell(FieldInfo afld, int in, int im );
    virtual ~TCell() {}

    void CmHelp();
 
    virtual QString getData()
    {
    	return QString("");
    }
    
    virtual void setData( QString )
    { }

    virtual bool dataCh()
    { return edited; }

};

/*!
  \class TCellInput
  \brief elemetary cell - Line Input 
*/
class TCellInput:
            public QLineEdit,
            public TCell
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent*);

protected slots:
    void CmHelp() { TCell::CmHelp(); }
    void setCh() { edited=true; }

public:
    TCellInput( FieldInfo afld, int in, int im, QWidget * parent = 0 );
    ~TCellInput()
    {}

    virtual QString getData()
    {
    	return text();
    }
    
    virtual void setData( QString data )
    { 
    	setText( data );
    }

};

/*!
  \class TCellCheck
  \brief elemetary cell - n-state CheckBox
*/
class TCellCheck:
            public QComboBox,
            public TCell
{
    Q_OBJECT

    gstring Vals;
    int startIndex;

protected:
    void keyPressEvent(QKeyEvent*);
 
protected slots:
    void CmHelp() { TCell::CmHelp(); }

public:
    TCellCheck(FieldInfo afld, int in, int im, QWidget * parent = 0);
    ~TCellCheck()
    {}

    virtual QString getData()
    {
    	return currentText();
    }

    virtual void setData( QString data )
     { startIndex = findText(data);
       if(startIndex >= 0 )
           setCurrentIndex(startIndex);
     }

    virtual bool dataCh()
    { return (startIndex!=currentIndex()); }

};


/*!
  \class TCellText
  \brief elemetary cell - Multiline input
*/
class TCellText:
            public QTextEdit,
            public TCell
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent*);
    bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
    void EvChange();
    void CmHelp() { TCell::CmHelp(); }
    void setCh() { edited=true; }

public:
    TCellText(FieldInfo afld, int in, int im, QWidget * parent = 0);
    ~TCellText()
    {}

    virtual QString getData()
    {
    	return toPlainText();
    }

    virtual void setData( QString data )
     { 
    	setPlainText(data);
        edited = false;
     }

};

inline
gstring visualizeEmpty(const gstring& text)
{
    return (text==S_EMPTY) ? emptiness : text;
}


inline int wdF(eFieldType ft, int npos, eEdit edit )
{
    if( ft == ftCheckBox ) {	// we'd like to get square 'checkbox cell'
//    	return 2 * pVisorImp->getCharWidth();
      int size = max( pVisorImp->getCharWidth(), pVisorImp->getCharHeight());
      if( edit == eYes )
              size *= 2;
      return size;
    }
    else {
    return npos * pVisorImp->getCharWidth();
    }
}

inline int htF(eFieldType ft, int ht)
{
    if( ft==ftText ) {
        return pVisorImp->getCharHeight()*ht/2+1;
    }
/*    else if( ft == ftCheckBox ) {
    int width = pVisorImp->getCharWidth();
    int height = pVisorImp->getCharHeight();
    return max(width, height);
    }
*/    else {
    return pVisorImp->getCharHeight();
    }
}


#endif   // _model_w_h
