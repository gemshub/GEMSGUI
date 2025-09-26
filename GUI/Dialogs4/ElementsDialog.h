//-------------------------------------------------------------------
// $Id: ElementsDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ElementsDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
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

#ifndef ElementsDialog_included
#define ElementsDialog_included

#include <QDialog>
#include <QButtonGroup>
#include <QStandardItemModel>
#include "filters_data.h"

namespace Ui {
class ElementsDialogData;
}

class ElementsDialog : public QDialog
{
    std::string prf_name;
    setFiltersData sf_data;
    elmWindowData  el_data;
    elmFilesConfData files_data;
    TCStringArray selNames;  // names of selected grups of files

    Q_OBJECT

    TCIntArray aBtmId1;     // lists correspondanse betvin bgElem
    TCStringArray aICkey1;  // buttons and IComp records
    TCIntArray aBtmId2;     // lists correspondanse betvin bgOther
    TCStringArray aICkey2;  // buttons and IComp records

    // from template
    TCIntArray aBtmId1_sel;  // lists from template bgElem
    TCStringArray aICkey2_sel;  // lists from template bgOther

    // working with open files
    int  isOpenFile( std::string& name );
    void setFilesList();
    void resetFilesSelection();
    void openFilesSelection();
    void openFilesICOMP();

    void setOpenFilesAsDefault();
    void setTreeWidget();
    void setSelectionTreeWidget();  // set up selection in wiget use selNames
    void getSelectionTreeWidget();
    void getTag( std::string tag, QStandardItem* pdb);
    void setTag( std::string fname, QStandardItem* pdb);
    void deleteTag( QString aTag, QStandardItem* pdb);

    // working with elements buttoms
    void EmptyData();
    void ResetData();
    void SetICompList();
    void SetData();
    void allSelected( TCStringArray& aICkeys );

    bool isAqueous() const ;
    bool isSorption() const ;

    void 	resetNextButton();
    void 	resetBackButton();

    Ui::ElementsDialogData *ui;
    QButtonGroup* bgElem;
    QButtonGroup* bgOther;
    QStandardItem* pkern;
    //FileNamesTreeModel* fileModel;

protected slots:

    void CmHelp();
    void CmOk();
    void CmSetFilters();
    void CmNext();
    void CmBack();
    void CmSelectAll();
    void CmClearSelection();
    void changeCheck( QStandardItem *item );
    void SetAqueous();
    void SetSorption();
    void SetIsotopes();
    void SetFiles();

public:

    ElementsDialog(QWidget* win, const char * prfName, elmWindowData  data,
                   const char* caption = nullptr );
    virtual ~ElementsDialog();

    const setFiltersData&  getFilters();
    const elmWindowData&   getData();

};

#endif // ElementsDialog_included


