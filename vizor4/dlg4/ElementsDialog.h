//-------------------------------------------------------------------
// $Id: ElementsDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ElementsDialog class
//
// Copyright (C) 2001-2008  S.Dmytriyeva
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

#ifndef ElementsDialog_included
#define ElementsDialog_included

#include <QDialog>
#include <QButtonGroup>

#include "ui_ElementsDialog4.h"
#include "filters_data.h"

class ElementsDialog : public QDialog, public Ui::ElementsDialogData
{
    gstring prf_name;
    setFiltersData sf_data;
    elmWindowData  el_data;
    elmFilesConfData files_data;

    Q_OBJECT

    TCIntArray aBtmId1;     // lists correspondanse betvin bgElem
    TCStringArray aICkey1;  // buttons and IComp records
    TCIntArray aBtmId2;     // lists correspondanse betvin bgOther
    TCStringArray aICkey2;  // buttons and IComp records

    // from template
    TCIntArray aBtmId1_sel;  // lists from template bgElem
    TCStringArray aICkey2_sel;  // lists from template bgOther

    void EmptyData();
    void ResetData();
    void SetICompList();

    // 0 no changed (no kernel, specifik or uncertain)
    // 1 to open, 2 to close
    int  isOpenFile( gstring& name );
    void setFilesList();
    void resetFilesSelection();
    void openFilesSelection();
    void openFilesICOMP();

    void SetData();
    void allSelected( TCStringArray& aICkeys );
    void openFiles( TCStringArray& names );
    bool isAqueous() const ;
    bool isGaseous() const ;
    bool isSolids() const ;
    bool isSolution() const ;
    bool isSorption() const ;

    QButtonGroup* bgElem;
    QButtonGroup* bgOther;

protected slots:
    void CmHelp();
    void CmOk();
//    void CmReset();
//    void CmPrevious();
    void CmSetFilters();

    void SetAqueous();
    void SetGaseous();
    void SetSolids();
    void SetSolutions();
    void SetSorption();
    void SetFiles();
    virtual void languageChange();
    

public:

    ElementsDialog(QWidget* win, const char * prfName,
         const char* caption = 0 );
    virtual ~ElementsDialog();

    const setFiltersData&  getFilters();
    const elmWindowData&   getData();

};

#endif // ElementsDialog_included


