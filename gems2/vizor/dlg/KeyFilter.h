//-------------------------------------------------------------------
// Id: gems/vizor/dlg/KeyFilter.h  version 2.0.0    2001
//
// Declaration of KeyFilter class
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
#ifndef _KeyFilter_h_
#define _KeyFilter_h_

#include <qdialog.h>
#include <qpushbt.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qaccel.h>

#include "array.h"
#ifndef _visor_w_h
#include "visor_w.h"
#endif
#ifndef _visor_h_
#include "visor.h"
#endif
#include "v_module.h"
#include "KeyFilter.h"



//=============================================
// KeyEdit dialog
//=============================================

class KeyFilter:
            public QDialog
{
    Q_OBJECT

    TIArray<QLineEdit> aEdit;

    int iRt;
    int iEd;
    bool allowTemplates;

protected slots:
    void EvSetAll();
    void EvGetList();
    void CmOk();
    void CmHelp();     // added 25.01.01  KD

protected:
    gstring SetKeyString();
    //  void done(int r);

public:
    KeyFilter(QWidget* win, int iRt, const char* keyFilter,
              const char* caption, bool allowTemplates=true );

    gstring getFilter();
};

#endif   // _KeyFilter_h_
