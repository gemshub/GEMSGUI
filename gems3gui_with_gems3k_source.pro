
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems3

DEFINES         += NODEARRAYLEVEL
#DEFINES         += NOMUPNONLOGTERM
DEFINES  += NO_JSONIO
#DEFINES += USE_NLOHMANNJSON
DEFINES += NDEBUG
DEFINES += USE_THERMOFUN
DEFINES += USE_THERMO_LOG
#DEFINES += USE_GEMS3K_SERVER
DEFINES += USE_GEMS3K_SOURCE
#!win32:!macx-clang:DEFINES += OVERFLOW_EXCEPT  #compile with nan inf exceptions
#_GEMIPM_version_stamp = "3.9.10"
#DEFINES += _GEMIPM_version_stamp

CONFIG+=sdk_no_version_check
CONFIG += c++17
CONFIG += warn_on
CONFIG += thread
QT += network
QT += sql xml svg
QT += charts
QT += widgets printsupport help concurrent

#RESOURCES += img.qrc
win32:RC_ICONS += Gems3.ico

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

RESOURCES      = ./GUI/GUI.qrc

SERVICES4_CPP  =  ./GUI/Services4
DATAMAN_CPP    =  ./GUI/Dataman
DIALOGS4_CPP   =  ./GUI/Dialogs4
CHARTS_CPP     =  ./GUI/charts
MODULES_CPP    =  ./Modules
SUBMODS_CPP    =  ./Modules/Submods
NUMERICS_CPP   =  ./Modules/Numerics

SERVICES4_H  =  $$SERVICES4_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS4_H   =  $$DIALOGS4_CPP
CHARTS_H     =  $$CHARTS_CPP
MODULES_H    =  $$MODULES_CPP
SUBMODS_H    =  $$SUBMODS_CPP
NUMERICS_H   =  $$NUMERICS_CPP

DEPENDPATH   += $$SERVICES4_H
DEPENDPATH   += $$DATAMAN_H  
DEPENDPATH   += $$DIALOGS4_H 
DEPENDPATH   += $CHARTS_H
DEPENDPATH   += $$MODULES_H
DEPENDPATH   += $$SUBMODS_H   
DEPENDPATH   += $$NUMERICS_H 

INCLUDEPATH   += $$SERVICES4_H
INCLUDEPATH   += $$DATAMAN_H  
INCLUDEPATH   += $$DIALOGS4_H 
INCLUDEPATH   += $$CHARTS_H
INCLUDEPATH   += $$MODULES_H
INCLUDEPATH   += $$SUBMODS_H   
INCLUDEPATH   += $$NUMERICS_H 

contains(DEFINES, USE_THERMOFUN) {

  TFUN_CFUN_H  =  /usr/local/include
  INCLUDEPATH  += $$TFUN_CFUN_H
!win32:LIBS += -L/usr/local/lib -lThermoFun -lChemicalFun
win32:LIBS += -LC:\usr\local\bin -lThermoFun -lChemicalFun
} ## end USE_THERMOFUN

MOC_DIR = tmp
UI_DIR  = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR       = obj

contains(DEFINES, USE_GEMS3K_SOURCE) {
    message("USE_GEMS3K_SOURCE is defined")
    GEMS3K_CPP     =  ../GEMS3K/GEMS3K
    GEMS3K_H       =  $$GEMS3K_CPP
    DEPENDPATH     += $$GEMS3K_H
    INCLUDEPATH    += $$GEMS3K_H
    LIBS           += -lzmq
    include($$GEMS3K_CPP/gems3k.pri)
} else {
    message("USE_GEMS3K_SOURCE is NOT defined")
    INCLUDEPATH   += "/usr/local/include/GEMS3K"
    DEPENDPATH    += "/usr/local/include/GEMS3K"
    LIBS          += -lzmq -lGEMS3K
}

include($$DATAMAN_CPP/Dataman.pri)
include($$MODULES_CPP/Modules.pri)
include($$SUBMODS_CPP/Submods.pri)
include($$NUMERICS_CPP/Numerics.pri)
include($$SERVICES4_CPP/Services4.pri)
include($$DIALOGS4_CPP/Dialogs4.pri)
include($$CHARTS_CPP/charts.pri)

#message("Defines: $$DEFINES")

SOURCES += \
    App/main.cpp
