//-------------------------------------------------------------------
// $Id$
//
// Implementation of TCWindow class
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
#include <stdio.h>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qkeycode.h>
#include <qaccel.h>

#include "units.h"
#include "page_i.h"
#include "visor.h"
#include "v_module.h"
#include "m_param.h"
#include "m_sdata.h"
#include "module_w.h"
#include "page_w.h"
#include "page_win_w.h"
#include "visor_w.h"
#include "service.h"
#include "query_w.h"
#include "help.h"
#include "dlg/CalcDialog.h"
//#include "dlg/CalcCheckDialog.h"



const int wdWIN = 500;
const int htWIN = 400;

const int wdTab = 200;
const int szScroll = 15;

TCWindow::TCWindow(TCModuleImp* pImp, CWinInfo& i, int page):
        QWidget(pImp),
        rInfo(i)
{
    vScr = hScr = 0;
    posX = posY = 0;
    i.pWin = this;
    pFileList = 0;

    for( int ii=0; ii<getPageCnt(); ii++ )
    {
        PageInfo& pi = rInfo.aPageInfo[ii];
        pi.pPage = new TCPage(pi);
    }

    pSH = new QScrollBar(QScrollBar::Horizontal, this);
    connect( pSH, SIGNAL(valueChanged(int)), SLOT(EvHScroll(int)) );
    pSH->setSteps(pVisorImp->getCellWidth(), pVisorImp->getCellWidth()*3);

    pSV = new QScrollBar(QScrollBar::Vertical, this);
    connect( pSV, SIGNAL(valueChanged(int)), SLOT(EvVScroll(int)) );
    pSV->setSteps(pVisorImp->getCharHeight(), pVisorImp->getCharHeight()*3);

    pTab = new QButtonGroup(this);
    pTab->setExclusive(true);
    pTab->setGeometry(0, 0, wdTab, szTab);
    connect( pTab, SIGNAL(clicked(int)), SLOT(EvTabSelChange(int)) );

    iCurPage = -1;
    if( page >= getPageCnt() )
        page = 0;

    int pos = getPageCnt()*55+2;

    // don't show button if single page

    if( getPageCnt() > 1 )
    {
        for( int ii=0; ii<getPageCnt(); ii++ )
        {
            QPushButton* p = new QPushButton(getPage(ii).GetName(), this);
            p->setGeometry(1+ii*55, 1, 55, szTab-2);
            p->setToggleButton(true);
            pTab->insert( p );
        }

        ((QPushButton*)(pTab->find(page)))->setOn(true);

        pTab->setFixedSize( pos, szTab );
        //  pTab->show();
    }

    pos += 5;

    iCurPage = page;

    getPage(page).show();
    getPage(page).setFocus();


    if( !getCModule().IsSubModule() )
    {
        pFilesBut = new QPushButton(this);//f
        pFilesBut->setGeometry(pos, 2, szFButton, szTab-4);
        pFilesBut->setToggleButton(true);
        QToolTip::add( pFilesBut, "Show open database files" );
        pFilesBut->setText("F");
        connect( pFilesBut, SIGNAL(toggled(bool)), SLOT(EvFileDown()) );
    }

    pPackKey = new QLabel(this);
    pPackKey->move(pos + szFButton + 6, 1);
    pPackKey->setFixedHeight(szTab - 2);
    pPackKey->setFrameStyle( QFrame::Box | QFrame::Raised );
    pPackKey->setAutoResize(true);
    
    QFont pk_font = pPackKey->font();
    pk_font.setBold(true);
    pPackKey->setFont(pk_font);

    pRTime = new QLabel(this);
//    pRTime->move(pos + szFButton + 4 + 100, 1);	// moved in ShowInfo
    pRTime->setFixedHeight(szTab - 2);
    pRTime->setFrameStyle( QFrame::Box | QFrame::Raised );
    pRTime->setAutoResize(true);
//    pRTime->setAlignment( AlignCenter );

    ShowInfo();

    pCloseBut = new QPushButton("Close", this);
    connect( pCloseBut, SIGNAL(clicked()), this, SLOT(CmClose()) );

    pCloseBut->setGeometry(width()-56, 1, 55, szTab-2);
    // resize does not work on this window - we have to maipulate TCModuleWin
    parentWidget()->resize(rInfo.init_width, rInfo.init_height);
}

TCWindow::~TCWindow()
{}



void
TCWindow::closeEvent(QCloseEvent* evt)
{
    rInfo.init_width = parentWidget()->width();
    rInfo.init_height = parentWidget()->height();

    QWidget::closeEvent(evt);
}

void
TCWindow::CmClose()
{
    parentWidget()->close();
}


