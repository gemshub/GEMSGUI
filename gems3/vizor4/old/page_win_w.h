//-------------------------------------------------------------------
// $Id: page_win_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TCWindow class
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef _page_win_w_h
#define _page_win_w_h

#include <qdialog.h>
//#include <qpushbt.h>
//#include <qscrbar.h>
#include <q3listbox.h>
#include <qlabel.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QCloseEvent>

#include "page_f.h"


//===========================================
// TCWindow class
//===========================================

/*!
  \class TCWindow
  \brief screen representation of the Module
*/

class TCWindow:
            public QWidget
{
    Q_OBJECT

    CWinInfo& rInfo;

    int iCurPage;

    QScrollBar* pSV;
    QScrollBar* pSH;
    int vScr, hScr;
    int posX, posY;

    Q3ButtonGroup *pTab;
    QPushButton* pFilesBut;
    QLabel* pRTime;
    QLabel* pPackKey;
    QPushButton* pCloseBut;

    Q3ListBox* pFileList;
    void ShowInfo();

    int getPageCnt() const
    {
        return rInfo.aPageInfo.GetCount();
    }
    TCPage& getPage(int ii) const
    {
        return *rInfo.aPageInfo[ii].pPage;
    }
    TSubModule& getCModule() const
    {
        return rInfo.rM;
    }

    void CalcScrollers();

protected:
    void resizeEvent(QResizeEvent* evt);
    void closeEvent(QCloseEvent* evt);

protected slots:
    void EvFileDown();
    void EvVScroll(int);
    void EvHScroll(int);
    void AjustScrollers();

public slots:
    void EvTabSelChange(int);
    void CmClose();

public:
    TCWindow(TCModuleImp* pImp, CWinInfo& w, int page=0);
    ~TCWindow();

    void Update();
};

#endif    // _page_win_w_h
