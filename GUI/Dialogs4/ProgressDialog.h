//-------------------------------------------------------------------
// $Id: ProgressDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of ProgressDialog class
//
// Copyright (C) 1996-2009  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef ProgressDialog_included
#define ProgressDialog_included

#include <QPaintEvent>
#include <QCloseEvent>
#include <QDialog>

#include <ctime>
#include "ui_ProgressDialog4.h"

class CalcThread;
class QTimer;

class ProgressDialog : public QDialog, public Ui::ProgressDialogData
{
    Q_OBJECT

    int ht_g;
    int ht_a;
    int ht_s;
    int ht_l;
    time_t last_update;
    CalcThread* calcThread;
    QTimer* timer;
    bool autoclose;
    clock_t t_start;

    void switchToAccept(bool isAccept);

public slots:
    virtual void CmClose();

protected slots:
    virtual void languageChange();

    virtual void CmAccept();
    virtual void CmStep();
    virtual void CmStop();
    virtual void CmResume();
    virtual void Run();

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