void
TCWindow::resizeEvent( QResizeEvent* evt )
{
    QWidget::resizeEvent( evt );

    pTab->setGeometry(0, 0, width(), szTab);
    pCloseBut->move(width()- pCloseBut->width() + 1, 1);

    if( iCurPage == -1 )
        return;
    AjustScrollers();
}

/*!
    Changes the page tab for module and calls TCModule::EvPageChanged()
*/
void
TCWindow::EvTabSelChange(int newSel)
{
    if( iCurPage == newSel )
        return;

    int prevPage = iCurPage;

    if( iCurPage != -1 )
    {
        getPage(iCurPage).hide();
        //    getPage(iCurPage).move(0,szTab);
        getPage(iCurPage).scroll(posX, posY);
    }

    iCurPage = newSel;

    //  getPage(iCurPage).move(0, szTab);
    getPage(iCurPage).Update();
    getPage(iCurPage).show();
    posX = posY = 0;
    pSH->setValue(0);
    pSV->setValue(0);
    AjustScrollers();
    getPage(iCurPage).SetFirstCellFocus();
    //  pTab->setFocus();
    // Sveta 13/03/2000 to change current page button
    ((QPushButton*)(pTab->find(iCurPage)))->setOn(true);

    if( getCModule().IsSubModule() )
        getCModule().EvPageChanged(prevPage);
}


void
TCWindow::EvHScroll(int x)
{
    getPage(iCurPage).scroll(posX-x, 0);
    posX = x;
}

void
TCWindow::EvVScroll(int y)
{
    getPage(iCurPage).scroll(0, posY-y);
    posY = y;
}

void
TCWindow::CalcScrollers()
{
    TCPage& pg = getPage(iCurPage);

    if( pg.width() < width() )
        hScr = 0;
    else
        hScr = szScroll;

    if( pg.height() < (height()-szTab) )
        vScr = 0;
    else
        vScr = szScroll;

    if( (vScr==0 && hScr==0) ||
            (vScr!=0 && hScr!=0) )
        return;

    if( vScr != 0 )    // hScr == 0
    {
        if( pg.width() > (width()-vScr) )
            hScr = szScroll;
    }
    else  // hScr != 0 , vScr == 0
    {
        if( pg.height() > (height()-szTab-hScr) )
            vScr = szScroll;
    }
}

void
TCWindow::AjustScrollers()
{
    CalcScrollers();

    pSH->setGeometry(0, height()-szScroll, width()-vScr, szScroll);
    pSV->setGeometry(width()-szScroll, szTab, szScroll, height()-szTab-hScr);

    TCPage& pg = getPage(iCurPage);

    int max;
    if( hScr != 0 )
    {
        max = pg.width() - (width()-vScr);
        pSH->setRange(0, max);
        pSH->setSteps(pVisorImp->getCellWidth(), width()-vScr);
        pSH->show();
    }
    else
        pSH->hide();

    if( vScr != 0 )
    {
        max = pg.height() - (height()-szTab-hScr);
        pSV->setRange(0, max);
        pSV->setSteps(pVisorImp->getCharHeight(), height()-szTab-hScr);
        pSV->show();
    }
    else
        pSV->hide();
}

/*! Shows information on current record
*/
void
TCWindow::ShowInfo()
{
    int nR;
    if( getCModule().pImp->IsSubModule() )
        nR = RT_SYSEQ;
    else nR = getCModule().rtNum();

    time_t time = rt[nR].Rtime();
    struct tm *time_s;
    tzset();
    time_s = localtime(&time);
    vstr str(20);
    strftime(str, 20, "%d/%m/%Y, %H:%M", time_s);

    pPackKey->setText( rt[nR].PackKey() );

    pRTime->setText( str.p );
    pRTime->move(pPackKey->x() + pPackKey->width() + 4, 1);

}

/*!
    Updates window
*/
void
TCWindow::Update()
{
    ShowInfo();
    for( int ii=0; ii<getPageCnt(); ii++ )
        getPage(ii).Update();
    AjustScrollers();
}

void
TCWindow::EvFileDown()
{
    if( !pFileList )
    {
        pFileList = new QListBox(this);
        pFileList->move(pFilesBut->x()+szFButton+4, 0);
        pFileList->setFixedSize(szTime+40, 70);
        pFileList->setVariableWidth(true);
        pFileList->show();

        const TCStringArray& s = rt[getCModule().rtNum()].GetOpenFiles();
        for( uint ii=0; ii<s.GetCount(); ii++ )
            pFileList->insertItem(s[ii].c_str());

        pFileList->setSelected(rt[getCModule().rtNum()].GetWorkFileName(), true);
    }
    else
    {
        delete pFileList;
        pFileList = 0;
    }
}


//---------------------- End of file  page_win_w.cpp ---------------------------
