
TEMPLATE    = app
#LANGUAGE        = C++
TARGET        = gems4
#VERSION         = 4.0.0

DEFINES         += Use_mt_mode
DEFINES         += NODEARRAYLEVEL
DEFINES         += _MYNOZLIB
#DEFINES         += NOMUPNONLOGTERM
#Old-database mode (comment out to switch to new database mode)
#DEFINES         += oldDBusing

CONFIG -= warn_on
CONFIG += warn_off
#CONFIG += help
CONFIG += thread
CONFIG += c++11
QT += network
QT += sql
QT += xml
QT += svg
#Qt += printsupport
lessThan( QT_MAJOR_VERSION, 5 ): CONFIG += help
greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets printsupport help concurrent

#RESOURCES += img.qrc
#\
#  -D_TC_PREFIX="\"$(prefix)\"" -D_TC_INCLUDEDIR="\"$(INCLUDEDIR)\"" \
#  -D_TC_LIBDIR="\"$(LIBDIR)\"" -D_TC_BINDIR="\"$(BINDIR)\"" -D_TC_LIBEXECDIR="\"$(LIBEXECDIR)\"" \
#  -D_TC_APPINC="\"-I$(INCLUDEDIR)\"" -D_TC_APPLIBS="\"-L$(LIBDIR) -ltcejdb @LIBS@\""

!win32 {
  DEFINES += __unix
}

macx-g++ {
  DEFINES += __APPLE__
  CONFIG -= warn_on
  CONFIG += warn_off
}

macx-clang {
  DEFINES += __APPLE__
  CONFIG -= warn_on
  CONFIG += warn_off
}
else {
 QMAKE_CXXFLAGS += -std=gnu99
 QMAKE_CFLAGS += -std=gnu99
}

RESOURCES      = ./GUI/GUI.qrc

QWT6_CPP       =  ./GUI/QWT
SERVICES4_CPP  =  ./GUI/Services4
DATAMAN_CPP    =  ./GUI/Dataman
DIALOGS4_CPP   =  ./GUI/Dialogs4
MODULES_CPP    =  ./Modules
SUBMODS_CPP    =  ./Modules/Submods
NUMERICS_CPP   =  ./Modules/Numerics

QWT6_H       =  $$QWT6_CPP
SERVICES4_H  =  $$SERVICES4_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS4_H   =  $$DIALOGS4_CPP
MODULES_H    =  $$MODULES_CPP
SUBMODS_H    =  $$SUBMODS_CPP
NUMERICS_H   =  $$NUMERICS_CPP

# Define the directory where GEMS4R source code is located
GEMS4R_DIR = $$PWD/../GEMS4R/GEMS4R

# Define the directory where the third-party libraries have been installed
CONFIG(debug, debug|release) THIRDPARTY_DIR = $$PWD/../build/debug/thirdparty
CONFIG(release, debug|release) THIRDPARTY_DIR = $$PWD/../build/release/thirdparty

# Define the directories where the headers of the third-party libraries have been installed
THIRDPARTY_INCLUDE_DIR += $$THIRDPARTY_DIR/include
THIRDPARTY_INCLUDE_DIR += $$THIRDPARTY_DIR/include/ejdb
THIRDPARTY_INCLUDE_DIR += $$THIRDPARTY_DIR/include/yaml-cpp

# Define the directories where the THIRDPARTY libraries have been installed
THIRDPARTY_LIBRARY_DIR1 = $$THIRDPARTY_DIR/lib
THIRDPARTY_LIBRARY_DIR2 = $$THIRDPARTY_DIR/lib/x86_64-linux-gnu

DEPENDPATH   += $$QWT6_H
DEPENDPATH   += $$SERVICES4_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS4_H
DEPENDPATH   += $$MODULES_H
DEPENDPATH   += $$SUBMODS_H
DEPENDPATH   += $$NUMERICS_H
DEPENDPATH   += $$GEMS4R_DIR
DEPENDPATH   += $$THIRDPARTY_INCLUDE_DIR

INCLUDEPATH   += $$QWT6_H
INCLUDEPATH   += $$SERVICES4_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS4_H
INCLUDEPATH   += $$MODULES_H
INCLUDEPATH   += $$SUBMODS_H
INCLUDEPATH   += $$NUMERICS_H
INCLUDEPATH   += $$GEMS4R_DIR
INCLUDEPATH   += $$THIRDPARTY_INCLUDE_DIR

LIBS += -L$$THIRDPARTY_LIBRARY_DIR1
LIBS += -L$$THIRDPARTY_LIBRARY_DIR2
LIBS += -lyaml-cpp -lejdb -lpugixml -lsundials_cvode -lsundials_nvecserial -lReaktoro

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR   = obj

include($$SERVICES4_CPP/Services4.pri)
include($$DIALOGS4_CPP/Dialogs4.pri)
include($$NUMERICS_CPP/Numerics.pri)
include($$QWT6_CPP/qwt.pri)
include($$SUBMODS_CPP/Submods.pri)
include($$MODULES_CPP/Modules.pri)
include($$DATAMAN_CPP/Dataman.pri)
include(Modules/GEMS4R/GEMS4R.pri)
