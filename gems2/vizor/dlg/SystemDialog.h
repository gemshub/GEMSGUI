//-------------------------------------------------------------------
// Id: gems/vizor/dlg/SystemDialog.h  version 2.0.0    2001
//
// Declaration of SystemDialog class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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

#ifndef SystemDialog_included
#define SystemDialog_included

#include "SystemDialogData.h"

class SystemDialog : public SystemDialogData
{
    Q_OBJECT

public:
    static SystemDialog* pDia;

    SystemDialog(QWidget* parent = NULL, const char* name = NULL);
    virtual ~SystemDialog();

    void Update();
protected:
    void closeEvent(QCloseEvent* ev);

protected slots:

    void CmBack();
    void CmSelect();
    void CmNew();
    void CmRemake();
    void CmSave();
    void CmRunIPM();
    void CmMoreIPM();
    void CmMoreBCC();
    void CmRunBCC();
    void CmPrintMtparam();
    void CmPrintSystem();
    void CmPrintEqstat();
    void CmHelp();
    void CmOpen_MTPARAM();
    void CmOpen_SYSTEM_ICOMP();
    void CmOpen_inp_PHASE_HL();
//    void CmOpen_SYSTEM_DCOMP();
    void CmOpen_EQSTAT_ICOMP();
    void CmOpen_res_PHASE_HL();
    void CmOpen_MULTI();

};
#endif // SystemDialog_included

