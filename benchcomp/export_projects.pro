
TEMPLATE	= app
LANGUAGE        = C++
TARGET		= export_gems3

DEFINES     += NO_GUI

DEFINES     += NODEARRAYLEVEL
#DEFINES    += NOMUPNONLOGTERM
DEFINES  += NO_JSONIO
#DEFINES += USE_NLOHMANNJSON
DEFINES += NDEBUG
DEFINES += USE_THERMOFUN
DEFINES += USE_THERMO_LOG
#DEFINES += USE_GEMS3K_SERVER
#!win32:!macx-clang:DEFINES += OVERFLOW_EXCEPT  #compile with nan inf exceptions

CONFIG +=sdk_no_version_check
CONFIG += c++17 cmdline
CONFIG += warn_on
CONFIG += thread
QT += core
##QT += charts


win32 {
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
}

!win32 {
  DEFINES += __unix
  QMAKE_CFLAGS += pedantic -Wall -Wextra -Wwrite-strings -Werror

  QMAKE_CXXFLAGS += -Wall -Wextra -Wcast-align -Wpointer-arith \
   -Wmissing-declarations \ #-Wundef -Weffc++ -Wshadow -Wformat-nonliteral -Winline
   -Wcast-qual -Wwrite-strings -Wno-unused-parameter \
   -Wfloat-equal -pedantic -ansi #-fsignaling-nans -ffinite-math-only

}

macx-g++ {
  DEFINES += __APPLE__
# macx:QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
  CONFIG -= warn_on
  CONFIG += warn_off
}

DATAMAN_CPP    =  ../GUI/Dataman
MODULES_CPP    =  ../Modules
SUBMODS_CPP    =  ../Modules/Submods
NUMERICS_CPP   =  ../Modules/Numerics
CHARTS_CPP     =  ../GUI/charts
SERVICES4_CPP  =  ../GUI/Services4
DIALOGS4_CPP   =  ../GUI/Dialogs4

DATAMAN_H    =  $$DATAMAN_CPP
MODULES_H    =  $$MODULES_CPP
SUBMODS_H    =  $$SUBMODS_CPP
NUMERICS_H   =  $$NUMERICS_CPP
CHARTS_H     =  $$CHARTS_CPP
SERVICES4_H  =  $$SERVICES4_CPP
DIALOGS4_H   =  $$DIALOGS4_CPP

DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$MODULES_H
DEPENDPATH   += $$SUBMODS_H
DEPENDPATH   += $$NUMERICS_H
DEPENDPATH   += $CHARTS_H
DEPENDPATH   += $$SERVICES4_H
DEPENDPATH   += $$DIALOGS4_H


INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$MODULES_H
INCLUDEPATH   += $$SUBMODS_H
INCLUDEPATH   += $$NUMERICS_H
INCLUDEPATH   += $$CHARTS_H
INCLUDEPATH   += $$SERVICES4_H
INCLUDEPATH   += $$DIALOGS4_H


contains(DEFINES, USE_THERMOFUN) {
  TFUN_CFUN_H  =  /usr/local/include
  INCLUDEPATH  += $$TFUN_CFUN_H
!win32:LIBS += -L/usr/local/lib -lThermoFun -lChemicalFun
win32:LIBS += -LC:\usr\local\bin -lThermoFun -lChemicalFun
} ## end USE_THERMOFUN

OBJECTS_DIR       = obj

# link lib
INCLUDEPATH   += "/usr/local/include/GEMS3K"
DEPENDPATH   += "/usr/local/include/GEMS3K"
LIBS += -lzmq -lGEMS3K

include($$MODULES_CPP/Modules.pri)
include($$SUBMODS_CPP/Submods.pri)
include($$NUMERICS_CPP/Numerics.pri)
include($$DATAMAN_CPP/Dataman.pri)
#include($$SERVICES4_CPP/Services4.pri)
#include($$DIALOGS4_CPP/Dialogs4.pri)
#include($$CHARTS_CPP/charts.pri)

SOURCES += \
    ../GUI/Services4/graph.cpp \
    ../GUI/Services4/units.cpp \
    ../GUI/Services4/visor.cpp \
    ../GUI/charts/chart_model.cpp \
    ../GUI/charts/graph_data.cpp \
    export_main.cpp \
    graph_window_new.cpp \
    service_new.cpp

HEADERS += \
    ../GUI/Services4/graph.h \
    ../GUI/Services4/graph_window.h \
    ../GUI/Services4/service.h \
    ../GUI/Services4/units.h \
    ../GUI/Services4/visor.h \
    ../GUI/charts/chart_model.h \
    ../GUI/charts/graph_data.h \


