//-------------------------------------------------------------------
// $Id$
//
// Declaration of TCPage, TField, TCell, TCellInput
//  TCellCheck, TCellText  classes
//
// Copyright (C) 1996-2001  A.Rysin
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license  
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _page_w_h
#define _page_w_h

#include <qdialog.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qevent.h>
#include <qcolor.h>
#include <qtooltip.h>

#include "v_object.h"
#include "module_w.h"
#include "page_s.h"
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
    TObject& rObj;
    int N, M;
    const bool edit;
    const eShowType showType;

    QColor backgroundColor;

    void CmHelp();

public:
    QWidget* pw;

    TCell(TObject& ro, int n, int m, bool edit_, eShowType showType_, QWidget* p);
    virtual ~TCell() {}

    // reread value from Object and display
    virtual void updateDisplay() = 0;
    // for scrolling (set Value in Object & set description)
    virtual void setIfChanged() {}

    void SetDescription();
    void changeCellNM(int n, int m)
    {
        N = n; M = m;
        updateDisplay();
        QToolTip::add(pw, (rObj.GetDescription(rObj.ndx(N,M))).c_str() );
    }

    TField* field() const
    {
        return (TField*)pw->parentWidget();
    }

    void updateCellBackground();
    void setGroupSelected(bool selected);
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
    QPopupMenu* createPopupMenu();
    void closeEvent(QCloseEvent*);
    void setIfChanged();

protected slots:
    void CmSDRef();
    void CmDComp();
    void CmCalc();
    void CmSelectObject();
    void CmHelp() { TCell::CmHelp(); }

public:
    TCellInput(TField&, int x, int y, eFieldType ft, int npos, eShowType,
               TObject&, int N, int M, bool ed, int ht);
    ~TCellInput()
    {}

    void updateDisplay();
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
    QPopupMenu* createPopupMenu();

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
            public QTextEdit,
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
    QPopupMenu* createPopupMenu(const QPoint& pos);
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


//===========================================
// TField class
//===========================================

/*!
  \class TField
  \brief represents screen view of the Object
  
  Class TFiled contains N*M elementary cells (TCell derivatives)
  Supports scrolled view with given parameters of viewable area
*/

class TField:
            public QWidget
{
    Q_OBJECT

    const FieldInfo& rInfo;

    TOArray<TCell*> aCtrl;
    int N, M;
    int iN, iM;	// current left & upper cells
    bool largeN, largeM;
    bool Lab;
    int xx, yy;

    int X, Y;		// OWL

    TCell* focused;

    bool selected;
    int selectN1, selectN2;
    int selectM1, selectM2;

    QString groupUndoContents;

    QScrollBar* pSV;
    QScrollBar* pSH;

    TObject& GetObj()
    {
        return *rInfo.pObj;
    }

    void setYPos(int pos);
    void setXPos(int pos);

    QString createString();
    void setFromString(const QString& str) throw (TError);

protected slots:
    void EvVScroll(int);
    void EvHScroll(int);

public:
    //enum ScrollPlace { SCR_NO=0, SCR_FRONT=1, SCR_BACK=2 };

    TField* pTied;	// synchronized vertical scroll
    short indTied;
    TField* pSticked;	// synchronized horizontal scroll
    short indSticked;


    TField(QWidget* p, const FieldInfo& fi, int x, int y,
           TField* tied, TField* sticked);
	   //, ScrollPlace tiedScroll, ScrollPlace stickedScroll);
    ~TField();

//    void AddVScroller(ScrollPlace scrPlace);
//    void AddHScroller(ScrollPlace scrPlace);

    TCPage* getPage() { return (TCPage*)parentWidget(); }

    bool SizeChanged();
    int GetW() const
    {
        return xx;
    }
    int GetH() const
    {
        return yy;
    }
    void SetFirstCellFocus();
    void setFocused(TCell* cell);

    void setSelected(bool selected);
    bool isSelected() const { return selected; }
    
public slots:
    void Update();
    void CmCopyToClipboard();
    void CmPasteFromClipboard();
    void selectionChanged();
    void objectChanged();
};


//===========================================
// TPage class
//===========================================

/*!
  \class TCPage
  \brief screen representation of one page of the Module
*/

class TCPage:
            public QWidget
{
    Q_OBJECT

    PageInfo& rInfo;

    int x,y;
    int mwidth, mheight;	// size of the Page

    bool fRedraw;
    bool firstRedraw;

    int getFieldCnt() const
    {
        return rInfo.aFieldInfo.GetCount();
    }
    TField* getField(int ii) const
    {
        return rInfo.aFieldInfo[ii].pField;
    }
    TSubModule& getWin() const
    {
        return rInfo.rWinInfo.rM;
    }

protected slots:
    //  void sizeEvent(QSizeEvent*);

public:
    TCPage(PageInfo& pi);
    ~TCPage();

    void AddFields();
    void RedrawFields();

    void show();
    void Update();

    void SetFirstCellFocus();
    void clearSelectedObjects();

    const char* GetName() const
    {
        return rInfo.name.c_str();
    }
};


#endif   // _page_w_h
