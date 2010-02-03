#qmake -spec win32-borland gems_qt.pro > a

TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gems3
VERSION         = 3.0.0

CONFIG		-= qt
CONFIG		+= thread warn_on debug windows

DEFINES         += Use_mt_mode
LIBS	        += -L./lib -lgems_qt -lqtmt -lqtmain


!win32 {
  DEFINES += __unix
}

macx-g++{
  DEFINES += __APPLE__
}

RESOURCES += img.qrc

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

OBJECTS_DIR       = obj

include($$MODS_CPP/mods.pri)
include($$NUMERICS_CPP/numerics.pri)
include($$SUBMOD_CPP/submod.pri)



