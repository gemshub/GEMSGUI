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
#include "gstring.h"

class ElementsDialog : public ElementsDialogData
{
    Q_OBJECT

    TCIntArray aBtmId1;     // lists correspondanse betvin bgElem
    TCStringArray aICkey1;  // buttons and IComp records
    TCIntArray aBtmId2;     // lists correspondanse betvin bgOther
    TCStringArray aICkey2;  // buttons and IComp records

    void ResetData();
    void SetICompList();

protected slots:
    void CmHelp();
    void CmOk();
    void CmReset();
    void CmPrevious();
    void SetIsotopes(bool on);
    void SetSorption(bool on);
    void SetAqueous(bool on);
    void SetGaseous(bool on);
    void SetKernel(bool on);
    void SetUncertain(bool on);
    void SetSpecific(bool on);

public:

    ElementsDialog(QWidget* win, const char * prfName,
         const char* caption = 0 );
    virtual ~ElementsDialog();

    void allSelected( TCStringArray& aICkeys );
    bool isIsotopes() const ;
    bool isGaseous() const ;
    bool isAqueous() const ;
    bool isSorption() const ;
    bool isKernel() const ;
    bool isUncertain() const ;
    bool isSpecific() const ;
};

#endif // ElementsDialog_included


