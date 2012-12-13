//-------------------------------------------------------------------
// $Id: ElementsDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ElementsDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
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

#ifndef ElementsDialog_included
#define ElementsDialog_included

#include <QDialog>
#include <QButtonGroup>
#include <QStandardItemModel>

#include "ui_ElementsDialog4.h"
#include "filters_data.h"

class ElementsDialog : public QDialog, public Ui::ElementsDialogData
{
    gstring prf_name;
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
    int  isOpenFile( gstring& name );
    void setFilesList();
    void resetFilesSelection();
    void openFilesSelection();
    void openFilesICOMP();

    void setOpenFilesAsDefault();
    void setTreeWidget();
    void setSelectionTreeWidget();  // set up selection in wiget use selNames
    void getSelectionTreeWidget();
    void getTag( gstring tag, QStandardItem* pdb);
    void setTag( gstring fname, QStandardItem* pdb);
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
    void changeCheck( QStandardItem *item );
    void SetAqueous();
    void SetSorption();
    void SetIsotopes();

    void SetFiles();
    virtual void languageChange();
    
public:

    ElementsDialog(QWidget* win, const char * prfName, elmWindowData  data,
         const char* caption = 0 );
    virtual ~ElementsDialog();

    const setFiltersData&  getFilters();
    const elmWindowData&   getData();

};

/*
class TreeFileLine
{
public:

    TreeFileLine(int aRow,gstring aTag, gstring aVer, TreeFileLine* aParent);
    ~TreeFileLine();
    void printTest();

    int row;
    gstring tag;
    gstring ver;

    TreeFileLine *parent;
    QList<TreeFileLine *> children;
};


// class FileNamesTreeModel
class FileNamesTreeModel: public QStandardItemModel
{
    Q_OBJECT

  TCStringArray fnamesData;

  TreeFileLine* rootNode;

  TreeFileLine* lineFromIndex(const QModelIndex& index) const;

public:

  FileNamesTreeModel( TCStringArray aFilesData,   QObject* parent = 0 );
  ~FileNamesTreeModel();

  QModelIndex index(int row, int column, const QModelIndex& parent) const;
  QModelIndex parent(const QModelIndex& child) const;
  int rowCount ( const QModelIndex & parent ) const;     //ok
  int columnCount ( const QModelIndex & parent  ) const; // ok
  QVariant data ( const QModelIndex & index, int role ) const;
  //bool setData ( const QModelIndex & index, const QVariant & value, int role );
  QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;
  //Qt::ItemFlags flags ( const QModelIndex & index ) const;

  void setupModelData(TCStringArray aFilesData);
  void printTest();
};
*/

#endif // ElementsDialog_included


