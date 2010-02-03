//-------------------------------------------------------------------
// $Id: page_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TCell, TCellInput
//  TCellCheck, TCellText  classes
//
// Copyright (C) 2008 S.Dmytriyeva
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

#ifndef _cell_w_h
#define _cell_w_h


#include "v_object.h"
#include "module_w.h"
#include "page_f.h"
#include "verror.h"


class QScrollBar;

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
    
	TObject* rObj;                // object 
	int iN, iM;                  // indexes in object  0 <= iN < rObj.getN() ; 0 <= iM < rObj.getM()
	
	const eFieldType fType;      // output field type
	const int nPos;             // field window length or units index for F_CHECKBOX
    const bool edit;             // read-only (+-?) 
    const eShowType showType;    // color type (not used ) 

    QColor backgroundColor;

    bool selected;
    QWidget* th; 
    
public:

    TCell(FieldInfo fld, int in, int im );
    virtual ~TCell() {}

    void SetDescription();
    void CmHelp();
    
    
    int GetN() const { return rObj->GetN(); }
    int GetM() const { return rObj->getM(); }
 
    void saveData( QString str )
    { rObj->SetString( str.latin1(), iN, iM ); } 
    
    QString loadData()
    { return QString(rObj->GetString( str.latin1(), iN, iM );  } 

    
    void setSelected( bool cellSelected)
    {
    	selected = cellSelected;
    }

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

    const eFieldType fieldType;

    void setValue();

signals:
    void fieldUpdate();

protected:
    void focusOutEvent(QFocusEvent*);
    void focusInEvent(QFocusEvent*);
    void keyPressEvent(QKeyEvent*);
//    void mousePressEvent(QMouseEvent*);
    Q3PopupMenu* createPopupMenu();
    void closeEvent(QCloseEvent*);
    void setIfChanged();

protected slots:
    void CmSDRef();
    void CmDComp();
    void CmCalc();
    void CmSelectObject();
    void CmSelectColumn();
    void CmSelectRow();
//    void CmSelectCell();
    void CmHelp() { TCell::CmHelp(); }
    void CmSelectAll();

public:
    TCellInput(TField&, int x, int y, eFieldType ft, int npos, eShowType,
               TObject&, int N, int M, bool ed, int ht);
    ~TCellInput()
    {}

    void updateDisplay();
    void setGroupSelected(bool selected);
    
public slots:
    void clear();
    void copy();
    void cut();
    void paste();
    void pasteTransposed();
};


/*!
  \class TCellCheck
  \brief elemetary cell - n-state CheckBox
*/

class TCellCheck:
            public QLineEdit,
            public TCell
{
    Q_OBJECT

    gstring Vals;
    uint currentIndex;

    void SetString(const gstring& s);

signals:
    void fieldUpdate();

protected:
    void keyPressEvent(QKeyEvent*);
    void focusOutEvent(QFocusEvent*);
    void focusInEvent(QFocusEvent*);
//    void mousePressEvent(QMouseEvent*);
    Q3PopupMenu* createPopupMenu();

protected slots:
    void SetIndex(int ii);
    void CmCalc();
    void CmHelp() { TCell::CmHelp(); }

public:
    TCellCheck(TField&, int x, int y, int npos, eShowType,
               TObject&, int N, int M, bool ed=true);
    ~TCellCheck()
    {}

    void updateDisplay();
};


/*!
  \class TCellText
  \brief elemetary cell - Multiline input
*/

class TCellText:
            public Q3TextEdit,
            public TCell
{
    Q_OBJECT

    void setValue();

signals:
    void fieldUpdate();

protected:
    void focusOutEvent(QFocusEvent*);
    void focusInEvent(QFocusEvent*);
    void keyPressEvent(QKeyEvent*);
//    void contextMenuEvent(QContextMenuEvent*);
    Q3PopupMenu* createPopupMenu(const QPoint& pos);
    void closeEvent(QCloseEvent*);
    void setIfChanged();

protected slots:
    void EvChange();
    void CmHelp() { TCell::CmHelp(); }

public:
    TCellText(TField&, int x, int y, int npos, eShowType,
              TObject&, int N, int M, bool ed, int ht);
    ~TCellText()
    {}

    void updateDisplay();
};




#endif   // _cell_w_h
