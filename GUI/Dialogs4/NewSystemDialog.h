//-------------------------------------------------------------------
// $Id: NewSystemDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of NewSystemDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
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

#include "ui_NewSystemDialog4.h"
#include "model_tree.h"

class NewSystemDialog : public QMainWindow, public Ui::NewSystemDialogData
{
    Q_OBJECT

    TTreeView *ListViewInput;
    TTreeView *ListViewResult;

    //--QLineEdit* pLine;
    //--QMenuBar * menu;

public:
    static NewSystemDialog* pDia;

    NewSystemDialog(QWidget* parent = NULL, const char* name = NULL);
    virtual ~NewSystemDialog();

    void resetList()
    {
        //ListViewResult->resetList();
        Update();
    }

    void setCurrentTab( int ii )
    {
       TabWid->setCurrentIndex(ii);
    }

    QSize sizeHint() const;

protected:

      void closeEvent(QCloseEvent* ev);
      //--void setActions();

    void defineInputList();
    void printInputList(fstream& f);
    void defineResultList();
    void printResultList(fstream& f);
    bool MessageToSave();
    
public slots:
    void Update();
    virtual void languageChange();
    void objectChanged();

    void CmSelect( const char *key=0  );
    //void CmNext();
    //void CmPrevious();
    void CmCreate();
    void CmNew();
    void CmRemake();
    void CmSave();
    void CmSaveAs();
    void CmDelete();

    //--void CmOpen_MTPARAM();
    //--void CmOpen_SYSTEM_ICOMP();
    //--void CmInsert_SYSTEM();
    void CmOutMulti();
    void CmReadMulti();
    void CmRunBCC();
    //--void CmRunIPM();
    //--void CmOpen_EQSTAT_ICOMP();
    //--void CmOpen_MULTI();
    void CmPrintMtparam();
    void CmPrintSystem();
    void CmPrintEqstat();
    void CmPrInput();
    void CmPrResults();

    //--void CmHelp();
    void CmHelp2();
    //--void CmMoreBCC();
    //--void CmMoreIPM();
    //--void CmHowto();
    //--void CmScript();

};

#endif // NewSystemDialog_included

