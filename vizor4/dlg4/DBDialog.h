//-------------------------------------------------------------------
// $Id: DBDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of DBDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
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

#ifndef DBDialog_included
#define DBDialog_included

#include <QWidget>
#include <QCloseEvent>

#include "ui_DBDialog4.h"

class DBDialog : public QWidget, public Ui::DBDialogData
{
    Q_OBJECT

public:

    DBDialog(QWidget* parent = NULL, const char* name = NULL);

    virtual ~DBDialog();

protected slots:
   virtual void languageChange();

    virtual void CmAdvanced();
    virtual void CmBack();
    virtual void CmHelp();
    virtual void CmOpen_COMPOS();
    virtual void CmOpen_DCOMP();
    virtual void CmOpen_ICOMP();
    virtual void CmOpen_REACDC();
    virtual void CmOpen_RTPARM();
    virtual void CmOpen_PHASE();

protected:
    void closeEvent(QCloseEvent* ev);

};

#endif // DBDialog_included
