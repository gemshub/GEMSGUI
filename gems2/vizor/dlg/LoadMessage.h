//-------------------------------------------------------------------
// Id: gems/vizor/dlg/LoadMessage.h  version 2.0.0    2001
//
// Declaration of LoadMessage class
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

#ifndef LoadMessage_included
#define LoadMessage_included

#include <qapplication.h>
#include <qprogressdialog.h>

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

#endif // LoadMessage_included
