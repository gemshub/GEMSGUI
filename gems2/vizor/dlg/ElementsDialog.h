//-------------------------------------------------------------------
// Id: gems/vizor/dlg/ElementsDialog.h  version 2.0.0    2001
//
// Declaration of ElementsDialog class
//
// Created : 011117    Modified: 011117
//
// Copyright (C) 2001  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ElementsDialog_included
#define ElementsDialog_included

#include "ElementsDialogData.h"
#include "filters_data.h"


class ElementsDialog : public ElementsDialogData
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
    bool isSorption() const ;


protected slots:
    void CmHelp();
    void CmOk();
//    void CmReset();
//    void CmPrevious();
    void CmSetFilters();

    void SetSorption();
    void SetAqueous();
    void SetGaseous();
    void SetFiles();

public:

    ElementsDialog(QWidget* win, const char * prfName,
         const char* caption = 0 );
    virtual ~ElementsDialog();

    const setFiltersData&  getFilters();
    const elmWindowData&   getData();

};

#endif // ElementsDialog_included


