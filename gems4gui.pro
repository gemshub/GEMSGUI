
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
#  PLATFORM_CPP       =  ./GUI/tcejdb/nix
  PLATFORM_CPP       =  ../standalone/tcejdb/nix
#  CONFIG( debug ) {
#   QMAKE_CFLAGS = -std=gnu99
#    QMAKE_CXXFLAGS = -std=gnu99
#  }
}

win32 {
#  PLATFORM_CPP       =  ./GUI/tcejdb/win32
  PLATFORM_CPP       =  ../standalone/tcejdb/win32
}

macx-g++ {
  DEFINES += __APPLE__
# macx:QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
#  CONFIG( debug ) {
#    QMAKE_CFLAGS = -std=gnu99
#    QMAKE_CXXFLAGS = -std=gnu99
#  }
  CONFIG -= warn_on
  CONFIG += warn_off
}



macx-clang {
  DEFINES += __APPLE__
# macx:QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
#  CONFIG( debug ) {
#    QMAKE_CFLAGS =
#    QMAKE_CXXFLAGS =
#  }
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
#EJDB_CPP       =  ./GUI/tcejdb
MODULES_CPP    =  ./Modules
SUBMODS_CPP    =  ./Modules/Submods
NUMERICS_CPP   =  ./Modules/Numerics
GEMS4K_CPP     =  ../standalone/GEMS4K
EJDB_CPP       =  ../standalone/tcejdb

QWT6_H       =  $$QWT6_CPP
SERVICES4_H  =  $$SERVICES4_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS4_H   =  $$DIALOGS4_CPP
EJDB_H       =  $$EJDB_CPP
MODULES_H    =  $$MODULES_CPP
SUBMODS_H    =  $$SUBMODS_CPP
NUMERICS_H   =  $$NUMERICS_CPP
GEMS4K_H     =  $$GEMS4K_CPP
PLATFORM_H   =  $$PLATFORM_CPP

DEPENDPATH   += $$QWT6_H
DEPENDPATH   += $$SERVICES4_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS4_H
DEPENDPATH   += $$EJDB_H
DEPENDPATH   += $$MODULES_H
DEPENDPATH   += $$SUBMODS_H
DEPENDPATH   += $$NUMERICS_H
DEPENDPATH   += $$GEMS4K_H
DEPENDPATH   += $$PLATFORM_H

INCLUDEPATH   += $$QWT6_H
INCLUDEPATH   += $$SERVICES4_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS4_H
INCLUDEPATH   += $$EJDB_H
INCLUDEPATH   += $$MODULES_H
INCLUDEPATH   += $$SUBMODS_H
INCLUDEPATH   += $$NUMERICS_H
INCLUDEPATH   += $$GEMS4K_H
INCLUDEPATH   += $$PLATFORM_H

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR   = obj


include($$SERVICES4_CPP/Services4.pri)
include($$DIALOGS4_CPP/Dialogs4.pri)
include($$NUMERICS_CPP/Numerics.pri)
include($$QWT6_CPP/qwt.pri)
include($$EJDB_CPP/tcejdb.pri)

include($$SUBMODS_CPP/Submods.pri)
include($$MODULES_CPP/Modules.pri)
include($$DATAMAN_CPP/Dataman.pri)
include($$GEMS4K_CPP/gems4k.pri)

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Reaktor/build/lib/release/ -lReaktor
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Reaktor/build/lib/debug/ -lReaktor
#else:unix: LIBS += -L$$PWD/../Reaktor/build/lib/ -lReaktor

unix|win32: LIBS += -lReaktor
unix|win32: LIBS += -lopenblas
unix|win32: LIBS += -llapack

INCLUDEPATH += $$PWD/../standalone/Reaktor
DEPENDPATH += $$PWD/../standalone/Reaktor

INCLUDEPATH += ../standalone/Reactor
DEPENDPATH += ../standalone/Reaktor

../standalone