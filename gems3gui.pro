
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems3
#VERSION         = 3.5.1

DEFINES         += Use_mt_mode
DEFINES         += NODEARRAYLEVEL
#DEFINES         += NOMUPNONLOGTERM
#DEFINES += USE_QWT
DEFINES  += NO_JSONIO


CONFIG += c++11
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
#QMAKE_CFLAGS += pedantic -Wall -Wextra -Wwrite-strings -Werror
##QMAKE_CXXFLAGS += -ansi -pedantic -Wall -Wextra -Weffc++
##QMAKE_CXXFLAGS += -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
#QMAKE_CXXFLAGS += -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith \
# -Wmissing-declarations -Winline -Wundef \
##-Wnested-externs -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter \
# -Wcast-qual -Wwrite-strings -Wno-unused-parameter \
#-Wfloat-equal -pedantic -ansi

#CFLAGS += -Wall
#CXXFLAGS += -Wall

#  CONFIG( release ) {
#    QMAKE_CFLAGS_RELEASE = -O3
#    QMAKE_CXXFLAGS_RELEASE = -O3
#  }
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
MODULES_CPP    =  ./Modules
SUBMODS_CPP    =  ./Modules/Submods
NUMERICS_CPP   =  ./Modules/Numerics
GEMS3K_CPP     =  ../standalone/GEMS3K

SERVICES4_H  =  $$SERVICES4_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS4_H   =  $$DIALOGS4_CPP
MODULES_H    =  $$MODULES_CPP
SUBMODS_H    =  $$SUBMODS_CPP
NUMERICS_H   =  $$NUMERICS_CPP
GEMS3K_H     =  $$GEMS3K_CPP

DEPENDPATH   += $$SERVICES4_H
DEPENDPATH   += $$DATAMAN_H  
DEPENDPATH   += $$DIALOGS4_H 
DEPENDPATH   += $$MODULES_H  
DEPENDPATH   += $$SUBMODS_H   
DEPENDPATH   += $$NUMERICS_H 
DEPENDPATH   += $$GEMS3K_H   

INCLUDEPATH   += $$SERVICES4_H
INCLUDEPATH   += $$DATAMAN_H  
INCLUDEPATH   += $$DIALOGS4_H 
INCLUDEPATH   += $$MODULES_H  
INCLUDEPATH   += $$SUBMODS_H   
INCLUDEPATH   += $$NUMERICS_H 
INCLUDEPATH   += $$GEMS3K_H   

contains(DEFINES, USE_QWT){
  QWT6_CPP       =  ./GUI/QWT
  QWT6_H  =  $$QWT6_CPP
  DEPENDPATH   += $$QWT6_H
  INCLUDEPATH   += $$QWT6_H
}
!contains(DEFINES, USE_QWT){
  CHARTS_CPP       =  ./GUI/charts
  CHARTS_H  =  $$CHARTS_CPP
  DEPENDPATH   += $CHARTS_H
  INCLUDEPATH   += $$CHARTS_H
}


MOC_DIR = tmp
UI_DIR  = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR       = obj

include($$DATAMAN_CPP/Dataman.pri)
include($$MODULES_CPP/Modules.pri)
include($$SUBMODS_CPP/Submods.pri)
include($$NUMERICS_CPP/Numerics.pri)
include($$GEMS3K_CPP/gems3k.pri)

message("Defines: $$DEFINES")
contains(DEFINES, USE_QWT) {

   include($$SERVICES4_CPP/Services4_old.pri)
   include($$DIALOGS4_CPP/Dialogs4_old.pri)
   include($$QWT6_CPP/qwt.pri)
}
!contains(DEFINES, USE_QWT) {
   include($$SERVICES4_CPP/Services4.pri)
   include($$DIALOGS4_CPP/Dialogs4.pri)
   include($$CHARTS_CPP/charts.pri)
}
