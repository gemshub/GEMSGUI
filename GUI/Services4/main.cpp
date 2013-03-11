//-------------------------------------------------------------------
// $Id: main.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation and Declaration of TIntegApp class, function main()
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
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


#include <QApplication>
#include <QSqlDatabase>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
// !!!! qt5 install in ubuntu libglu1-mega-dev
#else
#include <QApplication>
#include <QSqlDatabase>
#include <QSharedMemory>
#include <QMainWindow>
#include <QMessageBox>
#include <QWindowsStyle>
#endif

#include "visor.h"
#include "GemsMainWindow.h"

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
    cout << "QSqlDatabase: available drivers:" <<
          QSqlDatabase::drivers().join(QLatin1String(" ")).toLatin1().data() << endl;

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

#if QT_VERSION >= 0x050000
#include <QtWidgets>
       setStyle(QStyleFactory::create("windows"));
#else
       setStyle( new QWindowsStyle() );
#endif

       QIcon icon;
       icon.addFile(QString::fromUtf8(":/Icons/gems50.png"), QSize(), QIcon::Normal, QIcon::Off);
       setWindowIcon(icon);
   }
}

void
TIntegApp::InitMainWindow()
{
    pVisorImp = new TVisorImp(argc, argv);
    pVisorImp->show();
 //   pVisorImp->moveToolBar();
// init Help Window
    pVisorImp->GetHelp();
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
//        cout << "QSqlDatabase: available drivers: %s" <<
//              QSqlDatabase::drivers().join(QLatin1String(" ")).toLatin1().data() << endl;
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

