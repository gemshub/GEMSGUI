//-------------------------------------------------------------------
// $Id$
//
// Declaration of DBDialog class
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

#ifndef DBDialog_included
#define DBDialog_included

#include "DBDialogData.h"

class DBDialog : public DBDialogData
{
    Q_OBJECT

public:

    DBDialog(QWidget* parent = NULL, const char* name = NULL);

    virtual ~DBDialog();

protected slots:
    void CmOpen_RTPARM();
    void CmOpen_ICOMP();
    void CmOpen_PHASE();
    void CmOpen_COMPOS();
    void CmOpen_REACDC();
    void CmOpen_DCOMP();
    void CmBack();
    void CmAdvanced();
    void CmHelp();

protected:
    void closeEvent(QCloseEvent* ev);

};

#endif // DBDialog_included
