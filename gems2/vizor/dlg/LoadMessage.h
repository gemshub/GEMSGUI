//-------------------------------------------------------------------
// $Id$
//
// Declaration of LoadMessage class
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

#ifndef LoadMessage_included
#define LoadMessage_included

#include <qapplication.h>
#include <qprogressdialog.h>
#include <qdialog.h>

class LoadMessage : public QProgressDialog
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent* ev);

public:
    static LoadMessage* pDia;

    LoadMessage( QWidget* parent,
                 const char* name,
                 const char* msg,
                 int prog = 0,
                 int total = 100);

    virtual ~LoadMessage()
    {}


    void Update( const char* mess, int prog, int total = -1 );
};

#ifdef Use_mt_mode

#include <time.h>
#include <qevent.h>

class ProcessThread;
class QTimer;

class ProcessProgressDialog: public QDialog
{
    Q_OBJECT

    ProcessThread* calcThread;
    QTimer* timer;

    QPushButton*  pStopStep;
    QPushButton*  pClose;
    QPushButton*  pBreak;
    QPushButton*  pResume;
    QProgressBar* pProgress;

public slots:
    void CmClose();

protected slots:
    void CmStep();
    void CmStop();
    void CmResume();
    void CmBreak();
    void Run();

protected:
    void closeEvent(QCloseEvent* ev);

public:
    static ProcessProgressDialog* pDia;

    ProcessProgressDialog(QWidget* parent );
    virtual ~ProcessProgressDialog();

    void Update();
    void CalcFinished();
};

#endif

#endif // LoadMessage_included
