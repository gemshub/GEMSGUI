//-------------------------------------------------------------------
// $Id: main.cpp 968 2007-12-13 13:23:32Z gems $
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
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <qmessagebox.h>
#include <qwindowsstyle.h>

#include "visor.h"
#include "visor_w.h"
#include "MainDialog.h"

#include <QApplication>
#include <QMainWindow>


class TIntegApp:  public QApplication
{
 
 // Q_OBJECT
  
  int argc;
  char** argv;

public:
    TIntegApp(int& c, char** v);
    void InitMainWindow();
};

TIntegApp::TIntegApp(int& c, char** v):
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
    MainDialog* window = new MainDialog(pVisorImp);;
    pVisorImp->SetDialog( window);
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
    catch(TError& err)
    {
        cerr << "gems3: " << err.mess.c_str() << endl;
        gstring s = err.title;
        s += ": ";
        s += err.mess;
        QMessageBox::critical(0, "GEMS fatal error", s.c_str());
    }
    catch(...)
    {
        return -1;
        cerr << "gems3: Unknown exception: program aborted" << endl;
    }
    return 0;
}

//--------------------- End of main.cpp ---------------------------

