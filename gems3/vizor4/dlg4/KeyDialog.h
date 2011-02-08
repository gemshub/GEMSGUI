//-------------------------------------------------------------------
// $Id: KeyDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyDialog class
//
// Copyright (C) 1996-2008 A.Rysin, S.Dmytriyeva
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

#ifndef KeyDialog_included
#define KeyDialog_included

#include <QDialog>

#include "ui_KeyDialog4.h"
#include "gstring.h"


class KeyDialog : public QDialog, public Ui::KeyDialogData
{
    Q_OBJECT

    //  TStringArray keyList;
    bool multi;
    gstring keyFilter;
    int iRt;
    TCStringArray old_sel;

    void SetList();

protected slots:
    virtual void CmSelectAll();
    virtual void CmClearAll();
    virtual void languageChange();
    virtual void CmFilter();
    void CmHelp();


public:

    KeyDialog(QWidget* win, int irt, const char* key = "*",
              const char* caption = 0, bool filter=true);
    KeyDialog(QWidget* win, int irt, TCStringArray& sel,
              const char* key = "*", const char* caption = 0 );

    virtual ~KeyDialog();


    TCStringArray allSelectedKeys();
    gstring getKey();
};


#endif // KeyDialog_included
