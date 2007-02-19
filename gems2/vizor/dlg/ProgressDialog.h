//-------------------------------------------------------------------
// $Id$
//
// Declaration of ProgressDialog class
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

#ifndef ProgressDialog_included
#define ProgressDialog_included

#include <time.h>
#include "ProgressDialogData.h"

class CalcThread;
class QTimer;

class ProgressDialog : public ProgressDialogData
{
    Q_OBJECT

    int ht_g;
    int ht_a;
    int ht_s;
    time_t last_update;
    CalcThread* calcThread;
    QTimer* timer;
    bool autoclose;
    clock_t t_start;

    void switchToAccept(bool isAccept);

public slots:
    void CmClose();

protected slots:
    void CmStep();
    void CmStop();
    void CmAccept();
    void CmResume();
    void Run();

protected:
    void closeEvent(QCloseEvent* ev);
    void paintEvent(QPaintEvent* ev);

public:
    static ProgressDialog* pDia;

#ifdef Use_mt_mode
    ProgressDialog(QWidget* parent, bool step, bool autoclose=false);
#else
    ProgressDialog(QWidget* parent = NULL, bool step=false);
#endif
    virtual ~ProgressDialog();

    void Update(bool force=false);
    void CalcFinished();
};


#endif // ProgressDialog_included
