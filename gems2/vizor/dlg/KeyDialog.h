//-------------------------------------------------------------------
// Id: gems/vizor/dlg/KeyDialog.h  version 2.0.0    2001
//
// Declaration of KeyDialog class
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

#ifndef KeyDialog_included
#define KeyDialog_included

#include "KeyDialogData.h"

#include "gstring.h"
//using namespace std;

class KeyDialog : public KeyDialogData
{
    Q_OBJECT

    //  TStringArray keyList;
    gstring keyFilter;
    int iRt;

protected slots:
    void CmFilter();

public:

    KeyDialog(QWidget* win, int irt, const char* key = "*",
              const char* caption = 0, bool filter=true);

    virtual ~KeyDialog();


    gstring getKey();
};

#endif // KeyDialog_included
