//-------------------------------------------------------------------
// $Id$
//
// Declaration of NewSystemDialog class
//
// Copyright (C) 1996-2002  A.Rysin, S.Dmytriyeva
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

#ifndef SystemDialog_included
#define SystemDialog_included

#include <fstream>

#include <qmenubar.h>
#include <qlineedit.h>
#include <qtoolbar.h>
#include <qlistview.h>

#include "gstring.h"
#include "NewSystemDialogData.h"


class MLineEdit :
          public QLineEdit
{
   Q_OBJECT

    QToolBar* toolBar;
    QListViewItem* it;
    int col;
    int type;
    gstring Vals;

protected:
    void focusOutEvent(QFocusEvent* ev);
    void mousePressEvent(QMouseEvent* e);
//    void mouseDoubleClickEvent(QMouseEvent *ev);

protected slots:
    void SetIndex(int ii);
    void CmHide( int x, int y );
    void CmAccept();

public:

   MLineEdit( QWidget * parent, const char * name=0 );
    ~MLineEdit()
    {}

  void setData( QListViewItem *ait, int acol);
  void getData();
};


class NewSystemDialog : public NewSystemDialogData
{
    Q_OBJECT

   QMenuBar * menu;
   QToolBar* toolBar;
   QListViewItem* item1;
   QListViewItem* item2;
   MLineEdit* colEdit;


   int pSimplexCheck;
   int pStepwiseCheck;
   int pRaisedCheck;


public:
    static NewSystemDialog* pDia;

    NewSystemDialog(QWidget* parent = NULL, const char* name = NULL);
    virtual ~NewSystemDialog();

    void Update();

protected:
    void closeEvent(QCloseEvent* ev);
    bool event(QEvent* e);

    void LoadMenu();

    void loadList1();
    void saveList1();
    void printList1(fstream& f);
    void loadList2();
    void printList2(fstream& f);


protected slots:
    // for internal editing
    void CmEdit( QListViewItem *it, const QPoint & pos, int col);

    void CmNext();
    void CmPrevious();

    void CmSelect();
    void CmCreate();
    void CmNew();
    void CmRemake();
    void CmSave();
    void CmSaveAs();
    void CmDelete();

    void CmOpen_MTPARAM();
    void CmOpen_SYSTEM_ICOMP();
    void CmCommit();
    void CmReset();

    void CmRunBCC();
    void CmRunIPM();
    void CmSimplex();
    void CmPrecise();
    void CmStepwise();

    void CmOpen_EQSTAT_ICOMP();
    void CmOpen_MULTI();

    void CmPrintMtparam();
    void CmPrintSystem();
    void CmPrintEqstat();
    void CmPrInput();
    void CmPrResults();

    void CmHelp();
    void CmMoreBCC();
    void CmMoreIPM();
    void CmHowto();
    void CmScript();

    //for test
    void CmOutMulti();
    void CmReadMulti();

};
#endif // NewSystemDialog_included

