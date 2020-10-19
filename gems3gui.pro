
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems3
#VERSION         = 3.6.0

#DEFINES         += Use_mt_mode
DEFINES         += NODEARRAYLEVEL
#DEFINES += NOPARTICLEARRAY
#DEFINES         += NOMUPNONLOGTERM
DEFINES  += NO_JSONIO
#DEFINES += USE_OLD_NLOHMANJSON

CONFIG+=sdk_no_version_check
CONFIG += c++17
CONFIG += warn_on
#CONFIG += warn_off
#CONFIG += help
CONFIG += thread
QT += network
QT += sql
QT += xml
QT += svg
QT += charts
#Qt += printsupport
lessThan( QT_MAJOR_VERSION, 5 ): CONFIG += help
greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets printsupport help concurrent

#RESOURCES += img.qrc

!win32 {
  DEFINES += __unix

#CFLAGS += -Wall
#CXXFLAGS += -Wall

#  CONFIG( release ) {
#    QMAKE_CFLAGS_RELEASE = -O3
#    QMAKE_CXXFLAGS_RELEASE = -O3
#  }

QMAKE_CFLAGS += pedantic -Wall -Wextra -Wwrite-strings -Werror

QMAKE_CXXFLAGS += -Wall -Wextra -Wcast-align -Wpointer-arith \
   -Wmissing-declarations -Winline -Wundef \ #-Weffc++ -Wshadow -Wformat-nonliteral \
   -Wcast-qual -Wwrite-strings -Wno-unused-parameter \
   -Wfloat-equal -Wno-pedantic -ansi

#QMAKE_CXXFLAGS += -Wall -Wno-ignored-attributes -Wno-pedantic -Wno-variadic-macros -Wno-deprecated
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


MOC_DIR = tmp
UI_DIR  = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR       = obj

# link lib
INCLUDEPATH   += "/usr/local/include/GEMS3K"
DEPENDPATH   += "/usr/local/include/GEMS3K"
LIBS += -lzmq -lGEMS3K
#link sources
#GEMS3K_CPP     =  ../standalone/GEMS3K
#GEMS3K_H     =  $$GEMS3K_CPP
#DEPENDPATH   += $$GEMS3K_H
#INCLUDEPATH   += $$GEMS3K_H
#LIBS += -lzmq
#include($$GEMS3K_CPP/gems3k.pri)
#end link

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
