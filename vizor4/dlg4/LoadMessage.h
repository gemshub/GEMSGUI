//-------------------------------------------------------------------
// $Id: LoadMessage.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of LoadMessage class
//
// Copyright (C) 1996-2009  A.Rysin, S.Dmytriyeva
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

#ifndef LoadMessage_included
#define LoadMessage_included

#include <QProgressDialog>
#include <QDialog>
#include <QCloseEvent>

#include "calcthread.h"

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

//-----------------------------------------------------------------------

class ProcessProgressDialog: public QDialog
{
    Q_OBJECT

    ProcessThread *calcThread;

    QPushButton*  pStopStep;
    QPushButton*  pClose;
    QPushButton*  pBreak;
    QPushButton*  pResume;
    QProgressBar* pProgress;

public slots:
    void CmClose();
//    void slUpdate(bool force);
//    void slQuestion( void *result, QWidget* par, QString title, QString mess);

protected slots:
    void CmStep();
    void CmStop();
    void CmResume();
    void CmBreak();

protected:
    void closeEvent(QCloseEvent* ev);

public:
    static ProcessProgressDialog* pDia;

    ProcessProgressDialog(QWidget* parent );
    virtual ~ProcessProgressDialog();

    void Update();
    void CalcFinished();
};


#endif // LoadMessage_included
