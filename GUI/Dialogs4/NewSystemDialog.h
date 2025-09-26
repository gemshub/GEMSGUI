//-------------------------------------------------------------------
// $Id: NewSystemDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of NewSystemDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef SystemDialog_included
#define SystemDialog_included

#include <fstream>
#include <QMainWindow>
#include <QEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QFocusEvent>

#include "model_tree.h"

namespace Ui {
class NewSystemDialogData;
}

class NewSystemDialog : public QMainWindow
{
    Q_OBJECT

    Ui::NewSystemDialogData *ui;
    TTreeView *ListViewInput;
    TTreeView *ListViewResult;
    TObjectTable* PTitle = nullptr;
    TObjectTable* PComment = nullptr;

    TObjectModel* MTitle = nullptr;
    TObjectModel* MComment = nullptr;

    //--QLineEdit* pLine;
    //--QMenuBar * menu;

public:
    static NewSystemDialog* pDia;

    NewSystemDialog(QWidget* parent = nullptr, const char* name = nullptr);
    virtual ~NewSystemDialog();

    void resetList()
    {
        //ListViewResult->resetList();
        Update();
    }

    void setCurrentTab( int ii );

    QSize sizeHint() const;
    void clearEditFocus();  // commit last editor if exist

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
    void objectChanged();

    void CmSelect( const char *key=nullptr  );
    void CmNext();
    void CmPrevious();
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
    void CmProjectExportJson();
    void CmProjectImportJson();
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

