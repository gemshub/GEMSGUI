TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems3
VERSION         = 3.0.0

DEFINES         += Use_mt_mode
#DEFINES         += Use_qd_real

CONFIG		+= qt thread warn_on
CONFIG		+= help
QT += network
QT += sql
QT += xml

#LIBS	+= -lqd
RESOURCES += img.qrc

!win32 {
  DEFINES += __unix
  DEFINES += GEMS_RELEASE
}

macx-g++ {
#  DEFINES -= Use_qd_real
  DEFINES += __APPLE__
macx:QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
  CONFIG -= warn_on
  CONFIG += warn_off
# LIBS -= -lqd
}

VIZOR4_CPP      =  ./vizor4
KERNEL_CPP     =  ./vizor4/kernel
DLG4_CPP        =  ./vizor4/dlg4
MODS_CPP       =  ./mods
SUBMOD_CPP     =  ./mods/submod
NUMERICS_CPP     =  ./mods/numerics
GEMS3K_CPP     =  ../standalone/GEMS3K

VIZOR4_H      =  $$VIZOR4_CPP
KERNEL_H     =  $$KERNEL_CPP
DLG4_H        =  $$DLG4_CPP
MODS_H       =  $$MODS_CPP
SUBMOD_H     =  $$SUBMOD_CPP
NUMERICS_H     =  $$NUMERICS_CPP
GEMS3K_H     =  $$GEMS3K_CPP

DEPENDPATH   += $$VIZOR4_H
DEPENDPATH   += $$KERNEL_H
DEPENDPATH   += $$DLG4_H
DEPENDPATH   += $$MODS_H
DEPENDPATH   += $$SUBMOD_H
DEPENDPATH   += $$NUMERICS_H
DEPENDPATH   += $$GEMS3K_H

INCLUDEPATH   += $$VIZOR4_H
INCLUDEPATH   += $$KERNEL_H
INCLUDEPATH   += $$DLG4_H
INCLUDEPATH   += $$MODS_H
INCLUDEPATH   += $$SUBMOD_H
INCLUDEPATH   += $$NUMERICS_H
INCLUDEPATH   += $$GEMS3K_H

MOC_DIR = tmp
UI_DIR  = $$MOC_DIR
UI_SOURSEDIR  = $$MOC_DIR
UI_HEADERDIR  = $$MOC_DIR
OBJECTS_DIR       = obj

include($$DLG4_CPP/dlg4.pri)
include($$NUMERICS_CPP/numerics.pri)
include($$SUBMOD_CPP/submod.pri)
include($$MODS_CPP/mods.pri)
include($$KERNEL_CPP/kernel.pri)
include($$VIZOR4_CPP/vizor4.pri)
include($$GEMS3K_CPP/gems3k.pri)
