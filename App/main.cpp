//-------------------------------------------------------------------
// $Id: main.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation and Declaration of TIntegApp class, function main()
//
// Copyright (C) 1996-2023  S.Dmytriyeva, A.Rysin
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

#ifdef OVERFLOW_EXCEPT
#ifdef __linux__
#include <cfenv>
#elif _MSC_VER
#include <float.h>
#else
#include <cfenv>
#endif
#endif

#include <QApplication>
#include <QtWidgets>
#include "visor.h"
#include "GemsMainWindow.h"
#include "GEMS3K/jsonconfig.h"

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
    QApplication(c, v),
    argc(c),
    argv(v)
{
    //std::cout<< "Init TIntegApp " << std::endl;

    shMemory.setKey("gems3");
    if( shMemory.attach())
    {
        _isRunning = true;
    }
    else
    {
        _isRunning = false;
        if( !shMemory.create(10))
        {
            return;
        }
        setStyle(QStyleFactory::create("windows"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/gems50.png"), QSize(), QIcon::Normal, QIcon::Off);
        setWindowIcon(icon);
    }

    //std::cout<< "End TIntegApp " << std::endl;
}

void TIntegApp::InitMainWindow()
{
    pVisorImp = new TVisorImp(argc, argv);
    pVisorImp->show();
    // init Help Window
    pVisorImp->GetHelp();
}

int main(int argc, char* argv[])
{
    TIntegApp IntegApp(argc, argv);

#if  defined(OVERFLOW_EXCEPT)
#ifdef __linux__
    feenableexcept (FE_DIVBYZERO|FE_OVERFLOW|FE_UNDERFLOW);
#elif _MSC_VER
    _clearfp();
    _controlfp(_controlfp(0, 0) & ~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW),
               _MCW_EM);
#else

#endif
#endif

    if(IntegApp.isRunning())
    {
        gui_logger->critical("gems3: Unable to create second instance.");
        return -2;
    }
    try
    {
        IntegApp.InitMainWindow();

        //std::cout<< "exec IntegApp " << std::endl;
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
        auto s = err.title;
        s += ": ";
        s += err.mess;
        gui_logger->critical("GEMS fatal error: {}", s);
        QMessageBox::critical(0, "GEMS fatal error", s.c_str());
    }
    catch(...)
    {
        gui_logger->critical("gems3: Unknown exception: program aborted");
        return -1;
    }
    return 0;
}

//--------------------- End of main.cpp ---------------------------

