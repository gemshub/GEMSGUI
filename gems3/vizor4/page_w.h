//-------------------------------------------------------------------
// $Id: page_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TCPage, TQueryWindow and TCWindow classes
//
// Copyright (C) 1996-2009  A.Rysin,  S.Dmytriyeva
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

#ifndef _page_w_h
#define _page_w_h

#include <QScrollArea>
#include <QtGui/QStackedWidget>
#include "model_w.h"

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

    TOArray<TObjectModel*> aModels;
    TOArray<QWidget*> aFields;
    TOArray<int> aTypes;
    
    int getFieldCnt() const
    {
        return rInfo.aFieldInfo.GetCount();
    }
    
    TSubModule& getWin() const
    {
        return rInfo.rWinInfo.rM;
    }
    
protected slots:
  void objectChanged();
  
public:
    TCPage(PageInfo& pi,bool info = false, QWidget *parent = 0);
    ~TCPage();

    void AddFields( bool info = false );
    void RedrawFields();

    void show();
    void Update();

    const char* GetName() const
    {
        return rInfo.name.c_str();
    }
};

//===========================================
// TCWindow class
//===========================================

/*!
  \class TCWindow
  \brief screen representation of the Module
*/

class TCWindow: public QWidget
{
    Q_OBJECT

    CWinInfo& rInfo;
    int iCurPage;
    TOArray<TCPage*> aPages;

    QStackedWidget *pages;
    QScrollArea *scroll;

    QButtonGroup *pTab;
    QPushButton* pFilesBut;
    QLabel* pRTime;
    /*QLabel*/QLineEdit* pPackKey;
    QPushButton* pCloseBut;
//    QListWidget* pFileList;

    void ShowInfo();

    int getPageCnt() const
    {
        return rInfo.aPageInfo.GetCount();
    }

    TSubModule& getCModule() const
    {
        return rInfo.rM;
    }

protected:
    void closeEvent(QCloseEvent* evt);

protected slots:
    void EvFileDown();

public slots:
    void EvTabSelChange(int);
    void EmitEvTabSelChange(int);
//   void CmClose();

public:
    TCWindow(TCModuleImp* pImp, CWinInfo& w, int page=0);
    ~TCWindow();

    void Update();

    int curPage()
    { return iCurPage; }
};

//===========================================
// TQueryWindow class
//===========================================

/*!
  \class TQueryWindow
  \brief dialog for 'Remake' command
*/

class TQueryWindow:
            public QDialog
{
    Q_OBJECT

    CWinInfo& rInfo;
    QLabel* hint;
    TCPage* aPage;
    QScrollArea *scroll;

protected slots:
    void done(int result);

public:
    TQueryWindow(CWinInfo& w);

    void setHint(const char* txt)
    {
        hint->setText(txt);
    }
};

#endif   // _page_w_h
