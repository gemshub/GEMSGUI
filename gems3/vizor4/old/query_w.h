//-------------------------------------------------------------------
// $Id: query_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TQueryWindow class
// 
// Copyright (C) 1996-2001  A.Rysin
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
#ifndef _query_w_h
#define _query_w_h

#include <qdialog.h>
#include <qlabel.h>
#include <qsizepolicy.h>

#include "page_f.h"


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
