//-------------------------------------------------------------------
// $Id$
//
// Declaration of DBAdvDialog class
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


#ifndef DBAdvDialog_included
#define DBAdvDialog_included

#include "DBAdvDialogData.h"

class DBAdvDialog : public DBAdvDialogData
{
    Q_OBJECT

protected slots:

    void CmBack();
    void CmHelp();
    void CmFormats();
    void CmOpen_DUTERM();
    void CmOpen_PROBE();
    void CmOpen_PROCESS();
    void CmOpen_GTDEMO();
    void CmOpen_INTEG();
    void CmOpen_SData();
    void CmOpen_Const();
    void CmOpen_PROFILE();
    void CmThermoSystem();

protected:
    void closeEvent(QCloseEvent* e);

public:

    DBAdvDialog
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~DBAdvDialog();

};
#endif // DBAdvDialog_included
