//-------------------------------------------------------------------
// Id: gems/vizor/query_w.h  version 2.0.0    2001
//
// Declaration of TQueryWindow class
// 
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin
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
#ifndef _query_w_h
#define _query_w_h

#include <qdialog.h>
#include <qlabel.h>

#include "page_s.h"


/*!
  \class TQueryWindow
  \brief dialog for 'Remake' command
*/

class TQueryWindow:
            public QDialog
{
    Q_OBJECT

    CWinInfo& rInfo;
    QLabel* hint;

    void ShowInfo();
    void AddFields();

protected slots:
    void done(int result);

public:
    TQueryWindow(CWinInfo& w);

    void setHint(const char* txt)
    {
        hint->setText(txt);
    }
};

#endif    // _query_w_h
