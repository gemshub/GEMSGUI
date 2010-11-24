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
#include <QSharedMemory>
#include <QMainWindow>


class TIntegApp:  public QApplication
{
 
 // Q_OBJECT
  
  int argc;
  char** argv;

  bool _isRunning;
  QSharedMemory shMemory;

public:
    TIntegApp(int& c, char** v);
    void InitMainWindow();
    bool isRunning()
    { return _isRunning; }
};

TIntegApp::TIntegApp(int& c, char** v):
        QApplication( c, v),
      argc(c),
      argv(v)
{
    shMemory.setKey("gems3");
    if( shMemory.attach())
    {
        _isRunning = true;
    }
    else
    {  _isRunning = false;
       if( !shMemory.create(10))
        {
           return;
         }

       setStyle( new QWindowsStyle() );
       QIcon icon;
       icon.addFile(QString::fromUtf8(":/images/img/gems16.png"), QSize(), QIcon::Normal, QIcon::Off);
       setWindowIcon(icon);
   }
}

void
TIntegApp::InitMainWindow()
{
    pVisorImp = new TVisorImp(argc, argv);
    MainDialog* window = new MainDialog(pVisorImp);
    pVisorImp->SetDialog( window);
    pVisorImp->show();
}

int
main(int argc, char* argv[])
{
    TIntegApp IntegApp(argc, argv);

    if(IntegApp.isRunning())
    {
       cerr << "gems3: Unable to create second instance." << endl;
       return -2;
    }
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
        cerr << "gems3: Unknown exception: program aborted" << endl;
        return -1;
    }
    return 0;
}

//--------------------- End of main.cpp ---------------------------

