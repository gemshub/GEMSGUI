///-------------------------------------------------------------------
// $Id$
//
// Declaration of EQDialog class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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

#ifndef EQDialog_included
#define EQDialog_included

#include "EQDialogData.h"

class EQDialog : public EQDialogData
{
    Q_OBJECT

public:

    EQDialog
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~EQDialog();

protected slots:

    void CmAdvanced();
    void CmOpen_PROBE();
    void CmOpen_DUTERM();
    void CmOpen_PROCESS();
    void CmBack();
    void CmOpen_GTDEMO();
    void CmHelp();
    void CmOpen_INTEG();
    void CmSystemDialog();

protected:
    void closeEvent(QCloseEvent* ev);

};

extern EQDialog* pEQDialog;

#endif // EQDialog_included
