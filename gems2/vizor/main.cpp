//-------------------------------------------------------------------
// $Id$
//
// Implementation and Declaration of TIntegApp class, function main()
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
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
#include <qapplication.h>
#include <qapplication.h>
#include <qmsgbox.h>
#include <qwindowsstyle.h>

#include "visor.h"
#include "visor_w.h"
#include "dlg/MainDialog.h"


class TIntegApp:
            public QApplication
{
    int argc;
    char** argv;

public:
    TIntegApp(int c, char* v[]);
    void InitMainWindow();
};

TIntegApp::TIntegApp(int c, char* v[]):
        QApplication(c, v),
        argc(c),
        argv(v)
{
    setStyle( new QWindowsStyle() );
}

void
TIntegApp::InitMainWindow()
{
    pVisorImp = new TVisorImp(argc, argv);
    setMainWidget( pVisorImp );
    pVisorImp->SetDialog( new MainDialog(pVisorImp) );
    pVisorImp->show();
}


int
main(int argc, char* argv[])
{
    TIntegApp IntegApp(argc, argv);
    try
    {
        IntegApp.InitMainWindow();
        int res = IntegApp.exec();

        // clear static arrays in our order because they're interdependent
        // and static variables are destructed at random order
        aMod.Clear();
        rt.Clear();
        aObj.Clear();

	delete pVisorImp;

        return res;
    }
    catch(TFatalError& err)
    {
        gstring s = err.title;
        s += ": ";
        s += err.mess;
        QMessageBox::critical(0, "GEMS fatal error", s.c_str());
    }
    catch(TError& err)
    {
        gstring s = err.title;
        s += ": ";
        s += err.mess;
        QMessageBox::critical(0, "GEMS fatal error", s.c_str());
    }
    catch(...)
    {
        return -1;
    }
    return 0;
}

//--------------------- End of main.cpp ---------------------------

