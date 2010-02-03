
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems3
VERSION         = 3.0.0

DEFINES         += Use_mt_mode
DEFINES         += Use_qd_real
#DEFINES         += GEMITERTRACE

CONFIG		+= qt thread warn_on
#CONFIG		+= debug
#CONFIG		+= qmake_cache
QT += network

LIBS	+= -lqd
RESOURCES += img.qrc

!win32 {
  DEFINES += __unix
}

macx-g++ {
  DEFINES -= Use_qd_real
  DEFINES += __APPLE__
#  INCLUDEPATH += /Library/Frameworks/QtCore.framework/Versions/4/Headers
  CONFIG -= warn_on
  CONFIG += warn_off
  LIBS -= -lqd
}

#LIBS += -L /usr/local/lib/qd -lqd
#LIBS	+= -lqd
#RESOURCES += img.qrc

VIZOR4_CPP      =  ./vizor4
KERNEL_CPP     =  ./vizor4/kernel
LIBS_CPP       =  ./vizor4/libs
DLG4_CPP        =  ./vizor4/dlg4
MODS_CPP       =  ./mods
SUBMOD_CPP     =  ./mods/submod
NUMERICS_CPP     =  ./mods/numerics

VIZOR4_H      =  $$VIZOR4_CPP
KERNEL_H     =  $$KERNEL_CPP
LIBS_H       =  $$LIBS_CPP
DLG4_H        =  $$DLG4_CPP
MODS_H       =  $$MODS_CPP
SUBMOD_H     =  $$SUBMOD_CPP
NUMERICS_H     =  $$NUMERICS_CPP


DEPENDPATH   += ;$$VIZOR4_H;$$KERNEL_H;$$LIBS_H;$$DLG4_H;$$MODS_H;$$SUBMOD_H
DEPENDPATH   += ;$$NUMERICS_H
INCLUDEPATH   += ;$$VIZOR4_H;$$KERNEL_H;$$LIBS_H;$$DLG4_H;$$MODS_H;$$SUBMOD_H
INCLUDEPATH   += ;$$NUMERICS_H

MOC_DIR = tmp
UI_DIR  = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR       = obj

include($$DLG4_CPP/dlg4.pri)
include($$LIBS_CPP/libs.pri)
include($$NUMERICS_CPP/numerics.pri)
include($$SUBMOD_CPP/submod.pri)
include($$MODS_CPP/mods.pri)
include($$KERNEL_CPP/kernel.pri)
include($$VIZOR4_CPP/vizor4.pri)
