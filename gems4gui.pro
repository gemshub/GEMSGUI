
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems4
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
GEMS4K_CPP     =  ../standalone/GEMS4K

QWT6_H       =  $$QWT6_CPP
SERVICES4_H  =  $$SERVICES4_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS4_H   =  $$DIALOGS4_CPP
MODULES_H    =  $$MODULES_CPP
SUBMODS_H    =  $$SUBMODS_CPP
NUMERICS_H   =  $$NUMERICS_CPP
GEMS4K_H     =  $$GEMS4K_CPP

EJDB_PATH = ../standalone/EJDB
YAML_PATH = ../standalone/YAML

win32{
   EJDB_LIB_PATH =  $$EJDB_PATH/build-win32
}
unix{
   EJDB_LIB_PATH =  $$EJDB_PATH/build
}

YAML_LIB_PATH =  $$YAML_PATH/build
YAML_H =  $$YAML_PATH/include

EJDB_BSON_H = $$EJDB_PATH/src/bson
EJDB_EJDB_H = $$EJDB_PATH/src/ejdb
EJDB_TCUTIL_H = $$EJDB_PATH/src/tcutil

#EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated
CONFIG(release, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/release/src/generated
CONFIG(debug, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated

DEPENDPATH   += $$QWT6_H
DEPENDPATH   += $$SERVICES4_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS4_H
DEPENDPATH   += $$MODULES_H
DEPENDPATH   += $$SUBMODS_H
DEPENDPATH   += $$NUMERICS_H
DEPENDPATH   += $$GEMS4K_H

INCLUDEPATH   += $$QWT6_H
INCLUDEPATH   += $$SERVICES4_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS4_H
INCLUDEPATH   += $$MODULES_H
INCLUDEPATH   += $$SUBMODS_H
INCLUDEPATH   += $$NUMERICS_H
INCLUDEPATH   += $$GEMS4K_H
INCLUDEPATH   += $$EJDB_BSON_H
INCLUDEPATH   += $$EJDB_EJDB_H
INCLUDEPATH   += $$EJDB_GENERATED_H
INCLUDEPATH   += $$EJDB_TCUTIL_H
INCLUDEPATH   += $$YAML_H

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
include($$GEMS4K_CPP/gems4k.pri)
#include($$EJDB_PATH/ejdb.pri)
include($$EJDB_PATH/tcejdb.pri)

#CONFIG(release, debug|release): LIBS += -L$$EJDB_LIB_PATH/release/src/ -lejdb
#CONFIG(debug, debug|release): LIBS += -L$$EJDB_LIB_PATH/debug/src/ -lejdb

CONFIG(release, debug|release): LIBS += -L$$YAML_LIB_PATH/release/ -lyaml-cpp
CONFIG(debug, debug|release): LIBS += -L$$YAML_LIB_PATH/debug/ -lyaml-cpp


unix|win32: CONFIG(release, debug|release): LIBS += -L$$PWD/../standalone/Reaktoro/build/release/lib/ -lReaktoro
else: unix|win32: CONFIG(debug, debug|release): LIBS += -L$$PWD/../standalone/Reaktoro/build/debug/lib/ -lReaktoro

INCLUDEPATH += $$PWD/../standalone/Reaktoro $$PWD/../standalone/Reaktoro/dependencies
DEPENDPATH += $$PWD/../standalone/Reaktoro

#../standalone
