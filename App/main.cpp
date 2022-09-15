//-------------------------------------------------------------------
// $Id: main.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation and Declaration of TIntegApp class, function main()
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------


#include <QApplication>
//#include <QSqlDatabase>

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
#include <iostream>
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
    //cout << "QSqlDatabase: available drivers:" <<
    //      QSqlDatabase::drivers().join(QLatin1String(" ")).toStdString() << endl;

    gems3k_update_loggers( true, "gems3k_gui.log", spdlog::level::info);
    gui_logger->set_level(spdlog::level::debug);
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
//              QSqlDatabase::drivers().join(QLatin1String(" ")).toStdString() << endl;
        IntegApp.InitMainWindow();
        int res = IntegApp.exec();
        // clear static arrays in our order because they're interdependent
        // and static variables are destructed at random order
        aMod.clear();
        rt.clear();
        aObj.clear();

        delete pVisorImp;

        return res;
    }
    catch(TError& err)
    {
        cerr << "gems3: " << err.mess.c_str() << endl;
        auto s = err.title;
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

