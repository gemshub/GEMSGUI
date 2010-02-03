///-------------------------------------------------------------------
// $Id: EQDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of EQDialog class
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

#ifndef EQDialog_included
#define EQDialog_included

#include <QWidget>
#include <QCloseEvent>

#include "ui_EQDialog4.h"

class EQDialog : public QWidget, public Ui::EQDialogData
{
    Q_OBJECT

public:

    EQDialog( QWidget* parent = NULL,  const char* name = NULL );
    virtual ~EQDialog();

protected slots:
    virtual void languageChange();

    virtual void CmBack();
    virtual void CmHelp();
    virtual void CmOpen_DUTERM();
    virtual void CmOpen_GTDEMO();
    virtual void CmOpen_INTEG();
    virtual void CmOpen_PROBE();
    virtual void CmOpen_PROCESS();
    virtual void CmSystemDialog();
    virtual void CmAdvanced();

protected:
    void closeEvent(QCloseEvent* ev);

};

extern EQDialog* pEQDialog;

#endif // EQDialog_included
