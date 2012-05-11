//-------------------------------------------------------------------
// $Id: NewSystemDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of NewSystemDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#ifndef SystemDialog_included
#define SystemDialog_included

#include <fstream>

#include <qmenubar.h>
#include <qlineedit.h>

#include <QMainWindow>
#include <QEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QFocusEvent>

#include "gstring.h"
#include "ui_NewSystemDialog4.h"
#include "model_tree.h"


class NewSystemDialog : public QMainWindow, public Ui::NewSystemDialogData
{
    Q_OBJECT

    TTreeView *ListViewInput;
    TTreeView *ListViewResult;

    QLineEdit* pLine;
    QMenuBar * menu;

//   int pSimplexCheck;
//   int pStepwiseCheck;
//   int pRaisedCheck;

public:
    static NewSystemDialog* pDia;

    NewSystemDialog(QWidget* parent = NULL, const char* name = NULL);
    virtual ~NewSystemDialog();

//    void Update();

    void resetList()
    {
        //ListViewResult->resetList();
        Update();
    }


protected:

	void closeEvent(QCloseEvent* ev);
  //  bool event(QEvent* e);

    void setActions();

    void defineInputList();
//    void loadList1();
//    void saveList1();
    void printInputList(fstream& f);
    void defineResultList();
//    void loadList2();
    void printResultList(fstream& f);
    bool MessageToSave();
    
public slots:
    void CmSelect();
    void Update();

protected slots:
    virtual void languageChange();
    void objectChanged();

    void CmNext();
    void CmPrevious();

    void CmCreate();
    void CmNew();
    void CmRemake();
    void CmSave();
    void CmSaveAs();
    void CmDelete();

    void CmOpen_MTPARAM();
    void CmOpen_SYSTEM_ICOMP();
    void CmInsert_SYSTEM();
//    void CmCommit();
//    void CmReset();
    void CmOutMulti();
    void CmReadMulti();

    
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
    void CmHelp2();
    void CmMoreBCC();
    void CmMoreIPM();
    void CmHowto();
    void CmScript();

};
#endif // NewSystemDialog_included

