
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems3
##VERSION         = 3.1.0

DEFINES         += Use_mt_mode
DEFINES         += NODEARRAYLEVEL

CONFIG -= warn_on
CONFIG += warn_off
CONFIG += help
CONFIG += thread
QT += network
QT += sql
QT += xml

#LIBS	+= -lqd
#RESOURCES += img.qrc

!win32 {
  DEFINES += __unix
#  DEFINES += GEMS_RELEASE
}

macx-g++ {
  DEFINES += __APPLE__
macx:QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
  CONFIG -= warn_on
  CONFIG += warn_off
}

RESOURCES = ./GUI/GUI.qrc

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

MOC_DIR = tmp
UI_DIR  = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR       = obj

include($$SERVICES4_CPP/Services4.pri)
include($$DATAMAN_CPP/Dataman.pri)
include($$DIALOGS4_CPP/Dialogs4.pri)
include($$MODULES_CPP/Modules.pri)
include($$SUBMODS_CPP/Submods.pri)
include($$NUMERICS_CPP/Numerics.pri)
include($$GEMS3K_CPP/gems3k.pri)

