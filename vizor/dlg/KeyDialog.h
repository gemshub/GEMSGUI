//-------------------------------------------------------------------
// $Id$
//
// Declaration of KeyDialog class
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef KeyDialog_included
#define KeyDialog_included

#include "KeyDialogData.h"

#include "gstring.h"
//using namespace std;

class KeyDialog : public KeyDialogData
{
    Q_OBJECT

    //  TStringArray keyList;
    bool multi;
    gstring keyFilter;
    int iRt;
    TCStringArray old_sel;

    void SetList();

protected slots:
    void CmFilter();
    void CmClearAll();
    void CmSelectAll();


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
